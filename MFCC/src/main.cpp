#include<iostream>
#include<fstream>
#include<cmath>
#include<vector>
#include <complex> 
#include <bitset> 
#include "MFCC.cpp"


// Global variables
	int FS=8;                    // default sampling rate in KHz, actual value will be obtained from wave file
	int HIGH=4;                   // default high frequency limit in KHz
	int LOW=0;                    // default low frequency limit in KHz
	int FrmLen=25;             // frame length in ms
	int FrmSpace=10;           // frame space in ms
	const int LOGENERGY=1;        // whether to include log energy in the output
	const unsigned long FFTLen=512;           // FFT points
	const int FiltNum=26;              // number of filters
	const int PCEP=12;                 // number of cepstrum
	vector <double> Hamming;            // Hamming window vector
	float FiltWeight[FiltNum][FFTLen/2+1]; //This is the Mel filterbank weights             
	vector<float> Coeff; // This stores cepstrum and log energy

int main()
{
	WAV header;    // This struct stores wave file header
	FILE *sourcefile;
	ofstream outfile1("../data/cepstrum_navi.csv");     // This file stores output cepstrum
	ofstream outfile2("../data/weights_navi.csv");  // This file stores filter weights
	sourcefile=fopen("../data/ZiraRUS_navi.wav","rb");  // open the wave file as a binary file
	fread(&header,sizeof(WAV),1,sourcefile);   // read in the header
	FS=header.nSamplesPerSec/1000;  // Obtain sampling frequency
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
	InitFilt(FiltNum, FFTLen); // Initialize filter weights to all zero
	CreateFilt(FiltNum, FiltNum, FS*1000, HIGH*1000, LOW*1000);    // Compute filter weights
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
        Mel_EN(FiltNum, FFTLen, fft_mag, mel_energy); // This step computes output log energy of each channel
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