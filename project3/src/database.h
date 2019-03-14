#ifndef DATABASE_H
#define DATABASE_H


#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include "opencv2/opencv.hpp"
#include <string>

#define NUM_FEATURES 2

struct Region_features{
	std::vector<float> features;
	// feature 0 is perfecnt filled
	// feature 1 is bounding box ratio
	// feature 2 is second order moment about central axis
};

struct Entry{
    Region_features features;
    std::string name;
    // this is written out as name, features
};

struct Database{
    std::vector<Entry> entries;
    std::vector<float> feature_sds;
    std::vector<float> feature_mins;
    std::vector<float> feature_maxs;
};
std::string classify_scaledEuclideanDistance( Database* d, Region_features newObjetFeatures);

void computeFeatureAverages(Database* d);

Database* database_read(char* filename);

void database_write(char* filename, Database* d);

void database_addEntry(Entry e, Database* d);

//database_matchSSD()

//database_matchKNN
bool compareLow(std::pair<std::string, int> a, std::pair<std::string, int> b) ;

bool compareHigh(std::pair<std::string, int> a, std::pair<std::string, int> b) ;

std::string classifyKNN(Database * d, Region_features newObjectFeatures, int k);

int pair_contains(std::string s, std::vector<std::pair<std::string, int>> frequencies);


#endif