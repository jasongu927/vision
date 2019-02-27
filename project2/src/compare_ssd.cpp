/*
	Jason Gurevitch and Douglas Abrams
	Project 2

*/
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include "opencv2/opencv.hpp"

#define KERNEL_SIZE 99

/*
void histogram(cv::Mat& src int* hist){
	std::cout << "the thing" << std::endl;
	for( auto it = src.begin<cv::Vec3b>(); it != src.end<cv::Vec3b>(); ++it){
		(*it)[0]+=50;
	}
}

compare_hists(int* histal, int* histb, int* result){
	
}
*/

typedef struct{
	char name[50];
	int ssd;
}img_ssd;
 
bool compareImage_SSDs(const img_ssd &a, const img_ssd &b){
	return a.ssd < b.ssd;
}



int calculate_SSD(cv::Mat &comp, cv::Mat &source, int N){
	int comp_midx = comp.cols/2;
	int comp_midy = comp.rows/2;
	int src_midx = source.cols/2;
	int src_midy = source.rows/2;
	
	//std::cout << "middle of comp is " << comp_midx << "," <<comp_midy << std::endl;
	//std::cout << "middle of source is " << src_midx << "," <<src_midy << std::endl;
	if(N%2 == 0){
		std::cout << "kernel is even, that is wrong" << std::endl;
	}
	int ssd = 0; 
	int diff = (N-1)/2;
	for(int i = -diff; i < diff; i++){
		for(int j = -diff; j < diff; j++){
			//std::cout << "calculating point" << i + comp_midx <<"," << j + comp_midy <<std::endl;
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


float compare_SSD(char* comparison_photo, cv::Mat &source_pixels){
	cv::Mat comp_pixels = cv::imread(comparison_photo);
	//std::cout << "comparing photo " << comparison_photo << std::endl;
	float ssd = calculate_SSD(comp_pixels, source_pixels, KERNEL_SIZE);
	//std::cout <<"finished comparing" << std::endl;
	return ssd;
}



int traverse_directory(char* dirname, char* sourceFile, int topN){
	DIR *dirp;
	struct dirent *dp;
	
	dirp = opendir(dirname);
	if( dirp == NULL){
		std::cout << "directory does not exist" << std::endl;
		return(-1);
	}
	

	std::vector<img_ssd> ssds;
	//std::cout <<"starting loop" <<std::endl;	
	cv::Mat src = cv::imread(sourceFile);
	while( (dp = readdir(dirp)) != NULL){ // read the next image
		//std::cout <<"in loop" << std::endl;
		if( strstr(dp->d_name, ".jpg") ||
				strstr(dp->d_name, ".png") ||
				strstr(dp->d_name, ".ppm") ||
				strstr(dp->d_name, ".tif") ) {
				img_ssd pair;// = new(img_ssd);
				strcpy(pair.name, dp->d_name);
				pair.ssd = compare_SSD(dp->d_name, src);
				ssds.push_back(pair);
		}
	}
	//std::cout << "finished loop" << std::endl;
	
	std::sort(ssds.begin(), ssds.end(), compareImage_SSDs);
	for(int i = 0; i < topN; i++){
		std::cout << "name is " << ssds[i].name << std::endl;
	}
	/*
	for(auto it = ssds.begin(); it!= ssds.end(); ++it){
		delete(*it);
	}*/
	
	closedir(dirp);
	return(1);
	
}




int main(int argc, char *argv[]) {
	char filename[256];
	char dirname[256];

	
	if(argc < 3){
		std::cout << "please supply a filename and a directory" << std::endl;
		return(-1);
	}
	// if the user provided a directory path, use it
	strcpy(filename, argv[1]);
	
	strcpy(dirname, argv[2]);
	
	
	traverse_directory(dirname, filename, 5);
	/*
	// show the image in a window
	cv::imshow(filename, src);

	// wait for a key press (indefinitely)
	cv::waitKey(0);

	// get rid of the window
	cv::destroyWindow(filename);
*/
	// terminate the program
	printf("Terminating\n");

	return(0);
}
