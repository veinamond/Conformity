#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>
using namespace std;

stringstream logstream;
int strtoi(string s){
	int x = atoi(s.c_str());
	return x;
}
int twoceil(int k){//returns closest upside degree of 2
	int t=1;
	while (t<k){t=t*2;}
	return t;
}
string inttostr(int number)
{
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}
class Conformity{
	
private:
	vector <int*> M;
	vector <int> Matrix;
	vector <int> WeightedMatrix;
	vector <int> Reachability;
	vector <int> conformitylevel;
	vector <int> conformism;
	vector <int> Curagit;
	vector <int> Curloyal;
	stringstream Mstream;
	long int nVars;
	long int nClauses;
	int dimension;
	int nofsteps;

public:	
	::Conformity(){
		nVars=0;
		M.clear();
		Matrix.clear();
		}
	~Conformity(){
		nVars=0;
		nClauses=0;
		M.clear();
		Matrix.clear();
		conformitylevel.clear();
		conformism.clear();
		Mstream.clear();//clear any bits set
		Mstream.str(std::string());

	}
	vector<int> Sort(vector<int> a, int nV, int n){
	//nVars=nV;
	return HSort(a,n);
	}
	void GNPgraph(double p, int n);
	int initializeconformity (double percent, double prob);
	vector <int> Row (int i);
	vector <int> Column (int j);
	int matrixelement(int i, int j){
		return Matrix[i*dimension+j];
	}
	void restrictagit(){
		for (int i=0;i<Curagit.size();i++){
			if (Curagit[i]==1){Mstream	<<	i+1	<<" 0"<<endl;}
			else {Mstream << - (i+1) <<" 0"<<endl;}
		}
	}
	void restrictloyal(){
		for (int i=0;i<Curloyal.size();i++){
			if (Curloyal[i]==1){Mstream	<<dimension+i+1	<<" 0"<<endl;}
			else {Mstream << - (dimension+i+1) <<" 0"<<endl;}
		}
	}
	void construct_reachability_matrix (int radius);
	void construct_weights_matrix (int radius);
	vector <int> HMerge (vector<int> &a, vector<int> &b, int n);
	vector <int> HSort (vector<int>& a, int n);
	vector <int> SHSort (vector<int> a, int lown, int n);
	vector <int> SMerge (vector<int> a, vector <int> b, int n);
	vector <int> Selectodd(vector<int> a);
	vector <int> Selecteven(vector<int> a);
	bool calculate(vector<int> inp, int stepm);
	bool calculate(vector<int> inp, int stepm, string filename);
	void loadmatrixfromfile(int n, const char * filename);
	void savematrixtofile(const char * filename);
	void savematrixtofile_gv(const char * filename);
	void seqcounter(vector<int> a);
	void onlyonesimple(vector<int> t);
	void onlyonecomplex(vector<int> t);
	void Newvareqor(int nv, vector<int> right);	
	void printprogresstofile_gv(vector<int>agitators, vector<int> loyalists, vector<int> activity, const char *filename);
	void equalize(vector<int>r1, vector<int>r2);
	vector<int> vecnewvareqand(const vector<int> r1, const vector<int>r2);
	vector<int> construct_neighbourhood(int i);
	vector<int> vecnewvareqor(const vector<int> r1, const vector<int>r2);
	void Newvareqand(int nv, vector<int> right);
	void Print (const char * fn);
	void Printmatrix();
	void functioning( int step, bool agitated);
	void generalfunctioning( int step, bool agitated, bool loyaled, bool strict);
	void Notmoreatstart(int n, bool agitators, bool loyalists);
	void Notlessatend(int n);
	void Notmoreatend(int n);
	void Fixedpoint();
	void Dump (char * fn);
	vector<int> loadssfromfile(const char * filename);	
};
vector<int> Conformity::loadssfromfile(const char *filename){
	ifstream myfile;
	vector<int> a;
	myfile.open(filename);
	string s;
	while ( myfile.good() ){
    getline (myfile,s);
	if (s=="INDET") break;
		if ((s.find("SAT")==string::npos)&&(s.length()>4)){
			int k=0;
			int ks=0;
			int ke=0;
			bool b=true;
			while (b==true){
				if ((s[k]==' ')||(k==s.length()-1)){
					if (k==s.length()-1) {
						cout<<"THIS";
						b=false;
					}
					ke=k;
					string t=s.substr(ks,ke-ks);
					int r=strtoi(t);
					a.push_back(r);
					ks=ke;
				}
				k++;
				}	
	   }
	}
	myfile.close();
	return a;
}
void Conformity::loadmatrixfromfile(int n, const char * filename){
	Matrix.clear();
	M.clear();
	dimension=n;
	nVars=n;
	ifstream myfile;
	myfile.open(filename);
	string s;
	bool isconformity=false;
	bool isconformism=false;
	for (int t=0;t<n+4;t++){
		  getline (myfile,s);
		  if (s.find("Conformity level")!=string::npos){
			 isconformity=true; 
		  }
		  else if (s.find("Conformism")!=string::npos){
			  isconformity=false; 	  
			  isconformism=true; 
		  }
		  else {
			int r1=0;
			int r2=0;
			for (int i=0;i<s.length();i++){

				if ((s[i]==' ')||(s[i]=='\n')){
					r2=i;
					if ((r2-r1)>0){
						string tmp=s.substr(r1,r2-r1);
						int t=strtoi(tmp);
						if (!isconformity&&!isconformism) {
							Matrix.push_back(t);	
						}
						else if (isconformity){
							conformitylevel.push_back(t);
						}
						else if (isconformism){
							conformism.push_back(t);
						}
						r1=r2;
					}
				}

			}
		  }
		}		
	if (Matrix.size()!=dimension*dimension){cout<<endl<<"Matrix size"<<Matrix.size()<<" is wrong "<<endl; }	  
	myfile.close();
}
void Conformity::equalize(vector<int>r1, vector<int>r2){
	if (r1.size()!=r2.size()) exit;
	for (int i=0;i<r1.size();i++){
	Mstream<<-r1[i]<<" "<<r2[i]<<" 0"<<endl;
	Mstream<<r1[i]<<" "<<-r2[i]<<" 0"<<endl;
	nClauses+=2;
	}
}
void Conformity::savematrixtofile(const char* filename){
	ofstream myfile;
	myfile.open(filename);
	string s;
	for (int i=0;i<dimension;i++){
		for (int j=0;j<dimension;j++){
			myfile<<matrixelement(i,j)<<" ";
		}
		myfile<<endl;
	}
	myfile<<"Conformity level"<<endl;
	for (int i=0;i<dimension;i++){
		myfile<<conformitylevel[i]<<" ";
	}
	myfile<<endl;
	myfile<<"Conformism"<<endl;
	for (int i=0;i<dimension;i++){
		myfile<<conformism[i]<<" ";
	}
	myfile.close();
}

void Conformity::savematrixtofile_gv(const char* filename){
	ofstream out;
	out.open(filename);
	out<<"digraph ppp {"<<endl;
	out<<"\trankdir=LR;"<<endl;
	out<<"\tnode [shape=circle];"<<endl;
	for (int i=0;i<dimension;i++){
		for (int j=0;j<dimension;j++){
			if (matrixelement(i,j)>0){
				out<<"\t"<<"v_"<<i+1<<" -> v_"<<j+1<<" [ label=\"\",";
				switch (Matrix[i*dimension+j]){
				case 1: out<<"color=\"black\",";break;
				case 2: out<<"color=\"black\",";break;
				case 3: out<<"color=\"black\", style=dashed, ";break;
				}
				out<<"arrowhead=\"normal\" ];"<<endl;
			}
		}		
	}
	out<<"}"<<endl;
	out.close();
}
void Conformity::printprogresstofile_gv(vector<int>agitators, vector<int> loyalists, vector<int> activity, const char *filename){
	ofstream out;
	out.open(filename);
	double radius = 5;
	double M_PI = 3.14159265358979323846;
	vector<int> curinput;
	bool hasagit = false;
	bool hasloyal = false;
	bool hasactive = false;
	bool hasinactive = false;
	for (int i = 0; i < agitators.size(); i++){
		if (agitators[i] == 1)hasagit = true;
		if (loyalists[i] == 1)hasloyal = true;
		if ((activity[i] == 1)&&(agitators[i]==0))hasactive = true;
		if ((activity[i] == 0) && (loyalists[i] == 0))hasinactive = true;
	}
	for (int i = 0; i < dimension; i++){
		int cursum = 0;
		for (int j = 0; j < dimension; j++){
			if ((matrixelement(i, j) == 1) && (activity[j] == 1)) cursum++;
		}
		if (agitators[i] == 1){ curinput.push_back(0); }
		if (loyalists[i] == 1){ curinput.push_back(dimension); }
		if ((agitators[i] == 0) && (loyalists[i] == 0)){
			curinput.push_back(cursum);
		}
	}

	out << "digraph ppp {" << endl;
	out << "\trankdir=LR;" << endl;
	out << "\tnode [shape=plaintext];";
	for (int i = 0; i < activity.size(); i++)
	{
	out << " info_" << i + 1;
	}
	out << ";" << endl;
	if (hasinactive){//inactive
		//out << "\tnode [shape=circle, fillcolor=lightskyblue, style=\"filled\"];";
		out << "\tnode [shape=circle];";
		for (int i = 0; i < activity.size(); i++)
		{
			if ((activity[i] == 0) && (loyalists[i] == 0)){ out << " v_" << i + 1; }
		}
		out << ";" << endl;
	}
	if (hasloyal){//loyalists
		out << "\tnode [shape=circle, fillcolor=lawngreen, style=\"filled\"];";
		for (int i = 0; i < activity.size(); i++)
		{
			if (loyalists[i] == 1){ out << " v_" << i + 1; }
		}
		out << ";" << endl;
	}
	if (hasactive){
		//out << "\tnode [shape=circle, fillcolor=lightsalmon, style=\"filled\"];";
		out << "\tnode [shape=doublecircle];";
		for (int i = 0; i < activity.size(); i++)
		{
			if ((activity[i] == 1) && (agitators[i] == 0)){ out << " v_" << i + 1; }
		}
		out << ";" << endl;
	}
	if (hasagit){
		//out << "\tnode [shape=circle, fillcolor=crimson, style=\"filled\"];";
		out << "\tnode [shape=diamond];";
		for (int i = 0; i < activity.size(); i++)
		{
			if (agitators[i] == 1){ out << " v_" << i + 1; }
		}
		out << ";" << endl;
	}
	
	for (int i = 0; i < dimension; i++){

		double x = cos((M_PI * 2)*i / dimension)*radius;
		double y = sin((M_PI * 2)*i/ dimension)*radius;
		if (agitators[i] == 1){
			out << "\tv_" << i + 1 << "[label=\"v_" << i + 1 << "\", pos=\"" << x << "," << y << "!\", shape = \"diamond\"];" << endl;
		}
		else if (activity[i] == 1){
			out << "\tv_" << i + 1 << "[label=\"v_" << i + 1 << "\", pos=\"" << x << "," << y << "!\", shape = \"doublecircle\"];" << endl;
		}
		else { out << "\tv_" << i + 1 << "[label=\"v_" << i + 1 << "\", pos=\"" << x << "," << y << "!\", shape = \"circle\"];" << endl; }
		

		if ((agitators[i] == 0) && (loyalists[i] == 0)) { out << "\tinfo_" << i + 1 << "[label=\"" << curinput[i] << "(" << conformitylevel[i] << ")" << "\", pos=\"" << x*1.15 << "," << y*1.15 << "!\", shape = \"plaintext\"];" << endl; }
		else {
			if (agitators[i] == 1){ out << "\tinfo_" << i + 1 << "[label=\"" << "A" << "\", pos=\"" << x*1.15 << "," << y*1.15 << "!\", shape = \"plaintext\"];" << endl; }
		else 
		if (loyalists[i] == 1){ out << "\tinfo_" << i + 1 << "[label=\"" << "L" << "\", pos=\"" << x*1.15 << "," << y*1.15 << "!\", shape = \"plaintext\"];" << endl; }

		}
	}
	

	for (int i = 0; i<dimension; i++){
		for (int j = 0; j<dimension; j++){
			if (matrixelement(i, j)>0){
				out << "\t" << "v_" << j + 1 << " -> v_" << i + 1 << " [ label=\"\",";
				switch (activity[j]){
				//case 0: out << "color=\"green\","; break;
				//case 1: out << "color=\"red\","; break;
				case 0: out << "style=\"dashed\","; break;
				case 1: out << "style=\"solid\","; break;
				}
				out << "arrowhead=\"normal\" ];" << endl;
			}
		}
	}
	out << "}" << endl;
	
	out.close();
}
void Conformity::construct_reachability_matrix (int radius){
	vector<int> t(Matrix);
	for (int r=0;r<radius;r++){
		for (int i=0;i<dimension;i++){
			for (int j=0;j<dimension;j++){
				if (t[i*dimension+j]!=0){//if a_ij>0 & a_jh>0 (+some additional conditions) -> a_ih=a_ij+a_jh
					for (int h=0;h<dimension;h++){						
						if (i!=h){
							if ((t[j*dimension+h]>0)&&((t[i*dimension+h]==0)||(t[i*dimension+j]+t[j*dimension+h]<t[i*dimension+h]))){
								t[i*dimension+h]=t[i*dimension+j]+t[j*dimension+h];
			//					cout<<endl <<"Route "<<i+1<<" -> "<<j+1<< " -> "<< h+1<<endl;
							}
						}
						//if we can reduce the neighbourhood characteristic - just do it
					}
				}
			}
		}

		cout<<endl<<"reachability matrix step "<<r<<endl;
		for (int i=0;i<dimension;i++){
			for (int j=0;j<dimension;j++){
				cout<<t[i*dimension+j]<<" ";					
			}
			cout<<endl;
		}
	}	
	Reachability=t;
}
void Conformity::construct_weights_matrix (int radius){
	if (Reachability.size()==0)exit;
	vector<int> t;
	for(int i=0;i<Reachability.size();i++){
		if (Reachability[i]==0) {
			t.push_back(0);
		}
		else {
			if ((radius - Reachability[i]+1)>0){
				t.push_back(radius-Reachability[i]+1);
			}
			else{t.push_back(0);}
		}		
	}	
	cout<<endl<<"Weights matrix "<<endl;
	for (int i=0;i<dimension;i++){
		for (int j=0;j<dimension;j++){
			cout<<t[i*dimension+j]<<" ";
		}
		cout<<endl;
	}
	WeightedMatrix=t;
}
vector<int> Conformity::construct_neighbourhood(int i){
	vector<int> res;
	for (int j=0;j<dimension;j++){
			for (int h=0;h<WeightedMatrix[j*dimension+i];h++){
				res.push_back(j);
			}
		}
	return res;	
}
bool Conformity::calculate(vector<int> inp, int step){
	vector<int> agitators;
	vector<int> loyalists;
	vector<int> curstep;
	vector<int> nextstep;
	bool a=true;
	for (int i=0;i<dimension;i++){
		agitators.push_back(inp[i]);
		loyalists.push_back(inp[dimension+i]);
		curstep.push_back(inp[dimension*2+i]);
	}
	cout<<endl<<"Agitators"<<endl;
	logstream<<endl<<"Agitators"<<endl;
	for (int d=0;d<dimension;d++){
		cout<<agitators[d]<<" ";
		logstream<<agitators[d]<<" ";
	}
	cout<<endl<<"loyalists"<<endl;
	logstream<<endl<<"loyalists"<<endl;
	for (int d=0;d<dimension;d++){
		cout<<loyalists[d]<<" ";
		logstream<<loyalists[d]<<" ";
	}
	for (int i=0;i<dimension;i++){
		Curagit.push_back(agitators[i]);
		Curloyal.push_back(loyalists[i]);
	}
	//curstep=inp;
	cout<<endl<<"Step 0"<<endl;
	logstream<<endl<<"Step 0"<<endl;
	for (int d=0;d<dimension;d++){
		cout<<curstep[d]<<" ";
		logstream<<curstep[d]<<" ";
		
	}
	logstream<<endl;
	cout<<endl;
	for (int k=0;k<step;k++){
			nextstep.clear();
				for (int i=0;i<dimension;i++){
				int cursum=0;
				int deg=0;
				for (int j=0;j<dimension;j++){
					deg+=matrixelement(i,j);//WTF
				}
				for (int j=0;j<dimension;j++){
					if ((matrixelement(i,j)==1)&&(curstep[j]==1)) cursum++;
				}
				if (conformism[i]==1){	
					if (agitators[i]==1){nextstep.push_back(1);}
					else{
						if (loyalists[i]==1){
						nextstep.push_back(0);
						}
						else
						{
							if (cursum>=conformitylevel[i]){nextstep.push_back(1);}
							else {nextstep.push_back(0);}
						}
					}
				}
				else {	
					if (agitators[i]==1){nextstep.push_back(1);}
						else{
							if (loyalists[i]==1){
								nextstep.push_back(0);
							}
							else {
								if (cursum<=deg-conformitylevel[i]){nextstep.push_back(1);}
								else {nextstep.push_back(0);}
							}
					}
				}
			}
	
			curstep=nextstep;
			bool b=true;
			for (int i=0;i<dimension;i++){
				if (curstep[i]!=inp[(2+k+1)*dimension+i]){
					cout<<endl<<"Correctness on step "<<k+1<< " point  "<<i<<" is failed"<<endl;
					logstream<<endl<<"Correctness on step "<<k+1<< " point  "<<i<<" is failed"<<endl;
					b=false;
					a=false;
				}
			}
	
			cout<<endl<<"Step "<<k+1<<endl;
			logstream<<endl<<"Step "<<k+1<<endl;
			for (int d=0;d<dimension;d++){
				cout<<curstep[d]<<" ";
				logstream<<curstep[d]<<" ";
			}
			cout<<endl;
			logstream<<endl;
			if (b==true){
				cout<<endl<<"Step "<<k+1<<" is correct"<<endl;
				logstream<<endl<<"Step "<<k+1<<" is correct"<<endl;
			}
		}
	return a;
}
bool Conformity::calculate(vector<int> inp, int step, string filename){
	vector<int> agitators;
	vector<int> loyalists;
	vector<int> curstep;
	vector<int> nextstep;
	string tmpprogress=filename;
	bool a = true;
	for (int i = 0; i<dimension; i++){
		agitators.push_back(inp[i]);
		loyalists.push_back(inp[dimension + i]);
		curstep.push_back(inp[dimension * 2 + i]);
	}
	cout << endl << "Agitators" << endl;
	logstream << endl << "Agitators" << endl;
	for (int d = 0; d<dimension; d++){
		cout << agitators[d] << " ";
		logstream << agitators[d] << " ";
	}
	cout << endl << "loyalists" << endl;
	logstream << endl << "loyalists" << endl;
	for (int d = 0; d<dimension; d++){
		cout << loyalists[d] << " ";
		logstream << loyalists[d] << " ";
	}
	for (int i = 0; i<dimension; i++){
		Curagit.push_back(agitators[i]);
		Curloyal.push_back(loyalists[i]);
	}
	//curstep=inp;
	cout << endl << "Step 0" << endl;
	logstream << endl << "Step 0" << endl;
	for (int d = 0; d<dimension; d++){
		cout << curstep[d] << " ";
		logstream << curstep[d] << " ";
		string tmp = tmpprogress + "_0.txt";
		printprogresstofile_gv(agitators, loyalists, curstep, tmp.c_str());
	}
	logstream << endl;
	cout << endl;
	for (int k = 0; k<step; k++){
		nextstep.clear();
		for (int i = 0; i<dimension; i++){
			int cursum = 0;
			int deg = 0;
			for (int j = 0; j<dimension; j++){
				deg += matrixelement(i, j);
			}
			for (int j = 0; j<dimension; j++){
				if ((matrixelement(i, j) == 1) && (curstep[j] == 1)) cursum++;
				//if ((matrixelement(i, j) == 1) && (curstep[i] == 1)) cursum++;
			}
			if (conformism[i] == 1){
				if (agitators[i] == 1){ nextstep.push_back(1); }
				else{
					if (loyalists[i] == 1){
						nextstep.push_back(0);
					}
					else
					{
						if (cursum >= conformitylevel[i]){ nextstep.push_back(1); }
						else { nextstep.push_back(0); }
					}
				}
			}
			else {
				if (agitators[i] == 1){ nextstep.push_back(1); }
				else{
					if (loyalists[i] == 1){
						nextstep.push_back(0);
					}
					else {
						if (cursum <= deg - conformitylevel[i]){ nextstep.push_back(1); }
						else { nextstep.push_back(0); }
					}
				}
			}
		}

		curstep = nextstep;
		bool b = true;
		for (int i = 0; i<dimension; i++){
			if (curstep[i] != inp[(2 + k + 1)*dimension + i]){
				cout << endl << "Correctness on step " << k + 1 << " point  " << i << " is failed" << endl;
				logstream << endl << "Correctness on step " << k + 1 << " point  " << i << " is failed" << endl;
				b = false;
				a = false;
			}
		}
		string tmp = tmpprogress + "_"+inttostr(k+1)+".txt";
		printprogresstofile_gv(agitators, loyalists, curstep, tmp.c_str());

		cout << endl << "Step " << k + 1 << endl;
		logstream << endl << "Step " << k + 1 << endl;
		for (int d = 0; d<dimension; d++){
			cout << curstep[d] << " ";
			logstream << curstep[d] << " ";
		}
		cout << endl;
		logstream << endl;
		if (b == true){
			cout << endl << "Step " << k + 1 << " is correct" << endl;
			logstream << endl << "Step " << k + 1 << " is correct" << endl;
		}
	}
	return a;
}
void Conformity::Notmoreatstart(int n, bool agitators, bool loyalists){
	
	const int nzero=++nVars; //zero variable
	int * tmp;
	tmp=new(int[2]);
	tmp[0]=-nzero;
	tmp[1]=0;
	Mstream<<tmp[0]<<" "<<tmp[1]<<endl;
	nClauses++;
	//M.push_back(tmp);
	vector<int> tobesorted;
	for (int i=0;i<dimension;i++){
		if (!agitators&&!loyalists){
		tobesorted.push_back(dimension*2+i+1);
		}
		else if (agitators) {
			tobesorted.push_back(i+1);
		}
		else if (loyalists) {
			tobesorted.push_back(dimension+i+1);
		}
	}
	int hcnt=twoceil(tobesorted.size());
	for (int i=tobesorted.size();i<hcnt;i++){tobesorted.push_back(nzero);}
	vector<int> r;
	r=HSort(tobesorted,hcnt);
	tmp=new (int[2]);
	tmp[0]=-r[n];
	tmp[1]=0;
	//M.push_back(tmp);
	Mstream<<tmp[0]<<" "<<tmp[1]<<endl;
	nClauses++;
}
void Conformity::Notlessatend(int n){
	vector<int> tobesorted;
	const int nzero=++nVars; //zero variable
	int * tmp;
	tmp=new(int[2]);
	tmp[0]=-nzero;
	tmp[1]=0;
	//M.push_back(tmp);
	Mstream<<tmp[0]<<" "<<tmp[1]<<endl;
	nClauses++;
	for (int i=0;i<dimension;i++){
	tobesorted.push_back(dimension*(nofsteps+2)+i+1);
	}
	int hcnt=twoceil(tobesorted.size());
	for (int i=tobesorted.size();i<hcnt;i++){tobesorted.push_back(nzero);}
	vector<int> r;
	r=HSort(tobesorted,hcnt);
	tmp=new(int[2]);
	tmp[0]=r[n];
	tmp[1]=0;
	//M.push_back(tmp);
	Mstream<<tmp[0]<<" "<<tmp[1]<<endl;
	nClauses++;
}
void Conformity::Notmoreatend(int n){
	vector<int> tobesorted;
	const int nzero=++nVars; //zero variable
	int * tmp;
	tmp=new(int[2]);
	tmp[0]=-nzero;
	tmp[1]=0;
	//M.push_back(tmp);
	Mstream<<tmp[0]<<" "<<tmp[1]<<endl;
	nClauses++;
	for (int i=0;i<dimension;i++){
	tobesorted.push_back(dimension*(nofsteps+2)+i+1);
	}
	int hcnt=twoceil(tobesorted.size());
	for (int i=tobesorted.size();i<hcnt;i++){tobesorted.push_back(nzero);}
	vector<int> r;
	r=HSort(tobesorted,hcnt);
	tmp=new(int[2]);
	tmp[0]=-r[n];
	tmp[1]=0;
	//M.push_back(tmp);
	Mstream<<tmp[0]<<" "<<tmp[1]<<endl;
	nClauses++;
}
void Conformity::Fixedpoint(){
	vector<int> first;
	vector<int> last;
	for (int i=0;i<dimension;i++){
		first.push_back(i+1+2*dimension);
		last.push_back(i+1+3*dimension);	
	}
	for (int i=0;i<dimension;i++){
		Mstream<<first[i]<<" "<<-last[i]<<" 0"<<endl;
		Mstream<<-first[i]<<" "<<last[i]<<" 0"<<endl;
		nClauses+=2;
	}
}
void Conformity::Printmatrix(){
	cout<<endl<<"Graph matrix"<<endl;
	for (int i=0;i<dimension;i++){
		for (int j=0;j<dimension;j++){
			cout<<matrixelement(i,j)<<" ";
			logstream<<matrixelement(i,j)<<" ";
		}
		logstream<<endl;
		cout<<endl;
	}
	cout<<endl;
	logstream<<endl;
	cout<<"Conformity level"<<endl;
	logstream<<"Conformity level"<<endl;
	for (int i=0;i<dimension;i++){
		logstream<<conformitylevel[i]<<" ";
		cout<<conformitylevel[i]<<" ";
	}
	cout<<endl<<"Conformism"<<endl;
	logstream<<endl<<"Conformism"<<endl;	
	for (int i=0;i<dimension;i++){
		cout<<conformism[i]<<" ";
		logstream<<conformism[i]<<" ";
	}
	logstream<<endl;
	cout<<endl;
}
void Conformity::GNPgraph(double p, int n){
	dimension=n;
	nVars=n;
	nClauses=0;
	for (int i=0;i<n;i++){
	for (int j=0;j<n;j++){
	Matrix.push_back(0);
	}	
	}
	int RM=RAND_MAX;
	srand (time(NULL));
	for (int i=0;i<n;i++){
	for (int j=0;j<n;j++){
	//for (int j=i+1;jn;j++){
	if ((i!=j)&&(rand()<(p*RM))){Matrix[i*n+j]=1;} 	
	}
	}
}
int Conformity::initializeconformity (double percent, double prob){
 	int conf_percent =percent*dimension;
	int RM=RAND_MAX;
	srand (time(NULL));
	for (int i=0;i<dimension;i++){
	int t=0;
	for (int j=0;j<dimension;j++){
	t+=matrixelement(i,j);
	}
	if (conf_percent==0){
		conformitylevel.push_back(rand()*t/RM);		
		if (conformitylevel[i]==0){conformitylevel[i]=1;}
		cout<<"degree = "<<t<<", CLevel= "<<conformitylevel[i]<<endl;		
	}
	else {conformitylevel.push_back(t*percent+1);}
	//conformitylevel.push_back(rand()*t/RM+1);
	//	conformism.push_back(rand()%2);
	
	if (rand()<(prob*RM)){conformism.push_back(1);} else {conformism.push_back(0);}
	}
	return 1;
}
vector <int> Conformity::Row (int i){
	vector<int> res;
	for (int k=0;k<dimension;k++){
	res.push_back(dimension*i+k+1);
	}
	return res;
}
vector <int> Conformity::Column (int j){
	vector<int> res;
	for (int k=0;k<dimension;k++){
	res.push_back(j+dimension*k+1);
	}
	return res;
}
void Conformity::Newvareqor(int nv, vector<int>right){//not modified for stringstream
	int len=right.size();
	int *a;
	a=new(int[len+2]);
	a[0]=-nv;
	Mstream<<a[0]<<" ";
	for (int i=0;i<len;i++){
	a[i+1]=right[i];
	Mstream<<a[i+1]<<" ";
	}
	Mstream<<"0"<<endl;
	a[len+1]=0;	
	//M.push_back(a);
	nClauses++;
	for (int i=0;i<len;i++){
	a=new (int[3]);
	a[0]=nv;
	a[1]=-right[i];
	a[2]=0;
	Mstream<<a[0]<<" "<<a[1]<<" 0"<<endl;
	//M.push_back(a);
	nClauses++;
	}
}
vector<int> Conformity::vecnewvareqor(const vector<int> r1, const vector<int>r2){
	vector<int> nv;
	if (r1.size()!=r2.size()) exit;
	for (int i=0;i<r1.size();i++){
	nv.push_back(++nVars);
	}	
	for (int i=0;i<nv.size();i++){
	vector<int> tmp;
	tmp.push_back(r1[i]);
	tmp.push_back(r2[i]);
	Newvareqor(nv[i],tmp);
	}
	return nv;
}
void Conformity::Newvareqand(int nv, vector<int>right){
	int len=right.size();
	int *a;
	a=new(int[len+2]);
	a[0]=nv;
	Mstream<<a[0]<<" ";
	for (int i=0;i<len;i++){
	a[i+1]=-right[i];
	Mstream <<a[i+1]<<" ";
	}
	a[len+1]=0;
	Mstream<<"0"<<endl;
	//M.push_back(a);
	nClauses++;
	for (int i=0;i<len;i++){
	a=new (int[3]);
	a[0]=-nv;
	a[1]=right[i];
	a[2]=0;
	Mstream<<a[0]<<" "<<a[1]<<" 0"<<endl;
	//M.push_back(a);
	nClauses++;
	}
}
vector<int> Conformity::vecnewvareqand (const vector<int> r1, const vector<int> r2){//not modified for stringstream
 	vector<int> nv;
	if (r1.size()!=r2.size()) exit;
	for (int i=0;i<r1.size();i++){
	nv.push_back(++nVars);
	}	
	for (int i=0;i<nv.size();i++){
	vector<int> tmp;
	tmp.push_back(r1[i]);
	tmp.push_back(r2[i]);
	Newvareqand(nv[i],tmp);
	}
	return nv;
}
void Conformity::onlyonesimple(vector<int> t){//not modified for stringstream
	int len=t.size();
	for (int i=0;i<len;i++){
	for (int j=i+1;j<len;j++){
	int *a;
	a=new(int[3]);
	a[0]=-t[i];
	a[1]=-t[j];
	a[2]=0;
	Mstream<<a[0]<<" " <<a[1]<<" 0"<<endl;
	//M.push_back(a);
	}
	}
}
void Conformity::onlyonecomplex(vector<int> t){//not modified for stringstream
	int len=t.size();
	vector<int>nv;
	for(int i=0;i<len;i++){
	vector<int>r;
	for (int j=0;j<len;j++){
	if (j!=i) r.push_back(-t[j]);
	}
	nv.push_back(++nVars);
	Newvareqand(nVars,r);//+m+2 дизъюнкта
	r.clear();
	}
	//в итоге m*(m+2) дизъюнктов
	for (int i=0;i<len;i++){
	int * a;
	a=new(int[3]);
	a[0]=-t[i];
	a[1]=nv[i];
	a[2]=0;
	Mstream<<a[0]<<" " <<a[1]<<" 0"<<endl;
	//M.push_back(a);
	}
}

void Conformity::generalfunctioning( int step, bool agitated, bool loyaled, bool strict){
	Mstream.clear();
	nofsteps=step;
	nVars = (step+3)*dimension; // reserve first variables to be something meaningful
	const int nzero=++nVars; //zero variable
	Mstream<<-nzero<<" 0"<<endl;
	nClauses++;
	vector<int> agitators;
	vector<int> loyalists;
	vector<int> firststep;
	for (int i=0;i<dimension;i++){
		agitators.push_back(i+1);
		loyalists.push_back(-(dimension+i+1));
		firststep.push_back(dimension*2+i+1);
	}
	vector<int> curstep_vars;
	//zero step = initialization required if agitated or loyaled !=0
	//we also need to restrict simultaneous agitation and loyalization-_-
	for (int i=0;i<dimension;i++){
	if (agitated && loyaled ) {	Mstream<<-agitators[i]<<" "<<loyalists[i]<<" 0"<<endl; } //restriction
	if (agitated ){
		Mstream<<-agitators[i]<<" "<<firststep[i]<<" 0"<<endl;
		if (strict) {Mstream<<agitators[i]<<" "<<-firststep[i]<<" 0"<<endl;}
	}// if agitated then 1
	if (loyaled ){
		Mstream<<loyalists[i]<<" "<<-firststep[i]<<" 0"<<endl;
		if (!strict) {				
			Mstream<<-loyalists[i]<<" "<<firststep[i]<<" 0"<<endl;
		}
		//if (strict){Mstream<<-loyalists[i]<<" "<<firststep[i]<<" 0"<<endl;}
	}// if loyaled then 0
	}
 	for (int k=0;k<step;k++){//steps
		curstep_vars.clear();
		for (int i=0;i<dimension;i++){//vertices
			//build the neighbours list for the i-th vertex;
			vector<int> neighbours;
			for (int j=0;j<dimension;j++){
				//wrong order. Adjacency matrix is constructed vice versa
				if (matrixelement(i,j)==1){neighbours.push_back(j);}	
			}
			//count "1s" in this list
			//to do this we should add to the list variables (meaningless) that are equal to 0s from the start
			// for sorting nets technique to be sound and correct
			int neighbourssize=neighbours.size();	
			int hcnt=twoceil(neighbours.size());
			vector<int> tobesorted;
			for (int v=0;v<neighbours.size();v++){tobesorted.push_back((k+2)*dimension+neighbours[v]+1);}
			for (int v=neighbours.size();v<hcnt;v++){tobesorted.push_back(nzero);}
			if (tobesorted.size()==0){cout<<endl<<"ALARM ALARM ALARM!!!"<<endl;}
				vector<int> sorted;
				if (tobesorted.size()>1){
				sorted=HSort(tobesorted,hcnt);	
			}
			else 
				{
					sorted=tobesorted;
				}
			//if (sorted.size()<=conformitylevel[i]){
			//	cout<<endl<<"Sorted size "<<sorted.size()<<" is leq than conf_level "<<conformitylevel[i]<<" for "<<i<<"-th vertex!!! "<<endl;
			//	}
			//here conformism starts to mean something
			// if vertex is conformist (conformism[i]=1) then it does what all do
			// otherwise it does just the opposite, meaning that if the majority makes decision to do a Thing
			// this vertex does not.
			int newvarnum=0;
			if (conformism[i]==1){	
				newvarnum=sorted[conformitylevel[i]-1];
			}
			else {
				if (neighbourssize<=conformitylevel[i]){cout<<"PROBLEM HERE "<<endl;}
				newvarnum=-sorted[neighbourssize-conformitylevel[i]];
			}
			curstep_vars.push_back(newvarnum);
			neighbours.clear();
			sorted.clear();
			sorted.~vector();
			tobesorted.clear();
			tobesorted.~vector();
			/*if (agitated==false){
			Mstream<<dimension*(k+1)+i+1<<" "<<-newvarnum<<" 0"<<endl;
			nClauses++;
			Mstream<<-(dimension*(k+1)+i+1)<<" "<<newvarnum<<" 0"<<endl;
			nClauses++;
			}
			else {
			Mstream<<-(dimension*(k+1)+i+1)<<" "<<newvarnum<<" "<<i+1<<" 0"<<endl;
			nClauses++;
			Mstream<<dimension*(k+1)+i+1<<" "<<-newvarnum<<" 0"<<endl;
			nClauses++;
			Mstream<<dimension*(k+1)+i+1<<" "<<-(i+1)<<" 0"<<endl;
			nClauses++;	
			}	*/	
		}	
		vector<int>newstep;
		for (int l=0;l<dimension;l++){
			newstep.push_back(dimension*(k+1+2)+l+1);
		}
		if (!agitated&&!loyaled){
			equalize(newstep,curstep_vars);
		}
		else 
		if (agitated&&!loyaled){
		vector<int>tmpvars=vecnewvareqor(agitators,curstep_vars);
			equalize(newstep,tmpvars);
		}
		else 
		if (!agitated&&loyaled){
			vector<int> tmpvars2=vecnewvareqand(loyalists,curstep_vars);
			equalize(newstep,tmpvars2);
		}
		else
			if (agitated&&loyaled){
				vector<int>tmpvars=vecnewvareqor(agitators,curstep_vars);
				vector<int> tmpvars2=vecnewvareqand(loyalists,tmpvars);
				equalize(newstep,tmpvars2);
			}	
	}
}
void Conformity::functioning(int step, bool agitated){
	nofsteps=step;
	nVars = (step+1)*dimension; // reserve first variables to be something meaningful
	const int nzero=++nVars; //zero variable
	Mstream<<-nzero<<" 0"<<endl;
	nClauses++;
	for (int k=0;k<step;k++){//steps
	for (int i=0;i<dimension;i++){//vertices
	//build the neighbours list for the i-th vertex;
	vector<int> neighbours;
	for (int j=0;j<dimension;j++){
	if (matrixelement(i,j)==1){neighbours.push_back(j);}	
	}
	//count "1s" in this list
	//to do this we should add to the list variables (meaningless) that are equal to 0s from the start
	// for sorting nets technique to be sound and correct
	int neighbourssize=neighbours.size();
	int hcnt=twoceil(neighbours.size());
	vector<int> tobesorted;
	for (int v=0;v<neighbours.size();v++){tobesorted.push_back(k*dimension+neighbours[v]+1);}
	for (int v=neighbours.size();v<hcnt;v++){tobesorted.push_back(nzero);}
	vector<int> sorted=HSort(tobesorted,hcnt);
	
	if (sorted.size()<=conformitylevel[i]){
	cout<<endl<<"ALARM!!! "<<i<<" "<<endl;
	}
	//here conformism starts to mean something
	// if vertex is conformist (conformism[i]=1) then it does what all do
	// otherwise it does just the opposite, meaning that if the majority makes decision to do a Thing
	// this vertex does not.
	int newvarnum=0;
	if (conformism[i]==1){	
	newvarnum=sorted[conformitylevel[i]-1];
	}
	else {
	newvarnum=-sorted[neighbourssize-conformitylevel[i]];
	}
	neighbours.clear();
	sorted.clear();
	sorted.~vector();
	tobesorted.clear();
	tobesorted.~vector();
	if (agitated==false){
	Mstream<<dimension*(k+1)+i+1<<" "<<-newvarnum<<" 0"<<endl;
	nClauses++;
	Mstream<<-(dimension*(k+1)+i+1)<<" "<<newvarnum<<" 0"<<endl;
	nClauses++;
	}
	else {
	Mstream<<-(dimension*(k+1)+i+1)<<" "<<newvarnum<<" "<<i+1<<" 0"<<endl;
	nClauses++;
	Mstream<<dimension*(k+1)+i+1<<" "<<-newvarnum<<" 0"<<endl;
	nClauses++;
	Mstream<<dimension*(k+1)+i+1<<" "<<-(i+1)<<" 0"<<endl;
	nClauses++;	
	}	
	}
	}
}
void Conformity::Print(const char *fn){
	ofstream out;
	out.open(fn);
	out<<"p cnf "<<nVars<<" "<<nClauses<<endl;
	cout<<endl<<"M.size is "<<M.size()<<endl;
	out<<Mstream.rdbuf();	
	Mstream.clear();//clear any bits set
	Mstream.str(std::string());
	out.close();
	cout<<endl<<"Output finished"<<endl;
}
void Conformity::Dump(char *fn){
	ofstream out;
	out.open(fn,ios::app);
	for (int i=0;i<M.size();i++){
	int *t=M[i];
	int k=0;
	while (t[k]!=0){
	out<<t[k]<<" ";
	k++;
	}
	out<<"0\n";
	}
	M.clear();
	cout<<endl<<"Dump finished"<<endl;
	out.close();
}
void Conformity::seqcounter( vector<int> a)
{//rewrite for mstream
	int lenk=a.size();
	//nVars=lenk;
	vector<int> s;
	int *lcl;
	lcl=new(int [lenk+1]);
	for (int i=0;i<lenk-1;i++){
		lcl[i]=a[i];
		s.push_back(++nVars);
	}
	lcl[lenk-1]=a[lenk-1];
	lcl[lenk]=0;
	M.push_back(lcl);
	int * t;
	t=new(int[3]);
	t[0]=-a[1-1];
	t[1]=s[1-1];
	t[2]=0;
	M.push_back(t);
	t=new(int[3]);
	t[0]=-a[lenk-1];
	t[1]=-s[lenk-1-1];
	t[2]=0;
	M.push_back(t);
	for (int i=2;i<lenk;i++){
		t=new(int[3]);
		t[0]=-a[i-1];
		t[1]=s[i-1];
		t[2]=0;
		M.push_back(t);
		t=new(int[3]);
		t[0]=-s[i-1-1];
		t[1]=s[i-1];
		t[2]=0;
		M.push_back(t);
		t=new(int[3]);
		t[0]=-a[i-1];
		t[1]=-s[i-1-1];
		t[2]=0;
		M.push_back(t);
	}
}
vector <int> Conformity::Selectodd(vector<int> a){
	vector <int> res;
	for (int i=0;i<a.size();i=i+2){
	int t=a[i];
	res.push_back(t);
	}
	return res;
}
vector <int> Conformity::Selecteven(vector<int> a){
	vector <int> res;
	for (int i=1;i<a.size();i=i+2){
	int t=a[i];
	res.push_back(t);
	}
	return res;
}
vector <int> Conformity::HMerge(vector<int> &a, vector<int> &b, int n){
	vector<int> res;
	for (int i=0;i<2*n;i++){int t=++nVars;res.push_back(t);}
	if (n==1){
		Mstream<<-a[0]<<" "<<-b[0]<<" "<<res[1]<<" 0"<<endl;
		Mstream<<-a[0]<<" "<<res[0]<<" "<<" 0"<<endl;
		Mstream<<-b[0]<<" "<<res[0]<<" "<<" 0"<<endl;
		Mstream<<a[0]<<" "<<b[0]<<" "<<-res[0]<<" 0"<<endl;
		Mstream<<a[0]<<" "<<-res[1]<<" "<<" 0"<<endl;
		Mstream<<b[0]<<" "<<-res[1]<<" "<<" 0"<<endl;
		nClauses+=6;
	}
	else
		{
		vector<int> aeven=Selecteven(a);
		vector<int> aodd=Selectodd(a);
		vector<int> beven=Selecteven(b);
		vector<int> bodd=Selectodd(b);
	
		vector<int> D=HMerge(aodd,bodd,aodd.size());
		vector<int> E=HMerge(aeven,beven,aeven.size());	
		for (int i=1;i<n;i++){
			res[0]=D[0];
			res[2*n-1]=E[n-1];
	
			Mstream<<-D[i+1-1]<<" "<<-E[i-1]<<" "<<res[2*i+1-1]<<" 0"<<endl;
			Mstream<<-D[i+1-1]<<" "<<res[2*i-1]<<" "<<" 0"<<endl;
			Mstream<<-E[i-1]<<" "<<res[2*i-1]<<" "<<" 0"<<endl;

			Mstream<<D[i+1-1]<<" "<<E[i-1]<<" "<<-res[2*i-1]<<" 0"<<endl;
			Mstream<<D[i+1-1]<<" "<<-res[2*i+1-1]<<" "<<" 0"<<endl;
			Mstream<<E[i-1]<<" "<<-res[2*i+1-1]<<" "<<" 0"<<endl;	
			nClauses+=6;
		}	
	}
	return res;
}
vector <int> Conformity::SMerge (vector<int> a, vector <int> b, int n){//rewrite for Mstream
	vector<int> res;
	if (n==1){ int t=++nVars; res.push_back(t);
	   int r=++nVars; res.push_back(r);}
	else {
		res.push_back(0);
		for (int i=2;i<=n+1;i++){int t=++nVars;res.push_back(t);}
	}	
	if (n==1){
		int *cl1=new(int[4]);
		int *cl2=new(int[3]);
		int *cl3=new(int[3]);
		int *cl4=new(int[4]);
		int *cl5=new(int[3]);
		int *cl6=new(int[3]);
	
		cl1[0]=-a[0]; cl1[1]=-b[0]; cl1[2]=res[1]; cl1[3]=0;
		cl2[0]=-a[0]; cl2[1]=res[0]; cl2[2]=0;
		cl3[0]=-b[0]; cl3[1]=res[0]; cl3[2]=0;

		cl4[0]=a[0]; cl4[1]=b[0]; cl4[2]=-res[0]; cl4[3]=0;
		cl5[0]=a[0]; cl5[1]=-res[1]; cl5[2]=0;
		cl6[0]=b[0]; cl6[1]=-res[1]; cl6[2]=0;
		M.push_back(cl1);
		M.push_back(cl2);
		M.push_back(cl3);
		M.push_back(cl4);
		M.push_back(cl5);
		M.push_back(cl6);
	}
	else
	{
		vector<int> aeven=Selecteven(a);
		vector<int> aodd=Selectodd(a);
		vector<int> beven=Selecteven(b);
		vector<int> bodd=Selectodd(b);
	
		vector<int> D=SMerge(aodd,bodd,aodd.size());
		vector<int> E=SMerge(aeven,beven,aeven.size());
		res[0]=D[0];
		for (int i=1;i<=n/2;i++){
			int *cl1=new(int[4]);
			int *cl2=new(int[3]);
			int *cl3=new(int[3]);
			int *cl4=new(int[4]);
			int *cl5=new(int[3]);
			int *cl6=new(int[3]);
	

			cl1[0]=-D[i+1-1]; cl1[1]=-E[i-1]; cl1[2]=res[2*i+1-1]; cl1[3]=0;
			cl2[0]=-D[i+1-1]; cl2[1]=res[2*i-1]; cl2[2]=0;
			cl3[0]=-E[i-1];cl3[1]=res[2*i-1]; cl3[2]=0;

			cl4[0]=D[i+1-1];cl4[1]=E[i-1]; cl4[2]=-res[2*i-1]; cl4[3]=0;
			cl5[0]=D[i+1-1];cl5[1]=-res[2*i+1-1]; cl5[2]=0;
			cl6[0]=E[i-1]; cl6[1]=-res[2*i+1-1]; cl6[2]=0;
			M.push_back(cl1);
			M.push_back(cl2);
			M.push_back(cl3);
			M.push_back(cl4);
			M.push_back(cl5);
			M.push_back(cl6);
		}	
	}
	return res;
}
vector<int> Conformity::HSort(vector<int> &a, int n){
	vector<int>res;
	if (n/2>1){
		vector<int>l;
		vector<int>r;
		for (int i=0;i<n/2;i++){
			l.push_back(a[i]);
			r.push_back(a[n/2+i]);
		}
			vector<int> lsort=HSort(l,n/2);
			vector<int> rsort=HSort(r,n/2);
			res=HMerge(lsort,rsort,n/2);	
	}
	else{
		vector <int> l;
		vector <int> r;
		l.push_back(a[0]);
		r.push_back(a[1]);
		res=HMerge(l,r,1);
	}
	return res;
}
vector<int> Conformity::SHSort(vector<int> a, int lown, int n){
	vector<int>res;
	vector<vector<int>> ntuples;
	for (int i=0;i<a.size()/lown;i++){
		vector<int> t;
		for (int j=0;j<lown;j++){
			t.push_back(a[i*lown+j]);
		}
		ntuples.push_back(t);
	}
	vector<int> leftres;
	for (int k=1;k<ntuples.size();k++){
		if (k==1){
			vector<int> sortedleft=HSort(ntuples[0],lown);
			vector<int> sortedright=HSort(ntuples[1],lown);
			leftres=SMerge(sortedleft,sortedright,lown);
		}
		else
		{
			vector<int> rightres;
			vector<int> sortedright=HSort(ntuples[k],lown);
			leftres.erase(leftres.end()-1);
			rightres=SMerge(leftres,sortedright,lown);
			leftres=rightres;
		}
	}
	return leftres;
}
static inline double cpuTime( void ) 
{
    return ( double )clock( ) / CLOCKS_PER_SEC; 
}
std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

int gentests_noagit(int noftests, int dimension, int step, double gnpprob, double confprob, double confpercent, int nomorepercent, int nolesspercent, string foldername){
	std::wstring stemp = s2ws(foldername);
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);
	string logfilename=foldername+"\\testlog.txt";
	logstream<<"Number of tests "<<noftests<<endl;
	logstream<<"Dimension "<<dimension<<endl;
	logstream<<"Number of steps "<<step<<endl;
	logstream<<"Probability (Gnp graph) "<<gnpprob<<endl;
	logstream<<"Probablilty (conformity) "<<confprob<<endl;
	logstream<<"Conformity ";
	if (confpercent==0) {logstream<<"Random"<<endl;} else {logstream<<"level (percent) "<<confpercent<<endl;}
	logstream<<"Nomoreatstart (percent) "<<nomorepercent<<endl;
	logstream<<"Notlessatend (percent) "<<nolesspercent<<endl;	
	int nomore=dimension*nomorepercent/100;
	int conf=confpercent*dimension;
	double timestart=cpuTime();
	for (int i=0;i<noftests;i++){
			int noless=dimension*nolesspercent/100;
			logstream<<endl<<"Test No "<<i+1<<endl;
			double timecycle1=cpuTime();
			Conformity o;
			o.GNPgraph(gnpprob,dimension);
			o.initializeconformity(confpercent,confprob);
			string matrixfilename=foldername+"\\matrix_"+inttostr(i+1)+".txt";
			string matrixfilename_gv=foldername+"\\matrix_"+inttostr(i+1)+"_gv.txt";
			o.savematrixtofile(matrixfilename.c_str());
			o.savematrixtofile_gv(matrixfilename_gv.c_str());
			logstream<<"Matrix file name "<<matrixfilename<<endl;
			//o.functioning(step, agitated);
			//o.Notmoreatstart(nomore,1,0);
			o.Printmatrix();
			o.generalfunctioning(step,0,0,true);
			o.Notmoreatstart(nomore,0,0);
			o.Notmoreatstart(0,1,0);
			o.Notmoreatstart(0,0,1);
			o.Notlessatend(noless);
			//vector<int> a=o.HSort(l,k);
			string cnffilename=foldername+"\\conf_"+inttostr(i+1)+".cnf";
			o.Print(cnffilename.c_str());

			string cnfoutputfilename=foldername+"\\conf_"+inttostr(i+1)+".out";		
	}
	ofstream out;
	out.open(logfilename);
	out<<logstream.rdbuf();
	logstream.clear();
	return 1;
}
int gentests_noagit_fixedpoint(int noftests, int dimension, int step, double gnpprob, double confprob, double confpercent, int nomorepercent, int nolesspercent, string foldername){
	std::wstring stemp = s2ws(foldername);
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);
	string logfilename=foldername+"\\testlog.txt";
	logstream<<"Number of tests "<<noftests<<endl;
	logstream<<"Dimension "<<dimension<<endl;
	logstream<<"Number of steps "<<step<<endl;
	logstream<<"Probability (Gnp graph) "<<gnpprob<<endl;
	logstream<<"Probablilty (conformity) "<<confprob<<endl;
	logstream<<"Conformity ";
	if (confpercent==0) {logstream<<"Random"<<endl;} else {logstream<<"level (percent) "<<confpercent<<endl;}
	logstream<<"Nomoreatstart (percent) "<<nomorepercent<<endl;
	logstream<<"Notlessatend (percent) "<<nolesspercent<<endl;	
	int nomore=dimension*nomorepercent/100;
	int conf=confpercent*dimension;
	double timestart=cpuTime();
	for (int i=0;i<noftests;i++){
			int noless=dimension*nolesspercent/100;
			logstream<<endl<<"Test No "<<i+1<<endl;
			double timecycle1=cpuTime();
			Conformity o;
			o.GNPgraph(gnpprob,dimension);
			o.initializeconformity(confpercent,confprob);
			string matrixfilename=foldername+"\\matrix_"+inttostr(i+1)+".txt";
			o.savematrixtofile(matrixfilename.c_str());
			logstream<<"Matrix file name "<<matrixfilename<<endl;
			//o.functioning(step, agitated);
			//o.Notmoreatstart(nomore,1,0);
			o.Printmatrix();
			o.generalfunctioning(step,0,0,true);
			//o.Notmoreatstart(nomore,0,0);
			//o.Notmoreatstart(0,1,0);
			//o.Notmoreatstart(0,0,1);
			//o.Notlessatend(noless);
			o.Fixedpoint();
			//vector<int> a=o.HSort(l,k);
			string cnffilename=foldername+"\\conf_"+inttostr(i+1)+".cnf";
			o.Print(cnffilename.c_str());

			string cnfoutputfilename=foldername+"\\conf_"+inttostr(i+1)+".out";		
	}
	ofstream out;
	out.open(logfilename);
	out<<logstream.rdbuf();
	logstream.clear();
	return 1;
}
int gentests_agit(int noftests, int dimension, int step, double gnpprob, double confprob, double confpercent, int nomorepercent, int nolesspercent, string foldername){
	std::wstring stemp = s2ws(foldername);
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);
	
	
	string logfilename=foldername+"\\testlog.txt";
	
	logstream<<"Number of tests "<<noftests<<endl;
	
	logstream<<"Dimension "<<dimension<<endl;
	
	logstream<<"Number of steps "<<step<<endl;
	
	logstream<<"Probability (Gnp graph) "<<gnpprob<<endl;
	
	logstream<<"Probablilty (conformity) "<<confprob<<endl;
	
	logstream<<"Conformity ";
	if (confpercent==0) {logstream<<"Random"<<endl;} else {logstream<<"level (percent) "<<confpercent<<endl;}
	logstream<<"Nomoreatstart (percent) "<<nomorepercent<<endl;
	logstream<<"Notlessatend (percent) "<<nolesspercent<<endl;	
	int nomore=dimension*nomorepercent/100;

	double timestart=cpuTime();
	for (int i=0;i<noftests;i++){
			int noless=dimension*nolesspercent/100;
			logstream<<endl<<"Test No "<<i+1<<endl;
			double timecycle1=cpuTime();
			Conformity r;
			string r_matrixfilename = "D:\\Tests\\Linux\\conf111\\noagit_20_03_40_80\\matrix_" + inttostr(i + 1) + ".txt";
			r.loadmatrixfromfile(dimension, r_matrixfilename.c_str());
			r.generalfunctioning(step, 1, 0, true);
			r.Notmoreatstart(nomore, 0, 0);
			r.Notmoreatstart(nomore, 1, 0);
			r.Notmoreatstart(0, 0, 1);
			r.Notlessatend(noless);
			//vector<int> a=o.HSort(l,k);
			string cnffilename = foldername + "\\conf_" + inttostr(i + 1) + ".cnf";
			r.Print(cnffilename.c_str());

			/*

			Conformity o;
			o.GNPgraph(gnpprob,dimension);
			o.initializeconformity(confpercent,confprob);
			string matrixfilename=foldername+"\\matrix_"+inttostr(i+1)+".txt";
			string matrixfilename_gv=foldername+"\\matrix_"+inttostr(i+1)+"_gv.txt";
			o.savematrixtofile(matrixfilename.c_str());
			o.savematrixtofile_gv(matrixfilename_gv.c_str());
			logstream<<"Matrix file name "<<matrixfilename<<endl;
			o.Printmatrix();
			o.generalfunctioning(step,1,0,true);
			o.Notmoreatstart(nomore,0,0);
			o.Notmoreatstart(nomore,1,0);
			o.Notmoreatstart(0,0,1);
			o.Notlessatend(noless);
			//vector<int> a=o.HSort(l,k);
			string cnffilename=foldername+"\\conf_"+inttostr(i+1)+".cnf";
			o.Print(cnffilename.c_str());	
			*/
	}
	ofstream out;
	out.open(logfilename);
	out<<logstream.rdbuf();
	logstream.clear();
	out.close();
	return 1;
}

int gentests_loyaled_delayed(int noftests, int dimension, int step, int nomoreloyalpercent, int nomorepercent_end, string sourcefoldername,string targetfoldername){
	
	std::wstring stemp = s2ws(targetfoldername);
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);

	string logfilename=targetfoldername+"\\testlog.txt";
	logstream<<"Checking "<<endl;
	int nomore=dimension*nomorepercent_end/100;
	int nomoreloyal=dimension*nomoreloyalpercent/100;
	double timestart=cpuTime();
	for (int i=0;i<noftests;i++){
		cout<<endl<<"Checking test "<<i<<endl;
		logstream<<endl<<"Test No "<<i+1<<endl;
	
		double timecycle1=cpuTime();
		Conformity o;
		string matrixfilename=sourcefoldername+"\\matrix_"+inttostr(i+1)+".txt";
		o.loadmatrixfromfile(dimension,matrixfilename.c_str());
	
		string cnfoutputfilename=sourcefoldername+"\\ss\\res_conf_"+inttostr(i+1)+".cnf";
	
		vector<int>a = o.loadssfromfile(cnfoutputfilename.c_str());
		if (a.size()==0){
			cout<<endl<<"No solution"<<endl;
			logstream<<"No solution"<<endl;
		}	
		else {
			vector<int>b;
			for (int j=0;j<dimension*(step+3);j++){
					if (a[j]<0){b.push_back(0);}else {b.push_back(1);}
			}
			bool a=o.calculate(b,step);
			if (a==true){
				logstream<<"Solution is correct"<<endl;
			}
				else {
					logstream<<"Solution is incorrect"<<endl;
				}
				logstream<<"Generating test for loyalists"<<endl;
				o.generalfunctioning(step,1,1,false);
				o.restrictagit();				
				o.Notmoreatstart(nomoreloyal,0,1);
				o.Notmoreatend(nomore);
				string cnffilename=targetfoldername+"\\conf_"+inttostr(i+1)+".cnf";
				o.Print(cnffilename.c_str());
		}		
	}
	ofstream out;
	out.open(logfilename);
	out<<logstream.rdbuf();
	out.close();
	logstream.clear();
	return 1;
}

int gentests_loyaled(int noftests, int dimension, int step, int nomoreloyalpercent, int nomorepercent_end, string sourcefoldername,string targetfoldername){
	
	std::wstring stemp = s2ws(targetfoldername);
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);

	string logfilename=targetfoldername+"\\testlog.txt";
	logstream<<"Checking "<<endl;
	int nomore=dimension*nomorepercent_end/100;
	int nomoreloyal=dimension*nomoreloyalpercent/100;
	double timestart=cpuTime();
	for (int i=0;i<noftests;i++){
		cout<<endl<<"Checking test "<<i<<endl;
		logstream<<endl<<"Test No "<<i+1<<endl;
	
		double timecycle1=cpuTime();
		Conformity o;
		string matrixfilename=sourcefoldername+"\\matrix_"+inttostr(i+1)+".txt";
		o.loadmatrixfromfile(dimension,matrixfilename.c_str());
	
		string cnfoutputfilename=sourcefoldername+"\\ss\\res_conf_"+inttostr(i+1)+".cnf";
	
		vector<int>a = o.loadssfromfile(cnfoutputfilename.c_str());
		if (a.size()==0){
			cout<<endl<<"No solution"<<endl;
			logstream<<"No solution"<<endl;
		}	
		else {
			vector<int>b;
			for (int j=0;j<dimension*(step+3);j++){
					if (a[j]<0){b.push_back(0);}else {b.push_back(1);}
			}
			bool a=o.calculate(b,step);
			if (a==true){
				logstream<<"Solution is correct"<<endl;
			}
				else {
					logstream<<"Solution is incorrect"<<endl;
				}
				logstream<<"Generating test for loyalists"<<endl;
				
				o.generalfunctioning(1,1,1,true);
				o.restrictagit();
				o.Notmoreatstart(nomore,0,0);
				o.Notmoreatstart(nomoreloyal,0,1);
				o.Fixedpoint();
				string cnffilename=targetfoldername+"\\conf_"+inttostr(i+1)+".cnf";
				o.Print(cnffilename.c_str());
		}		
	}
	ofstream out;
	out.open(logfilename);
	out<<logstream.rdbuf();
	out.close();
	logstream.clear();
	return 1;
}


int runtests_noagit(int noftests, int dimension, int step, double gnpprob, double confprob, double confpercent, int nomorepercent, int nolesspercent, string foldername){
	std::wstring stemp = s2ws(foldername);
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);
	
	ofstream out;
	string logfilename=foldername+"\\testlog.txt";
	out.open(logfilename.c_str());
	if (out.is_open()){cout<<"Logfile succesfully open "<<endl;}

	out<<"Number of tests "<<noftests<<endl;
	logstream<<"Number of tests "<<noftests<<endl;
	out<<"Dimension "<<dimension<<endl;
	logstream<<"Dimension "<<dimension<<endl;
	out<<"Number of steps "<<step<<endl;
	logstream<<"Number of steps "<<step<<endl;
	out<<"Probability (Gnp graph) "<<gnpprob<<endl;
	logstream<<"Probability (Gnp graph) "<<gnpprob<<endl;
	out<<"Probablilty (conformity) "<<confprob<<endl;
	logstream<<"Probablilty (conformity) "<<confprob<<endl;
	out<<"Conformity ";
	logstream<<"Conformity ";
	if (confpercent==0) {out<<"Random"<<endl;} else {out<<"level (percent) "<<confpercent<<endl;}
	out<<"Nomoreatstart (percent) "<<nomorepercent<<endl;
	out<<"Notlessatend (percent) "<<nolesspercent<<endl;	
	logstream<<"Nomoreatstart (percent) "<<nomorepercent<<endl;
	logstream<<"Notlessatend (percent) "<<nolesspercent<<endl;	
	int nomore=dimension*nomorepercent/100;

	double timestart=cpuTime();
	for (int i=0;i<noftests;i++){
			int noless=dimension*nolesspercent/100;
			out<<endl<<"Test No "<<i+1<<endl;
			logstream<<endl<<"Test No "<<i+1<<endl;
			double timecycle1=cpuTime();
			Conformity o;
			o.GNPgraph(gnpprob,dimension);
			o.initializeconformity(confpercent,confprob);
			string matrixfilename=foldername+"\\matrix_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".txt";
			o.savematrixtofile(matrixfilename.c_str());
			logstream<<"Matrix file name "<<matrixfilename<<endl;
			//o.functioning(step, agitated);
			//o.Notmoreatstart(nomore,1,0);
			o.Printmatrix();
			o.generalfunctioning(step,1,1,true);
			o.Notmoreatstart(nomore,0,0);
			o.Notmoreatstart(0,1,0);
			o.Notmoreatstart(0,0,1);
			o.Notlessatend(noless);
			//vector<int> a=o.HSort(l,k);
			string cnffilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".cnf";
			o.Print(cnffilename.c_str());

			string cnfoutputfilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".out";
			string s="D:\\Tests\\cryptominisat32.exe --threads=8 "+cnffilename+" "+cnfoutputfilename;
			system(s.c_str());
			double timecycle2=cpuTime();
			out<<"Solving took "<<timecycle2-timecycle1<<" seconds"<<endl;
			logstream<<"Solving took "<<timecycle2-timecycle1<<" seconds"<<endl;
			vector<int>a =o.loadssfromfile(cnfoutputfilename.c_str());
			if (a.size()==0){
					cout<<endl<<"No solution"<<endl;
					out<<"No solution"<<endl;
					logstream<<"No solution"<<endl;
				}
				else {
					vector<int>b;
					for (int p=0;p<dimension*(step+3);p++){
						if (a[p]<0){b.push_back(0);}else {b.push_back(1);}
					} 
					bool z=o.calculate(b,step);
					if (z==true){
						out<<"Solution is correct"<<endl;
						logstream<<"Solution is correct"<<endl;
					}
						else {
							logstream<<"Solution is incorrect"<<endl;
						}							
		}	
				string tt=foldername+"\\generallog.txt";
				ofstream outlog;
				outlog.open(tt.c_str(),ios::app);
				outlog<<logstream.rdbuf();
				outlog.close();
	}
	out.close();
	return 1;
}
int runtests_agit(int noftests, int dimension, int step, double gnpprob, double confprob, double confpercent, int nomorepercent, int nolesspercent, string foldername){
	std::wstring stemp = s2ws(foldername);
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);
	
	ofstream out;
	string logfilename=foldername+"\\testlog.txt";
	out<<"Number of tests "<<noftests<<endl;
	logstream<<"Number of tests "<<noftests<<endl;
	out<<"Dimension "<<dimension<<endl;
	logstream<<"Dimension "<<dimension<<endl;
	out<<"Number of steps "<<step<<endl;
	logstream<<"Number of steps "<<step<<endl;
	out<<"Probability (Gnp graph) "<<gnpprob<<endl;
	logstream<<"Probability (Gnp graph) "<<gnpprob<<endl;
	out<<"Probablilty (conformity) "<<confprob<<endl;
	logstream<<"Probablilty (conformity) "<<confprob<<endl;
	out<<"Conformity ";
	logstream<<"Conformity ";
	if (confpercent==0) {out<<"Random"<<endl;} else {out<<"level (percent) "<<confpercent<<endl;}
	out<<"Nomoreatstart (percent) "<<nomorepercent<<endl;
	out<<"Notlessatend (percent) "<<nolesspercent<<endl;	
	logstream<<"Nomoreatstart (percent) "<<nomorepercent<<endl;
	logstream<<"Notlessatend (percent) "<<nolesspercent<<endl;	
	int nomore=dimension*nomorepercent/100;

	double timestart=cpuTime();
	for (int i=0;i<noftests;i++){
			int noless=dimension*nolesspercent/100;
			out<<endl<<"Test No "<<i+1<<endl;
			logstream<<endl<<"Test No "<<i+1<<endl;
			double timecycle1=cpuTime();
			Conformity o;
			o.GNPgraph(gnpprob,dimension);
			o.initializeconformity(confpercent,confprob);
			string matrixfilename=foldername+"\\matrix_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".txt";
			o.savematrixtofile(matrixfilename.c_str());
			logstream<<"Matrix file name "<<matrixfilename<<endl;
			//o.functioning(step, agitated);
			//o.Notmoreatstart(nomore,1,0);
			o.Printmatrix();
			o.generalfunctioning(step,1,1,true);
			o.Notmoreatstart(nomore,0,0);
			o.Notmoreatstart(nomore,1,0);
			o.Notmoreatstart(0,0,1);
			o.Notlessatend(noless);
			//vector<int> a=o.HSort(l,k);
			string cnffilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".cnf";
			o.Print(cnffilename.c_str());

			string cnfoutputfilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".out";
			string s="D:\\Tests\\cryptominisat32.exe --threads=8 "+cnffilename+" "+cnfoutputfilename;
			system(s.c_str());
			double timecycle2=cpuTime();
			out<<"Solving took "<<timecycle2-timecycle1<<" seconds"<<endl;
			logstream<<"Solving took "<<timecycle2-timecycle1<<" seconds"<<endl;
			vector<int>a =o.loadssfromfile(cnfoutputfilename.c_str());
			if (a.size()==0){
					cout<<endl<<"No solution"<<endl;
					logstream<<endl<<"No solution"<<endl;
					out<<"No solution"<<endl;
				}
				else {
					vector<int>b;
					for (int p=0;p<dimension*(step+3);p++){
						if (a[p]<0){b.push_back(0);}else {b.push_back(1);}
					} 
					bool z=o.calculate(b,step);
					if (z==true){
						out<<"Solution is correct"<<endl;
						logstream<<"Solution is correct"<<endl;
					}
					else {
						out<<"Solution is incorrect"<<endl;
						logstream<<"Solution is incorrect"<<endl;
					}							
		}	
				string tt=foldername+"\\generallog.txt";
				ofstream outlog;
				outlog.open(tt.c_str(),ios::app);
				outlog<<logstream.rdbuf();
				outlog.close();
	}
	out.close();
	return 1;
}

int runtests_loyaled_vs_agit(int noftests, int dimension, int step, double gnpprob, double confprob, double confpercent, int nomorepercent, int nolesspercent, string foldername){
	std::wstring stemp = s2ws(foldername);
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);
	
	ofstream out;
	string logfilename=foldername+"\\testlog.txt";
	out<<"Number of tests "<<noftests<<endl;
	logstream<<"Number of tests "<<noftests<<endl;
	out<<"Dimension "<<dimension<<endl;
	logstream<<"Dimension "<<dimension<<endl;
	out<<"Number of steps "<<step<<endl;
	logstream<<"Number of steps "<<step<<endl;
	out<<"Probability (Gnp graph) "<<gnpprob<<endl;
	logstream<<"Probability (Gnp graph) "<<gnpprob<<endl;
	out<<"Probablilty (conformity) "<<confprob<<endl;
	logstream<<"Probablilty (conformity) "<<confprob<<endl;
	out<<"Conformity ";
	logstream<<"Conformity ";
	if (confpercent==0) {out<<"Random"<<endl;} else {out<<"level (percent) "<<confpercent<<endl;}
	out<<"Nomoreatstart (percent) "<<nomorepercent<<endl;
	out<<"Notlessatend (percent) "<<nolesspercent<<endl;	
	logstream<<"Nomoreatstart (percent) "<<nomorepercent<<endl;
	logstream<<"Notlessatend (percent) "<<nolesspercent<<endl;	
	int nomore=dimension*nomorepercent/100;

	double timestart=cpuTime();
	for (int i=0;i<noftests;i++){
			int noless=dimension*nolesspercent/100;
			out<<endl<<"Test No "<<i+1<<endl;
			logstream<<endl<<"Test No "<<i+1<<endl;
			double timecycle1=cpuTime();
			Conformity o;
			o.GNPgraph(gnpprob,dimension);
			o.initializeconformity(confpercent,confprob);
			string matrixfilename=foldername+"\\matrix_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".txt";
			o.savematrixtofile(matrixfilename.c_str());
			logstream<<"Matrix file name "<<matrixfilename<<endl;
			//o.functioning(step, agitated);
			//o.Notmoreatstart(nomore,1,0);
			o.Printmatrix();
			o.generalfunctioning(step,1,1,true);
			o.Notmoreatstart(nomore,0,0);
			o.Notmoreatstart(nomore,1,0);
			o.Notmoreatstart(0,0,1);
			o.Notlessatend(noless);
			//vector<int> a=o.HSort(l,k);
			string cnffilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".cnf";
			o.Print(cnffilename.c_str());

			string cnfoutputfilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".out";
			string s="D:\\Tests\\cryptominisat32.exe --threads=8 "+cnffilename+" "+cnfoutputfilename;
			system(s.c_str());
			double timecycle2=cpuTime();
			out<<"Solving took "<<timecycle2-timecycle1<<" seconds"<<endl;
			logstream<<"Solving took "<<timecycle2-timecycle1<<" seconds"<<endl;
			vector<int>a =o.loadssfromfile(cnfoutputfilename.c_str());
			if (a.size()==0){
					cout<<endl<<"No solution"<<endl;
					logstream<<"No solution"<<endl;
					out<<"No solution"<<endl;
				}
				else {
					vector<int>b;
					for (int p=0;p<dimension*(step+3);p++){
						if (a[p]<0){b.push_back(0);}else {b.push_back(1);}
					} 
					bool z=o.calculate(b,step);
					if (z==true){
						out<<"Solution is correct"<<endl;
						logstream<<"Solution is correct"<<endl;
					}
					else {
						out<<"Solution is incorrect"<<endl;
						logstream<<"Solution is incorrect"<<endl;
					}		
			
				o.generalfunctioning(1,1,1,true);
				o.restrictagit();
				o.Notmoreatstart(nomore,0,0);
				o.Notmoreatstart(nomore,0,1);
				o.Notmoreatend(nomore);
				cnffilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+"_LOYALED.cnf";
				o.Print(cnffilename.c_str());
				string cnfoutputfilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+"_LOYALED.out";
				string s="D:\\Tests\\cryptominisat32.exe --threads=8 "+cnffilename+" "+cnfoutputfilename;
				double timecycle3=cpuTime();
				system(s.c_str());
				double timecycle4=cpuTime();
				logstream<<"Solving took "<<timecycle4-timecycle3<<" seconds"<<endl;
				vector<int>aa =o.loadssfromfile(cnfoutputfilename.c_str());
				logstream<<"_________________Loyalists stage____________________"<<endl;
				if (aa.size()==0){
						cout<<endl<<"No solution"<<endl;
						logstream<<"No solution"<<endl;
						out<<"No solution"<<endl;
					}
					else {
						vector<int>bb;
						for (int p=0;p<dimension*(step+3);p++){
							if (aa[p]<0){bb.push_back(0);}else {bb.push_back(1);}
						} 
						bool zz=o.calculate(bb,step);
						if (zz==true){
							out<<"Solution is correct"<<endl;
							logstream<<"Solution is correct"<<endl;
						}
						else {
							out<<"Solution is incorrect"<<endl;
							logstream<<"Solution is incorrect"<<endl;
						}		
					}		
		}	
				string tt=foldername+"\\generallog.txt";
				ofstream outlog;
				outlog.open(tt.c_str(),ios::app);
				outlog<<logstream.rdbuf();
				outlog.close();
	}

	out.close();
	return 1;
}
int runtests_loyaled_vs_agit_delayed(int noftests, int dimension, int step, double gnpprob, double confprob, double confpercent, int nomorepercent, int nomoreloyalpercent, int nolesspercent, string foldername){
	std::wstring stemp = s2ws(foldername);
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);


	
	ofstream out;
	string logfilename=foldername+"\\testlog.txt";
	out<<"Number of tests "<<noftests<<endl;
	logstream<<"Number of tests "<<noftests<<endl;
	out<<"Dimension "<<dimension<<endl;
	logstream<<"Dimension "<<dimension<<endl;
	out<<"Number of steps "<<step<<endl;
	logstream<<"Number of steps "<<step<<endl;
	out<<"Probability (Gnp graph) "<<gnpprob<<endl;
	logstream<<"Probability (Gnp graph) "<<gnpprob<<endl;
	out<<"Probablilty (conformity) "<<confprob<<endl;
	logstream<<"Probablilty (conformity) "<<confprob<<endl;
	out<<"Conformity ";
	logstream<<"Conformity ";
	if (confpercent==0) {out<<"Random"<<endl;} else {out<<"level (percent) "<<confpercent<<endl;}
	out<<"Nomoreatstart (percent) "<<nomorepercent<<endl;
	out<<"Notlessatend (percent) "<<nolesspercent<<endl;	
	logstream<<"Nomoreatstart (percent) "<<nomorepercent<<endl;
	logstream<<"Notmoreatend (percent) "<<nolesspercent<<endl;	
	logstream<<"Notmoreloyal (percent) "<<nomoreloyalpercent<<endl;	
	int nomore=dimension*nomorepercent/100;
	int nomoreloyal=dimension*nomoreloyalpercent/100;
	double timestart=cpuTime();
	for (int i=0;i<noftests;i++){
			int noless=dimension*nolesspercent/100;
			out<<endl<<"Test No "<<i+1<<endl;
			logstream<<endl<<"Test No "<<i+1<<endl;
			double timecycle1=cpuTime();
			Conformity o;
			o.GNPgraph(gnpprob,dimension);
			o.initializeconformity(confpercent,confprob);
			string matrixfilename=foldername+"\\matrix_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".txt";
			o.savematrixtofile(matrixfilename.c_str());
			logstream<<"Matrix file name "<<matrixfilename<<endl;
			//o.functioning(step, agitated);
			//o.Notmoreatstart(nomore,1,0);
			o.Printmatrix();
			o.generalfunctioning(step,1,1,true);
			o.Notmoreatstart(nomore,0,0);
			o.Notmoreatstart(nomore,1,0);
			o.Notmoreatstart(0,0,1);
			o.Notlessatend(noless);
			//vector<int> a=o.HSort(l,k);
			string cnffilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".cnf";
			o.Print(cnffilename.c_str());

			string cnfoutputfilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+".out";
			string s="D:\\Tests\\cryptominisat32.exe --threads=8 "+cnffilename+" "+cnfoutputfilename;
			system(s.c_str());
			double timecycle2=cpuTime();
			out<<"Solving took "<<timecycle2-timecycle1<<" seconds"<<endl;
			logstream<<"Solving took "<<timecycle2-timecycle1<<" seconds"<<endl;
			vector<int>a =o.loadssfromfile(cnfoutputfilename.c_str());
				if (a.size()==0){
					cout<<endl<<"No solution"<<endl;
					logstream<<"No solution"<<endl;
					out<<"No solution"<<endl;
				}
				else {
					vector<int>b;
					for (int p=0;p<dimension*(step+3);p++){
						if (a[p]<0){b.push_back(0);}else {b.push_back(1);}
					} 
					bool z=o.calculate(b,step);
					if (z==true){
						out<<"Solution is correct"<<endl;
						logstream<<"Solution is correct"<<endl;
					}
					else {
						out<<"Solution is incorrect"<<endl;
						logstream<<"Solution is incorrect"<<endl;
					}							
					

				o.generalfunctioning(step,1,1,false);
				o.restrictagit();
				//o.Notmoreatstart(nomore,0,0);
				o.Notmoreatstart(nomoreloyal,0,1);
				o.Notmoreatend(nomore);
				cnffilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+"_LOYALED.cnf";
				o.Print(cnffilename.c_str());
				string cnfoutputfilename=foldername+"\\conf_"+inttostr(dimension)+"_"+inttostr(step)+"_"+inttostr(i+1)+"_LOYALED.out";
				string s="D:\\Tests\\cryptominisat32.exe --threads=8 "+cnffilename+" "+cnfoutputfilename;
				double timecycle3=cpuTime();
				system(s.c_str());
				double timecycle4=cpuTime();
				logstream<<"Solving took "<<timecycle4-timecycle3<<" seconds"<<endl;
				vector<int>aa =o.loadssfromfile(cnfoutputfilename.c_str());
				logstream<<"_________________Loyalists stage____________________"<<endl;
				if (aa.size()==0){
						cout<<endl<<"No solution"<<endl;
						logstream<<"No solution"<<endl;
						out<<"No solution"<<endl;
					}
					else {
						vector<int>bb;
						for (int p=0;p<dimension*(step+3);p++){
							if (aa[p]<0){bb.push_back(0);}else {bb.push_back(1);}
						} 
						bool zz=o.calculate(bb,step);
						if (zz==true){
							out<<"Solution is correct"<<endl;
							logstream<<"Solution is correct"<<endl;
						}
						else {
							out<<"Solution is incorrect"<<endl;
							logstream<<"Solution is incorrect"<<endl;
						}		
					}		
		}		
				string tt=foldername+"\\generallog.txt";
				ofstream outlog;
				outlog.open(tt.c_str(),ios::app);
				outlog<<logstream.rdbuf();
				outlog.close();
	}

	out.close();
	return 1;
}


int checktests(int noftests, int dimension, int step, double gnpprob, double confprob, double confpercent, int nomorepercent, int nolesspercent, string foldername){
	std::wstring stemp = s2ws(foldername);
	LPCWSTR result = stemp.c_str();
	string logfilename=foldername+"\\testlog.txt";
	logstream<<"Checking "<<endl;
	
	double timestart=cpuTime();
	for (int i=0;i<noftests;i++){
		cout<<endl<<"Checking test "<<i<<endl;
		logstream<<endl<<"Test No "<<i+1<<endl;
	
		double timecycle1=cpuTime();
		Conformity o;
		string matrixfilename=foldername+"\\matrix_"+inttostr(i+1)+".txt";
		string matrixfilename_gv=foldername+"\\matrix_"+inttostr(i+1)+"_gv.txt";
		o.loadmatrixfromfile(dimension,matrixfilename.c_str());
		o.savematrixtofile_gv(matrixfilename_gv.c_str());
		string cnfoutputfilename=foldername+"\\ss\\res_conf_"+inttostr(i+1)+".cnf";
		string tempprogress = foldername + "\\progress";
		vector<int>a = o.loadssfromfile(cnfoutputfilename.c_str());
		if (a.size()==0){
			cout<<endl<<"No solution"<<endl;
			logstream<<"No solution"<<endl;
		}	
		else {
			vector<int>b;
			for (int i=0;i<dimension*(step+3);i++){
					if (a[i]<0){b.push_back(0);}else {b.push_back(1);}
			}
			bool t=o.calculate(b,step,tempprogress.c_str());
			if (t==true){
				logstream<<"Solution is correct"<<endl;
			}
				else {
					logstream<<"Solution is incorrect"<<endl;
				}
		}		
	}
	ofstream out;
	out.open(logfilename);
	out<<logstream.rdbuf();
	out.close();
	logstream.clear();
	return 1;
}
int main (){
	Conformity a;
	a.GNPgraph(0.4,10);
	a.initializeconformity(0,0);
	a.Printmatrix();
	a.construct_reachability_matrix(3);
	a.construct_weights_matrix(2);

	int q;
	cin>>q;
}