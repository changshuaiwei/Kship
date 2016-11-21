#include "indi.h"

indi::indi()
{
	birthyear=0;
	sex='.';
	indi_ID=0;
	M_ID=0;
	F_ID=0;
	inner_ID=0;
	Fam_ID=0;
	M_addr=0;
	F_addr=0;
	children.clear();
	tmp_child=0;
}

indi::~indi()
{
	birthyear=0;
	sex='.';
	indi_ID=0;
	M_ID=0;
	F_ID=0;
	inner_ID=0;
	Fam_ID=0;
	M_addr=0;
	F_addr=0;
	children.clear();
	tmp_child=0;
}