#include "Predictor.h"

int main(int argc, char **argv){
	Predictor pred;
	pred.readAFile(argv[1]);
	//Test Always Taken
	pred.alwaysTakenOrNotTaken("at");
	//Test Always Not Taken
	pred.alwaysTakenOrNotTaken("ant");
	//test 1 bit bimodal predictor
	pred.biPrediction(16,1);
	pred.biPrediction(32,1);
	pred.biPrediction(64,1);
	pred.biPrediction(128,1);
	pred.biPrediction(256,1);
	pred.biPrediction(512,1);
	pred.biPrediction(1024,1);
	pred.biPrediction(2048,1);
	//test 2 bit bimodal predictor
	pred.biPrediction(16,2);
	pred.biPrediction(32,2);
	pred.biPrediction(64,2);
	pred.biPrediction(128,2);
	pred.biPrediction(256,2);
	pred.biPrediction(512,2);
	pred.biPrediction(1024,2);
	pred.biPrediction(2048,2);
	pred.writeAFile(argv[2]);
	return 0;
}
