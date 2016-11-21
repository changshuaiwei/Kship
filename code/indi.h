#ifndef _INDI_H
#define _INDI_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class indi
{
public:
	indi();
	~indi();
	int birthyear;
	char sex;
	int indi_ID;
	int M_ID;
	int F_ID;
	int inner_ID;
	int Fam_ID;
	indi * M_addr;
	indi * F_addr;
	vector<indi*> children;
	indi * tmp_child;//for construct tree
};

#endif