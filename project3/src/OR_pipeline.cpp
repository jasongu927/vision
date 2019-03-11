/*
	Jason Gurevitch and Douglas Abrams
	Project 3
	Task 3

*/
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include "opencv2/opencv.hpp"
#include "OR_pipeline.h"
#include <math.h>



void OR_pipeline(cv::Mat& src, cv::Mat& output, int threshold){
	cv::Mat grayscaled;
    grayscaleThreshold(src, grayscaled, threshold );
	cv::Mat tmp = grayscaled.clone();
	for (int j = 1; j < 5; j++){
		erode(tmp, tmp, j , 3, true);
		dilate(tmp, tmp, j , 3, true);
	}
	//output = tmp.clone();
	findComponents(tmp, output, true);
	//output = grayscaled.clone();
}

std::vector<std::vector<point>> getRegions(cv::Mat &src, int numLabels){
	auto region_ids = std::vector<std::vector<point>>(numLabels);
	for(int i = 0; i < src.rows; i++){
		for(int j = 0; j < src.cols; j++){
			
			int region = (int)src.at<uchar>(cv::Point(i, j));
			point p = point(j, i);

			region_ids[region].push_back(p);
		}
	}
	return region_ids;
}


int getMoment(int p, int q, std::vector<point> region_points){
	int sum = 0;
	for (auto it = region_points.begin(); it != region_points.end(); ++it){
		point p0 = (*it);
		sum += pow(p0.x, p)*pow(p0.y, q);
	}
	return sum;
}

int getMomentAboutCenter(int p, int q, std::vector<point> region_points, point c0){
	int sum = 0;
	for (auto it = region_points.begin(); it != region_points.end(); ++it){
		point p0 = (*it);
		sum += pow(p0.x - c0.x, p) * pow(p0.y - c0.y, q); 
	}
	return sum;
}
point getCentroid(std::vector<point> region_points){
	float dnom = (float) getMoment(0, 0, region_points);
	int Xc = (int)(getMoment(1, 0, region_points) / dnom);
	int Yc = (int)(getMoment(0, 1, region_points) / dnom);
	return point(Xc, Yc);
}

void grayscaleThreshold(cv::Mat &src, cv::Mat &dest, int threshold ){
	cv::Mat gray;
	cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);
	cv::threshold(gray, dest, threshold, 255, 1);
}

float getCentralAxis(std::vector<point> region_points){

	point centroid = getCentroid(region_points);
	float alpha  = std::atan2( (2 * getMomentAboutCenter(1, 1, region_points, centroid)) , getMomentAboutCenter(2, 0, region_points, centroid) - 
											 getMomentAboutCenter(0, 2, region_points, centroid));
	return alpha;
}

 float getExtent(float axis_angle, std::vector<point> region_points, point centroid){
	float max_val = 0;
	for(auto it = region_points.begin(); it!= region_points.end(); ++it){
		point p0 = (*it);
		float val = project(axis_angle, p0, centroid);
		if(val > max_val){
			max_val = val;
		}
	}
	return sqrt(max_val);
 }

float project(float axis_angle, point p, point centroid){
	float magnitude = (p.x - centroid.x) * (p.x - centroid.x) + (p.y - centroid.y) * (p.y - centroid.y);
	return magnitude * cos(axis_angle);
}

std::vector<point> orientedBoundinBox(std::vector<point> region_points, point centroid, float major_axis){

	float minor_axis = major_axis + M_PI/2;

	float major_extent = getExtent(major_axis, region_points, centroid);
	float minor_extent = getExtent(minor_axis, region_points, centroid);

	//combine extens to get four corners
	//return as array of points

	float major_x_extent = major_extent * cos (major_axis); 
	float minor_x_entent = minor_extent * cos (minor_axis); 
	float major_y_extent = major_extent * sin (major_axis); 
	float minor_y_extent = minor_extent * sin (minor_axis); 

	float x_dist = major_x_extent + minor_x_entent;
	float y_dist = major_y_extent + minor_y_extent;

	std::vector<point> corners;
	corners.push_back(point(centroid.x + x_dist, centroid.y + y_dist));
	corners.push_back(point(centroid.x + x_dist, centroid.y - y_dist));
	corners.push_back(point(centroid.x - x_dist, centroid.y - y_dist));
	corners.push_back(point(centroid.x - x_dist, centroid.y + y_dist));

	return corners;
}

float getPercentFilled(std::vector<point> region_points, std::vector<point> corners, float major_extent, float minor_extent){
	int amount_foregroud = getMoment(0, 0, region_points);
	float area = (major_extent * 2) * (minor_extent * 2);
	return amount_foregroud/area;
	
}
float getBoundingBoxRatio(std::vector<point> region_points, point centroid, float major_axis){
	float major_extent = getExtent(major_axis, region_points, centroid);
	float minor_extent = getExtent(major_axis + M_PI/2, region_points, centroid);
	return major_extent/minor_extent;
}

float getSecondOrderMomentAboutCentralAxis(std::vector<point> region_points, float major_axis, point c0){
	float sum = 0;
	float minor_axis = major_axis + M_PI/2;

	for (auto it = region_points.begin(); it != region_points.end(); ++it){
		point p0 = (*it);
		float tmp = p0.x - c0.x * cos(minor_axis) + p0.y - c0.y * sin(minor_axis); 
        sum += tmp * tmp;
	}
	return sum/ getMoment(0,0, region_points);
}

int findComponents(cv::Mat &src, cv::Mat &dest, bool visualize){
	cv::Mat tmp;
	int numLabels =	cv::connectedComponents(src, tmp, 4);
	

	if(visualize){
		float scale = 255/numLabels;
	  	for( auto it = tmp.begin<int>(); it != tmp.end<int>(); ++it){
		 	if( *it != 0){
				(*it) = ((*it) * scale);
			}
	 	}
	}
	tmp.convertTo(dest, src.type());
	return numLabels;
}

void erode(cv::Mat &src, cv::Mat &dest,  int iterations, int dim, bool useCross){
	int element_type;
	if (useCross){
		element_type = cv::MORPH_CROSS;
	}
	else {
		element_type = cv::MORPH_RECT;
	}
	cv::Mat element = cv::getStructuringElement(element_type, cv::Size(dim, dim));
	cv::Mat tmp;
	tmp = src.clone();
	for (int i = 0; i < iterations; i++){
		cv::erode(tmp, dest, element);
		tmp = dest.clone();
	}

}
void dilate(cv::Mat &src, cv::Mat &dest,  int iterations, int dim=3, bool useCross=true){
	int element_type;
	if (useCross){
		element_type = cv::MORPH_CROSS;
	}
	else {
		element_type = cv::MORPH_RECT;
	}
	cv::Mat element = cv::getStructuringElement(element_type, cv::Size(dim, dim));
	cv::Mat tmp;
	tmp = src.clone();
	for (int i = 0; i < iterations; i++){
		cv::dilate(tmp, dest, element);
		tmp = dest.clone();
	}
}
