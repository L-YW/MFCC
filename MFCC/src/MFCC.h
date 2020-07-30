#include <stdint.h>

class MFCC{
    public:
        MFCC();
        void InitHamming(int FrmLen);
        void HammingWindow(short* buf,float* data, int FrmLen);
        float FrmEnergy(short* data, int FrmLen);
        void zero_fft(float *buffer,vector<complex<float> >& vec, int FrmLen, const unsigned long FFTLen);
        void FFT(const unsigned long & fftlen, vector<complex<float> >& vec);
        void InitFilt(const int FiltNum, const unsigned long FFTLen); 
        void CreateFilt(const int FiltNum, const unsigned long FFTLen, int Fs, int high, int low);
        void mag_square(vector<complex<float> > & vec, vector<float> & vec_mag, const unsigned long FFTLen);
        void Mel_EN(const int FiltNum, const unsigned long FFTLen, vector<float>& vec_mag, float * M_energy);
        void Cepstrum(float *M_energy, const int FiltNum, const int PCEP);
        void the_main_thing(const char* cep_filename, const char* weight_filename, const char* wav_filename);
}