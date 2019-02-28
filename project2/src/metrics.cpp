/*
	Jason Gurevitch and Douglas Abrams
	Project 2
	Task 1

*/
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include "opencv2/opencv.hpp"
#include "metrics.h"
 
bool compare(const img_metric &a, const img_metric &b){
	return a.metric < b.metric;
}

int SSD(cv::Mat &comp, cv::Mat &source, int N){

	int comp_midx = comp.cols/2;
	int comp_midy = comp.rows/2;
	int src_midx = source.cols/2;
	int src_midy = source.rows/2;
 

	if(N%2 == 0){
		std::cout << "kernel is even, that is wrong" << std::endl;
	}
	int ssd = 0; 
	int diff = (N-1)/2;
	for(int i = -diff; i < diff; i++){
		for(int j = -diff; j < diff; j++){


			cv::Vec3b comp_pix = comp.at<cv::Vec3b>(cv::Point(i + comp_midx, j + comp_midy));
			cv::Vec3b src_pix = source.at<cv::Vec3b>(cv::Point(i + src_midx, j + src_midy));

			int pix_ssd = 0;

			for(int c = 0; c < 3; c++){
				int color_diff = src_pix.val[c] - comp_pix.val[c];
				pix_ssd += color_diff * color_diff;
			}
			ssd += pix_ssd;
		}
	}
	return ssd;
}

//extensions
// use our own histograms instead of OpenCV ones


color_hist* make_hist(bool is_grayscale){
	color_hist* hist = new(color_hist);
	hist->is_grayscale = is_grayscale;
	for(int i = 0; i < 255; i++){
		hist -> red_hist[i] = 0;
		hist -> blue_hist[i] = 0;
		hist -> green_hist[i] = 0;
	}
	return hist;
}

color_hist* make_hist_from_image(cv::Mat& src){
	color_hist* hist = make_hist(true);
	for(auto it = src.begin<cv::Vec3b>(); it != src.end<cv::Vec3b>(); ++it){
		hist->blue_hist[(*it)[0]]++; //bgr
		hist->green_hist[(*it)[1]]++;
		hist->red_hist[(*it)[2]]++;
		hist->total_pixels++;
	}
	return hist;
}

color_hist* compare_hists(color_hist* src, color_hist* compare){
	color_hist* result = make_hist(false);
	for(int i = 0; i < 255; i++){
		result -> red_hist[i] = src -> red_hist[i] - compare -> red_hist[i];
		result -> blue_hist[i] = src -> blue_hist[i] - compare -> blue_hist[i];
		result -> green_hist[i] = src -> green_hist[i] - compare -> green_hist[i];
	}
	return result;
}

int hist_ssd(color_hist* src){
	int sum = 0;
	for(int i = 0; i < 255; i++){
		sum += src -> red_hist[i] * src -> red_hist[i];
		sum += src -> green_hist[i] * src -> green_hist[i];
		sum += src -> blue_hist[i] * src -> blue_hist[i];
	}
	return sum;
}

