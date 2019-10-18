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
struct mDat{
	unsigned long long address;
	std::string branchVal;
};
struct ans{
	std::string predType; //might not need this
	unsigned long long correct;
};
class Predictor{
	public:
		long long numBranches;
		std::vector<mDat> info; //contains an address and a branch value
		std::vector<ans> output; //contains the prediction type and the number correct
		void readAFile(std::string file);
		void writeAFile(std::string file);
		void alwaysTakenOrNotTaken(std::string takenornottaken);
		//void alwaysNotTaken();
		void biPrediction(int sizeTable, int singleOrDoubleBit); //arg 2 is 1 or 2
		//void biPredDouble(int sizeTable);
		//void gShare(int grBits, int gCount);
		//void tournament(int biTable, int gShareTable, int tourTable, int grBits);
		
};

#endif
