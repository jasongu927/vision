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
#include "OR_pipeline.h"


enum Video_state{
	WAITING,
	CLASSIFYING,
};

int main(int argc, char *argv[]) {
	cv::VideoCapture *capdev;
	char label[256];
	int quit = 0;
	int frameid = 0;
	char buffer[256];
	char database_name[256];
	std::vector<int> pars;

	pars.push_back(5);

	if( argc < 3 ) {
	    printf("Usage: %s <label> <database>\n", argv[0]);
	    exit(-1);
	}

	// open the video device
	capdev = new cv::VideoCapture(0);
	if( !capdev->isOpened() ) {
		printf("Unable to open video device\n");
		return(-1);
	}

	strcpy(label, argv[1]);
	strcpy(database_name, argv[2]);

	Database* d;
	d = database_read(database_name);
	cv::Size refS( (int) capdev->get(cv::CAP_PROP_FRAME_WIDTH ),
		       (int) capdev->get(cv::CAP_PROP_FRAME_HEIGHT));

	printf("Expected size: %d %d\n", refS.width, refS.height);

	cv::namedWindow("Video", 1); // identifies a window?
	cv::Mat frame;
    cv::Mat src;
	Video_state video_state = WAITING;

	for(;!quit;) {
		*capdev >> src; // get a new frame from the camera, treat as a stream

		if( src.empty() ) {
		  printf("frame is empty\n");
		  break;
		}
		std::vector<Region_features> features = OR_pipeline(src, 110, true, frame);
		
		if(video_state == CLASSIFYING){
			for(int i = 0; i < features.size(); i++){
				std::string name = classify_scaledEuclideanDistance(d, features[i]);
				std::cout << "object " << i << " has been classified as " << name << std::endl;
			} 
		}
		cv::imshow("Video", frame);


		int key = cv::waitKey(10);

		switch(key) {
		case 'a':
			video_state = WAITING;
			break;
		case 'c':
			if (video_state != CLASSIFYING){
				//std::cout << "computing feature averages" << std::endl;
				computeFeatureAverages(d);
				//std::cout <<"wooo" <<std::endl;
			}
			video_state = CLASSIFYING;
			break;
		case 'n':{
			char name[256];
			std::cout << "enter a label" << std::endl;
			std::cin >> name;
			std::cout << "adding entry with name " << name << std::endl;
			Entry e;
			e.name = std::string(name);
			e.features = features[1];
			database_addEntry(e, d);
		}
			break;
		case 'w':
			database_write(database_name, d);
			break;
		case 'q':
		    quit = 1;
		    break;
		default:
		    break;
		}

	}

	// terminate the video capture
	printf("Terminating\n");
	delete capdev;

	return(0);
}
