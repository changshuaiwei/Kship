#include "kinship.h"

kinship::kinship()
{
	_largest_indi_ID=0;
	_seed=0;
}

kinship::~kinship()
{
	_largest_indi_ID=0;
	_seed=0;
}

void kinship::readfile(string filename)
{
	_ori_file=filename;

	ifstream rd(filename.c_str());
	string buf_str;
	vector<string> vec_str;

	getline(rd, buf_str);
	split_string(buf_str, _header);
	int max=0;
	int i=0;

	while(getline(rd, buf_str))
	{
		if(buf_str.size()==0) continue;
		if(buf_str[0]=='#') continue;

		split_string(buf_str, vec_str);

		indi tmp;
		if(vec_str[0]!=".") tmp.indi_ID=atoi(vec_str[0].c_str());
		if(vec_str[1]!=".") tmp.sex=vec_str[1][0];
		if(vec_str[2]!=".") tmp.birthyear=atoi(vec_str[2].c_str());
		if(vec_str[3]!=".") tmp.F_ID=atoi(vec_str[3].c_str());
		if(vec_str[4]!=".") tmp.M_ID=atoi(vec_str[4].c_str());

		if(tmp.indi_ID>max){
			max=tmp.indi_ID;
		}
		tmp.inner_ID=i;

		_ori_data.push_back(tmp);
		i++;
	}
	_largest_indi_ID=max;

	construct_pedi();
}

void kinship::dataWithFamID()
{
	string fn="fam."+_ori_file;
	ofstream wt(fn.c_str());

	wt<<"Fml_id\tIndid\tSex\tYbirth\tFatid\tMotid\n";
	for(int i=0; i<_ori_data.size(); i++){
		wt<<_ori_data[i].Fam_ID<<"\t"
			<<_ori_data[i].indi_ID<<"\t"
			<<_ori_data[i].sex<<"\t"
			<<_ori_data[i].birthyear<<"\t"
			<<_ori_data[i].F_ID<<"\t"
			<<_ori_data[i].M_ID<<"\n";
	}
}

void kinship::restrict_bith(int year_min, int year_max)
{
	_restricted.clear();
	int i,j;
	for(i=0; i<_ori_data.size(); i++){
		if(_ori_data[i].birthyear>=year_min && _ori_data[i].birthyear <= year_max){
			_restricted.push_back(&_ori_data[i]);
		}
	}
}

void kinship::restrict_id(string id_file)
{
	cout<<"\nrestricted individual to selected id\n";
	_indi_id_cand.clear();
	ifstream rd(id_file.c_str());
	string buf_str;
	while (getline(rd,buf_str))
	{
		if(buf_str.size()==0) continue;
		else if(buf_str[0]=='\0' || buf_str[0]=='#') continue;

		int tmp_id=atoi(buf_str.c_str());
		_indi_id_cand.push_back(tmp_id);
	}

	_restricted.clear();
	int i,j;
	for(i=0; i<_ori_data.size(); i++){
		for(j=0; j<_indi_id_cand.size(); j++){
			if(_ori_data[i].indi_ID==_indi_id_cand[j]){
				_restricted.push_back(&_ori_data[i]);
				break;
			}

		}
	}

}

void kinship::all_kinship(int seed, string outfile, bool ex_relative)
{
	_seed=seed;
	_out_file=outfile;
	cout<<"\nnow get sample of pairs...";
	sample_all(ex_relative);
	cout<<"\nsample got\n";
	int i, j;
	double ksp=0;
	for(i=0; i<_indiv1.size(); i++){
		cout<<"\ncalculate the coefficient between "<<_indiv1[i]->indi_ID<<" and "<<_indiv2[i]->indi_ID<<"...";
		ksp=cal_kinship(_indiv1[i], _indiv2[i]);
		_kinsp.push_back(ksp);
		cout<<"= "<<ksp<<"\n";
	}

	cout<<"\nnow output result.....";
	ofstream wt(outfile.c_str());
	wt<<"#this is the result file for kinship coefficient\n\n";

	wt<<"indiv1\tindiv2\tkinship\n";
	for(i=0; i<_indiv1.size(); i++){
		wt<<_indiv1[i]->indi_ID<<"\t"<<_indiv2[i]->indi_ID<<"\t"<<_kinsp[i]<<"\n";
	}
}

void kinship::random_kinship(int size, int seed, string outfile, bool ex_relative)
{
	_seed=seed;
	_out_file=outfile;
	cout<<"\nnow get sample of pairs...";
	sample(size, ex_relative);
	cout<<"\nsample got\n";
	int i, j;
	double ksp=0;
	for(i=0; i<size; i++){
		cout<<"\ncalculate the coefficient between "<<_indiv1[i]->indi_ID<<" and "<<_indiv2[i]->indi_ID<<"...";
		ksp=cal_kinship(_indiv1[i], _indiv2[i]);
		_kinsp.push_back(ksp);
	}

	cout<<"\nnow output result.....";
	ofstream wt(outfile.c_str());
	wt<<"#this is the result file for kinship coefficient\n\n";

	wt<<"indiv1\tindiv2\tkinship\n";
	for(i=0; i<size; i++){
		wt<<_indiv1[i]->indi_ID<<"\t"<<_indiv2[i]->indi_ID<<"\t"<<_kinsp[i]<<"\n";
	}
}
void kinship::sample_all(bool ex_relative)
{
	_indiv1.clear(); _indiv2.clear();

	int t_size=_restricted.size();
	int idx1, idx2, i, j, k;
	k=0;
	if(ex_relative){
		for(i=0; i<t_size; i++){
			idx1=i;
			for(j=i+1; j<t_size; j++){
				idx2=j;
				if(check_two_indi(_restricted[idx1],_restricted[idx2])){
					_indiv1.push_back(_restricted[idx1]);
					_indiv2.push_back(_restricted[idx2]);
					k++;
					if((k%1000)==0){
						cout<<k<<"th pairs\t";
					}
				}
			}
		}
	}else{
		for(i=0; i<t_size; i++){
			idx1=i;
			for(j=i+1; j<t_size; j++){
				idx2=j;
				if(_restricted[idx1]->inner_ID!=_restricted[idx2]->inner_ID){
					_indiv1.push_back(_restricted[idx1]);
					_indiv2.push_back(_restricted[idx2]);
					k++;
					if((k%1000)==0){
						cout<<k<<"th pairs\t";
					}
				}
			}
		}
	}
	

	cout<<"\ntotoally "<<k<<" pairs needed calculated\n";
}

void kinship::sample(int num, bool ex_relative)
{
	_indiv1.clear(); _indiv2.clear();
	
	int t_size=_restricted.size();
	int idx1, idx2, i, j;
	if(ex_relative){
		for(i=0; i<num; i++){
			do{
				do{
					idx1=int(ran1(_seed)*t_size);
				}while(idx1>=_restricted.size());

				do 
				{
					idx2=int(ran1(_seed)*t_size);
				} while (idx2>=_restricted.size() || idx2==idx1);

			}while(!(check_two_indi(_restricted[idx1],_restricted[idx2])));

			_indiv1.push_back(_restricted[idx1]);
			_indiv2.push_back(_restricted[idx2]);
		}
	}else{
		for(i=0; i<num; i++){
			do{
				do{
					idx1=int(ran1(_seed)*t_size);
				}while(idx1>=_restricted.size());

				do 
				{
					idx2=int(ran1(_seed)*t_size);
				} while (idx2>=_restricted.size() || idx2==idx1);

			}while(_restricted[idx1]->inner_ID==_restricted[idx2]->inner_ID);

			_indiv1.push_back(_restricted[idx1]);
			_indiv2.push_back(_restricted[idx2]);
		}
	}
}

double kinship::cal_kinship(indi* indiv1, indi* indiv2)
{
	vector<indi*> ans;
	common_ans(ans, indiv1, indiv2);
	if(ans.size()==0) return 0;
	
	int i, j;
	double ksp=0;
	vector< vector<indi*> > routs1, routs2;
	for(i=0; i<ans.size(); i++){
		//cout<<"\nget the rout for the first one";
		cal_rout(routs1, indiv1, ans[i]);
		//cout<<"\nget the rout for the second one";
		cal_rout(routs2, indiv2, ans[i]);
		//cout<<"\ncalculate inbreeding";
		double ibrd=cal_inbreeding(ans[i]);
		//cout<<"\ncalculate coeff";
		double coeff=rout_coeff(routs1, routs2);
		ksp+=(1.0+ibrd)*coeff;
	}
	return ksp;
}

double kinship::cal_inbreeding(indi* indiv)
{
	if(indiv->F_addr==0 || indiv->M_addr==0){
		return 0;
	}else{
		double ibrd=cal_kinship(indiv->F_addr, indiv->M_addr);
		return ibrd;
	}
}

double kinship::rout_coeff(vector< vector<indi*> > & routs1, vector< vector<indi*> > & routs2)
{
	int i, j;
	double coeff=0;
	for(i=0; i<routs1.size(); i++){
		for(j=0; j<routs2.size(); j++){
			if(compare_rout(routs1[i], routs2[j])){
				double rout_lgth=routs1[i].size()+routs2[j].size()+1.0;
				coeff+=pow(0.5, rout_lgth);
			}
		}
	}

	return coeff;
}

bool kinship::compare_rout(vector<indi*> & rout1, vector<indi*> & rout2)//no common nodes on the two rout
{
	bool different=true;

	vector<int> indi_ID(_largest_indi_ID+1, 0);
	int i, j;
	for(i=0; i<rout1.size(); i++){
		indi_ID[rout1[i]->indi_ID]=1;
	}
	for(i=0; i<rout2.size(); i++){
		if(indi_ID[rout2[i]->indi_ID]==1){
			different=false;
			break;
		}
	}
	if(rout1.size()==0 && rout2.size()==0) different=false;	

	return different;
}

void kinship::cal_rout(vector< vector<indi*> > & routs, indi* indiv, indi* ans)
{
	indi* root=indiv;
	indi* tmp=root;
	indi* aim=ans;
	_tmp_routs.clear();

	search_tree(tmp, aim, root);

	routs=_tmp_routs;
	_tmp_routs.clear();
}

void kinship::search_tree(indi* tmp, indi* aim, indi* root)
{
	if(tmp==aim){
		indi* tmp0=tmp;
		vector<indi*> tmp_rout;
		while(tmp0!=root){
			tmp0=tmp0->tmp_child;
			tmp_rout.push_back(tmp0);
		}
		_tmp_routs.push_back(tmp_rout);
		return;
	}else{ 
		if(tmp->F_addr!=0){
			tmp->F_addr->tmp_child=tmp;
			search_tree(tmp->F_addr, aim, root);
		}
		if(tmp->M_addr!=0){
			tmp->M_addr->tmp_child=tmp;
			search_tree(tmp->M_addr, aim, root);
		}
	}
}

void kinship::common_ans(vector<indi*> & ans, indi* indiv1, indi* indiv2)
{
	vector<indi*> ans1, ans2;

	if(indiv1->inner_ID==indiv2->inner_ID) return;
	
	ancestor(ans1, indiv1);
	ancestor(ans2, indiv2);

	int i, j;
	vector<int> tmp_inner_ID;
	tmp_inner_ID.resize(_ori_data.size()+1);


	for(i=0; i<ans1.size(); i++){
		int tmpid=ans1[i]->inner_ID;
		tmp_inner_ID[tmpid]=1;
	}

	for(i=0; i<ans2.size(); i++){
		int tmpid=ans2[i]->inner_ID;
		if(tmp_inner_ID[tmpid]==1){
			ans.push_back(ans2[i]);
		}
	}

	ans1.clear(); ans2.clear();
}

bool kinship::check_two_indi(indi* indiv1, indi* indiv2)
{
	vector<indi*> relat1, relat2;
	ancestor(relat1, indiv1, 2);
	ancestor(relat2, indiv2, 2);
	relat1.push_back(indiv1);
	relat2.push_back(indiv2);

	int i, j;
	vector<int> tmp_inner_ID;
	tmp_inner_ID.resize(_ori_data.size()+1);

	for(i=0; i<relat1.size(); i++){
		int tmpid=relat1[i]->inner_ID;
		tmp_inner_ID[tmpid]=1;
	}

	for(i=0; i<relat2.size(); i++){
		int tmpid=relat2[i]->inner_ID;
		if(tmp_inner_ID[tmpid]==1){
			return false;
		}
	}

	return true;
}

void kinship::ancestor(vector<indi*> & ans, indi* indiv)
{
	vector<indi*> tmp;
	tmp.push_back(indiv);
	vector<indi*> tmp1;
	ans.push_back(indiv);

	int i, j;
	while (tmp.size()>0)
	{
		for(i=0; i<tmp.size(); i++){
			if(tmp[i]->F_addr!=0){
				ans.push_back(tmp[i]->F_addr);
				tmp1.push_back(tmp[i]->F_addr);
			}
			if(tmp[i]->M_addr!=0){
				ans.push_back(tmp[i]->M_addr);
				tmp1.push_back(tmp[i]->M_addr);
			}
		}
		tmp.clear();
		tmp=tmp1;
		tmp1.clear();
	}
}

void kinship::ancestor(vector<indi*> & ans, indi* indiv, int generation)
{
	vector<indi*> tmp;
	tmp.push_back(indiv);
	vector<indi*> tmp1;

	int i, j;
	for(j=0; j<generation; j++)
	{
		for(i=0; i<tmp.size(); i++){
			if(tmp[i]->F_addr!=0){
				ans.push_back(tmp[i]->F_addr);
				tmp1.push_back(tmp[i]->F_addr);
			}
			if(tmp[i]->M_addr!=0){
				ans.push_back(tmp[i]->M_addr);
				tmp1.push_back(tmp[i]->M_addr);
			}
		}
		tmp.clear();
		tmp=tmp1;
		tmp1.clear();
	}
}

void kinship::construct_pedi()
{
	cout<<"now we construct pedigree....";
	int i, j;
	for(i=0; i<_ori_data.size(); i++)
	{
		cout<<"\nnow add the "<<i+1<<"th person....";
		if(_ori_data[i].M_ID!=0){
			for(j=0; j<_ori_data.size(); j++){
				if(_ori_data[i].M_ID==_ori_data[j].indi_ID){
					_ori_data[i].M_addr=&_ori_data[j];
					(_ori_data[j].children).push_back(&_ori_data[i]);
					break;
				}
			}

			if(j==_ori_data.size()){
				indi* tmp = new indi();
				tmp->indi_ID=_ori_data[i].M_ID;
				_ori_data.push_back(*tmp);
				_ori_data[i].M_addr=&_ori_data[j];
				(_ori_data[j].children).push_back(&_ori_data[i]);
			}
		}

		if(_ori_data[i].F_ID!=0){
			for(j=0; j<_ori_data.size(); j++){
				if(_ori_data[i].F_ID==_ori_data[j].indi_ID){
					_ori_data[i].F_addr=&_ori_data[j];
					(_ori_data[j].children).push_back(&_ori_data[i]);
					break;
				}
			}

			if(j==_ori_data.size()){
				indi* tmp = new indi();
				tmp->indi_ID=_ori_data[i].F_ID;
				_ori_data.push_back(*tmp);
				_ori_data[i].M_addr=&_ori_data[j];
				(_ori_data[j].children).push_back(&_ori_data[i]);
			}
		}
	}
}

void kinship::assign_FamID()
{
	int i, j, k;
	int count=1;
	for(i=0; i<_ori_data.size(); i++){
		if(_ori_data[i].Fam_ID == 0){
			surrounding_FamID(&_ori_data[i], count);
			count++;
		}
	}
}

bool kinship::surrounding_FamID(indi * id, int & count)
{
	if(id==0) return false;
	if(id->Fam_ID!=0) return false;

	id->Fam_ID=count;
	surrounding_FamID(id->F_addr, count);
	surrounding_FamID(id->M_addr, count);
	for(int i=0; i<(id->children).size(); i++){
		surrounding_FamID((id->children)[i],count);
	}
}

long kinship::split_string(const string &str, vector <string> &vec_str, string separator)
{
	if(str.empty()) return 0;
	vec_str.clear();

	long i=0;
	bool look=false;
	string str_buf;
	string symbol_pool="`1234567890-=~!@#$%^&*()_+qwertyuiop[]\\asdfghjkl;'zxcvbnm,./QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>? \t\n";
	string::size_type pos;

	for(i=0; i<separator.size(); i++) 
	{
		pos=symbol_pool.find(separator[i]);
		if( pos!=string::npos ) symbol_pool.erase(symbol_pool.begin()+pos);
	}

	for(i=0; i<str.size(); i++)
	{		
		if( symbol_pool.find(str[i])!=string::npos )
		{
			if(!look) look=true;
			str_buf += str[i];
		}
		else
		{
			if(look) 
			{
				look=false;
				vec_str.push_back(str_buf);
				str_buf.erase(str_buf.begin(), str_buf.end());
			}
		}
	}
	if(look) vec_str.push_back(str_buf);

	return vec_str.size();
}

double kinship::ran1(int &idum)
{
	const int IA=16807,IM=2147483647,IQ=127773,IR=2836,NTAB=32;
	const int NDIV=(1+(IM-1)/NTAB);
	const double EPS=3.0e-16,AM=1.0/IM,RNMX=(1.0-EPS);
	static int iy=0;
	static vector<int> iv(NTAB);
	int j,k;
	double temp;

	if (idum <= 0 || !iy) {
		if (-idum < 1) idum=1;
		else idum = -idum;
		for (j=NTAB+7;j>=0;j--) {
			k=idum/IQ;
			idum=IA*(idum-k*IQ)-IR*k;
			if (idum < 0) idum += IM;
			if (j < NTAB) iv[j] = idum;
		}
		iy=iv[0];
	}
	k=idum/IQ;
	idum=IA*(idum-k*IQ)-IR*k;
	if (idum < 0) idum += IM;
	j=iy/NDIV;
	iy=iv[j];
	iv[j] = idum;
	if ((temp=AM*iy) > RNMX) return RNMX;
	else return temp;
}
