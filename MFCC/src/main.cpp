#include <stdint.h>
#include "./MFCC.h"
#include "./std_ft.h"
using namespace std;
MFCC mfcc;
standardization stdft;


int main()
{
    int FS=8;                    // default sampling rate in KHz, actual value will be obtained from wave file
	int HIGH=4;                   // default high frequency limit in KHz
	int LOW=0;                    // default low frequency limit in KHz
	int FrmLen=25;             // frame length in ms
	int FrmSpace=10;           // frame space in ms
	const int LOGENERGY=1;        // whether to include log energy in the output
	const unsigned long FFTLen=512;           // FFT points
	const int FiltNum=26;              // number of filters
	const int PCEP=12;                 // number of cepstrum

    //man_voice.wav의 cepstrum.csv, weights.csv 생성
	mfcc.the_main_thing("../data/wav/man_voice.wav", "../data/cepstrum/cep_man.csv", "../data/weight/weights_man.csv", FiltNum,  PCEP, FrmLen, FrmSpace, HIGH, LOW, LOGENERGY, FFTLen);
    printf("cep_man.csv 생성\n");
    printf("weights_man.csv 생성\n");
    //cepstrum.csv를 바탕으로 featur.csv 생성
    stdft.standard_features("../data/cepstrum/cep_man.csv", "../data/feature/feature_man.csv");
    printf("feature_man.csv 생성\n");
}