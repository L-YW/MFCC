#include <stdint.h>
#include <vector>
#include <complex>
using namespace std; 
using std::vector;

class MFCC{    
    public:
        MFCC();
        void InitHamming(int FrmLen);
        void HammingWindow(short* buf,float* data, int FrmLen);
        float FrmEnergy(short* data, int FrmLen);
        void zero_fft(float *buffer,vector<complex<float> >& vec, int FrmLen, const unsigned long FFTLen);
        void FFT(const unsigned long & fftlen, vector<complex<float> >& vec);
        void InitFilt(float (*w)[257], const int FiltNum, const unsigned long FFTLen); 
        void CreateFilt(float (*w)[257], const int FiltNum, const unsigned long FFTLen, int Fs, int high, int low);
        void mag_square(vector<complex<float> > & vec, vector<float> & vec_mag, const unsigned long FFTLen);
        void Mel_EN(float (*w)[257], const int FiltNum, const unsigned long FFTLen, vector<float>& vec_mag, float * M_energy);
        void Cepstrum(float *M_energy, const int FiltNum, const int PCEP);
        int the_main_thing(const char* wav_filename, const char* cep_filename, const char* weight_filename, const int FiltNum, const int PCEP, int FrmLen, int FrmSpace, int HIGH, int LOW, int LOGENERGY, const unsigned long FFTLen);
};