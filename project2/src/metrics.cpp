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


float getColorRatio(cv::Vec3b pixel, char colorOfInteret){
	int b = pixel[0]; 
	int g = pixel[1]; 
	int r = pixel[2]; 
	if (b == 0){
		b = 1;
	}
	if (g == 0){
		g =1 ;
	}
	if (r == 0){
		r = 1;
	}
	float ratio;
	if (colorOfInteret ==  'g'){
		ratio = g/(r+b+g);
	}else if (colorOfInteret == 'r'){
		ratio = r/(r+b+g);
	}else if (colorOfInteret ==  'b'){
		ratio = b/(r+b+g);
	}else{
		ratio = g/(r+b+g);
	}
	return ratio;
}


float GREENESS (cv::Mat &comp, cv::Mat &source){

	int compX = comp.cols;
	int compY = comp.rows;
	int srcX = source.cols;
	int srcY = source.rows;

	float sum_greeness_comp = 0;
	float sum_greeness_src = 0;

	for (int i = 0; i < compX; i++){
		for (int j = 0; j < compY; j++){
				cv::Vec3b comp_pix = comp.at<cv::Vec3b>(cv::Point(i, j));
				sum_greeness_comp += getColorRatio(comp_pix, 'g');
		}
	}

	for (int i = 0; i < srcX; i++){
		for (int j = 0; j < srcY; j++){
				cv::Vec3b src_pix = source.at<cv::Vec3b>(cv::Point(i, j));
				sum_greeness_src += getColorRatio(src_pix, 'g');
		}
	}
		float norm_sum_greeness_comp = sum_greeness_comp/(compX * compY);
	float norm_sum_greeness_src = sum_greeness_src/(srcX * srcY);

	return std::abs(norm_sum_greeness_src - norm_sum_greeness_comp);
}

float extensionMetric (cv::Mat &comp, cv::Mat &source){

	int compX = comp.cols;
	int compY = comp.rows;
	int srcX = source.cols;
	int srcY = source.rows;

	float sum_greeness_comp = 0;
	float sum_blueness_comp = 0;
	float sum_redness_comp = 0;

	float sum_greeness_src = 0;
	float sum_blueness_src = 0;
	float sum_redness_src = 0;

	for (int i = 0; i < compX; i++){
		for (int j = 0; j < compY; j++){
				cv::Vec3b comp_pix = comp.at<cv::Vec3b>(cv::Point(i, j));
				sum_greeness_comp += getColorRatio(comp_pix, 'g');
				sum_blueness_comp += getColorRatio(comp_pix, 'b');
				sum_redness_comp += getColorRatio(comp_pix, 'r');
		}
	}

	for (int i = 0; i < srcX; i++){
		for (int j = 0; j < srcY; j++){
				cv::Vec3b src_pix = source.at<cv::Vec3b>(cv::Point(i, j));
				sum_greeness_src += getColorRatio(src_pix, 'g');
				sum_blueness_src += getColorRatio(src_pix, 'b');
				sum_redness_src += getColorRatio(src_pix, 'r');

		}
	}
	float norm_sum_greeness_comp = sum_greeness_comp/(compX * compY);
	float norm_sum_blueness_comp = sum_blueness_comp/(compX * compY);
	float norm_sum_redness_comp = sum_redness_comp/(compX * compY);

	float norm_sum_greeness_src = sum_greeness_src/(srcX * srcY);
	float norm_sum_blueness_src = sum_blueness_src/(srcX * srcY);
	float norm_sum_redness_src = sum_redness_src/(srcX * srcY);

	float greenDiff = std::abs(norm_sum_greeness_src - norm_sum_greeness_comp);
	float blueDiff = std::abs(norm_sum_blueness_comp - norm_sum_blueness_src);
	float redDiff = std::abs(norm_sum_redness_comp - norm_sum_redness_src);
	
	if (greenDiff < blueDiff){
		if ( greenDiff < redDiff){
			return greenDiff;
		}
		else {
			return redDiff;
		}
	}
	else{
		if (blueDiff < redDiff){
			return blueDiff;
		}
		else{
			return redDiff;
		}
	}

}

//task 3 create concentric squares of influence for histogram compairson
double multi_layerHIST(cv::Mat &comp, cv::Mat &source, int layers){
	int comp_midx = comp.cols/2;
	int comp_midy = comp.rows/2;
	int src_midx = source.cols/2;
	int src_midy = source.rows/2;
	double result = 0;
	int scale = 1;
	for(int i = 0; i < layers; i++){
		int comp_scalex = comp_midx/(scale);
		int comp_scaley = comp_midy/(scale);
		int src_scalex = src_midx/(scale);
		int src_scaley = src_midy/(scale);
		cv::Mat subsection_comp (comp, cv::Rect(comp_midx - comp_scalex, comp_midy - comp_scaley,
											comp_midx + comp_scalex, comp_midy + comp_scaley));
		
		cv::Mat subsection_src (source, cv::Rect(src_midx - src_scalex, src_midy - src_scaley,
											src_midx + src_scalex, src_midy + src_scaley));
		result += HIST(subsection_comp, subsection_src);
		scale *= 2;
	}
	return result;
}

void sobel_gradient(cv::Mat& src, cv::Mat& dest){
	
	cv::Mat kernx = (cv::Mat_<char>(3,3) << -1, 0, 1,
						-2, 0, 2,
						-1, 0 , 1);

	cv::Mat kerny = (cv::Mat_<char>(3,3) << -1, -2, -1,
						0, 0, 0,
						1, 2 , 1);	


	cv::Mat tmpx;
	cv::Mat tmpy;

	cv::filter2D(src, tmpx, src.depth(), kernx);
	cv::filter2D(src, tmpy, src.depth(), kerny);

	dest.create(src.size(), src.type());

	auto ity = tmpy.begin<cv::Vec3b>();
	auto itx = tmpx.begin<cv::Vec3b>();
	for( auto it = dest.begin<cv::Vec3b>(); it != dest.end<cv::Vec3b>(); ++it){
		// add 1 to prevent divide by 0 errors
		(*it)[0] = atan((*itx)[0]/(1 + (*ity)[0]));
		(*it)[1] = atan((*itx)[1]/(1 + (*ity)[1]));
		(*it)[2] = atan((*itx)[2]/(1 + (*ity)[2]));
		itx++;
		ity++;
	}
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
//in place standard_chromaticity
void standard_chromaticize(cv::Mat& src){
	for( auto it = src.begin<cv::Vec3b>(); it != src.end<cv::Vec3b>(); ++it){
		int r, g, b;
		//format bgr
		float color_sum = (*it)[0]+(*it)[1]+(*it)[2];
		//std::cout << color_sum << std::endl;
		color_sum = color_sum = 0 ? 1 : color_sum;
		r = ((*it)[2]/color_sum) * 255;
		g = ((*it)[1]/color_sum) * 255;
		b = 255 - r - g;
		(*it)[0] = b;
		(*it)[1] = g;
		(*it)[2] = r;
	}
}

//task 2
//almost all code taken from OpenCV documentation
double HIST2D(cv::Mat &comp, cv::Mat &source){
    int RGBbin = 32;
	int histSize[] = {RGBbin, RGBbin};
    float RGBrange[] = { 0, 256 };

    const float* ranges[] = { RGBrange, RGBrange };
    cv::MatND src;
    cv::MatND compared;

    int channels[] = {0, 1};
    cv::calcHist( &comp, 1, channels, cv::Mat(), // do not use mask
             compared, 2, histSize, ranges,
             true, // the histogram is uniform
             false );

	cv::calcHist( &source, 1, channels, cv::Mat(), // do not use mask
             src, 2, histSize, ranges,
             true, // the histogram is uniform
             false );
	

	double metric = cv::compareHist(src, compared, 1);

	return metric;
}

double HIST(cv::Mat &comp, cv::Mat &source){
    int RGBbin = 32;
	int histSize[] = {RGBbin};
    float RGBrange[] = { 0, 256 };

    const float* ranges[] = { RGBrange};
    cv::MatND src;
    cv::MatND compared;

	double metric = 0;
	for(int i = 0; i < 3; i++){
		int channels[] = {i};
		cv::calcHist( &comp, 1, channels, cv::Mat(), // do not use mask
				 compared, 1, histSize, ranges,
				 true, // the histogram is uniform
				 false );

		cv::calcHist( &source, 1, channels, cv::Mat(), // do not use mask
				 src, 1, histSize, ranges,
				 true, // the histogram is uniform
				 false );
	

	 metric += cv::compareHist(src, compared, 1);
	}
	return metric;
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

