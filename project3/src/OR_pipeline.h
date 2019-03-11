/*
	Jason Gurevitch and Douglas Abrams
	Project 2
	Task 1

*/

#ifndef OR_pipeline_H
#define OR_pipeline_H
 
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include "opencv2/opencv.hpp"


struct point {
	int x;
	int y;

	point(int xnew, int ynew){
		x = xnew;
		y = ynew;
	}
};

void grayscaleThreshold(cv::Mat &src, cv::Mat &dest, int threshold );

void OR_pipeline(cv::Mat& src, cv::Mat& frame, int threshold);

void erode(cv::Mat &src, cv::Mat &dest,  int iterations, int dim, bool useCross);

void dilate(cv::Mat &src, cv::Mat &dest,  int iterations, int dim, bool useCross);

int findComponents(cv::Mat &src, cv::Mat &dest, bool visualize);

std::vector<std::vector<point>> getRegions(cv::Mat &src, int numLabels);

int getMoment(int p, int q, std::vector<point> region_points);

point getCentroid(std::vector<point> region_points);

float getCentralAxis(std::vector<point> region_points);

float getExtent(float axis_angle, std::vector<point> region_points, point centroid);

float project(float axis_angle, point p, point centroid);

std::vector<point> orientedBoundinBox(std::vector<point> region_points, point centroid, float major_axis);

float getPercentFilled(std::vector<point> region_points, std::vector<point> corners, float major_extent, float minor_extent);

float getBoundingBoxRatio(std::vector<point> region_points, point centroid, float major_axis);

float getSecondOrderMomentAboutCentralAxis(std::vector<point> region_points, float major_axis, point centroid);



#endif