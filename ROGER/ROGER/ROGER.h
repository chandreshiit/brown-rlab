/*Particle filter for IMGPE

  Define the Particle Filter Class
    add,predict
Dang

TODO:
Make more ROGER, less PF
standardize order of arguments
Explain parameters
Needs serious cleaning and efficiency - split?
*/


#ifndef __ROGER_H__
#define __ROGER_H__

#include "SOGP.h"
#include "ROGER_aux.h"
#include <vector>

class Expert{
 public:
  Expert(){};
  Expert(SOGPParams param,double k,const ColumnVector& m, double v,const Matrix& l){
    //printf("Making expert\n");
    m_SOGP = new SOGP(param);
    k_0=k;mu_0=m;v_0=v;lambda_0=l;
    count=0;
  };
  //Incorporate this data into this expert
  void sit(const ColumnVector& in, const ColumnVector& out);
  double lp_tpp(const ColumnVector& in,bool update);//really this should be above? Rename
  //Wrappers for the GP stuff
  double lp(const ColumnVector&in, const ColumnVector& out){
    return m_SOGP->log_prob(in,out);
  } 
  ReturnMatrix predict(const ColumnVector& in,double &sigconf,bool conf=false){
    return m_SOGP->predict(in,sigconf,conf);
  }
  
  int count; //Same as current_size m_GP.current_size
  bool printTo(FILE *fp);
  bool printTo(const char *name){
    FILE *fp=fopen(name,"w");
    bool ret=printTo(fp);
    fclose(fp);
    return ret;
  };
  bool readFrom(FILE *fp);
  bool readFrom(const char *name){
    FILE *fp=fopen(name,"r");
    bool ret=readFrom(fp);
    fclose(fp);
    return ret;
  };


  private:
  //The Gaussian Process
  SOGP *m_SOGP;
  //Sufficient Statistics for lp_tpp
  ColumnVector mean;
  Matrix sumsq;
  Matrix inv_cov;
  double log_det_cov;
  //Input space paramters - confidence (in) center, confidence (in) invcov
  double k_0,v_0;
  //Note!  v_0 must be >= input_dimensionality ?
  ColumnVector mu_0;
  Matrix lambda_0;
};

//Should this start with an empty expert?
class Particle{
 public: 
  SOGPParams sogpparam;        //Params for new Experts
  double k_0,v_0;
  ColumnVector mu_0;
  Matrix lambda_0;
  vector<Expert>experts;
  vector<int>assignments;//The seating arrangement
  int kplus;
  void sitat(const ColumnVector& in, const ColumnVector& out, int table);
  Particle(){
    kplus=0;
  }
  Particle(Particle *p){
    sogpparam=p->sogpparam;
    k_0=p->k_0;
    mu_0=p->mu_0;
    v_0=p->v_0;
    lambda_0=p->lambda_0;
    for(unsigned int e=0;e<p->experts.size();e++){
      Expert nexp(p->experts[e]);
      experts.push_back(nexp);
    }
    for(unsigned int a=0;a<p->assignments.size();a++)
      assignments.push_back(p->assignments[a]);
    kplus=p->kplus;
  };  
  void printass(FILE *fp){
    for(unsigned int a=0;a<assignments.size();a++)
      fprintf(fp,"%d ",assignments[a]);
  };
  bool printTo(FILE *fp);
  bool printTo(const char *name){
    FILE *fp=fopen(name,"w");
    bool ret=printTo(fp);
    fclose(fp);
    return ret;
  };
  bool readFrom(FILE *fp);
  bool readFrom(const char *name){
    FILE *fp=fopen(name,"r");
    bool ret=readFrom(fp);
    fclose(fp);
    return ret;
  };
};


enum SAMPLE_SCHEME{MAX,SAMPLE,AVERAGE,FIXED};

class ParticleFilter{
 public:
  int NumParticles;
  Particle **particles;
  double *weights;
  Expert *empty_expert;
  int din,dout;
  void addM(const Matrix&, const Matrix&);
  void add(const ColumnVector& input,const ColumnVector& output);
  //These should be in Particles, no?
  ReturnMatrix predict(const ColumnVector &input, double &sigconf, SAMPLE_SCHEME part_sample, SAMPLE_SCHEME expert_sample,int &part,int &expert,bool conf=false);
  ReturnMatrix predict(const ColumnVector &input, double &sigconf, SAMPLE_SCHEME part_sample, SAMPLE_SCHEME expert_sample,bool conf=false){
    int part,expert;
    return predict(input,sigconf,part_sample,expert_sample,part,expert,conf);
  }
  ReturnMatrix predict(const ColumnVector &input, double &sigconf,bool conf=false){
    int part,expert;
    return predict(input,sigconf,MAX,MAX,part,expert,conf);
  }

  double alpha;  //Concentration parameter
  int count;

  ParticleFilter(){
    alpha = 0;
    NumParticles=0;
    weights=NULL;
    empty_expert=NULL;
    particles=NULL;
    init();
  }
  ParticleFilter(int NP,double a,SOGPParams param,double k,const ColumnVector& m,double v,const Matrix& l){
    NumParticles=NP;
    alpha=a;
    particles=new Particle*[NP];
    weights=new double[NP];
    for(int p=0;p<NP;p++){
      particles[p]=new Particle();
      particles[p]->sogpparam=param;
      weights[p]=1.0/NumParticles;
      particles[p]->k_0=k;
      particles[p]->mu_0=m;
      particles[p]->v_0=v;
      particles[p]->lambda_0=l;
    }
    empty_expert=new Expert(param,k,m,v,l);
    init();
  }
  int mapi;

  bool printTo(FILE *);
  bool printTo(const char *fn){
    FILE *fp=fopen(fn,"w");
    bool ret = printTo(fp);
    fclose(fp);
    return ret;
  }
  bool readFrom(FILE *);
  bool readFrom(const char *fn){
    FILE *fp=fopen(fn,"r");
    bool ret=readFrom(fp);
    fclose(fp);
    return ret;
  }


  bool verbose;

  FILE *given_assignments;
  void use_given(const char *fn){
    printf("Using given experts in %s\n",fn);
    given_assignments=fopen(fn,"r");
    if(!given_assignments){
      perror("ROGER use_given:");
      exit(-1);
    }
  }
  void add_given(const ColumnVector& input,const ColumnVector&output,int exp);
  void add_given(const ColumnVector& input,const ColumnVector& output);
  vector<double> ew;
 private:
  int new_particles;//Number of putative particles to generate...forward computed
  void init(){
    count=0;
    mapi=0;
    din=0;
    dout=0;
    verbose=false;
    given_assignments=NULL;
  }
  
};

typedef ParticleFilter ROGER;

#endif
