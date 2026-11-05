#include "../Source/Dsp.h"
#include <cstdio>
#include <random>
#include <algorithm>
using namespace wir3d;
// Voz sintetica: tren de pulsos a f0 filtrado por dos resonancias (F1,F2)
static std::vector<float> vowel(double sr,float f0,float F1,float F2,float sec,std::mt19937& rng){
  std::uniform_real_distribution<float> U(-1,1); int N=(int)(sr*sec); std::vector<float> x(N);
  Biquad r1,r2; auto res=[&](Biquad& b,double f,double bw){ double w0=2*M_PI*f/sr, r=std::exp(-M_PI*bw/sr); b.b0=(float)(1-r); b.b1=0; b.b2=0; b.a1=(float)(-2*r*std::cos(w0)); b.a2=(float)(r*r); };
  res(r1,F1,90); res(r2,F2,110); double ph=0;
  for(int i=0;i<N;++i){ ph+=f0/sr; float p=0; if(ph>=1){ph-=1;p=1;} float s=r2.process(r1.process(p+0.002f*U(rng))); x[i]=s*3.0f; }
  return x; }
static double bandE(const std::vector<float>& x,size_t off,size_t N,double sr,double f0,double f1){ double e=0; for(double f=f0;f<f1;f+=7){ double re=0,im=0; for(size_t i=0;i<N;++i){ double w=0.5-0.5*std::cos(2*M_PI*i/N); double a=2*M_PI*f*i/sr; re+=w*x[off+i]*std::cos(a); im-=w*x[off+i]*std::sin(a);} e+=re*re+im*im;} return e; }
// pico de la envolvente: maximo del espectro suavizado (ventana de 160 Hz) entre f0 y f1
static double envPeak(const std::vector<float>& x,size_t off,size_t N,double sr,double lo,double hi){ std::vector<double> mag; std::vector<double> fr; for(double f=lo;f<=hi;f+=10){ double re=0,im=0; for(size_t i=0;i<N;++i){ double w=0.5-0.5*std::cos(2*M_PI*i/N); double a=2*M_PI*f*i/sr; re+=w*x[off+i]*std::cos(a); im-=w*x[off+i]*std::sin(a);} mag.push_back(re*re+im*im); fr.push_back(f);} double best=0,bf=0; for(size_t i=0;i<mag.size();++i){ double s=0; int c=0; for(int j=-8;j<=8;++j){ long k=(long)i+j; if(k>=0&&k<(long)mag.size()){ s+=mag[k]; ++c; } } s/=c; if(s>best){best=s;bf=fr[i];} } return bf; }

static std::vector<std::pair<double,double>> harmonicPeaks(const std::vector<float>& x,size_t off,size_t N,double sr,double f0){
  std::vector<double> A; int K=(int)(3200.0/f0); for(int k=1;k<=K;++k){ double f=k*f0, re=0,im=0; for(size_t i=0;i<N;++i){ double w=0.5-0.5*std::cos(2*M_PI*i/N); double a=2*M_PI*f*i/sr; re+=w*x[off+i]*std::cos(a); im-=w*x[off+i]*std::sin(a);} A.push_back(std::sqrt(re*re+im*im)); }
  std::vector<std::pair<double,double>> pk; for(int k=1;k+1<(int)A.size();++k) if(A[k]>A[k-1]&&A[k]>A[k+1]) pk.push_back({A[k],(k+1)*f0});
  std::sort(pk.begin(),pk.end(),[](auto&a,auto&b){return a.first>b.first;}); if(pk.size()>3) pk.resize(3); return pk; }
int main(){ std::mt19937 rng(5);
  for(double sr: {44100.,48000.}){
    // 1) reconstruccion perfecta de la celosia (sin shifter), con y sin retardo
    auto x=vowel(sr,140,700,1200,1.5f,rng); FormantEngine fe; fe.prepare(sr);
    for(int D: {0,777,1632}){
      FormantEngine g; g.prepare(sr); std::vector<float> e(x.size(),0.f); std::vector<float> y(x.size(),0.f); std::vector<float> ebuf(x.size()+D+1,0.f);
      double err=0,en=0; double eRes=0,eIn=0;
      for(size_t n=0;n<x.size();++n){ g.push(x[n]); float r=g.analyse(0,x[n]); ebuf[n]=r; float rd = n>=(size_t)D? ebuf[n-D]:0.f; y[n]=g.synthesise(0,rd,D); if(n>(size_t)(sr*0.5)){ float ref= n>=(size_t)D? x[n-D]:0.f; err+=(y[n]-ref)*(y[n]-ref); en+=ref*ref; eRes+=r*r; eIn+=x[n]*x[n]; } }
      printf("sr %.0f celosia D=%4d: error de reconstruccion %.1f dB | ganancia de prediccion %.1f dB (orden %d)\n",sr,D,10*std::log10(err/en+1e-30),10*std::log10(eIn/eRes),g.getOrder()); }
    // 2) desplazar +4 st: formantes sin y con preservacion
    float ratio=std::pow(2.f,4.f/12.f);
    for(int mode=0; mode<3; ++mode){ // 0=sin cambio (referencia), 1=shifter normal, 2=con formantes
      PitchShifter ps; ps.prepare(sr); ps.setPeriod((float)(sr/140.0)); FormantEngine g; g.prepare(sr); std::vector<float> y(x.size()); int L=ps.latencySamples();
      for(size_t n=0;n<x.size();++n){ float in=x[n]; if(mode==2){ g.push(in); float e=g.analyse(0,in); float s=ps.process(e,ratio); y[n]=g.synthesise(0,s,L);} else if(mode==1) y[n]=ps.process(in,ratio); else y[n]=in; }
      size_t off=(size_t)(sr*0.6), N=16384; double f0o = mode==0?140.0:140.0*ratio; auto pk=harmonicPeaks(y,off,N,sr,f0o); std::sort(pk.begin(),pk.end(),[](auto&a,auto&b){return a.second<b.second;});
      printf("  %-26s picos de la envolvente (Hz, de menor a mayor):",mode==0?"entrada (F1=700, F2=1200)":(mode==1?"+4 st normal":"+4 st con preservacion")); for(auto&p:pk) printf(" %.0f",p.second); printf("\n"); }
    // 3) tono con preservacion: f0 de salida
    { PitchShifter ps; ps.prepare(sr); ps.setPeriod((float)(sr/140.0)); FormantEngine g; g.prepare(sr); int L=ps.latencySamples(); std::vector<float> y(x.size());
      for(size_t n=0;n<x.size();++n){ g.push(x[n]); float e=g.analyse(0,x[n]); y[n]=g.synthesise(0,ps.process(e,ratio),L); }
      PitchDetector d; d.prepare(sr); double s=0; int c=0; for(size_t n=0;n<y.size();++n) if(d.push(y[n])&&n>sr*0.8&&d.isVoiced()){ s+=d.midiNote(); ++c; }
      printf("  tono de salida con preservacion: midi %.2f (esperado %.2f)\n", s/c, 69+12*std::log2(140.0*ratio/440.0)); }
  }
}
