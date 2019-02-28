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


