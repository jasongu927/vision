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

typedef struct{
	char name[100];
	int metric;
}img_metric;
 
bool compare(const img_metric &a, const img_metric &b);

int SSD(cv::Mat &comp, cv::Mat &source, int N);

