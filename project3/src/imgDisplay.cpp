/*
	Bruce A. Maxwell
	S19
	Simple example of video capture and manipulation
	Based on OpenCV tutorials

	Compile command (macos)

	clang++ -o vid -I /opt/local/include vidDisplay.cpp -L /opt/local/lib -lopencv_core -lopencv_highgui -lopencv_video -lopencv_videoio

	use the makefiles provided

	make vid

*/
#include <cstdio>
#include <opencv2/opencv.hpp>
#include "metrics.h"


int main(int argc, char *argv[]) {

	cv::Mat src;
	char filename[256];

	// usage
	if(argc < 2) {
		printf("Usage %s <image filename>\n", argv[0]);
		exit(-1);
	}
	strcpy(filename, argv[1]);

	// read the image
	src = cv::imread(filename);

	// test if the read was successful
	if(src.data == NULL) {
		printf("Unable to read image %s\n", filename);
		exit(-1);
	}

    cv::Mat frame;
    
    OR_pipeline(src, frame, 110);
	
	
// create a window
	cv::namedWindow(filename, 1);

	// edit the source image here

	// show the image in a window
	cv::imshow(filename, frame);

	// wait for a key press (indefinitely)
	cv::waitKey(0);

	// get rid of the window
	cv::destroyWindow(filename);
	


	return(0);
}
