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
	double* in=new double[len];
	std::vector<std::vector<double> >allvelocity(len,std::vector<double>(atomnum*3,0.0));
	for(struct{std::list<double* >::iterator a;size_t i;} s={ve_list.begin(),0};s.a!=ve_list.end();s.a++,s.i++){
		for(size_t j=0;j<atomnum*3;j++){
			allvelocity[s.i][j]=(*s.a)[j];
		}
	}
	for(size_t i=0;i<len;i++){
		in[i]=allvelocity[i][0];
	}
	fftw_complex *out,*outall;/* Output */
	fftw_plan p;/*Plan*/
	out=(fftw_complex* ) fftw_malloc(sizeof(fftw_complex)*(len/2+1));
	outall=(fftw_complex* )fftw_malloc(sizeof(fftw_complex)*(len/2+1));
	std::fstream power_spectra;
	power_spectra.open("power.txt",std::fstream::out);
	for(size_t i=0;i<len/2+1;i++){
		outall[i][0]=0.0;
		outall[i][1]=0.0;
	}
	for(size_t i=0;i<atomnum*3;i++){
		for(size_t j=0;j<len;j++){
			in[j]=allvelocity[j][i];
		}
		p=fftw_plan_dft_r2c_1d(len,in,out,FFTW_ESTIMATE);
		for(size_t j=0;j<len/2+1;j++){
			outall[j][0]=outall[j][0]+out[j][0];
			outall[j][1]=outall[j][1]+out[j][1];
		}
		fftw_execute(p);
		fftw_destroy_plan(p);
	}
	for(size_t i=0;i<len/2+1;i++){
		outall[i][0]=outall[i][0]/(len/2+1);
		outall[i][1]=outall[i][1]/(len/2+1);
	}
	power_spectra<<"Freqeucy(cm^-1) Amplitude"<<std::endl;
	for(size_t i=0;i<len/2+1;i++){
		//0.029999 transform Thz to cm^-1
		power_spectra<<i/(len+0.0)*2*3.141592653*1000*33.356<<" "<<sqrt(outall[i][0]*outall[i][0]+outall[i][1]*outall[i][1])<<std::endl;
	}
	fftw_free(out);
}
int main(){
	double timestep=1.0;//timestep 1fs
	int atomnum=48;
	std::vector<double> old_position(atomnum*3,0.0);
	std::vector<double> new_position(atomnum*3,0.0);
	std::vector<std::vector<double> > unit_vector(3,std::vector<double>(3,0.0));
	double* velocity_temp;
	std::list<double* > velocity;
	double temp_double;
	double double_x;
	double double_y;
	double double_z;
	std::string temp_string;
	std::fstream fs;
	std::istringstream stream_temp;
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
				velocity_temp[i]=(temp_double-round(temp_double))/timestep;
			}
		}
		if(temp_string.find("Lattice unit vectors")!=std::string::npos){
			for(size_t i=0;i<3;i++){
				getline(fs,temp_string);
				stream_temp.clear();
				stream_temp.str(temp_string);
				for(size_t j=0;j<3;j++){
					stream_temp>>unit_vector[i][j];
				}
			}
			for(size_t i=0;i<atomnum;i++){
				double_x=0.0;
				double_y=0.0;
				double_z=0.0;
				for(size_t j=0;j<3;j++){
					double_x=double_x+velocity_temp[3*i+j]*unit_vector[j][0];
					double_y=double_y+velocity_temp[3*i+j]*unit_vector[j][1];
					double_z=double_x+velocity_temp[3*i+j]*unit_vector[j][2];
				}
				velocity_temp[3*i+0]=double_x;
				velocity_temp[3*i+1]=double_y;
				velocity_temp[3*i+2]=double_z;
			}
			velocity.push_back(velocity_temp);
		}
	}
	autospeed(velocity,atomnum);
	for(std::list<double* >::iterator a=velocity.begin();a!=velocity.end();a++){
		delete [] *a;
	}
}
