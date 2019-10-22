#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <math.h>
#include <sstream>
#include <fstream>
#include <bitset>
struct myData{
	unsigned long long address;
	std::string branchVal;
	unsigned long long target;
};


class Predictor{
	public:
		unsigned long long correct;
		long long numBranches;
		long long numberAccess;
		std::vector<myData> info; //contains an address and a branch value
		std::vector<unsigned long long> output; 
		void readAFile(std::string file);
		void writeAFile(std::string file);
		void alwaysTakenOrNotTaken(std::string takenornottaken);
		void biPrediction(int sizeTable, int singleOrDoubleBit); //arg 2 is 1 or
		void gShare(int gCount);
		void tournament();
		void BTB();
		
};

#endif
