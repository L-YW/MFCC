#include <stdint.h>
#include "./MFCC.h"
#include "./std_ft.h"
#include "./learn_classify.h"
using namespace std;
MFCC mfcc;
standardization stdft;
learn_and_classify lc_manager;


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

    //----------------------------------------------
    //man_voice.wav의 cepstrum.csv, weights.csv 생성
    //----------------------------------------------
	mfcc.the_main_thing("../data/wav/man_voice.wav", "../data/cepstrum/cep_man.csv", "../data/weight/weights_man.csv", FiltNum,  PCEP, FrmLen, FrmSpace, HIGH, LOW, LOGENERGY, FFTLen);
    printf("cep_man.csv 생성\n");
    printf("weights_man.csv 생성\n");
    //--------------------------------------
    //cepstrum.csv를 바탕으로 featur.csv 생성
    //--------------------------------------
    stdft.standard_features("../data/cepstrum/cep_man.csv", "../data/feature/feature_man.csv");
    printf("feature_man.csv 생성\n");
    //--------------------------------------
    // learn & classify
    //--------------------------------------
    lc_manager.train_intellino("../data/cepstrum/cepstrum_A3.csv", -1, false); //cat 1
    lc_manager.train_intellino("../data/cepstrum/cepstrum_B3.csv", -1, false); //cat 1
    lc_manager.train_intellino("../data/cepstrum/cepstrum_Anavi.csv", -1, false); //cat 2
    lc_manager.train_intellino("../data/cepstrum/cepstrum_Bnavi.csv", -1, false); //cat 2
    lc_manager.test_intellino("../data/cepstrum/cepstrum_G3.csv", "../data/result.txt", true);
    // lc_manager.test_multi("../data/cepstrum/cep_A3.csv", "../data/result.txt", -1, true);
}