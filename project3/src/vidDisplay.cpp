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
#include "database.h"
#include <unistd.h>
#include <string>


enum Video_state{
	WAITING,
	CLASSIFYING,
};

void addLabel(Region_features features, cv::Mat displayed, std::string label){
	point centroid = features.centroid;
	char feature_text[100];
	cv::circle(displayed, cv::Point(centroid.x, centroid.y), 3, cv::Scalar(0, 255, 255));
	cv::putText(displayed, label, cv::Point(centroid.x + 5, centroid.y),  cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 255, 0), 1);
	if(features.features.size() == 3){
		sprintf(feature_text, "%5.1f, %5.1f, %5.1f", features.features[0], features.features[1], features.features[2]);
		cv::putText(displayed, std::string(feature_text), cv::Point(centroid.x, centroid.y + 25), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(160, 255, 0), 1);
	}
	else {
		std::cout << features.features.size();
	}
}

int main(int argc, char *argv[]) {
	cv::VideoCapture *capdev;
	Database d;
	char label[256];
	int quit = 0;
	int frameid = 0;
	char buffer[256];
	char database_name[256];
	std::vector<int> pars;
	std::string name;
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
	database_read(database_name, &d);

	cv::Size refS( (int) capdev->get(cv::CAP_PROP_FRAME_WIDTH ),
		       (int) capdev->get(cv::CAP_PROP_FRAME_HEIGHT));

	printf("Expected size: %d %d\n", refS.width, refS.height);
	cv::namedWindow("Video", 1); // identifies a window?
	cv::Mat frame;
    cv::Mat src;
	Video_state video_state = WAITING;

	for(;!quit;) {
		//std::cout << "starting loop" << std::endl;
		*capdev >> src; // get a new frame from the camera, treat as a stream

		if( src.empty() ) {
		  printf("frame is empty\n");
		  break;
		}
		//std::cout << "starting OR pipeline" << std::endl;
		std::vector<Region_features> features = OR_pipeline(src, 110, true, frame);
		//std:: cout << "finished OR pipeline" << std::endl;
		if(video_state == CLASSIFYING){
			//std::cout << "starting classify" <<std::endl;
			for(int i = 0; i < features.size(); i++){
				name = std::string(classifyKNN(&d, features[i], 5));
				//name = classify_scaledEuclideanDistance(&d, features[i]);
				std::cout << "object " << i << " has been classified as " << name << std::endl;
				addLabel(features[i], frame, name);
			} 
			//std::cout << "finished classify" << std::endl;
		}
		//std::cout << "trying to show frame " << label << std::endl;
		cv::imshow("Video", frame);
		//std::cout << "showed frame" << std::endl;
		int key = cv::waitKey(10);


		//std::cout << "pressed key" << (char) (key) << std::endl;
		switch(key) {
		case 'a':
			video_state = WAITING;
			break;
		case 'c':
			if (video_state != CLASSIFYING){
				//std::cout << "computing feature averages" << std::endl;
				computeFeatureAverages(&d);
				video_state = CLASSIFYING;
				//std::cout <<"wooo" <<std::endl;
			}
			break;
		case 'n':{
			char name[256];
			std::cout << "enter a label" << std::endl;
			std::cin >> name;
			std::cout << "adding entry with name " << name << std::endl;
			Entry e;
			e.name = std::string(name);
			e.features = features[0];
			database_addEntry(e, &d);
			computeFeatureAverages(&d);
		}
			break;
		case 'w':
			database_write(database_name, &d);
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
