#ifndef _KINSHIP_H
#define _KINSHIP_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <limits>

#include "indi.h"

using namespace std;

class kinship
{
public:
	kinship();
	~kinship();
	void readfile(string filename);
	void restrict_bith(int year_min, int year_max);
	void random_kinship(int size, int seed, string outfile, bool ex_relative=false);
	void all_kinship(int seed, string outfile, bool ex_relative=false);
	void restrict_id(string id_file);


	static long split_string(const string &str, vector <string> &vec_str, string separator=" ,\t;\n");
	void common_ans(vector<indi*> & ans, indi* indiv1, indi* indiv2);
	void cal_rout(vector< vector<indi*> > & routs, indi* indiv, indi* ans);
	double cal_kinship(indi* indiv1, indi* indiv2);
	double cal_inbreeding(indi* indiv);
	double ran1(int &idum);

	void assign_FamID();
	void dataWithFamID();

private:
	void construct_pedi();
	
	bool surrounding_FamID(indi * id, int & count);
	void ancestor(vector<indi*> & ans, indi* indiv);
	void ancestor(vector<indi*> & ans, indi* indiv, int generation);
	void search_tree(indi* tmp, indi *aim, indi* root);
	double rout_coeff(vector< vector<indi*> > & routs1, vector< vector<indi*> > & routs2);
	bool compare_rout(vector<indi*> & rout1, vector<indi*> & rout2);
	void sample(int num, bool ex_relative=false);
	void sample_all(bool ex_relative=false);
	bool check_two_indi(indi* indiv1, indi* indiv2);//check two individual if they are relatives(within two generation)
	string _ori_file;
	string _out_file;
	vector<indi> _ori_data;
	int _largest_indi_ID;
	int _seed;

	vector<string> _header;
	vector< vector<indi*> > _tmp_routs;

	vector<indi*> _restricted;
	
	vector<indi*> _indiv1;
	vector<indi*> _indiv2;
	vector<double> _kinsp;
	vector<int> _indi_id_cand;
};


#endif