/*
	Jason Gurevitch and Douglas Abrams
	Project 2
	Task 1

*/
#ifndef METRICS_H
#define METRICS_H
 
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include "opencv2/opencv.hpp"

typedef struct{
	char name[100];
	int metric;
}img_metric;

typedef struct{
	int red_hist[255];
	int blue_hist[255];
	int green_hist[255];
	bool is_grayscale;
	int total_pixels;
}color_hist;
 
bool compare(const img_metric &a, const img_metric &b);

int SSD(cv::Mat &comp, cv::Mat &source, int N);

color_hist* make_hist(bool is_grayscale);

color_hist* make_hist_from_image(cv::Mat& src);

color_hist* compare_hists(color_hist* src, color_hist* compare);

int hist_ssd(color_hist* src);

void sobel_gradient(cv::Mat& src, cv::Mat& dest);

void standard_chromaticize(cv::Mat& src);

double HIST(cv::Mat &comp, cv::Mat &source);

double HIST2D(cv::Mat &comp, cv::Mat &source);

int getYellowRatio(cv::Vec3b pixel);

int YELLOWNESS (cv::Mat &comp, cv::Mat &source);

double multi_layerHIST(cv::Mat &comp, cv::Mat &source, int layers);

#endif
