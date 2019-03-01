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

#define KERNEL_SIZE 5


int TASK1(char* dirname, char* sourceFile, int topN){
    DIR *dirp;
	struct dirent *dp;
	
	dirp = opendir(dirname);
	if( dirp == NULL){
		std::cout << "directory does not exist" << std::endl;
		return(-1);
	}
	
	std::vector<img_metric> ssds;
	cv::Mat src = cv::imread(sourceFile);

	while( (dp = readdir(dirp)) != NULL){ // read the next image
		if( strstr(dp->d_name, ".jpg") ||
				strstr(dp->d_name, ".png") ||
				strstr(dp->d_name, ".ppm") ||
				strstr(dp->d_name, ".tif") ) {
				img_metric pair;
                sprintf(pair.name, "%s/%s", dirname, dp->d_name);
				// strcpy(pair.name, dp->d_name);


                cv::Mat comp_pixels = cv::imread(pair.name);
                pair.metric = SSD(comp_pixels, src, KERNEL_SIZE);
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


    TASK1(dirname, filename, 5);


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
