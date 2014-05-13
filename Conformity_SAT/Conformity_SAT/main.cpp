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
enum class Conformity_problem_type {Simple, Agitated, Loyal_VS_Agit, Loyal_VS_Agit_delayed, Undefined};
enum class Conformity_graph {GNP_Graph, WS_Graph, Barabashi_graph};
enum class Conformity_conformitylevel_type {ThresholdConformityLevel, RandomConformityLevel};
enum class Conformity_conformists {ConformistsOnly, NonConformistsOnly, MixedConformists};
enum class Conformity_restrictions {Restrict_to_inactive, Restrict_to_active, Restrict_nothing};
enum class Conformity_weights {Weights_at_random, Weights_Decrease_with_distance, Weights_Increase_with_distance};
enum class Conformity_neighbourhood_type {AlwaysFullNeighborhood,From_Small_to_Big, From_Big_to_Small};
enum class Conformity_At_Start {GEQStart,LEQStart, Undefined};
enum class Conformity_At_End {GEQEnd,LEQEnd,Undefined};
int strtoi(string s){
	int x = atoi(s.c_str());
	return x;
}
double strtodouble(string s){
	return atof(s.c_str());
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
string dbltostr(double number)
{
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}
class GVinfo{
	public:
		string id;
		string fillcolor;
		double height;
		double width;
		string label;
		string shape;
		string style;
		string xlabel;
		double pos_x;
		double pos_y;
		GVinfo(){
			id="";
			fillcolor="";
			height=0;
			width=0;
			pos_x=0;
			pos_y=0;	
		}
		GVinfo(string id_in, string fc, double h, double w, string lab, string xlab, double x, double y){
			id=id_in;
			fillcolor=fc;
			height=h;
			width=w;
			label=lab;
			xlabel=xlab;
			pos_x=x;
			pos_y=y;
		}	
		void extract_value(string f, string & lp, string  &rp){
			int k1=f.find('=');
			lp=f.substr(0,k1);
			rp=f.substr(k1+1);
		}
		void parse (string in){
			int k1=-1;
			int k2=0;
			vector<string> descr;
			int i=0;
			bool depth=true;
			while (i<in.length()){
				if (in[i]=='\"') depth=!depth;
				if ((depth&(in[i]=='['))||(depth&(in[i]==','))||(depth&(in[i]==']'))){
					string tmp=in.substr(k1+1,i-1-k1);
					descr.push_back(tmp);					
					k1=i;
				}
				i++;
			}
			id=descr[0];
			while (id.find(' ')!=string::npos){
				id.replace((size_t)id.find(' '),1,"");
			}
			for (int i=1;i<descr.size();i++){
				string lp;
				string rp;
				extract_value(descr[i],lp,rp);
				if (lp=="height"){
					height=strtodouble(rp);
				}
				if (lp=="width"){
					width=strtodouble(rp);
				}
				if (lp=="label"){
					label=rp;
				}
				if (lp=="xlabel"){
					xlabel=rp;
				}
				if (lp=="shape"){
					shape=rp;
				}
				if (lp=="style"){
					style=rp;
				}
				if (lp=="fillcolor"){
					fillcolor=rp;
				}
				if (lp=="pos"){
					string t1=rp.substr(1,rp.find(',')-1);
					pos_x=strtodouble(t1);
					t1=rp.substr(rp.find(',')+1);
					t1=t1.substr(0,t1.length()-1);
					pos_y=strtodouble(t1);
				}
			}
		}
		string Tostring(){
			string res;
			res=id+"\t[";
			res+="fillcolor="+(fillcolor==""?"\"\"":fillcolor)+",\n\t";			
			res+="height="+dbltostr(height)+",\n\t";
			res+="label="+(label==""?"\"\"":label)+",\n\t";
			res+="xlabel="+(xlabel==""?"\"\"":xlabel)+",\n\t";
			res+="pos=\""+dbltostr(pos_x)+","+dbltostr(pos_y)+"\",\n\t";
			res+="shape="+shape+",\n\t";
			res+="style="+(style==""?"\"\"":style)+",\n\t";
			res+="width="+dbltostr(width)+"];\n";
			return res;
		}
};
class GVedge{
public:
	string arrowhead;
	string color;
	int lp;
	int rp;
	void parse (int l, int r, string ah, string cl){
		lp=l;
		rp=r;
		arrowhead=ah;
		color=cl;
	}
	GVedge(int l, int r, string ah, string cl){
		parse(l,r,ah,cl);
	}
	string Tostring(){
		string res="v_"+inttostr(lp+1) + " -> v_" + inttostr(rp+1) + "\t[arrowhead="+arrowhead+
			",\n\t"+"color="+color+"];\n";
		return res;
	}
};
class GV_graph{
public:
	vector<GVinfo> nodes;
	vector<GVedge> edges;
	string headers;
	vector<int> gv_conformitylevels;
	vector<int> gv_vertexdegrees;
	vector<int> gv_agitators;
	vector<int> gv_loyalists;
	GV_graph(){
		headers="";
	}
	void loadedges(vector<int> matrix, int dim){
		for (int i=0;i<dim;i++){
			for (int j=0;j<dim;j++){
				if (matrix[i*dim+j]==1){
					GVedge a(i,j,"normal","black");
					edges.push_back(a);
				}
			}
		}
	}
	void parse(string filename);
	int find_index(string name){
		int res=-1;
		for (int i=0;i<nodes.size();i++){
			if (nodes[i].id==name) {
				res=i;
				return res;
			}
		}
		return res;
	}

	GV_graph(string filename){ 
		nodes.clear();
		headers="";
		parse(filename);
	}	
	void loaddata(vector<int> agitators, vector<int> loyalists, vector<int> conformitylevels, vector<int> degrees);
	void refresh(vector<int> current_input, vector<int> activity);
	void print (string filename);
};

void GV_graph::parse(string filename){
	ifstream in;
	in.open(filename.c_str());
	string res;
	string buf;
	string s;
	string old_s;
	bool readingblock=false;
	getline(in,s);
	headers+=s;
	while (in.good()){
		getline(in,s);
		
		if ((!readingblock)&&(s.find(" [")!=string::npos)){
			readingblock=true;
			buf="";
		}

		buf+=s;
		if (s.find("];")!=string::npos){
			readingblock=false;
			
			if (buf.find("graph")!=string::npos){
				headers+=buf;				
			}
			else if ((buf.find("->")==string::npos)&&(buf.find("info")==string::npos)){				
				while (buf.find('\t')!=string::npos){
					buf.replace((size_t)buf.find('\t'),1,"");
				}
				GVinfo tmp;
				tmp.parse(buf);
				nodes.push_back(tmp);			
			}
		}
	}
	in.close();
}
void GV_graph::loaddata(vector<int> agitators, vector<int> loyalists, vector<int> conformitylevels, vector<int> degrees){
	gv_conformitylevels=conformitylevels;
	gv_agitators=agitators;
	gv_loyalists=loyalists;
	gv_vertexdegrees=degrees;
}

void GV_graph::refresh(vector<int> current_input, vector<int> activity){
	for (int i=0;i<activity.size();i++){

		int cv_ind=find_index((string)("v_"+inttostr(i+1)));
		
		if (gv_agitators[i]==1){
			nodes[cv_ind].shape="doublecircle";
			nodes[cv_ind].fillcolor="crimson";
			nodes[cv_ind].style="\"filled\"";
			nodes[cv_ind].xlabel="\"A ["+inttostr(gv_vertexdegrees[i])+"]\"";
		}
		if (gv_loyalists[i]==1){
			nodes[cv_ind].shape="doublecircle";
			nodes[cv_ind].fillcolor="lawngreen";
			nodes[cv_ind].style="\"filled\"";
			nodes[cv_ind].xlabel="\"L ["+inttostr(gv_vertexdegrees[i])+"]\"";
		}
		if ((gv_agitators[i]==0)&&(gv_loyalists[i]==0)){
			if (activity[i]==1) {
				nodes[cv_ind].fillcolor="lightsalmon";
			}else{
				nodes[cv_ind].fillcolor="lightskyblue";
			}
			nodes[cv_ind].shape="circle";
			nodes[cv_ind].style="\"filled\"";
			nodes[cv_ind].xlabel="\""+ inttostr(current_input[i])+"("+inttostr(gv_conformitylevels[i])+")["+inttostr(gv_vertexdegrees[i])+"]\"";			
		}
	}
	for (int i=0;i<edges.size();i++){
		if (activity[edges[i].lp]==1){
			edges[i].color="red";
		}
		else{
			edges[i].color="green";
		}
	}	
}
void GV_graph::print(string filename){
	ofstream out;
	out.open(filename);
	out<<headers<<endl;
	for (int i=0;i<nodes.size();i++){
		out<<nodes[i].Tostring();
	}
	for (int i=0;i<edges.size();i++){
		out<<edges[i].Tostring();
	}
	out<<"}"<<endl;
	out.close();
}
class Conformity_problem{
public:
	Conformity_problem();
	void print (string filename);
	bool loadfromfile(string filename);
	Conformity_problem (Conformity_problem_type p_t, int startvalue, int endvalue, int nofagit, int nofloyal, int neighbourhood_radius){
		conformity_problem_type=p_t;
		Start_Value_Percent=startvalue;
		End_Value_Percent=endvalue;
		Number_of_agitators=nofagit;
		Number_of_loyalists=nofloyal;
		Neighbourhood_radius=neighbourhood_radius;
		if (p_t==Conformity_problem_type::Simple){
			Hasagitators=false;
			Hasloyalists=false;
			conformity_at_start=Conformity_At_Start::LEQStart;
			conformity_at_end=Conformity_At_End::GEQEnd;
			conformity_restrictions = Conformity_restrictions::Restrict_nothing;
		}
		if (p_t==Conformity_problem_type::Agitated){
			Hasagitators=true;
			Hasloyalists=false;
			conformity_at_start=Conformity_At_Start::LEQStart;
			conformity_at_end=Conformity_At_End::GEQEnd;
			conformity_restrictions = Conformity_restrictions::Restrict_to_inactive;// only agitators are active
		}
		if (p_t==Conformity_problem_type::Loyal_VS_Agit){
			Hasagitators=true;
			Hasloyalists=true;
			conformity_at_start=Conformity_At_Start::Undefined;
			conformity_at_end=Conformity_At_End::Undefined;
			conformity_restrictions = Conformity_restrictions::Restrict_to_inactive; // only agitators are active
		}
		if (p_t==Conformity_problem_type::Loyal_VS_Agit_delayed){
			Hasagitators=true;
			Hasloyalists=true;
			conformity_at_start=Conformity_At_Start::GEQStart;
			conformity_at_end=Conformity_At_End::LEQEnd;
			conformity_restrictions = Conformity_restrictions::Restrict_to_active; //only loyalists are inactive
		}
		if (p_t==Conformity_problem_type::Undefined){
			Hasagitators=true;
			Hasloyalists=true;
			conformity_at_start=Conformity_At_Start::Undefined;
			conformity_at_end=Conformity_At_End::Undefined;
			conformity_restrictions = Conformity_restrictions::Restrict_nothing;
		}
	}
	Conformity_restrictions conformity_restrictions;
	Conformity_problem_type conformity_problem_type;
	Conformity_At_Start conformity_at_start;
	int Start_Value_Percent;
	Conformity_At_End conformity_at_end;
	int End_Value_Percent;

	bool Hasagitators;
	bool Hasloyalists;
	int Number_of_agitators;
	int Number_of_loyalists;
	int Neighbourhood_radius;

};
void Conformity_problem::print(string filename){
	ofstream out;
	out.open(filename.c_str());
	out<<"Conformity problem type: ";
	if (conformity_problem_type==Conformity_problem_type::Agitated){out<<"Agitated";}
	if (conformity_problem_type==Conformity_problem_type::Loyal_VS_Agit){out<<"Loyal vs Agit";}
	if (conformity_problem_type==Conformity_problem_type::Loyal_VS_Agit_delayed){out<<"Loyal vs Agit delayed";}
	if (conformity_problem_type==Conformity_problem_type::Simple){out<<"Simple";}
	if (conformity_problem_type==Conformity_problem_type::Undefined){out<<"Undefined";}
	out<<endl;
	out<<"Conformity restrictions: ";
	if (conformity_restrictions==Conformity_restrictions::Restrict_nothing){out<<"restrict nothing";}
	if (conformity_restrictions==Conformity_restrictions::Restrict_to_active){out<<"restrict to active";}
	if (conformity_restrictions==Conformity_restrictions::Restrict_to_inactive){out<<"restrict to inactive";}
	out<<endl;
	out<<"Conformity at start: ";
	if (conformity_at_start==Conformity_At_Start::GEQStart){out << "GEQ";}
	if (conformity_at_start==Conformity_At_Start::LEQStart){out << "LEQ";}
	if (conformity_at_start==Conformity_At_Start::Undefined){out << "Undefined";}
	out<<endl;
	out<<"Conformity at end: ";
	if (conformity_at_end==Conformity_At_End::GEQEnd){out << "GEQ";}
	if (conformity_at_end==Conformity_At_End::LEQEnd){out << "LEQ";}
	if (conformity_at_end==Conformity_At_End::Undefined){out << "Undefined";}
	out <<endl;
	out<<"Start Value Percent: "<<Start_Value_Percent<<endl;
	out<<"End Value Percent: "<<End_Value_Percent<<endl;
	out<<"Has agitators: "<<Hasagitators<<endl;
	out<<"Has loyalists: "<<Hasloyalists<<endl;
	out<<"Number of agitators: "<<Number_of_agitators<<endl;
	out<<"Number of loyalists: "<<Number_of_loyalists<<endl;
	out<<"Neighbourhood_radius: "<<Neighbourhood_radius<<endl;
	out.close();
}
bool Conformity_problem::loadfromfile(string filename){
	ifstream in;
	in.open(filename.c_str());
	string s;
	while ( in.good() ){
		getline (in,s);
		int k=s.find(":");
		if (s.find("Conformity problem type")!=string::npos){
			string tmp=s.substr(k+2);
			if (tmp=="Agitated"){conformity_problem_type=Conformity_problem_type::Agitated;}
			if (tmp=="Loyal vs Agit"){conformity_problem_type=Conformity_problem_type::Loyal_VS_Agit;}
			if (tmp=="Loyal vs Agit delayed"){conformity_problem_type=Conformity_problem_type::Loyal_VS_Agit_delayed;}
			if (tmp=="Simple"){conformity_problem_type=Conformity_problem_type::Simple;}
			if (tmp=="Undefined"){conformity_problem_type=Conformity_problem_type::Undefined;}
		}		
		if (s.find("Conformity restrictions")!=string::npos){
			string tmp=s.substr(k+2);
			if (tmp=="restrict nothing"){conformity_restrictions=Conformity_restrictions::Restrict_nothing;}
			if (tmp=="restrict to active"){conformity_restrictions=Conformity_restrictions::Restrict_to_active;}
			if (tmp=="restrict to inactive"){conformity_restrictions=Conformity_restrictions::Restrict_to_inactive;}			
		}		
		if (s.find("Conformity at start")!=string::npos){
			string tmp=s.substr(k+2);
			if (tmp=="GEQ"){conformity_at_start=Conformity_At_Start::GEQStart;}
			if (tmp=="LEQ"){conformity_at_start=Conformity_At_Start::LEQStart;}
			if (tmp=="Undefined"){conformity_at_start=Conformity_At_Start::Undefined;}			
		}		
		if (s.find("Conformity at end")!=string::npos){
			string tmp=s.substr(k+2);
			if (tmp=="GEQ"){conformity_at_end=Conformity_At_End::GEQEnd;}
			if (tmp=="LEQ"){conformity_at_end=Conformity_At_End::LEQEnd;}
			if (tmp=="Undefined"){conformity_at_end=Conformity_At_End::Undefined;}			
		}		
		if (s.find("Start Value Percent")!=string::npos){
			string tmp=s.substr(k+2);
			Start_Value_Percent=strtoi(tmp);
		}
		if (s.find("End Value Percent")!=string::npos){
			string tmp=s.substr(k+2);
			End_Value_Percent=strtoi(tmp);
		}
		if (s.find("Has agitators")!=string::npos){
			string tmp=s.substr(k+2);
			int e=strtoi(tmp);
			if (e==0) {Hasagitators=false;}else {Hasagitators=true;}
		}
		if (s.find("Has loyalists")!=string::npos){
			string tmp=s.substr(k+2);
			int e=strtoi(tmp);
			if (e==0) {Hasloyalists=false;}else {Hasloyalists=true;}
		}
		if (s.find("Number of agitators")!=string::npos){
			string tmp=s.substr(k+2);
			Number_of_agitators = strtoi(tmp);
		}
		if (s.find("Number of loyalists")!=string::npos){
			string tmp=s.substr(k+2);
			Number_of_loyalists = strtoi(tmp);
		}
		if (s.find("Neighbourhood_radius")!=string::npos){
			string tmp=s.substr(k+2);
			Neighbourhood_radius = strtoi(tmp);
		}

	}
	in.close();
	return true;
}

Conformity_problem::Conformity_problem(){
	Start_Value_Percent=0;
	End_Value_Percent=0;
	Hasagitators=false;
	Hasloyalists=false;
	Number_of_agitators=0;
	Number_of_loyalists=0;
	Neighbourhood_radius=0;
}
class Conformity_Parameters{
public:
	void print (string filename);
	bool loadfromfile(string filename);
	Conformity_Parameters();
	int dimension;//number of vertices in graph
	int number_of_steps;
	Conformity_problem conformity_problem;
	Conformity_graph conformity_graph;
	double graph_parameter_1; //probability of an edge for GNP / probability of switching in WS
	int graph_parameter_2; //WS model - degree of vertex at start
	Conformity_conformitylevel_type conformity_conformitylevel_type;
	double conformitylevel_parameter; // threshold or leq.
	Conformity_conformists conformity_conformists;
	double conformists_parameter; // probability of "Conformists"
	
	Conformity_weights conformity_weights;
	int Weights_radius;
	Conformity_neighbourhood_type conformity_neighbourhood_type;	
};
void Conformity_Parameters::print(string filename){
	ofstream out;
	out.open(filename.c_str());
	out<<"Dimension: "<<dimension<<endl;
	out<<"Number of steps: "<<number_of_steps<<endl;
	out<<"Graph type: "; 
	if (conformity_graph==Conformity_graph::GNP_Graph){out<<"GNP";} 
	if (conformity_graph==Conformity_graph::WS_Graph){out<<"WS";} 
	out <<endl;
	out<<"Graph parameter 1: "<<graph_parameter_1<<endl;
	out<<"Graph parameter 2: "<<graph_parameter_2<<endl;
	out<<"Conformity level type: ";
	if (conformity_conformitylevel_type==Conformity_conformitylevel_type::RandomConformityLevel){ out<< "Random";}
	if (conformity_conformitylevel_type==Conformity_conformitylevel_type::ThresholdConformityLevel){ out<< "Threshold";}
	out<<endl;
	out<<"Conformitylevel parameter: " << conformitylevel_parameter<<endl;
	out<<"Conformists: ";
	if (conformity_conformists==Conformity_conformists::ConformistsOnly){out<< "conformists only";}
	if (conformity_conformists==Conformity_conformists::MixedConformists){out<< "mixed";}
	if (conformity_conformists==Conformity_conformists::NonConformistsOnly){out<< "nonconformists only";}
	out<<endl;
	out<<"Conformists parameter: "<<conformists_parameter<<endl;
	out<<"Conformity weights type: ";
	if (conformity_weights==Conformity_weights::Weights_at_random){out<< "weights at random";}
	if (conformity_weights==Conformity_weights::Weights_Decrease_with_distance){out<< "weights decrease with distance";}
	if (conformity_weights==Conformity_weights::Weights_Increase_with_distance){out<< "weights increase with distance";}
	out<<endl;
	out<<"Weights radius: "<<Weights_radius<<endl;
	out<<"Neighbourhood type: ";
	if (conformity_neighbourhood_type==Conformity_neighbourhood_type::AlwaysFullNeighborhood){out <<"always full neighborhood";}
	if (conformity_neighbourhood_type==Conformity_neighbourhood_type::From_Big_to_Small){out <<"from big to small";}
	if (conformity_neighbourhood_type==Conformity_neighbourhood_type::From_Small_to_Big){out <<"from small to big";}
	out<<endl;
	out.close();
}
bool Conformity_Parameters::loadfromfile(string filename){
	ifstream in;
	in.open(filename.c_str());
	string s;
	while ( in.good() ){
		getline (in,s);
		int k=s.find(":");
		if (s.find("Number of steps")!=string::npos){
			string tmp=s.substr(k+2);
			number_of_steps = strtoi(tmp);
		}
		if (s.find("Dimension")!=string::npos){
			string tmp=s.substr(k+2);
			dimension = strtoi(tmp);
		}
		if (s.find("Graph type")!=string::npos){
			string tmp=s.substr(k+2);
			if (tmp=="GNP"){conformity_graph=Conformity_graph::GNP_Graph;}
			if (tmp=="WS"){conformity_graph=Conformity_graph::WS_Graph;}			
		}		
		if (s.find("Graph parameter 1")!=string::npos){
			string tmp=s.substr(k+2);
			graph_parameter_1=atof(tmp.c_str());	
		}		
		if (s.find("Graph parameter 2")!=string::npos){
			string tmp=s.substr(k+2);
			graph_parameter_2=atoi(tmp.c_str());	
		}	
		if (s.find("Conformity level type")!=string::npos){
			string tmp=s.substr(k+2);
			if (tmp=="Random"){conformity_conformitylevel_type=Conformity_conformitylevel_type::RandomConformityLevel;}
			if (tmp=="Threshold"){conformity_conformitylevel_type=Conformity_conformitylevel_type::ThresholdConformityLevel;}
		}	
		if (s.find("Conformitylevel parameter")!=string::npos){
			string tmp=s.substr(k+2);
			conformitylevel_parameter=atof(tmp.c_str());	
		}	
		if (s.find("Conformists")!=string::npos){
			string tmp=s.substr(k+2);
			if (tmp=="conformists only"){conformity_conformists=Conformity_conformists::ConformistsOnly;}
			if (tmp=="mixed"){conformity_conformists=Conformity_conformists::MixedConformists;}
			if (tmp=="nonconformists only"){conformity_conformists=Conformity_conformists::NonConformistsOnly;}
		}	
		if (s.find("Conformists parameter")!=string::npos){
			string tmp=s.substr(k+2);
			conformists_parameter=atof(tmp.c_str());	
		}			
		if (s.find("Conformity weights type")!=string::npos){
			string tmp=s.substr(k+2);
			if (tmp=="weights at random"){conformity_weights=Conformity_weights::Weights_at_random;}
			if (tmp=="weights decrease with distance"){conformity_weights=Conformity_weights::Weights_Decrease_with_distance;}
			if (tmp=="weights increase with distance"){conformity_weights=Conformity_weights::Weights_Increase_with_distance;}
		}	
		if (s.find("Weights radius")!=string::npos){
			string tmp=s.substr(k+2);
			Weights_radius=atoi(tmp.c_str());	
		}	
		if (s.find("Neighbourhood type")!=string::npos){
			string tmp=s.substr(k+2);
			if (tmp=="always full neighborhood"){conformity_neighbourhood_type=Conformity_neighbourhood_type::AlwaysFullNeighborhood;}
			if (tmp=="from big to small"){conformity_neighbourhood_type=Conformity_neighbourhood_type::From_Big_to_Small;}
			if (tmp=="from small to big"){conformity_neighbourhood_type=Conformity_neighbourhood_type::From_Small_to_Big;}
		}	
	}
	in.close();
	return true;
}


Conformity_Parameters::Conformity_Parameters(){
		dimension=0;
		number_of_steps=0;
		graph_parameter_1=0;
		graph_parameter_2=0;
		conformitylevel_parameter=0;
		conformists_parameter=0;
		Weights_radius=0;
}
stringstream logstream;


class Conformity{
private:
	vector <int*> M;
	vector <int> Matrix;
	vector <int> WeightedMatrix;
	vector <int> vertexdegrees;
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
	::Conformity(Conformity_Parameters params);
	::Conformity(Conformity_Parameters params, string matrixfilename, string weightmatrixfilename);
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

	void GNPgraph(double p, int n);
	void WSgraph(int n, int k, double p);
	void Barabashi_graph(int n, int k);

	void make_conformity_levels(Conformity_conformitylevel_type conformity_type, double conformity_parameter);
	void make_conformists(Conformity_conformists conformists_type, double conformists_parameter);
	void make_weights_matrix(Conformity_weights conformity_weights, int weights_radius);
	int initializeconformity (double percent, double prob);


	vector <int> Row (int i);
	vector <int> Column (int j);
	int matrixelement(int i, int j){
		return Matrix[i*dimension+j];
	}
	void restrictagit(){
		for (int i=0;i<Curagit.size();i++){
			if (Curagit[i]==1){Mstream	<<	i+1	<<" 0"<<endl; nClauses++;}
			else {Mstream << - (i+1) <<" 0"<<endl; nClauses++;}
		}
	}
	void restrictloyal(){
		for (int i=0;i<Curloyal.size();i++){
			if (Curloyal[i]==1){Mstream	<<dimension+i+1	<<" 0"<<endl;}
			else {Mstream << - (dimension+i+1) <<" 0"<<endl;}
		}
	}
	void restrict_most_active(int n, bool agitated, bool loyaled){
		vector<int> r=get_most_significant(n);
		for (int i=0;i<r.size();i++){
			if (agitated){Mstream<<-(r[i]+1)<<" 0"<<endl; nClauses++;}
			if (loyaled){Mstream<<-(dimension+r[i]+1)<<" 0"<<endl; nClauses++;}						
		}
	}
	void construct_reachability_matrix (int radius);
	void construct_weights_matrix (int radius);
	vector<int> construct_neighbourhood(int i, int radius);
	vector<int> get_most_significant(int amount);
	vector<int> Sort(vector<int> a, int nV, int n){
		//nVars=nV;
		return HSort(a,n);
	}
	vector <int> HMerge (vector<int> &a, vector<int> &b, int n);
	vector <int> HSort (vector<int>& a, int n);
	vector <int> SHSort (vector<int> a, int lown, int n);
	vector <int> SMerge (vector<int> a, vector <int> b, int n);
	vector <int> Selectodd(vector<int> a);
	vector <int> Selecteven(vector<int> a);
	void onlyonesimple(vector<int> t);
	void onlyonecomplex(vector<int> t);
	void Newvareqor(int nv, vector<int> right);	
	vector<int> vecnewvareqor(const vector<int> r1, const vector<int>r2);
	void Newvareqand(int nv, vector<int> right);
	vector<int> vecnewvareqand(const vector<int> r1, const vector<int>r2);
	void equalize(vector<int>r1, vector<int>r2);
	void seqcounter(vector<int> a);

	bool calculate(vector<int> inp, int stepm);
	bool calculate(vector<int> inp, int stepm, string filename);
	
	bool calculate(vector<int> inp, Conformity_Parameters conf_params, Conformity_problem conf_problem, bool verbosity, string filename);
	
	void loadmatrixfromfile(int n, const char * filename);
	void savematrixtofile(const char * filename);
	void saveweightsmatrixtofile(const char* filename);
	void savematrixtofile_gv(const char * filename);
	void loadweightsmatrixfromfile(int n, string filename);
	
	void printprogresstofile_neato_gv(vector<int>agitators, vector<int> loyalists, vector<int> activity, const char *filename);
	
	string parsedotfile(string filename);
	void printprogresstofile_gv_dot(vector<int>agitators, vector<int> loyalists, vector<int> activity, string filename, string dotfilename);

	void printprogresstofile_gv(vector<int>agitators, vector<int> loyalists, vector<int> activity, const char *filename);

	void Print (const char * fn);
	void Printmatrix();
	
	void functioning( int step, bool agitated);
	void generalfunctioning( int step, bool agitated, bool loyaled, bool strict);
	void generalfunctioning(Conformity_problem c_p);

	void Notmoreatstart(int n, vector<int> tobesorted, bool agitators, bool loyalists);
	void Notlessatend(int n, vector<int> tobesorted);
	void Notmoreatend(int n, vector<int> tobesorted);
	void Fixedpoint();
	void Dump (char * fn);
	vector<int> loadssfromfile(const char * filename);	
};
Conformity::Conformity(Conformity_Parameters params){
	M.clear();
	Matrix.clear();
	WeightedMatrix.clear();
	Reachability.clear();
	conformitylevel.clear();
	conformism.clear();
	Curagit.clear();
	Curloyal.clear();
	nVars=0;
	nClauses=0;
	dimension=params.dimension;
	nofsteps=params.number_of_steps;

	//graph structure
	if (params.conformity_graph==Conformity_graph::GNP_Graph){
		GNPgraph(params.graph_parameter_1,params.dimension);
	}
	else if (params.conformity_graph==Conformity_graph::WS_Graph){
		WSgraph(params.dimension,params.graph_parameter_2,params.graph_parameter_1);
	}
	else if (params.conformity_graph==Conformity_graph::Barabashi_graph){
		Barabashi_graph(params.dimension,6);
	}

	//weights_matrix 
	make_weights_matrix(params.conformity_weights,params.Weights_radius);
	
	//conformity levels
	//when we introduce conformity_neighbourhood_types = assume that neighbourhood can change with time
	// if only in a limited way, then we need to rewrite this part.
	make_conformity_levels(params.conformity_conformitylevel_type,params.conformitylevel_parameter);
	
	// conformism
	make_conformists(params.conformity_conformists, params.conformists_parameter);
}
Conformity::Conformity(Conformity_Parameters params, string matrixfilename, string weightmatrixfilename){
	M.clear();
	Matrix.clear();
	WeightedMatrix.clear();
	Reachability.clear();
	conformitylevel.clear();
	conformism.clear();
	Curagit.clear();
	Curloyal.clear();
	nVars=0;
	nClauses=0;
	dimension=params.dimension;
	nofsteps=params.number_of_steps;
	loadmatrixfromfile(dimension, matrixfilename.c_str());
	loadweightsmatrixfromfile(dimension,weightmatrixfilename.c_str());	
}
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

void Conformity::loadweightsmatrixfromfile(int n, string filename){
	WeightedMatrix.clear();
	ifstream myfile;
	myfile.open(filename.c_str());
	string s;
	for (int t=0;t<n;t++){
		getline (myfile,s);
		int r1=0;
		int r2=0;
		for (int i=0;i<s.length();i++){
			if ((s[i]==' ')||(s[i]=='\n')){
				r2=i;
				if ((r2-r1)>0){
					string tmp=s.substr(r1,r2-r1);
					int t=strtoi(tmp);
					WeightedMatrix.push_back(t);	
					r1=r2;
				}
			}			
		  }
		}		
	if (WeightedMatrix.size()!=dimension*dimension){cout<<endl<<"Weighted Matrix size"<<Matrix.size()<<" is wrong "<<endl; }	  
	else{
		for (int i=0;i<dimension;i++){
			int t=0;
			for (int j=0;j<dimension;j++){
				t+=WeightedMatrix[j*dimension+i];
			}
			vertexdegrees.push_back(t);	
		}
	}
	myfile.close();
	
}

vector<int> Conformity::get_most_significant(int amount){
	vector<int> res;
	vector<int> d(vertexdegrees);
	for (int i=0;i<dimension;i++){res.push_back(i);}
	for(int i=0; i<dimension; i++){
		for(int j=i+1; j<dimension; j++){
		 if (d[j]>d[i]){
			 int tmp=d[j];
			 d[j]=d[i];
			 d[i]=tmp;
			 tmp=res[j];
			 res[j]=res[i];
			 res[i]=tmp;
		 }
		}
	}
	res.resize(amount);
	return res;
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
	if (conformitylevel.size()>0){
		myfile<<"Conformity level"<<endl;
		for (int i=0;i<dimension;i++){
			myfile<<conformitylevel[i]<<" ";
		}
		myfile<<endl;
	}
	if (conformism.size()>0){
		myfile<<"Conformism"<<endl;
		for (int i=0;i<dimension;i++){
			myfile<<conformism[i]<<" ";
		}	
	}
	myfile.close();
}
void Conformity::saveweightsmatrixtofile(const char* filename){
	ofstream myfile;
	myfile.open(filename);
	string s;
	for (int i=0;i<dimension;i++){
		for (int j=0;j<dimension;j++){
			myfile<<WeightedMatrix[dimension*i+j]<<" ";
		}
		myfile<<endl;
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
void Conformity::printprogresstofile_neato_gv(vector<int>agitators, vector<int> loyalists, vector<int> activity, const char *filename){
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
		out << "\tnode [shape=circle, fillcolor=lightskyblue, style=\"filled\"];";
	//	out << "\tnode [shape=circle];";
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
		out << "\tnode [shape=circle, fillcolor=lightsalmon, style=\"filled\"];";
		//out << "\tnode [shape=doublecircle];";
		for (int i = 0; i < activity.size(); i++)
		{
			if ((activity[i] == 1) && (agitators[i] == 0)){ out << " v_" << i + 1; }
		}
		out << ";" << endl;
	}
	if (hasagit){
		out << "\tnode [shape=circle, fillcolor=crimson, style=\"filled\"];";
		//out << "\tnode [shape=diamond];";
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
			out << "\tv_" << i + 1 << "[label=\"v_" << i + 1 << "\", pos=\"" << x << "," << y << "!\", shape = \"circle\"];" << endl;
		}
		else if (activity[i] == 1){
			out << "\tv_" << i + 1 << "[label=\"v_" << i + 1 << "\", pos=\"" << x << "," << y << "!\", shape = \"doublecircle\"];" << endl;
		}
		else { out << "\tv_" << i + 1 << "[label=\"v_" << i + 1 << "\", pos=\"" << x << "," << y << "!\", shape = \"circle\"];" << endl; }
		

		if ((agitators[i] == 0) && (loyalists[i] == 0)) { out << "\tinfo_" << i + 1 << "[label=\"" << curinput[i] << "(" << conformitylevel[i] << ")" <<"["<<vertexdegrees[i]<<"]"<< "\", pos=\"" << x*1.15 << "," << y*1.15 << "!\", shape = \"plaintext\"];" << endl; }
		else {
			if (agitators[i] == 1){ out << "\tinfo_" << i + 1 << "[label=\"" << "A" <<"["<<vertexdegrees[i]<<"]"<< "\", pos=\"" << x*1.15 << "," << y*1.15 << "!\", shape = \"plaintext\"];" << endl; }
		else 
		if (loyalists[i] == 1){ out << "\tinfo_" << i + 1 << "[label=\"" << "L"<<"["<<vertexdegrees[i]<<"]" << "\", pos=\"" << x*1.15 << "," << y*1.15 << "!\", shape = \"plaintext\"];" << endl; }

		}
	}
	

	for (int i = 0; i<dimension; i++){
		for (int j = 0; j<dimension; j++){
			if (matrixelement(i, j)>0){
				out << "\t" << "v_" << j + 1 << " -> v_" << i + 1 << " [ label=\"\",";
				switch (activity[j]){
				case 0: out << "color=\"green\","; break;
				case 1: out << "color=\"red\","; break;
				//case 0: out << "style=\"dashed\","; break;
				//case 1: out << "style=\"solid\","; break;
				}
				out << "arrowhead=\"normal\" ];" << endl;
			}
		}
	}
	out << "}" << endl;
	
	out.close();
}

void Conformity::printprogresstofile_gv(vector<int>agitators, vector<int> loyalists, vector<int> activity, const char *filename){
	ofstream out;
	out.open(filename);
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
		out << "\tnode [shape=circle, fillcolor=lightskyblue, style=\"filled\"];";
	//	out << "\tnode [shape=circle];";
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
		out << "\tnode [shape=circle, fillcolor=lightsalmon, style=\"filled\"];";
		//out << "\tnode [shape=doublecircle];";
		for (int i = 0; i < activity.size(); i++)
		{
			if ((activity[i] == 1) && (agitators[i] == 0)){ out << " v_" << i + 1; }
		}
		out << ";" << endl;
	}
	if (hasagit){
		out << "\tnode [shape=circle, fillcolor=crimson, style=\"filled\"];";
		//out << "\tnode [shape=diamond];";
		for (int i = 0; i < activity.size(); i++)
		{
			if (agitators[i] == 1){ out << " v_" << i + 1; }
		}
		out << ";" << endl;
	}
	
	for (int i = 0; i < dimension; i++){

	
		if (agitators[i] == 1){
			out << "\tv_" << i + 1 << "[label=<v<SUB>" << i + 1 << "</SUB>>, shape = \"circle\"];" << endl;
		}
		else if (activity[i] == 1){
			out << "\tv_" << i + 1 << "[label=<v<SUB>" << i + 1 << "</SUB>>, shape = \"doublecircle\"];" << endl;
		}
		else { out << "\tv_" << i + 1 << "[label=<v<SUB>" << i + 1 << "</SUB>>, shape = \"circle\"];" << endl; }
		

		if ((agitators[i] == 0) && (loyalists[i] == 0)) { out << "\tinfo_" << i + 1 << "[label=\"" << curinput[i] << "(" << conformitylevel[i] << ")" <<"["<<vertexdegrees[i]<<"]"<< "\", shape = \"plaintext\"];" << endl; }
		else {
			if (agitators[i] == 1){ out << "\tinfo_" << i + 1 << "[label=\"" << "A" <<"["<<vertexdegrees[i]<<"]"<< "\", shape = \"plaintext\"];" << endl; }
		else 
		if (loyalists[i] == 1){ out << "\tinfo_" << i + 1 << "[label=\"" << "L"<<"["<<vertexdegrees[i]<<"]" << "\", shape = \"plaintext\"];" << endl; }

		}
	}	

	for (int i = 0; i<dimension; i++){
		for (int j = 0; j<dimension; j++){
			if (matrixelement(i, j)>0){
				out << "\t" << "v_" << j + 1 << " -> v_" << i + 1 << " [ label=\"\",";
				switch (activity[j]){
				case 0: out << "color=\"green\","; break;
				case 1: out << "color=\"red\","; break;
				//case 0: out << "style=\"dashed\","; break;
				//case 1: out << "style=\"solid\","; break;
				}
				out << "arrowhead=\"normal\" ];" << endl;
			}
		}
	}
	out << "}" << endl;
	
	out.close();
}



string Conformity::parsedotfile(string filename){
	ifstream in;
	in.open(filename.c_str());
	string res;
	string buf;
	string s;
	vector<GVinfo> tr;
	bool blockread=true;
	bool accept=false;
	while (in.good()){
		getline (in,s);
		if ((s.find(" [")!=string::npos)&&(s.find('->')==string::npos)){blockread=true;}
		if ((s.find("[fillcolor")!=string::npos)&&(s.find('->')==string::npos)){accept=true;}
		if (blockread) res=res+s+'\n';
		if (accept) buf+=s;
		if (s.find("];")!=string::npos){blockread=false;accept=false;}
		if ((!accept)&&(buf.size()>0)){			
			while (buf.find('\t')!=string::npos){
				buf.replace((size_t)buf.find('\t'),1,"");
			}
			GVinfo tmp;
			tmp.parse(buf);
			tr.push_back(tmp);
			buf="";
		}
	}
	in.close();
	return res;
}

void Conformity::printprogresstofile_gv_dot (vector<int>agitators, vector<int> loyalists, vector<int> activity, string filename, string dotfilename){
	ofstream out;
	out.open(filename);
	string temp=parsedotfile(dotfilename);
	out<<temp.c_str();
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
	
	for (int i = 0; i<dimension; i++){
		for (int j = 0; j<dimension; j++){
			if (matrixelement(i, j)>0){
				out << "\t" << "v_" << j + 1 << " -> v_" << i + 1 << " [ label=\"\",";
				switch (activity[j]){
				case 0: out << "color=\"green\","; break;
				case 1: out << "color=\"red\","; break;
				//case 0: out << "style=\"dashed\","; break;
				//case 1: out << "style=\"solid\","; break;
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
	if (radius>1){
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
	}
	Reachability=t;
}
void Conformity::make_weights_matrix(Conformity_weights conformity_weights, int weights_radius){
	
	vector<int> t;
	if (conformity_weights == Conformity_weights::Weights_at_random){
		construct_reachability_matrix(1);
		int RM=RAND_MAX;
		srand (time(NULL));
		for(int i=0;i<Reachability.size();i++){
			if (Reachability[i]==0) {
				t.push_back(0);
			}
			else {
				t.push_back((int)(rand()*(weights_radius-1)/RM+1));	
			}		
		}	
	}
	if (conformity_weights == Conformity_weights::Weights_Decrease_with_distance){
		construct_reachability_matrix(weights_radius);
		for(int i=0;i<Reachability.size();i++){
			if (Reachability[i]==0) {
				t.push_back(0);
			}
			else {
				if ((weights_radius - Reachability[i]+1)>0){
					t.push_back(weights_radius -Reachability[i]+1);
				}
				else{t.push_back(0);}
			}		
		}	
	}
	if (conformity_weights == Conformity_weights::Weights_Increase_with_distance){
		construct_reachability_matrix(weights_radius);
		for(int i=0;i<Reachability.size();i++){
			if (Reachability[i]==0) {
				t.push_back(0);
			}
			else {
				if ((Reachability[i]>0)&&(Reachability[i]<=weights_radius)){
					t.push_back(Reachability[i]);
				}
				else{t.push_back(0);}
			}		
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
vector<int> Conformity::construct_neighbourhood(int i, int radius){
	vector<int> res;
	for (int j=0;j<dimension;j++){
			if (WeightedMatrix[j*dimension+i]<=radius){
				for (int h=0;h<WeightedMatrix[j*dimension+i];h++){
					res.push_back(j);
				}
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

bool Conformity::calculate(vector<int> inp, Conformity_Parameters conf_params, Conformity_problem conf_problem, bool verbosity, string filename){
	vector<int> agitators;
	vector<int> loyalists;
	vector<int> curstep;
	vector<int> nextstep;
	string logfile=filename;
	ofstream log_out;
	GV_graph gvg;
	string newpathname=filename+"_detailed\\";
	
	string bat_filename=newpathname+"make_images.bat";
	ofstream out_bat;
	
	if (verbosity) {
		std::wstring stemp = s2ws(newpathname);
		LPCWSTR result = stemp.c_str();
		CreateDirectory(result, NULL);
		out_bat.open(bat_filename);
		out_bat<<"cd /d "<<newpathname<<endl;
	}

	bool agitated=conf_problem.Hasagitators; // true if we have agitators
	bool loyaled=conf_problem.Hasloyalists;  // true if we have loyalists

	log_out.open(logfile.c_str());
	
	bool a = true;
	int reservation=0;
	if (agitated) reservation++;
	if (loyaled) reservation++;
	if (!agitated&loyaled) reservation=1;
	for (int i = 0; i<dimension; i++){
		if (agitated) agitators.push_back(inp[i]); else agitators.push_back(0);
		if (loyaled) loyalists.push_back(inp[dimension + i]); else loyalists.push_back(0);
		curstep.push_back(inp[dimension * reservation + i]);
	}

	vector<int>curinput;
	for (int i = 0; i < dimension; i++){		
			curinput.push_back(0);		
	}

	if (verbosity==true) {
		cout << endl << "Agitators" << endl;
		log_out << endl << "Agitators" << endl;
		for (int d = 0; d<dimension; d++){
			cout << agitators[d] << " ";
			log_out<< agitators[d] << " ";
		}
		cout << endl << "loyalists" << endl;
		log_out << endl << "loyalists" << endl;
		for (int d = 0; d<dimension; d++){
			cout << loyalists[d] << " ";
			log_out<< loyalists[d] << " ";
		}
	}
	for (int i = 0; i<dimension; i++){
		Curagit.push_back(agitators[i]);
		Curloyal.push_back(loyalists[i]);
	}
	//curstep=inp;
	if (verbosity==true){
		cout << endl << "Step 0" << endl;
		log_out << endl << "Step 0" << endl;
		for (int d = 0; d<dimension; d++){
			cout << curstep[d] << " ";
			log_out << curstep[d] << " ";			
		}
		if (verbosity){
			string tmp = "step_0.txt";
			string tmpimg = "step_0.dot";
			string tmpimg2 = "step_0.png";
			printprogresstofile_gv(agitators, loyalists, curstep, (newpathname+tmp).c_str());
			out_bat<<"sfdp -Tdot "<<tmp<<" -o "<<tmpimg<<endl;			
			out_bat.close();
			system(bat_filename.c_str());
			gvg.parse(newpathname+tmpimg);
			gvg.loadedges(Matrix,dimension);
			gvg.loaddata(agitators,loyalists,conformitylevel,vertexdegrees);
			gvg.refresh(curinput,curstep);
			gvg.print(newpathname+tmp);

		//	printprogresstofile_gv_dot(agitators, loyalists, curstep, (newpathname+tmp).c_str(),newpathname+"step_0.dot");
			out_bat.open(bat_filename);
			out_bat<<"cd /d "<<newpathname<<endl;
			out_bat<<"sfdp -Tpng "<<tmp<<" -o "<<tmpimg2<<endl;
		}
		log_out << endl;
		cout << endl;
	}

	for (int k = 0; k<conf_params.number_of_steps; k++){
		nextstep.clear();
		curinput.clear();
		for (int i = 0; i<dimension; i++){
			int cursum = 0;
			int deg = 0;
			for (int j = 0; j<dimension; j++){
				deg += WeightedMatrix[j*dimension+i];
			}
			for (int j = 0; j<dimension; j++){
				if ((curstep[j] == 1)) cursum+=WeightedMatrix[j*dimension+i];
				//if ((matrixelement(i, j) == 1) && (curstep[i] == 1)) cursum++;
			}
			curinput.push_back(cursum);
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
			if (curstep[i] != inp[(reservation + k + 1)*dimension + i]){
				cout << endl << "Correctness on step " << k + 1 << " point  " << i << " is failed" << endl;
				log_out << endl << "Correctness on step " << k + 1 << " point  " << i << " is failed" << endl;
				b = false;
				a = false;
			}
		}
		if (verbosity==true){			
			if (verbosity){
				string tmp =  "step_"+inttostr(k+1)+".txt";
				string tmpimg = "step_"+inttostr(k+1)+".png";				
				gvg.refresh(curinput,curstep);
				gvg.print(newpathname+tmp);
				//printprogresstofile_gv_dot(agitators, loyalists, curstep, (newpathname+tmp).c_str(), newpathname+"step_0.dot");			
				out_bat<<"sfdp -Tpng "<<tmp<<" -o "<<tmpimg<<endl;
			}

			cout << endl << "Step " << k + 1 << endl;
			log_out << endl << "Step " << k + 1 << endl;
			for (int d = 0; d<dimension; d++){
				cout << curstep[d] << " ";
				log_out << curstep[d] << " ";
			}
			cout << endl;
			log_out << endl;
		}
		if (b == true){
			cout << endl << "Step " << k + 1 << " is correct" << endl;
			log_out << endl << "Step " << k + 1 << " is correct" << endl;
		}
	}
	if (verbosity) {
		out_bat.close();
		system(bat_filename.c_str());
	}
	log_out.close();
	
	return a;	
	
}

void Conformity::Notmoreatstart(int n, vector<int> tobesorted, bool agitators, bool loyalists){
	
	const int nzero=++nVars; //zero variable
	int * tmp;
	tmp=new(int[2]);
	tmp[0]=-nzero;
	tmp[1]=0;
	Mstream<<tmp[0]<<" "<<tmp[1]<<endl;
	nClauses++;
	//M.push_back(tmp);
	int hcnt=twoceil(tobesorted.size());
	for (int i=tobesorted.size();i<hcnt;i++){tobesorted.push_back(nzero);}
	vector<int> r;
	r=HSort(tobesorted,hcnt);
	for (int i=0;i<r.size();i++){
		Mstream<<r[i]<<" ";
	}
	Mstream<<"0"<<endl;
	nClauses++;

	tmp=new (int[2]);
	tmp[0]=-r[n];
	tmp[1]=0;
	//M.push_back(tmp);
	Mstream<<tmp[0]<<" "<<tmp[1]<<endl;
	nClauses++;
}
void Conformity::Notlessatend(int n, vector<int> tobesorted){
	const int nzero=++nVars; //zero variable
	int * tmp;
	tmp=new(int[2]);
	tmp[0]=-nzero;
	tmp[1]=0;
	//M.push_back(tmp);
	Mstream<<tmp[0]<<" "<<tmp[1]<<endl;
	nClauses++;
	int hcnt=twoceil(tobesorted.size());
	for (int i=tobesorted.size();i<hcnt;i++){tobesorted.push_back(nzero);}
	vector<int> r;
	r=HSort(tobesorted,hcnt);
	for (int i=0;i<r.size();i++){
		Mstream<<r[i]<<" ";
	}
	Mstream<<"0"<<endl;
	nClauses++;
	tmp=new(int[2]);
	tmp[0]=r[n];
	tmp[1]=0;
	//M.push_back(tmp);
	Mstream<<tmp[0]<<" "<<tmp[1]<<endl;
	nClauses++;
}
void Conformity::Notmoreatend(int n, vector<int> tobesorted){
	const int nzero=++nVars; //zero variable
	int * tmp;
	tmp=new(int[2]);
	tmp[0]=-nzero;
	tmp[1]=0;
	//M.push_back(tmp);
	Mstream<<tmp[0]<<" "<<tmp[1]<<endl;
	nClauses++;	
	int hcnt=twoceil(tobesorted.size());
	for (int i=tobesorted.size();i<hcnt;i++){tobesorted.push_back(nzero);}
	vector<int> r;
	r=HSort(tobesorted,hcnt);
	for (int i=0;i<r.size();i++){
		Mstream<<r[i]<<" ";
	}
	Mstream<<"0"<<endl;
	nClauses++;
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
	cout<<endl<<"Graph matrix "<<endl;
	for (int i=0;i<dimension;i++){
		for (int j=0;j<dimension;j++){
			cout<<Matrix[i*dimension+j]<<" ";
		}
		cout<<endl;
	}
}


void Conformity::WSgraph (int n, int k, double prob){
	vector<int>wsmatrix;
	//construct a regular ring
	for (int i=0;i<n;i++){
		for (int j=0;j<n;j++){
			int tmp;
			tmp=abs(i-j)%(n-(k/2));
			if ((tmp>=0)&&(tmp<=k/2)) {
				wsmatrix.push_back(1);
			}
			else {
				wsmatrix.push_back(0);
			}
		}
	}	
	//ring ready in theory..
	for (int i=0;i<n;i++){
		cout<<endl;
		for (int j=0;j<n;j++){
			cout<<wsmatrix[i*n+j]<<" ";
		}
	}
	
	int RM=RAND_MAX;
	srand (time(NULL));
//	cout<<endl<<"Number of 1s is " << countones(Matrix)<<endl;
	//here we should morph the ring to something..
	vector<int>newmatrix;
	for (int i=0;i<dimension;i++){
		for (int j=0;j<dimension;j++){
			newmatrix.push_back(wsmatrix[i*dimension+j]);
		}
	}
	
	for (int i=0;i<dimension;i++){
		for (int j=0;j<dimension;j++){
		    if ((wsmatrix[i*dimension+j]==1)&&(rand()<prob*RM)){
				//rewire to any other vertex with uniform distribution, apparently
				bool b=false;
				while (!b){
					int tmp = rand()%dimension;
					if ((newmatrix[i*dimension+tmp]==0)&&(tmp!=i)){
						newmatrix[i*dimension+tmp]=newmatrix[i*dimension+j];
						newmatrix[i*dimension+j]=0;						
						b=true;
					}
				}
			}
		}
	}

	cout<<endl;
	Matrix=newmatrix;

	for (int i=0;i<dimension;i++){
		cout<<endl;
		for (int j=0;j<dimension;j++){
			cout<<Matrix[i*dimension+j]<<" ";
		}
	}	
	cout<<endl<<"Graph matrix "<<endl;
	for (int i=0;i<dimension;i++){
		for (int j=0;j<dimension;j++){
			cout<<Matrix[i*dimension+j]<<" ";
		}
		cout<<endl;
	}
}
void Conformity::Barabashi_graph(int n, int k){
	dimension=n;
	nVars=n;
	nClauses=0;
	vector<int> degrees;
	int degsum=0;
	for (int i=0;i<n;i++){
		degrees.push_back(0);
		for (int j=0;j<n;j++){
			Matrix.push_back(0);
		}	
	}
	//initialize simple structure of k elements first
	for (int i=0;i<k;i++){
		Matrix[i*n+i+1]=1;
		Matrix[(i+1)*n+i]=1;
		degrees[i]++;
		degrees[i+1]++;
		degsum++;
	}
	
	int RM=RAND_MAX;
	srand (time(NULL));
	for (int i=k;i<n;i++){
		bool connected=false;
		for (int j=0;j<i;j++){
			double t=double(rand())/RM;
			double tr=double(degrees[j])/degsum;			
			if (t<tr){
				connected=true;
				degrees[j]++;
				degrees[i]++;
				degsum+=2;
				Matrix[j*n+i]=1;
				Matrix[i*n+j]=1;
			}
		}
		if (!connected) i--;
	}
	cout<<endl<<"Graph matrix "<<endl;
	for (int i=0;i<dimension;i++){
		for (int j=0;j<dimension;j++){
			cout<<Matrix[i*dimension+j]<<" ";
		}
		cout<<endl;
	}
}
void Conformity::make_conformity_levels(Conformity_conformitylevel_type conformity_type, double conformity_parameter){
	if ((conformity_parameter>1)||(conformity_parameter<0)) exit;
	int RM=RAND_MAX;
	srand (time(NULL));
	
	cout<<endl<<"Vertex degrees"<<endl;
	
	for (int i=0;i<dimension;i++){
		int t=0;
		for (int j=0;j<dimension;j++){
			t+=WeightedMatrix[j*dimension+i];
		}
		vertexdegrees.push_back(t);
		cout<<t<<" ";

		if (conformity_type == Conformity_conformitylevel_type::RandomConformityLevel){
			conformitylevel.push_back(rand()*t/RM);		
			if (conformitylevel[i]==0){conformitylevel[i]=1;}
			//cout<<"degree = "<<t<<", CLevel= "<<conformitylevel[i]<<endl;
		}
		else if (conformity_type == Conformity_conformitylevel_type::ThresholdConformityLevel){
			conformitylevel.push_back(t*conformity_parameter+1);
		}	
	}
	cout<<endl<<"Conformity levels "<<endl;
	for (int i=0;i<dimension;i++){
		cout<<conformitylevel[i]<<" ";
	}
}
void Conformity::make_conformists(Conformity_conformists conformists_type, double conformists_parameter){
	
	if (conformists_type==Conformity_conformists::ConformistsOnly){
		for (int i=0;i<dimension;i++){
			conformism.push_back(1);
		}
	}
	if (conformists_type==Conformity_conformists::NonConformistsOnly){
		for (int i=0;i<dimension;i++){
			conformism.push_back(0);
		}
	}
	if (conformists_type==Conformity_conformists::MixedConformists){
		int RM=RAND_MAX;
		for (int i=0;i<dimension;i++){
			if (rand()<(conformists_parameter*RM)){conformism.push_back(1);} else {conformism.push_back(0);}
		}		
	}
	cout<<endl<<"Conformists"<<endl;
	for (int i=0;i<dimension;i++){
		cout<<conformism[i]<<" ";
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
		vertexdegrees.push_back(t);
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
			nClauses++;
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
		nClauses++;
		//M.push_back(a);
	}
}

void Conformity::generalfunctioning(int step, bool agitated, bool loyaled, bool strict){
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
			neighbours=construct_neighbourhood(i,1);
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



void Conformity::generalfunctioning(Conformity_problem c_p){
	Mstream.clear();
	vector<int> agitators;
	vector<int> loyalists;
	vector<int> firststep;
		

	bool agitated=c_p.Hasagitators; // true if we have agitators
	bool loyaled=c_p.Hasloyalists;  // true if we have loyalists
	bool strict; // true if say only agitators should be active
	
	int reservation_k=1;
	if (agitated) reservation_k=2;
	if (loyaled) reservation_k=3;

	for (int i=0;i<dimension;i++){
		agitators.push_back(i+1);
		loyalists.push_back(-(dimension+i+1)); //notice the minus!!!
		firststep.push_back(dimension*(reservation_k-1)+i+1);
	}

	nVars = (nofsteps+reservation_k)*dimension; // reserve first variables to be something meaningful
	const int nzero=++nVars; //zero variable
	Mstream<<-nzero<<" 0"<<endl;
	nClauses++;

	vector<int> curstep_vars;
	//zero step = initialization required if agitated or loyaled !=0
	//we also need to restrict simultaneous agitation and loyalization, do we?

	for (int i=0;i<dimension;i++){
		if (agitated && loyaled ) {	
			Mstream<<-agitators[i]<<" "<<loyalists[i]<<" 0"<<endl; 
			nClauses++;
		} //restriction
		
		if (agitated){
			Mstream<<-agitators[i]<<" "<<firststep[i]<<" 0"<<endl;
			nClauses++;
			if (c_p.conformity_restrictions==Conformity_restrictions::Restrict_to_inactive){
					Mstream<<agitators[i]<<" "<<-firststep[i]<<" 0"<<endl;
					nClauses++;
			}
		}// if agitated then 1
		if (loyaled ){
			Mstream<<loyalists[i]<<" "<<-firststep[i]<<" 0"<<endl;
			nClauses++;
			if (c_p.conformity_restrictions==Conformity_restrictions::Restrict_to_active){
				Mstream<<-loyalists[i]<<" "<<firststep[i]<<" 0"<<endl;
				nClauses++;
			}
			//if (strict){Mstream<<-loyalists[i]<<" "<<firststep[i]<<" 0"<<endl;}
		}// if loyaled then 0
	}

 	for (int k=0;k<nofsteps;k++){//steps
		curstep_vars.clear();
		for (int i=0;i<dimension;i++){//vertices
			//build the neighbours list for the i-th vertex;
			vector<int> neighbours;
			neighbours=construct_neighbourhood(i,c_p.Neighbourhood_radius);
			//count "1s" in this list
			//to do this we should add to the list variables (meaningless) that are equal to 0s from the start
			// for sorting nets technique to be sound and correct
			int neighbourssize=neighbours.size();	
			int hcnt=twoceil(neighbours.size());
			vector<int> tobesorted;
			for (int v=0;v<neighbours.size();v++){tobesorted.push_back((k+reservation_k-1)*dimension+neighbours[v]+1);}
			for (int v=neighbours.size();v<hcnt;v++){tobesorted.push_back(nzero);}			
			vector<int> sorted;
			if (tobesorted.size()>1){
				sorted=HSort(tobesorted,hcnt);	
			}
			else 
			{
				sorted=tobesorted;
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
				//if (neighbourssize<=conformitylevel[i]){cout<<"PROBLEM HERE "<<endl;}
				newvarnum=-sorted[neighbourssize-conformitylevel[i]];
			}
			curstep_vars.push_back(newvarnum);
			neighbours.clear();
			sorted.clear();
			sorted.~vector();
			tobesorted.clear();
			tobesorted.~vector();
			
		}	
		vector<int>newstep;
		for (int l=0;l<dimension;l++){
			newstep.push_back(dimension*(k+reservation_k)+l+1);
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

	int at_start=dimension*c_p.Start_Value_Percent/100;
	int at_end=dimension*c_p.End_Value_Percent/100;
	vector<int> laststep;	
	for (int i=0;i<dimension;i++){
		laststep.push_back(dimension*(nofsteps + reservation_k-1)+i+1);
		loyalists[i]=-loyalists[i]; // tricky thing it is
	}

	if (c_p.conformity_at_start==Conformity_At_Start::GEQStart){
		if (agitated==true){
			Notmoreatstart(c_p.Number_of_agitators,agitators, 1,0);
		}
		if (loyaled==true){
			Notmoreatstart(c_p.Number_of_loyalists,loyalists, 0,1);
		}
	}
	else if (c_p.conformity_at_start==Conformity_At_Start::LEQStart){
		Notmoreatstart(at_start, firststep, 0,0);
		if (agitated==true){
			Notmoreatstart(c_p.Number_of_agitators,agitators, 1,0);
		}
		if (loyaled==true){		
			Notmoreatstart(c_p.Number_of_loyalists,loyalists, 0,1);
		}
	}	
	
	if (c_p.conformity_at_end==Conformity_At_End::GEQEnd){
		Notlessatend(at_end,laststep);
	}
	else if (c_p.conformity_at_end==Conformity_At_End::LEQEnd){
		Notmoreatend(at_end, laststep);
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

void runtests_Plos(int noftests, Conformity_Parameters params, Conformity_problem prob_first, Conformity_problem prob_second, string pathname, int restrictions_agit, int restrictions_loyal){
	std::wstring stemp = s2ws(pathname);

	string logfilename=pathname+"correctness_check_log.txt";
	ofstream out;
	out.open(logfilename);

	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);
		
	string param_file=pathname+"parameters.txt";
	params.print(param_file);

	string problem_param_file=pathname+"problem.txt";
	prob_first.print(problem_param_file);

	string filename_template;
	if (params.conformity_graph==Conformity_graph::GNP_Graph){filename_template="gnp";}
	if (params.conformity_graph==Conformity_graph::WS_Graph){filename_template="ws";}
	filename_template+=inttostr(params.dimension)+"_";
	filename_template+="0"+inttostr(params.graph_parameter_1*10)+"_";
	
	string filename_template_loyal=filename_template+"_lvsad_";

	if (params.conformity_graph==Conformity_graph::WS_Graph){filename_template+=inttostr(params.graph_parameter_2)+"_";}
	if (params.conformity_weights==Conformity_weights::Weights_at_random){filename_template+="rndw";}
	if (params.conformity_weights==Conformity_weights::Weights_Decrease_with_distance){filename_template+="decw";}
	if (prob_first.conformity_problem_type==Conformity_problem_type::Simple){filename_template+="_simp";}
	if (prob_first.conformity_problem_type==Conformity_problem_type::Agitated){filename_template+="_agit";}
	for (int i=0;i<noftests;i++){
			Conformity r(params);
			r.generalfunctioning(prob_first);
			r.restrict_most_active(restrictions_agit,true,false);
			string r_matrixfilename = pathname+filename_template+"_matrix_" + inttostr(i + 1) + ".txt";
			string r_gvfilename=pathname+filename_template+"_gv_"+inttostr(i+1)+".dot";
			string r_weightsmatrixfilename = pathname+filename_template+"_weights_" + inttostr(i + 1) + ".txt";
			r.savematrixtofile(r_matrixfilename.c_str());
			r.savematrixtofile_gv(r_gvfilename.c_str());
			r.saveweightsmatrixtofile(r_weightsmatrixfilename.c_str());
			string cnffilename = pathname+ filename_template+"_"+inttostr(i + 1) + ".cnf";
			r.Print(cnffilename.c_str());
			string cnffilename_out = pathname+ filename_template+"_"+inttostr(i + 1) + ".out";
			string cmd_str="d:\\tests\\cryptominisat32.exe "+cnffilename+ " "+cnffilename_out;
			system(cmd_str.c_str());

			vector<int> o = r.loadssfromfile(cnffilename_out.c_str());						
			if (o.size()==0){
				cout<<endl<<"No solution"<<endl;
				out<<"No solution"<<endl;
			}	
			else {
				vector<int>b;
				for (int j=0;j<params.dimension*(params.number_of_steps+3);j++){
						if (o[j]<0){b.push_back(0);}else {b.push_back(1);}
				}

				string tmp=pathname+filename_template+"_"+inttostr(i+1)+"_process_log";
				bool t=r.calculate(b,params,prob_first,true,tmp.c_str());
				if (t==true){
					cout<<"Solution is correct"<<endl;
					out<<"Solution is correct"<<endl;
					}
				else {
					cout<<"Solution is incorrect"<<endl;
					out<<"Solution is incorrect"<<endl;
				}

				r.generalfunctioning(prob_second);
				r.restrictagit();	
				r.restrict_most_active(restrictions_loyal, false,true);
				string loyaledcnffilename=pathname+filename_template_loyal+"_"+inttostr(i+1)+".cnf";
				string loyaledcnffilename_out=pathname+filename_template_loyal+"_"+inttostr(i+1)+".out";
				r.Print(loyaledcnffilename.c_str());
				string cmd_str_loyal="d:\\tests\\cryptominisat32.exe "+loyaledcnffilename+ " "+loyaledcnffilename_out;
				system(cmd_str_loyal.c_str());
				vector<int> o2= r.loadssfromfile(loyaledcnffilename_out.c_str());						
				if (o2.size()==0){
					cout<<endl<<"No solution"<<endl;
					out<<"No solution"<<endl;
				}	
				else {
					vector<int>b;
					for (int j=0;j<params.dimension*(params.number_of_steps+3);j++){
							if (o2[j]<0){b.push_back(0);}else {b.push_back(1);}
					}

					string tmp=pathname+filename_template_loyal+"_"+inttostr(i+1)+"_process_log";
					bool t=r.calculate(b,params,prob_second,true,tmp.c_str());
					if (t==true){
						cout<<"Solution is correct"<<endl;
						out<<"Solution is correct"<<endl;
						}
					else {
						cout<<"Solution is incorrect"<<endl;
						out<<"Solution is incorrect"<<endl;
					}
				}
		}
	}
	out.close();
}
void gentests_gen(int noftests, Conformity_Parameters params, Conformity_problem prob, string pathname){
	std::wstring stemp = s2ws(pathname);
	
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);
	
	string bat_filename=pathname+"run_tests.bat";
	ofstream out_bat;
	out_bat.open(bat_filename);
	out_bat<<"cd /d "<<pathname<<endl;
	
	string param_file=pathname+"parameters.txt";
	params.print(param_file);

	string problem_param_file=pathname+"problem.txt";
	prob.print(problem_param_file);

	string filename_template;
	if (params.conformity_graph==Conformity_graph::GNP_Graph){filename_template="gnp";}
	if (params.conformity_graph==Conformity_graph::WS_Graph){filename_template="ws";}
	filename_template+=inttostr(params.dimension)+"_";
	filename_template+="0"+inttostr(params.graph_parameter_1*10)+"_";
	if (params.conformity_graph==Conformity_graph::WS_Graph){filename_template+=inttostr(params.graph_parameter_2)+"_";}
	if (params.conformity_weights==Conformity_weights::Weights_at_random){filename_template+="rndw";}
	if (params.conformity_weights==Conformity_weights::Weights_Decrease_with_distance){filename_template+="decw";}
	if (prob.conformity_problem_type==Conformity_problem_type::Simple){filename_template+="_simp";}
	if (prob.conformity_problem_type==Conformity_problem_type::Agitated){filename_template+="_agit";}
	for (int i=0;i<noftests;i++){
			Conformity r(params);
			r.generalfunctioning(prob);
			r.restrict_most_active(5,true,false);
			string r_matrixfilename = pathname+filename_template+"_matrix_" + inttostr(i + 1) + ".txt";
			string r_gvfilename=pathname+filename_template+"_gv_"+inttostr(i+1)+".dot";
			string r_weightsmatrixfilename = pathname+filename_template+"_weights_" + inttostr(i + 1) + ".txt";
			r.savematrixtofile(r_matrixfilename.c_str());
			r.savematrixtofile_gv(r_gvfilename.c_str());
			r.saveweightsmatrixtofile(r_weightsmatrixfilename.c_str());
			string cnffilename = pathname+ filename_template+"_"+inttostr(i + 1) + ".cnf";
			r.Print(cnffilename.c_str());
			out_bat<<"cryptominisat32.exe "<<cnffilename<< " "<<pathname+ filename_template+"_"+inttostr(i + 1) +"_out"<<endl;
	}
	out_bat.close();
}
void gentests_gen_loyaled(int noftests, Conformity_problem prob_loyal, string sourcepathname, string newpathname){
	
	std::wstring stemp = s2ws(newpathname);
	
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);

	string logfilename=sourcepathname+"correctness_check_log.txt";
	ofstream out;
	out.open(logfilename);

	Conformity_Parameters params;
	Conformity_problem prob;
	string param_file=sourcepathname+"parameters.txt";
	params.loadfromfile(param_file);
	string problem_param_file=sourcepathname+"problem.txt";
	prob.loadfromfile(problem_param_file);
	
	string param_file_new=newpathname+"parameters.txt";
	params.print(param_file_new);
	string problem_param_file_new=newpathname+"problem.txt";
	prob.print(problem_param_file_new);
	
	prob_loyal.Neighbourhood_radius=prob.Neighbourhood_radius;

	string loyal_problem_param_file;
	if (prob_loyal.conformity_problem_type==Conformity_problem_type::Loyal_VS_Agit){loyal_problem_param_file=newpathname+"problem_loyal.txt";}
	if (prob_loyal.conformity_problem_type==Conformity_problem_type::Loyal_VS_Agit_delayed){loyal_problem_param_file=newpathname+"problem_loyal_del.txt";}
	prob_loyal.print(loyal_problem_param_file);

	string filename_template;
	if (params.conformity_graph==Conformity_graph::GNP_Graph){filename_template="gnp";}
	if (params.conformity_graph==Conformity_graph::WS_Graph){filename_template="ws";}
	filename_template+=inttostr(params.dimension)+"_";
	filename_template+="0"+inttostr(params.graph_parameter_1*10)+"_";
	if (params.conformity_graph==Conformity_graph::WS_Graph){filename_template+=inttostr(params.graph_parameter_2)+"_";}
	if (params.conformity_weights==Conformity_weights::Weights_at_random){filename_template+="rndw";}
	if (params.conformity_weights==Conformity_weights::Weights_Decrease_with_distance){filename_template+="decw";}
	string filename_template_loyal=filename_template;
	if (prob_loyal.conformity_problem_type==Conformity_problem_type::Loyal_VS_Agit){filename_template_loyal+="_loyalvsagit";}
	if (prob_loyal.conformity_problem_type==Conformity_problem_type::Loyal_VS_Agit_delayed){filename_template_loyal+="_loyalvsagit_del";}
	
	if (prob.conformity_problem_type==Conformity_problem_type::Simple){filename_template+="_simp";}
	if (prob.conformity_problem_type==Conformity_problem_type::Agitated){filename_template+="_agit";}
	
	for (int i=0;i<noftests;i++){
			string r_matrixfilename = sourcepathname+filename_template+"_matrix_" + inttostr(i + 1) + ".txt";
			string r_weightsmatrixfilename = sourcepathname+filename_template+"_weights_" + inttostr(i + 1) + ".txt";
			string sat_solutionfilename=sourcepathname+"minisat_simp_"+filename_template+"_"+inttostr(i + 1)+"_cnf.txt";
			Conformity r(params,r_matrixfilename,r_weightsmatrixfilename);

			string r_matrixfilename_new = newpathname+filename_template+"_matrix_" + inttostr(i + 1) + ".txt";
			string r_weightsmatrixfilename_new = newpathname+filename_template+"_weights_" + inttostr(i + 1) + ".txt";
			r.savematrixtofile(r_matrixfilename_new.c_str());
			r.saveweightsmatrixtofile(r_weightsmatrixfilename_new.c_str());
			
			vector<int> o= r.loadssfromfile(sat_solutionfilename.c_str());						
			if (o.size()==0){
				cout<<endl<<"No solution"<<endl;
				out<<"No solution"<<endl;
			}	
			else {
				vector<int>b;
				for (int j=0;j<params.dimension*(params.number_of_steps+3);j++){
						if (o[j]<0){b.push_back(0);}else {b.push_back(1);}
				}
				string tmp=sourcepathname+filename_template+"_"+inttostr(i+1)+"_process.log";
				bool t=r.calculate(b,params,prob,true,tmp.c_str());
				if (t==true){
					cout<<"Solution is correct"<<endl;
					out<<"Solution is correct"<<endl;
					}
				else {
					cout<<"Solution is incorrect"<<endl;
					out<<"Solution is incorrect"<<endl;
				}
				r.generalfunctioning(prob_loyal);
				r.restrictagit();	
				string loyaledcnfname;
				if (prob_loyal.conformity_problem_type==Conformity_problem_type::Loyal_VS_Agit){loyaledcnfname=newpathname+filename_template_loyal+"_"+inttostr(i+1)+".cnf";}
				if (prob_loyal.conformity_problem_type==Conformity_problem_type::Loyal_VS_Agit_delayed){loyaledcnfname=newpathname+filename_template_loyal+"_"+inttostr(i+1)+".cnf";}
				r.Print(loyaledcnfname.c_str());
			}
	}
	out.close();
}
void checktests_gen(int noftests,  string pathname){
	string logfilename=pathname+"correctness_check_log.txt";
	ofstream out;
	out.open(logfilename);

	Conformity_Parameters params;
	Conformity_problem prob;
	string param_file=pathname+"parameters.txt";
	params.loadfromfile(param_file);
	string problem_param_file=pathname+"problem.txt";
	prob.loadfromfile(problem_param_file);

	string filename_template;
	if (params.conformity_graph==Conformity_graph::GNP_Graph){filename_template="gnp";}
	if (params.conformity_graph==Conformity_graph::WS_Graph){filename_template="ws";}
	filename_template+=inttostr(params.dimension)+"_";
	filename_template+="0"+inttostr(params.graph_parameter_1*10)+"_";
	if (params.conformity_graph==Conformity_graph::WS_Graph){filename_template+=inttostr(params.graph_parameter_2)+"_";}
	if (params.conformity_weights==Conformity_weights::Weights_at_random){filename_template+="rndw";}
	if (params.conformity_weights==Conformity_weights::Weights_Decrease_with_distance){filename_template+="decw";}
	if (prob.conformity_problem_type==Conformity_problem_type::Simple){filename_template+="_simp";}
	if (prob.conformity_problem_type==Conformity_problem_type::Agitated){filename_template+="_agit";}
	string prefix="";
	string postfix="_out";
	//string prefix="minisat_simp_";
	//string postfix="_cnf.txt";
	for (int i=0;i<noftests;i++){
			string r_matrixfilename = pathname+filename_template+"_matrix_" + inttostr(i + 1) + ".txt";
			string r_weightsmatrixfilename = pathname+filename_template+"_weights_" + inttostr(i + 1) + ".txt";
			string sat_solutionfilename=pathname+prefix+filename_template+"_"+inttostr(i + 1)+postfix;
			Conformity r(params,r_matrixfilename,r_weightsmatrixfilename);
			vector<int> o= r.loadssfromfile(sat_solutionfilename.c_str());						
			if (o.size()==0){
				cout<<endl<<"No solution"<<endl;
				out<<"No solution"<<endl;
			}	
			else {
				vector<int>b;
				for (int j=0;j<params.dimension*(params.number_of_steps+3);j++){
						if (o[j]<0){b.push_back(0);}else {b.push_back(1);}
				}
				string tmp=pathname+filename_template+"_"+inttostr(i+1)+"_process.log";
				bool t=r.calculate(b,params,prob,true,tmp.c_str());
				if (t==true){
						cout<<"Solution is correct"<<endl;
						out<<"Solution is correct"<<endl;
				}
					else {
						cout<<"Solution is incorrect"<<endl;
						out<<"Solution is incorrect"<<endl;
					}
			}
	}
	out.close();
}
void checktests_gen_loyaled(int noftests,  string pathname){
	string logfilename=pathname+"correctness_check_log.txt";
	ofstream out;
	out.open(logfilename);

	Conformity_Parameters params;
	Conformity_problem prob;
	Conformity_problem prob_loyal;
	string param_file=pathname+"parameters.txt";
	params.loadfromfile(param_file);
	string problem_param_file=pathname+"problem.txt";
	prob.loadfromfile(problem_param_file);
	string loyal_problem_param_file;
	loyal_problem_param_file=pathname+"problem_loyal_del.txt";
	prob_loyal.loadfromfile(loyal_problem_param_file);

	string filename_template;
	if (params.conformity_graph==Conformity_graph::GNP_Graph){filename_template="gnp";}
	if (params.conformity_graph==Conformity_graph::WS_Graph){filename_template="ws";}
	filename_template+=inttostr(params.dimension)+"_";
	filename_template+="0"+inttostr(params.graph_parameter_1*10)+"_";
	if (params.conformity_graph==Conformity_graph::WS_Graph){filename_template+=inttostr(params.graph_parameter_2)+"_";}
	if (params.conformity_weights==Conformity_weights::Weights_at_random){filename_template+="rndw";}
	if (params.conformity_weights==Conformity_weights::Weights_Decrease_with_distance){filename_template+="decw";}
	string filename_template_loyal=filename_template;
	if (prob_loyal.conformity_problem_type==Conformity_problem_type::Loyal_VS_Agit){filename_template_loyal+="_loyalvsagit";}
	if (prob_loyal.conformity_problem_type==Conformity_problem_type::Loyal_VS_Agit_delayed){filename_template_loyal+="_loyalvsagit_del";}
	
	if (prob.conformity_problem_type==Conformity_problem_type::Simple){filename_template+="_simp";}
	if (prob.conformity_problem_type==Conformity_problem_type::Agitated){filename_template+="_agit";}
	
	for (int i=0;i<noftests;i++){
			string r_matrixfilename = pathname+filename_template+"_matrix_" + inttostr(i + 1) + ".txt";
			string r_weightsmatrixfilename = pathname+filename_template+"_weights_" + inttostr(i + 1) + ".txt";
			string sat_solutionfilename=pathname+"minisat_simp_"+filename_template_loyal+"_"+inttostr(i + 1)+"_cnf.txt";
			
			Conformity r(params,r_matrixfilename,r_weightsmatrixfilename);
			vector<int> o = r.loadssfromfile(sat_solutionfilename.c_str());						
			if (o.size()==0){
				cout<<endl<<"No solution"<<endl;
				out<<"No solution"<<endl;
			}	
			else {
				vector<int>b;
				for (int j=0;j<params.dimension*(params.number_of_steps+3);j++){
						if (o[j]<0){b.push_back(0);}else {b.push_back(1);}
				}
				string tmp=pathname+filename_template+"_"+inttostr(i+1)+"_process.log";
				bool t=r.calculate(b,params,prob_loyal,true,tmp.c_str());
				if (t==true){
						cout<<"Solution is correct"<<endl;
						out<<"Solution is correct"<<endl;
				}
					else {
						cout<<"Solution is incorrect"<<endl;
						out<<"Solution is incorrect"<<endl;
					}
			}
	}
	out.close();
}

int main (){
	Conformity ttt;
	/*ttt.Barabashi_graph(50,2);
	ttt.savematrixtofile("D:\\tests\\Conformity\\barabashi50.txt");
	ttt.savematrixtofile_gv("D:\\tests\\Conformity\\barabashi_gv50.txt");*/


	Conformity_Parameters p;
	p.dimension=40;
	p.number_of_steps=10;
	p.conformity_graph=Conformity_graph::Barabashi_graph;
	p.graph_parameter_1=0.3;
	p.graph_parameter_2=20;
	p.conformity_conformitylevel_type=Conformity_conformitylevel_type::RandomConformityLevel;
	p.conformitylevel_parameter=0;
	p.conformity_conformists=Conformity_conformists::ConformistsOnly;
	p.conformity_weights=Conformity_weights::Weights_Decrease_with_distance;
	//p.conformity_weights=Conformity_weights::Weights_at_random;
	p.conformists_parameter=0;
	p.Weights_radius=1;
	//p.Weights_radius=4;
	p.conformity_neighbourhood_type=Conformity_neighbourhood_type::AlwaysFullNeighborhood;
	
	int agitators_percent=20;
	int loyalists_percent=30;

	Conformity_problem simple_problem(Conformity_problem_type::Simple, 30, 80, 0, 0, p.Weights_radius);
	Conformity_problem agitated(Conformity_problem_type::Agitated, 20, 80, p.dimension*agitators_percent/100, 0, p.Weights_radius);

	//for these two problems we actually need the agitators configuration obtained from some previous work. so....
	//Conformity_problem (Conformity_problem_type p_t, int startvalue, int endvalue, int nofagit, int nofloyal, int neighbourhood_radius)
	int nagit=p.dimension*agitators_percent/100;
	int nloyal=p.dimension*loyalists_percent/100;
	Conformity_problem loyaled_vs_agit(Conformity_problem_type::Loyal_VS_Agit, 100-loyalists_percent, agitators_percent, nagit, nloyal, p.Weights_radius);
	Conformity_problem loyaled_vs_agit_delayed(Conformity_problem_type::Loyal_VS_Agit_delayed, 100-loyalists_percent, agitators_percent, nagit, nloyal, p.Weights_radius);
	
	runtests_Plos(10,p,agitated,loyaled_vs_agit_delayed,"D:\\tests\\Conformity\\Barabashi_40_test\\",5,0);
	p.dimension=30;
	runtests_Plos(10,p,agitated,loyaled_vs_agit_delayed,"D:\\tests\\Conformity\\Barabashi_30_test\\",5,0);

	/*
	gentests_gen(10,p,agitated,"D:\\tests\\Conformity\\Barabashi_100_agit\\");
	system("D:\\tests\\Conformity\\Barabashi_100_agit\\run_tests.bat");
	checktests_gen(10,"D:\\tests\\Conformity\\Barabashi_100_agit\\");

	//need to resolve asap.
	
	
	
	/*	
	p.graph_parameter_1=0.2;
	p.conformity_graph=Conformity_graph::WS_Graph;
	
	gentests_gen(10,p,agitated,"D:\\Tests_backup\\26.03.14\\ws200_02_20_decw_agit\\");
	p.graph_parameter_1=0.4;
	gentests_gen(10,p,agitated,"D:\\Tests_backup\\26.03.14\\ws200_04_20_decw_agit\\");

	
	
	p.Weights_radius=3;
	p.conformity_weights=Conformity_weights::Weights_at_random;
	agitated.Neighbourhood_radius=p.Weights_radius;
	p.graph_parameter_1=0.2;
	gentests_gen(10,p,agitated,"D:\\Tests_backup\\26.03.14\\ws200_02_20_rndw_agit\\");
	p.graph_parameter_1=0.4;
	gentests_gen(10,p,agitated,"D:\\Tests_backup\\26.03.14\\ws200_04_20_rndw_agit\\");
	

	//gentests_gen_loyaled(10,loyaled_vs_agit_delayed,"D:\\Tests_backup\\gnp200_03_decw_agit\\","D:\\Tests_backup\\gnp200_03_decw_lvsa_del\\");
	//gentests_gen_loyaled(10,loyaled_vs_agit_delayed,"D:\\Tests_backup\\gnp200_03_rndw_agit\\","D:\\Tests_backup\\gnp200_03_rndw_lvsa_del\\");
	//gentests_gen_loyaled(10,loyaled_vs_agit_delayed,"D:\\Tests_backup\\ws200_03_20_decw_agit\\","D:\\Tests_backup\\ws200_03_20_decw_lvsa_del\\");
	//gentests_gen_loyaled(10,loyaled_vs_agit_delayed,"D:\\Tests_backup\\ws200_03_20_rndw_agit\\","D:\\Tests_backup\\ws200_03_20_rndw_lvsa_del\\");

	
	/*
	gentests_gen(10,p,simple_problem,"D:\\shared_folder\\Тесты_конформность\\new_tests_9\\");
	
	gentests_gen(10,p,agitated,"D:\\shared_folder\\Тесты_конформность\\new_tests_10\\");
	
	p.conformity_graph=Conformity_graph::WS_Graph;
	
	gentests_gen(10,p,simple_problem,"D:\\shared_folder\\Тесты_конформность\\new_tests_11\\");
	
	gentests_gen(10,p,agitated,"D:\\shared_folder\\Тесты_конформность\\new_tests_12\\");
	
	p.Weights_radius=3;
	p.conformity_weights=Conformity_weights::Weights_at_random;
	p.conformity_graph=Conformity_graph::GNP_Graph;

	simple_problem.Neighbourhood_radius=p.Weights_radius;
	agitated.Neighbourhood_radius=p.Weights_radius;
	
	gentests_gen(10,p,simple_problem,"D:\\shared_folder\\Тесты_конформность\\new_tests_13\\");
	
	gentests_gen(10,p,agitated,"D:\\shared_folder\\Тесты_конформность\\new_tests_14\\");
	
	p.conformity_graph=Conformity_graph::WS_Graph;
	
	gentests_gen(10,p,simple_problem,"D:\\shared_folder\\Тесты_конформность\\new_tests_15\\");
	
	gentests_gen(10,p,agitated,"D:\\shared_folder\\Тесты_конформность\\new_tests_16\\");
	
	/*

	
	string	test_to_be_checked_matrix="D:\\shared_folder\\Тесты_конформность\\agit_batch1\\matrix_1.txt";
	string	test_to_be_checked_ss="D:\\shared_folder\\Тесты_конформность\\agit_batch1\\agit_1.out";

	Conformity a(p,test_to_be_checked_matrix);

	vector<int> o = a.loadssfromfile(test_to_be_checked_ss.c_str());
	if (o.size()==0){
		cout<<endl<<"No solution"<<endl;
		logstream<<"No solution"<<endl;
	}	
	else {
		vector<int>b;
		for (int i=0;i<p.dimension*(p.number_of_steps+3);i++){
				if (o[i]<0){b.push_back(0);}else {b.push_back(1);}
		}
		bool t=a.calculate(b,p,agitated,true,"D:\\shared_folder\\Тесты_конформность\\agit_batch1\\agit_1_process.txt");
		if (t==true){
			cout<<"Solution is correct"<<endl;
		}
			else {
				cout<<"Solution is incorrect"<<endl;
			}
	}		

	/*
	string cnfinputfilename="D:\\tests\\Conformity\\test1.cnf";
	a.generalfunctioning(agitated);
	a.Print(cnfinputfilename.c_str());
	string testpath="D:\\tests\\Conformity\\agit_batch3\\";
	
	std::wstring stemp = s2ws(testpath);
	LPCWSTR result = stemp.c_str();
	CreateDirectory(result, NULL);
	
	string param_file=testpath+"parameters.txt";
	p.print(param_file);

	string problem_param_file=testpath+"problem.txt";
	agitated.print(problem_param_file);

	for (int i=0;i<p.number_of_steps;i++){
			Conformity r(p);
			r.generalfunctioning(agitated);
			string r_matrixfilename = testpath+"matrix_" + inttostr(i + 1) + ".txt";
			r.savematrixtofile(r_matrixfilename.c_str());
			string cnffilename = testpath+ "conf_" + inttostr(i + 1) + ".cnf";
			r.Print(cnffilename.c_str());
	}

	*/

	/*string cnfoutputfilename="D:\\tests\\Conformity\\test1.out";
	string cryptolog="D:\\tests\\Conformity\\crypto.log";
	string tempprogress = "D:\\tests\\Conformity\\test1_tempprogress.out";

	string cryptominisat_call= "D:\\tests\\Conformity\\cryptominisat32.exe "+cnfinputfilename+" "+cnfoutputfilename +" > "+cryptolog;
	system(cryptominisat_call.c_str());
	vector<int> e = a.loadssfromfile(cnfoutputfilename.c_str());
		if (e.size()==0){
			cout<<endl<<"No solution"<<endl;
			logstream<<"No solution"<<endl;
		}	
		else {
			vector<int>b;
			for (int i=0;i<p.dimension*(p.number_of_steps+3);i++){
					if (e[i]<0){b.push_back(0);}else {b.push_back(1);}
			}
			bool t=a.calculate(b,p,simple_problem,true, tempprogress.c_str());
			if (t==true){
				cout<<"Solution is correct"<<endl;
			}
				else {
					cout<<"Solution is incorrect"<<endl;
				}
		}		
		*/
	int q;
	cin>>q;
}