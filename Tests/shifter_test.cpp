#include "../Source/Dsp.h"
#include <cstdio>
using namespace wir3d;
int main(){ int fails=0,total=0; double worstHz=0;
  for(double sr: {44100.,48000.,96000.}) for(float ratio: {.5f,1.f,1.059463f,2.f}) for(int block: {1,127,512,1023}){
    const int N=(int)(sr*1.5), skip=(int)(sr*.5); std::vector<float> in(N),out(N);
    for(int i=0;i<N;++i) in[i]=.4f*std::sin(2.0*M_PI*220.0*i/sr);
    PitchShifter ps; ps.prepare(sr); ps.setPeriod((float)(sr/220.0));
    for(int i=0;i<N;i+=block){ int n=std::min(block,N-i); for(int k=0;k<n;++k) out[i+k]=ps.process(in[i+k],ratio); }
    double e=0; int cross=0; bool fin=true; for(int i=skip;i<N;++i){ fin&=std::isfinite(out[i]); e+=out[i]*out[i]; if(out[i-1]<=0&&out[i]>0) ++cross; }
    double rms=std::sqrt(e/(N-skip)), hz=cross*sr/(N-skip); bool ok=fin&&rms>.08&&rms<.7&&std::abs(hz-220*ratio)<4; ++total; worstHz=std::max(worstHz,std::abs(hz-220*ratio)); if(!ok){ ++fails; printf("FALLO sr=%.0f ratio=%.3f block=%d rms=%.3f hz=%.1f\n",sr,ratio,block,rms,hz);} }
  printf("shifter vs criterio QPitch (48 casos): %d fallos, peor error de frecuencia %.2f Hz (limite 4)\n",fails,worstHz);
  // reset -> silencio
  PitchShifter ps; ps.prepare(48000); for(int i=0;i<20000;++i) ps.process(.4f*std::sin(i*.03f),1.3f); ps.clear(); float mx=0; for(int i=0;i<8192;++i) mx=std::max(mx,std::abs(ps.process(0.f,1.f))); printf("tras clear(): salida maxima con silencio de entrada = %.2e\n",mx); }
