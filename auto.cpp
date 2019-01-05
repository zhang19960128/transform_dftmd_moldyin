#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <istream>
#include <complex.h>
#include <fftw3.h>
#include <string>
#include <math.h>
double innerprod(std::list<double*>& ve_list,int atomnum,size_t framei,size_t framej){
	double sum=0.0;
	std::list<double*>::iterator framei_iterator=ve_list.begin();
	std::list<double*>::iterator framej_iterator=ve_list.begin();
	for(size_t i=0;i<framei;i++){
			framei_iterator++;
	}
	for(size_t j=0;j<framej;j++){
		framej_iterator++;
	}
	for(size_t i=0;i<3*atomnum;i++){
		sum=sum+(*framei_iterator)[i]*(*framej_iterator)[i];
	}
	return sum;
}
double autocorre(std::list<double*>& ve_list,int atomnum,size_t interval){
	double sum=0.0;
	size_t len=ve_list.size();
	for(size_t i=0;i<len-interval;i++){
		sum=sum+innerprod(ve_list,atomnum,i,i+interval);
	}
	return sum/(len-interval);
}
void autospeed(std::list<double*>& ve_list,int atomnum){
	size_t len=ve_list.size();
	/*
	 * this part is used to test whether the output is what we want.
	for(std::list<double*>::iterator i=ve_list.begin();i!=ve_list.end();i++){
		for(int j=0;j<5*3*cell*cell*cell;j++){
				if(j%3==0){
					std::cout<<std::endl;
				}
				std::cout<<*((*i)+j)<<" ";
		}
	}
	*/
	std::fstream autoout;
	autoout.open("autocorrelation_of_velocity.txt",std::fstream::out);
	double* in=new double[len];
	double temp=0.0;
	for(size_t i=0;i<len-1;i++){
		temp=autocorre(ve_list,atomnum,i);
		autoout<<temp<<std::endl;
	  in[i]=temp;
	}
	fftw_complex *out;/* Output */
	fftw_plan p;/*Plan*/
	out=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(len/2+1));
	p=fftw_plan_dft_r2c_1d(len,in,out,FFTW_ESTIMATE);
	fftw_execute(p);
	for(size_t i=0;i<len/2+1;i++){
		std::cout<<out[i][0]<<" "<<out[i][1]<<std::endl;
	}
	fftw_destroy_plan(p);
	fftw_free(out);
}
int main(){
	double timestep=1.0;//timestep 1fs
	int atomnum=48;
	std::vector<double> old_position(atomnum*3,0.0);
	std::vector<double> new_position(atomnum*3,0.0);
	double* velocity_temp;
	std::list<double* > velocity;
	double temp_double;
	std::string temp_string;
	std::fstream fs;
	std::isstringstream stream_temp;
	fs.open("IonFor.dat.MD.jiahao",std::fstream::in);
	while(getline(fs,temp_string)){
		if(temp_string.find("Reduced ionic position")!=std::string::npos){
			for(size_t i=0;i<atomnum*3;i++){
				old_position[i]=new_position[i];
			}
			for(size_t i=0;i<atomnum;i++){
				stream_temp.clear();
				getline(fs,temp_string);
				stream_temp.clear();
				stream_temp.str(temp_string);
				for(size_t j=0;j<3;j++){
					stream_temp>>new_position[3*i+j];
				}
				stream_temp.clear();
			}
			velocity_temp=new double [atomnum*3];
			for(size_t i=0;i<atomnum*3;i++){
				temp_double=new_position[i]-old_position[i];
				velocity_temp[i]=temp_double-round(temp_double);
			}
		}
	}
	for(std::list<double* >::iterator a=velocity.begin();a!=velocity.end();a++){
		delete *a [];
	}
}
