#include "../Source/Dsp.h"
#include <cstdio>
#include <random>
using namespace wir3d;
int main(){
  int fails=0,total=0; double worst=0,minConf=1;
  for(double sr: {44100.,48000.,96000.}) for(float hz: {55.f,130.f,227.f,340.f,440.f,880.f}){
    PitchDetector d; d.prepare(sr); d.setRange(55,1000);
    int N=(int)(sr*.15); float last=0; bool v=false,cf=0;
    for(int i=0;i<N;++i){ double ph=i*6.283185307179586*hz/sr; float x=.2f*std::sin(ph)+.08f*std::sin(2*ph)+.04f*std::sin(3*ph); if(d.push(x)){ v=d.isVoiced(); last=d.midiNote(); minConf=std::min<double>(minConf,d.getConfidence()); } }
    float exp=69+12*std::log2(hz/440.f); double err=v?std::abs(last-exp)*100:1e4; ++total; worst=std::max(worst,err);
    if(err>3.0){ ++fails; printf("FALLO sr=%.0f hz=%.0f voiced=%d err=%.2f cents\n",sr,hz,v,err); } }
  printf("A) criterio QPitch (<=3 cents, 18 casos): %d fallos, peor %.2f cents, confianza minima %.2f\n",fails,worst,minConf);
  // B) barrido aleatorio 60-900 Hz, voces con armonicos variables
  std::mt19937 rng(11); std::uniform_real_distribution<float> U(0,1); int bad=0,n=0; double w2=0,sum=0;
  for(double sr: {44100.,48000.,96000.}) for(int k=0;k<60;++k){ float hz=60*std::pow(900.f/60.f,U(rng)); float a2=.1f+.5f*U(rng), a3=.05f+.4f*U(rng);
    PitchDetector d; d.prepare(sr); d.setRange(55,1000); float last=0; bool v=false; int N=(int)(sr*.3);
    for(int i=0;i<N;++i){ double ph=i*6.283185307179586*hz/sr; float x=.3f*std::sin(ph)+.3f*a2*std::sin(2*ph+0.4)+.3f*a3*std::sin(3*ph+1.1)+.01f*(U(rng)-.5f); if(d.push(x)){ v=d.isVoiced(); last=d.midiNote(); } }
    float exp=69+12*std::log2(hz/440.f); double err=v?std::abs(last-exp)*100:1e4; ++n; w2=std::max(w2,err); sum+=err; if(err>3.0){++bad; printf("  fuera: sr=%.0f hz=%.1f err=%.1f\n",sr,hz,err);} }
  printf("B) 180 casos aleatorios 60-900 Hz con ruido: %d fuera de 3 cents, error medio %.2f, peor %.2f cents\n",bad,sum/n,w2);
  // C) silencio tras reset
  PitchDetector d; d.prepare(48000); d.setRange(55,1000); for(int i=0;i<20000;++i){ double ph=i*6.28318*220/48000.; d.push(.3f*std::sin(ph)); } bool before=d.isVoiced(); d.reset(); d.setRange(55,1000); bool any=false; for(int i=0;i<20000;++i) if(d.push(0.f)&&d.isVoiced()) any=true;
  printf("C) voz antes de reset=%d; silencio tras reset voiced=%d (debe ser 0)\n",before,any);
}
