#include "Predictor.h"
using namespace std;
void Predictor::readAFile(string file){
	string behavior;
	unsigned long long addr;
	unsigned long long target;
	ifstream infile(file.c_str());
	string infoVal;
	int counter = 0;
	while(infile>>hex>>addr>>behavior>>hex>>target){
		branchTargetBuffer buf;
		buf.btbAddress = addr;
		numBranches++;
		myData data;
		data.address = addr;
		if(behavior == "T"){
			data.branchVal = "T";
			buf.btbBranchVal = "T";
		}
		else{
			data.branchVal = "NT";
			buf.btbBranchVal = "NT";
		}
		counter++;
		info.push_back(data);
		buff.push_back(buf);
	}
	infile.close();
}
void Predictor::writeAFile(string file){
	ofstream out(file.c_str());
	for(unsigned int i = 0; i<output.size();i++){
		out << output[i]<< "," << numBranches <<"; ";
		if(i==0||i==1||i==9||i==17||i==26||i==27)out<<"\n"; 
		cout << output[i]<< "," << numBranches <<"; ";
		if(i==0||i==1||i==9||i==17||i==26||i==27)cout<<"\n";
	}
	out.close();
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
		output.push_back(takenCount);
	}
	if(torn == "ant"){
		output.push_back(nTakenCount);	
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
		output.push_back(correctP);	
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
			//we are checking to see if
			int tableKey;
			tableKey = info[i].address % sizeOfTable; //an index for the least significant bit of the state
			unsigned int currentState = predCount[tableKey];
			//get the current state from the prediction counter	
			
			if((compare << 1) == (currentState & 2)){
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
		output.push_back(correctP);	

	}
}
void Predictor::gShare(int histBits){
	unsigned long long correctP = 0;
	unsigned int history = 0;
	vector<unsigned int> globalC(2048,3); //preset to ST with default size of 2048
	unsigned int compare; //compare is used because branch value is a string and must be compared with an int
	for(unsigned int i = 0; i < info.size(); i++){
		//PC is XOR-ed with the global history bits to 
		//generate the index (currrent) into the predictor table (gloabalC).
		//we shifted the history to keep track of T and NT, but must & the 
		//history with ((unsigned int)(-1 + pow(2,histBits))) so that we 
		//have the correct size before we XOR with address
		unsigned int updatedHistSize = history & ((unsigned int)(-1 + pow(2,histBits)));
		int current = (info[i].address ^ updatedHistSize) % 2048;
		unsigned int currentState = globalC[current];
		if(info[i].branchVal == "T"){ //t is 1
			compare = 1;
		}
		if(info[i].branchVal == "NT"){ //nt = 0
			compare = 0;
		}
		if((compare << 1) == (currentState &2)){
			correctP++;
		}
		if(compare == 1){
			if(currentState != 3){
				currentState++;
			}
		}else{
			if(currentState != 0){
				currentState--;
			}
		}
		//shift hist before or with current branchValue
		//to keep track of the history and current and then update.
		history = history << 1; 
		history = history | compare;
		globalC[current] = currentState;
	}
	output.push_back(correctP);
}

void Predictor::tournament(){
	unsigned long long correctP = 0;
	unsigned int history = 0;
	
	vector<unsigned int> bimodC(2048,3); //preset to ST with default size of 2048
	vector<unsigned int> tournamentC(2048,3); //preset to ST with default size of 2048
	vector<unsigned int> globalC(2048,3); //preset to ST with default size of 2048
	
	unsigned int compare; //compare is used because branch value is a string and must be compared with an int
	for(unsigned int i = 0; i < info.size(); i++){
	
		unsigned int updatedHistSize = history & ((unsigned int)(-1 + pow(2,11))); //11 bit hist
		unsigned long long infoAdd = info[i].address;
		
		int currentGshare = (infoAdd ^ updatedHistSize) % 2048;
		int currentTourn = infoAdd % 2048;
		int currentBimod = infoAdd % 2048;
		
		unsigned int currentStateGshare = globalC[currentGshare];
		unsigned int currentStateTourn = tournamentC[currentTourn];
		unsigned int currentStateBimod = bimodC[currentBimod];
		
		unsigned int bimodPred = currentStateBimod & 2;
		unsigned int gsharePred = currentStateGshare & 2;
		
		if(info[i].branchVal == "T"){ //t is 1
			compare = 1;
		}
		if(info[i].branchVal == "NT"){ //nt = 0
			compare = 0;
		}
		
		if(compare == 0){
			if(currentStateGshare != 0){
				currentStateGshare--;
			}
			if(currentStateBimod != 0){
				currentStateBimod--;
			}
			
		}else{
			if(currentStateGshare != 3){
				currentStateGshare++;
			}
			if(currentStateBimod != 3){
				currentStateBimod++;
			}
			
		}
		//shift hist before or with current branchValue
		//to keep track of the history and current and then update.
		history = history << 1; 
		history = history | compare;
		globalC[currentGshare] = currentStateGshare;
		bimodC[currentBimod] = currentStateBimod;
		
		if(bimodPred == gsharePred){ 
			/*If the two predictors provide the same prediction, then the
			corresponding selector counter remains the same.*/
			
			if((compare <<1) == bimodPred){
				correctP++;
			}
		}
		else{ 
			/*If one of the predictors is correct
			and the other one is wrong, then the selector’s counter is decremented or
			incremented to move towards the predictor that was correct.*/
			//gshare pred
			if((currentStateTourn & 2) == 2){ //10
				if((compare << 1) == gsharePred){
					correctP++;
					if(currentStateTourn != 3){
						currentStateTourn++;
					}
				}
				else{
					if(currentStateTourn != 0){
						currentStateTourn--;
					}
				}
			}
			else{
		 	//Bimod pred
				if((compare << 1) == bimodPred){
					correctP++;
					if(currentStateTourn != 0){
						currentStateTourn--;
					}
				}
				else{
					if(currentStateTourn != 3){
							currentStateTourn++;
					}
				}
			}
		}
		tournamentC[currentTourn] = currentStateTourn;
	}
	output.push_back(correctP);
}
void Predictor::BTB(){
	/*. In this part of the project, you are asked to model
	the Branch Target Buffer performance. Assume that the Branch Target Buffer is
	integrated with the Bimodal Predictor of size 512 entries, which is initialized as
	discussed in question (3). The size of the Branch Target Buffer is 128 entries. If a
	prediction is “taken”, then the target address is read from the BTB. Estimate the
	number of BTB accesses and BTB hits.*/

	vector<unsigned int> predCount(512,3); 
	vector<unsigned int> btbCount(128);
	unsigned int compare;
	unsigned int btbAccess;
	unsigned int btbHit;

		for(unsigned long long i = 0; i<info.size(); i++){
			unsigned long long j;
			j=i;
			if(j < 128){

				buff[j].btbAddress = info[i].address;
				buff[j].btbBranchVal = info[i].branchVal;
			}
			if(j == 128){
				j=0;
				buff[j].btbAddress = info[i].address;
				buff[j].btbBranchVal = info[i].branchVal;
			} 
			if(info[i].branchVal == "T"){ //t is 1
				compare = 1;
			}
			if(info[i].branchVal == "NT"){ //nt = 0
				compare = 0;
			}
			//we are checking to see if
			int tableKey;
			int btbTableKey;
			tableKey = info[i].address % 512; //an index for the least significant bit of the state
			btbTableKey = buff[j].btbAddress % 128;
			unsigned int currentState = predCount[tableKey];
			unsigned int btbCurrentState = btbCount[btbTableKey];
			//get the current state from the prediction counter	
			if((compare << 1) == (btbCurrentState & 2)){
				btbAccess++;
				btbHit++;
			}
			if(compare == 1){
				//not ST

				if(currentState!=3){
					currentState++;
					btbCurrentState++;
					btbAccess++;
				}
			}
			else { //not SNT
				if(currentState!=0){
					currentState--;
					btbCurrentState--;

				}
			}
			predCount[tableKey] = currentState;
			btbCount[btbTableKey] = btbCurrentState;
		}
		cout << btbAccess<< "," << btbHit <<"; "<<endl;
		//out << btbAccess<<"," << btbHit << "; ";
}

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
	//test gshare
	pred.gShare(3);
	pred.gShare(4);
	pred.gShare(5);
	pred.gShare(6);
	pred.gShare(7);
	pred.gShare(8);
	pred.gShare(9);
	pred.gShare(10);
	pred.gShare(11);
	//test tournament
	pred.tournament();
	pred.writeAFile(argv[2]);
	//pred.BTB();
	return 0;
}
