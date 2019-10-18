#include "Predictor.h"
using namespace std;
void Predictor::readAFile(string file){
	string behavior;
	unsigned long long addr;
	unsigned long long target;
	ifstream infile(file.c_str());
	string infoVal;
	while(infile>>hex>>addr>>behavior>>hex>>target){
		numBranches++;
		mDat data;
		data.address = addr;
		if(behavior == "T"){
			data.branchVal = "T";
		}
		else{
			data.branchVal = "NT";
		}
		info.push_back(data);
	}
	infile.close();
}
void Predictor::writeAFile(string file){
	ofstream out(file.c_str());
	for(unsigned int i = 0; i<output.size();i++){
		
		out << output[i].correct<< "," << numBranches <<"; ";
		if(i==0||i==1||i==8||i==17||i==25)out<<"\n"; 
		cout << output[i].correct<< "," << numBranches <<"; " << endl;
		if(i==0||i==1||i==8||i==17||i==25)cout<<"\n";
	}
}
void Predictor::alwaysTakenOrNotTaken(string torn){
	unsigned long long takenCount = 0;
	unsigned long long nTakenCount = 0;
	for(unsigned long long i = 0; i<info.size(); i++){
		if(info[i].branchVal == "T"){
			takenCount++;
		}
		if(info[i].branchVal == "NT"){
			nTakenCount++;
		}
	}
	
	if(torn == "at"){
		ans answer;
		answer.correct = takenCount;
		answer.predType = "Taken";
		output.push_back(answer);
	}
	else if(torn == "ant"){
		ans answer;
		answer.correct = nTakenCount;
		answer.predType = "Not Taken";
		output.push_back(answer);	
	}
	
}
void Predictor::biPrediction(int sizeOfTable, int singleOrDouble){
	unsigned long long correctP = 0;
	if(singleOrDouble == 1){
		//this sets all predictions to 1, or Taken
		vector<unsigned int> predCount(sizeOfTable,1);
		int tableKey;
		for(unsigned long long i = 0; i<info.size(); i++){
			tableKey = info[i].address % sizeOfTable;
			unsigned int compare;
			if(info[i].branchVal == "T"){ //t is 1
				compare = 1;
			}
			if(info[i].branchVal == "NT"){ //nt = 0
				compare = 0;
			}
			if(predCount[tableKey] == compare){
				correctP++;
			}
			else{
				predCount[tableKey] = compare;
			}
		}
		ans answer;
		answer.correct = correctP;
		answer.predType = "Bimodal Single Bit";
		output.push_back(answer);	
	}
	if(singleOrDouble == 2){
		//this sets all predictions to 3, or ST
		vector<unsigned int> predCount(sizeOfTable,3); 
		unsigned int compare;
		

		for(unsigned long long i = 0; i<info.size(); i++){
			if(info[i].branchVal == "T"){ //t is 1
					compare = 1;
				}
			if(info[i].branchVal == "NT"){ //nt = 0
				compare = 0;
			}
			int tableKey;
			tableKey = info[i].address % sizeOfTable; //an index for the least significant bit of the state
			unsigned int currentState = predCount[tableKey];
			//get the current state from the prediction counter	
			
			if((compare <<1) == (currentState & 2)){
				correctP++;
			}
			if(compare == 1){
				//not ST
				if(currentState!=3){
					currentState++;
				}
			}
			else { //not SNT
				if(currentState!=0){
					currentState--;
				}
			}
			predCount[tableKey] = currentState;
		}
		ans answer;
		answer.correct = correctP;
		answer.predType = "Bimodal Two Bit";
		output.push_back(answer);	

	}
}
/*

void Predictor::biPredDouble(int sizeTable){}
*/
