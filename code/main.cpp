
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

#include "indi.h"
#include "kinship.h"

using namespace std;

void con_kinship(int argc, char* argv[]);

void main(int argc, char* argv[])
{
	con_kinship(argc, argv);

	system("pause");
}

void con_kinship(int argc, char* argv[])
{
/*
#kinship coefficient config file!

#data file
_ori_file data.txt

#result file
_out_file res.txt

#seed
_seed 2009

#how many kinship should calculate
_size 300

#whether to add fam id
_FamID Yes

#if not calculate kinship for relatives(within two generation)
_ex_relative No

#if random sampled(Yes or No)
_all Yes

#year range
_year 1800 1900

#if use file to select people
_id_sel Yes

#file name for inclusion
_id_file indi_id.txt

*/
	argc=1;
	argv[1]=new char[1024];
	cout<<"\nthis the kinship coefficient calculator.\n";
	cout<<"\nplease input name of config file:";
	cin>>argv[1];

	string ori_file, out_file;
	int seed, size, year_min, year_max;
	bool all_selected=false;
	bool id_select=false;
	bool ex_relative=false;
	bool FamID=false;
	string id_file;


	ifstream rd(argv[1]);
	string buf_str;
	vector<string> vec_str;

	while(getline(rd, buf_str)){
		if(buf_str.size()==0) continue;
		if(buf_str[0]=='#') continue;

		kinship::split_string(buf_str, vec_str);

		if(vec_str[0]=="_ori_file") ori_file=vec_str[1];
		else if(vec_str[0]=="_out_file") out_file=vec_str[1];
		else if(vec_str[0]=="_id_file") id_file=vec_str[1];
		else if(vec_str[0]=="_seed") seed=atoi(vec_str[1].c_str());
		else if(vec_str[0]=="_size") size=atoi(vec_str[1].c_str());
		else if(vec_str[0]=="_all") {
			if(vec_str[1]=="Yes") all_selected=true;
			else all_selected=false;
		}
		else if(vec_str[0]=="_id_sel"){
			if(vec_str[1]=="Yes") id_select=true;
			else id_select=false;
		}
		else if(vec_str[0]=="_FamID"){
			if(vec_str[1]=="Yes") FamID=true;
			else FamID=false;
		}
		else if(vec_str[0]=="_ex_relative"){
			if(vec_str[1]=="Yes") ex_relative=true;
			else ex_relative=false;
		}
		else if(vec_str[0]=="_year"){
			year_min=atoi(vec_str[1].c_str());
			year_max=atoi(vec_str[2].c_str());
		}
		
	}

	kinship kins;
	kins.readfile(ori_file);

	if(FamID){
		kins.assign_FamID();
		kins.dataWithFamID();
	}

	if(id_select){
		kins.restrict_id(id_file);
	}
	else {
		kins.restrict_bith(year_min, year_max);
	}

	if(all_selected){
		kins.all_kinship(seed,out_file);
	}else{
		kins.random_kinship(size, seed, out_file);
	}

	cout<<"\nmission complete, have fun!";
}

