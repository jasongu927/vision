/*
	Jason Gurevitch and Douglas Abrams
	Project 2
	Task 2 (extension)
*/
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include "opencv2/opencv.hpp"
#include "metrics.h"

int TASK2(char* dirname, char* sourceFile, int topN){
	DIR *dirp;
	struct dirent *dp;
	
	dirp = opendir(dirname);
	if( dirp == NULL){
		std::cout << "directory does not exist" << std::endl;
		return(-1);
	}
	

	std::vector<img_metric> ssds;
	//std::cout <<"starting loop" <<std::endl;	
	cv::Mat src = cv::imread(sourceFile);

	while( (dp = readdir(dirp)) != NULL){ // read the next image
		//std::cout <<"in loop" << std::endl;
		if( strstr(dp->d_name, ".jpg") ||
				strstr(dp->d_name, ".png") ||
				strstr(dp->d_name, ".ppm") ||
				strstr(dp->d_name, ".tif") ) {
				img_metric pair;// = new(img_metric);
				sprintf(pair.name, "%s/%s", dirname, dp->d_name);
				cv::Mat comp = cv::imread(pair.name);
				pair.metric = HIST(comp , src);


				ssds.push_back(pair);

		}
	}
	
	std::sort(ssds.begin(), ssds.end(), compare);
	for(int i = 0; i < topN; i++){
		std::cout << "name is " << ssds[i].name << std::endl;
	}
	
	closedir(dirp);
	return(1);	
} 

int TASK2_homemadeHist(char* dirname, char* sourceFile, int topN){
	DIR *dirp;
	struct dirent *dp;
	
	dirp = opendir(dirname);
	if( dirp == NULL){
		std::cout << "directory does not exist" << std::endl;
		return(-1);
	}
	

	std::vector<img_metric> ssds;
	//std::cout <<"starting loop" <<std::endl;	
	cv::Mat src = cv::imread(sourceFile);
	color_hist* src_hist = make_hist_from_image(src);
	while( (dp = readdir(dirp)) != NULL){ // read the next image
		//std::cout <<"in loop" << std::endl;
		if( strstr(dp->d_name, ".jpg") ||
				strstr(dp->d_name, ".png") ||
				strstr(dp->d_name, ".ppm") ||
				strstr(dp->d_name, ".tif") ) {
				img_metric pair;// = new(img_metric);
				sprintf(pair.name, "%s/%s", dirname, dp->d_name);
				cv::Mat comp_image = cv::imread(pair.name);
				color_hist* comp_hist = make_hist_from_image(comp_image);
				color_hist* diff_hist = compare_hists(src_hist, comp_hist);
				pair.metric = hist_ssd(diff_hist);
				//pair.ssd = compare_SSD(pair.name, src);
				ssds.push_back(pair);
				delete(comp_hist);
				delete(diff_hist);
				//delete(&comp_image);
		}
	}
	delete(src_hist);
	//std::cout << "finished loop" << std::endl;
	
	std::sort(ssds.begin(), ssds.end(), compare);
	for(int i = 0; i < topN; i++){
		std::cout << "name is " << ssds[i].name << std::endl;
	}
	
	closedir(dirp);
	return(1);	
}


int main(int argc, char *argv[]) {
	char filename[256];
	char dirname[256];
    int topN;
	if(argc < 4){
		std::cout << "please supply a filename and a directory" << std::endl;
		return(-1);
	}
	// if the user provided a directory path, use it
	strcpy(filename, argv[1]);
	strcpy(dirname, argv[2]);
    topN = atoi(argv[3]);


    TASK2(dirname, filename, 5);


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
