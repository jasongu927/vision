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


int TASK4(char* dirname, char* sourceFile, int topN){

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


    TASK4(dirname, filename, 5);


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