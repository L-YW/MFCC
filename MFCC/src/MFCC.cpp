// This program generates MFCC features, not including delta terms
// Users can specify frame length and space, FFT points, number of filters, number of cepstrums, low and high frequency limits
// Users can also specify wether to have log energy term in the output
// Sampling rate is obtained from the wave file

// INPUT: a windows wave file
// OUTPUT: cepstrum.txt: stores output cepstrums, each row is a feature vector
//         weights.txt: stores filterbank weights, each row is a channel

// Created by Xiaoyu Liu

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <complex> 
#include <bitset> 
#include "MFCC.h"

using namespace std;
using std::vector;

MFCC::MFCC(){}

typedef struct     // WAV stores wave file header
{
        int rId;    
        int rLen;   
        int wId;    
        int fId;    

        int fLen;   

        short wFormatTag;       
        short nChannels;        
        int nSamplesPerSec;   // Sampling rate. This returns to FS variable
        int nAvgBytesPerSec;  // All other parameters are not used in processing
        short nBlockAlign;      
        short wBitsPerSample;   
        int dId;              
        int wSampleLength;    
}WAV;

// Global variables
const int FiltNum1 = 26;
const double PI=3.1415926536;
vector <double> Hamming;
vector<float> Coeff; // This stores cepstrum and log energy
float FiltWeight[FiltNum1][257];
// This function create a hamming window
void MFCC::InitHamming(int FrmLen)
{
	float two_pi=8.0F*atan(1.0F);   // This is just 2*pi;
	float temp;
	int i;
	for( i=0;i<FrmLen;i++)
	{
		temp=(float)(0.54-0.46*cos((float)i*two_pi/(float)(FrmLen-1)));  // create a Hamming window of length FrmLen
		Hamming.push_back(temp);
    }
}

void MFCC::HammingWindow(short* buf,float* data, int FrmLen)  // This function multiply a Hamming window to a frame
{
	int i;
	for(i=0;i<FrmLen;i++)
	{
		data[i]=buf[i]*Hamming[i];
	}
}

float MFCC::FrmEnergy(short* data, int FrmLen)        // This function computes frame energy
{
	int i;
	float frm_en=0.0;
	for(i=0;i<FrmLen;i++)
	{
		frm_en=frm_en+data[i]*data[i];
	}
	return frm_en;
}

void MFCC::zero_fft(float *data,vector<complex<float> >& vec, int FrmLen, const unsigned long FFTLen) // This function does zero padding and FFT
{	
	for(int i=0;i<FFTLen;i++)     // This step does zero padding
	{
		if(i<FrmLen)
		{
			vec.push_back(complex<float>(data[i]));
		}
		else
		{
			vec.push_back(complex<float>(0));
		}
	}
	FFT(FFTLen, vec);    // Compute FFT
}

void MFCC::FFT(const unsigned long & fftlen, vector<complex<float> >& vec) 
{ 		 
	unsigned long ulPower = 0;  
	unsigned long fftlen1 = fftlen - 1; 
	while(fftlen1 > 0) 
	{ 
		ulPower++; 
		fftlen1=fftlen1/2; 
	} 


	bitset<sizeof(unsigned long) * 8> bsIndex;
	unsigned long ulIndex; 
	unsigned long ulK; 
	for(unsigned long p = 0; p < fftlen; p++) 
	{ 
		ulIndex = 0; 
		ulK = 1; 
		bsIndex = bitset<sizeof(unsigned long) * 8>(p); 
		for(unsigned long j = 0; j < ulPower; j++) 
			{ 
				ulIndex += bsIndex.test(ulPower - j - 1) ? ulK : 0; 
				ulK *= 2; 
			} 

		if(ulIndex > p) 
			{ 
				complex<float> c = vec[p]; 
				vec[p] = vec[ulIndex]; 
				vec[ulIndex] = c; 
			} 
	} 


	vector<complex<float> > vecW; 
	for(unsigned long i = 0; i < fftlen / 2; i++) 
		{ 
			vecW.push_back(complex<float>(cos(2 * i * PI / fftlen) , -1 * sin(2 * i * PI / fftlen))); 
		} 



	unsigned long ulGroupLength = 1; 
	unsigned long ulHalfLength = 0;  
	unsigned long ulGroupCount = 0;  
	complex<float> cw; 
	complex<float> c1;  
	complex<float> c2;  
	for(unsigned long b = 0; b < ulPower; b++) 
		{ 
			ulHalfLength = ulGroupLength; 
			ulGroupLength *= 2; 
			for(unsigned long j = 0; j < fftlen; j += ulGroupLength) 
				{ 
					for(unsigned long k = 0; k < ulHalfLength; k++) 
						{ 
							cw = vecW[k * fftlen / ulGroupLength] * vec[j + k + ulHalfLength]; 
							c1 = vec[j + k] + cw; 
							c2 = vec[j + k] - cw; 
							vec[j + k] = c1; 
							vec[j + k + ulHalfLength] = c2; 
						} 
				} 
		} 
} 


// This function initialize filter weights to 0

void MFCC::InitFilt(float (*w)[257], const int FiltNum, const unsigned long FFTLen)
{
  	int i,j;
  	for (i=0;i<FiltNum;i++)
    	for (j=0;j<FFTLen/2+1;j++)
	    	 *(*(w+i)+j)=0.0;
}

// This function creates a Mel weight matrix

void MFCC::CreateFilt(float (*w)[257], const int FiltNum, const unsigned long FFTLen, int Fs, int high, int low)
{
   float df=(float) Fs/(float) FFTLen;    // FFT interval
   int indexlow=round((float) FFTLen*(float) low/(float) Fs); // FFT index of low freq limit
   int indexhigh=round((float) FFTLen*(float) high/(float) Fs); // FFT index of high freq limit

   float melmax=2595.0*log10(1.0+(float) high/700.0); // mel high frequency
   float melmin=2595.0*log10(1.0+(float) low/700.0);  // mel low frequency
   float melinc=(melmax-melmin)/(float) (FiltNum+1); //mel half bandwidth
   float melcenters[FiltNum];        // mel center frequencies
   float fcenters[FiltNum];          // Hertz center frequencies
   int indexcenter[FiltNum];         // FFT index for Hertz centers
   int indexstart[FiltNum];   //FFT index for the first sample of each filter
   int indexstop[FiltNum];    //FFT index for the last sample of each filter
   float increment,decrement; // increment and decrement of the left and right ramp
   float sum=0.0;
   int i,j;
   for (i=1;i<=FiltNum;i++)
   {
	     melcenters[i-1]=(float) i*melinc+melmin;   // compute mel center frequencies
		 fcenters[i-1]=700.0*(pow(10.0,melcenters[i-1]/2595.0)-1.0); // compute Hertz center frequencies
		 indexcenter[i-1]=round(fcenters[i-1]/df); // compute fft index for Hertz centers		 
   }
   for (i=1;i<=FiltNum-1;i++)  // Compute the start and end FFT index of each channel
      {
	    indexstart[i]=indexcenter[i-1];
		indexstop[i-1]=indexcenter[i];		
	  }
   indexstart[0]=indexlow;
   indexstop[FiltNum-1]=indexhigh;
   for (i=1;i<=FiltNum;i++)
   {
      increment=1.0/((float) indexcenter[i-1]-(float) indexstart[i-1]); // left ramp
	  for (j=indexstart[i-1];j<=indexcenter[i-1];j++)
	     w[i-1][j]=((float)j-(float)indexstart[i-1])*increment;
	  decrement=1.0/((float) indexstop[i-1]-(float) indexcenter[i-1]);    // right ramp
	  for (j=indexcenter[i-1];j<=indexstop[i-1];j++)
	     w[i-1][j]=1.0-((float)j-(float)indexcenter[i-1])*decrement;		 
   }

   for (i=1;i<=FiltNum;i++)     // Normalize filter weights by sum
   {
       for (j=1;j<=FFTLen/2+1;j++)
	      sum=sum+w[i-1][j-1];
	   for (j=1;j<=FFTLen/2+1;j++)
	      w[i-1][j-1]=w[i-1][j-1]/sum;
	   sum=0.0;
   }
}

void MFCC::mag_square(vector<complex<float> > &vec, vector<float> &vec_mag, const unsigned long FFTLen) // This function computes magnitude squared FFT
{
  int i;
  float temp;
  for (i=1;i<=FFTLen/2+1;i++)
     {
	   temp = vec[i-1].real()*vec[i-1].real()+vec[i-1].imag()*vec[i-1].imag();
	   vec_mag.push_back(temp);
	 }
       	   	   
}

void MFCC::Mel_EN(float (*w)[257], const int FiltNum, const unsigned long FFTLen, vector<float>& vec_mag, float * M_energy) // computes log energy of each channel
{
   int i,j;
   for (i=1;i<=FiltNum;i++)    // set initial energy value to 0
     M_energy[i-1]=0.0F;
   
   for (i=1;i<=FiltNum;i++)
   {
     for (j=1;j<=FFTLen/2+1;j++)
         M_energy[i-1]=M_energy[i-1]+w[i-1][j-1]*vec_mag[j-1];
     M_energy[i-1]=(float)(log(M_energy[i-1]));			 
   }

}

// Compute Mel cepstrum

void MFCC::Cepstrum(float *M_energy, const int FiltNum, const int PCEP)
{	
	int i,j;
	float Cep[PCEP];
    for (i=1;i<=PCEP;i++)
	{ Cep[i-1]=0.0F;    // initialize to 0
	  for (j=1;j<=FiltNum;j++)
          Cep[i-1]=Cep[i-1]+M_energy[j-1]*cos(PI*((float) i)/((float) FiltNum)*((float) j-0.5F)); // DCT transform
      Cep[i-1]=sqrt(2.0/float (FiltNum))*Cep[i-1];
	  Coeff.push_back(Cep[i-1]);   // store cepstrum in this vector
    }	
	  
}

int MFCC::the_main_thing(const char* wav_filename, const char* cep_filename, const char* weight_filename, const int FiltNum, const int PCEP, int FrmLen, int FrmSpace, int HIGH, int LOW, int LOGENERGY, const unsigned long FFTLen)
{
	WAV header;    // This struct stores wave file header
	FILE *sourcefile;
	sourcefile=fopen(wav_filename, "rb");  // open the wave file as a binary file
	ofstream outfile1(cep_filename);     // This file stores output cepstrum
	ofstream outfile2(weight_filename);  // This file stores filter weights
	
	fread(&header,sizeof(WAV), 1, sourcefile);   // read in the header
	int FS=header.nSamplesPerSec/1000;  // Obtain sampling frequency
	if (HIGH>(int) (FS/2))                      // Check pre-defined high frequency
	   HIGH=(int) (FS/2);
	if (LOW>HIGH)                               // Check pre-defined low frequency
	   LOW=(int)(HIGH/2);
	FrmLen=FrmLen*FS;                          // Obtain frame length in samples
	FrmSpace=FrmSpace*FS;                      // Obtain frame space in samples
	
	short buffer[FrmLen];    // buffer stores a frame of data, each 2 byte
	float data[FrmLen];
	float energy=0.0;
	float mel_energy[FiltNum]; // This stores the channel output energy for a frame

	vector<complex<float> > zero_padded;   // zero_padded is a vector which stores the zero padded data and FFT
	vector <float> fft_mag;                // This is the magnitude squared FFT
		
	InitHamming(FrmLen);      //Create a Hamming window of length FrmLen
	InitFilt(FiltWeight, FiltNum, FFTLen); // Initialize filter weights to all zero
	CreateFilt(FiltWeight, FiltNum, FiltNum, FS*1000, HIGH*1000, LOW*1000);    // Compute filter weights
	for (int i=0;i<FiltNum; i++)          // Output filter weights to a file
	  { for (int j=0;j<FFTLen/2+1;j++)
	       outfile2<<FiltWeight[i][j]<<' ';
	   outfile2<<endl;
	  } 
	
    // While loop reads in each frame, and compute cepstrum features
	while(fread(buffer,sizeof(short),FrmLen,sourcefile)==FrmLen)  //  continue to read in a frame of data
	{

		HammingWindow(buffer, data, FrmLen);  // multiply Hamming window to speech, return to data 
		energy=FrmEnergy(buffer, FrmLen);//Get frame energy without windowing
		zero_fft(data,zero_padded, FrmLen, FFTLen); // This step first zero pad data, and do FFT
		mag_square(zero_padded, fft_mag, FFTLen);    // This step does magnitude square for the first half of FFT
        Mel_EN(FiltWeight, FiltNum, FFTLen, fft_mag, mel_energy); // This step computes output log energy of each channel
		Cepstrum(mel_energy, FiltNum, PCEP);
		if (LOGENERGY)   // whether to include log energy term or not
		   Coeff.push_back(energy);
		   
		zero_padded.clear(); // clear up fft vector
		fft_mag.clear();    // clear up fft magnitude 
		fseek(sourcefile, -(FrmLen-FrmSpace), SEEK_CUR); // move to the next frame
	}

	int length=Coeff.size();  // Output cepstrum and log energy to a file. Each row is a feature vector
	for(int i=0;i<length;++i)
	{
		outfile1<<Coeff[i]<<',';
		if((i+1)%(PCEP+LOGENERGY)==0)
			outfile1<<endl;
	}

    fclose(sourcefile);
	return 0;

}