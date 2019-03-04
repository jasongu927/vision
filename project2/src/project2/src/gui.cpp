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


typedef struct{
    char windowName[50];
    std::vector<img_metric> image_metrics;
}show_images_struct;

void showImage(int index, void* params_struct){
    show_images_struct* params = (show_images_struct*) params_struct;
    //std::cout << "trying to display image" << params->image_metrics[index].name << " in window " << params->windowName << std::endl;
    cv::Mat img = cv::imread(params->image_metrics[index].name);
    cv::Mat img_small = cv::Mat(500, 500, img.type());
    cv::resize(img, img_small, img_small.size(), 0, 0);
    
    cv::imshow(params->windowName, img_small);
}

void runGui (char* dirname, char* sourceFile, int topN, int metric){

    DIR *dirp;
	struct dirent *dp;
	
	dirp = opendir(dirname);
	if( dirp == NULL){
		std::cout << "directory does not exist" << std::endl;
		return;
	}
	
	std::vector<img_metric> ssds;
	cv::Mat src = cv::imread(sourceFile);

    cv::Mat src_grad; // precompute source gradient so you don't do it every time
    if(metric == 3){
        sobel_gradient(src, src_grad);
    }
    if(metric == 1 || 2){
        standard_chromaticize(src);
    }
    int i = 0;
	while( (dp = readdir(dirp)) != NULL){ // read the next image
		if( strstr(dp->d_name, ".jpg") ||
				strstr(dp->d_name, ".png") ||
				strstr(dp->d_name, ".ppm") ||
				strstr(dp->d_name, ".tif") ) {
                if(i % 100 == 0){
                    std::cout << "processed " << i << " images" <<std::endl;
                }
				img_metric pair;
                sprintf(pair.name, "%s/%s", dirname, dp->d_name);

                cv::Mat comp_pixels = cv::imread(pair.name);

                //run the right metric 
                if(metric == 0){
                    pair.metric = SSD(comp_pixels, src, 5);
                }else if(metric == 1){
                    standard_chromaticize(comp_pixels);
                    pair.metric = HIST(comp_pixels, src);
                }else if(metric == 2){
                    standard_chromaticize(comp_pixels);
                    pair.metric = multi_layerHIST(comp_pixels, src, 8);
                }else if(metric == 3){
                    pair.metric = HIST(comp_pixels, src);
                    cv::Mat comp_grad;
                    sobel_gradient(comp_pixels, comp_grad);
                    pair.metric += HIST(comp_grad, src_grad);
                }else if(metric == 4){
				    pair.metric = GREENESS(comp_pixels , src);
                }
                else{
                    pair.metric = extensionMetric(comp_pixels , src);
                }
                ssds.push_back(pair);
                i++;
		}
	}
    std::sort(ssds.begin(), ssds.end(), compare);


    char winName2[256] = "output";
	cv::namedWindow(winName2, 1);
    show_images_struct stuff;
    strcpy(stuff.windowName, winName2);
    stuff.image_metrics = ssds;

    cv::Mat best = cv::imread(ssds[0].name);
    cv::Mat best_small = cv::Mat(500, 500, best.type());
    cv::resize(src, best_small, best_small.size(), 0, 0);
    cv::imshow(winName2, best_small);
    int imageToShow = 0;
    //std::cout << "trying to display image" << (stuff.image_metrics[0]).name << " in window " << stuff.windowName << std::endl;
    cv::createTrackbar( "# imgs to show", winName2, &imageToShow, topN, showImage, (void*)(&stuff));
    cv::waitKey(0);
    while(true){ 

	 	char k = (char)cv::waitKey(0);
	 	if (k == 'y'){
            std::cout << "user pressed y, exiting inner function" << std::endl;
            cv::destroyWindow(winName2);
	 		break;
	 	}
    }
}

int main (int argc, char *argv[]){

	char filename[256];
	char dirname[256];
	if(argc < 3){
		std::cout << "please supply a filename and a directory" << std::endl;
		return(-1);
	}
	// if the user provided a directory path, use it
	strcpy(filename, argv[1]);
	strcpy(dirname, argv[2]);


    //initial window
	char winName[256] = "press x to exit";
	cv::namedWindow(winName, 0);
    int nImgsDisp = 0;
    cv::createTrackbar( "# imgs to show", winName, &nImgsDisp, 20);

    int method = 0;
    cv::createTrackbar( "method #", winName, &method, 5);

    //show source image
    cv::Mat src = cv::imread(filename);
    cv::Mat src_small = cv::Mat(500, 500, src.type());
    cv::resize(src, src_small, src_small.size(), 0, 0);

    while(true){ 
        nImgsDisp = cv::getTrackbarPos("# imgs to show", winName);
        method = cv::getTrackbarPos("# imgs to show", winName);
        
        cv::imshow(winName, src_small);
        

	 	char k = (char)cv::waitKey(0);
	 	if (k == 'x'){
	 		break;
	 	}
         if (k == 'r'){
             runGui(dirname, filename, nImgsDisp, method);
         }
        //  if (k == 'e'){
        //  //    cv::destroyWindow(winName2);
        //  }
    }


	// terminate the program
	printf("Terminating\n");

	return(0);
}

    