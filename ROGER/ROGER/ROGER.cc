/* Particle Filter

Dang

TODO:
Finish prediction schemes
*/

#include "ROGER.h"
#include <stdio.h>
#include <algorithm>
#include <map>

void Particle::sitat(const ColumnVector& in, const ColumnVector& out,int table){
  //printf("Sitting At %d (%d)\n",table,kplus);
  if(table>kplus){
    printf("Particle::This isn't the next table! %d\n",table);
    exit(-1);
  }
  
  if(table==kplus){ //Could be pointer
    Expert nexp(sogpparam,k_0,mu_0,v_0,lambda_0);
    experts.push_back(nexp);
    kplus++;
  }

  //Add to the expert
  experts[table].sit(in,out);
  assignments.push_back(table);
}

void Expert::sit(const ColumnVector& in, const ColumnVector& out){
  //printf("Sitting\n");
  //Add to the GP
  m_SOGP->add(in,out);
   //Add to the mean of the input and count;
  if(count==0){
    mean=in;
    sumsq = in*in.t();
  }
  else{
    mean *= count;
    mean+=in;
    mean*=(1/((double)count+1));
    sumsq+=in*in.t();
  }
  count++;

  //Update the SuffStat
  lp_tpp(in,true);
}

//The update should be done elsewhere? - split?
double Expert::lp_tpp(const ColumnVector& in,bool update){
  //Update to match dimensionality...Done elsewhere?
  int din = in.Nrows();
  if(mu_0.Nrows()!=din){
    double tmp=mu_0(1);
    mu_0.ReSize(din);
    for(int d=1;d<=din;d++)
      mu_0(d)=tmp;
  }
  if(lambda_0.Nrows()!=din){
    double tmp=lambda_0(1,1);
    lambda_0.ReSize(din,din);
    //I think this should be diagonal
    for(int i=1;i<=din;i++)
      for(int j=1;j<=din;j++)
	if(i==j)
	  lambda_0(i,j)=tmp;
	else
	  lambda_0(i,j)=0;
  }

  //printf("Expert::lp_tpp d=%d, count = %d\n",in.Nrows(),count);
  double n = count;
  double d = in.Nrows();
  double k_n,v_n;
  ColumnVector mu_n((int)d),zm((int)d);
  Matrix S((int)d,(int)d),lambda_n((int)d,(int)d);
  if(count==0){
    //printf("LP_TPP:No count\n");
    mu_n=mu_0;
    k_n=k_0;
    v_n=v_0;
    lambda_n=lambda_0;
  }
  else{
    //printf("LP_TPP:%d %d %d\n",count,mu_0.Nrows(),mean.Nrows());
    mu_n = (k_0/(k_0+n))*mu_0 + (n/(k_0+n))*mean;
    //printf("qq\n");
    k_n = k_0+n;
    v_n = v_0+n;
    S = sumsq - n*mean*mean.t();
    //
    Matrix s2 = n*mean*mean.t();
    zm = mean-mu_0;
    lambda_n=lambda_0;
    //printf("\t\tLam = %lf -> ",lambda_n(1,1));
    lambda_n+=S;
    //printf("%lf -> ",lambda_n(1,1));
    lambda_n+=(k_0*n/k_n)*(zm*zm.t());
    //printf("%lf        %lf %lf %lf\n",lambda_n(1,1),sumsq(1,1),s2(1,1),mean(1));
  }
  
  double v = v_n-2+1;
  Matrix Sigma = (lambda_n*(k_n+1))/(k_n*v);
  ColumnVector mu=mu_n;

  //The update
  if(update || count==0){
    LogAndSign ldet = Sigma.LogDeterminant();
    log_det_cov = ldet.LogValue();
    //printf("AA %d %d\n",Sigma.Nrows(),Sigma.Ncols());
    /*for(int i=1;i<=Sigma.Nrows();i++){
      for(int j=1;j<=Sigma.Ncols();j++){
	printf("%lf ",Sigma(i,j));
      }
      printf("\n");
      }*/
    inv_cov = Sigma.i();
    //printf("BB\n");
  }
  if(!update){
    //The predictive prob
    double foo;
    
    double vd=v+d;
    double lp;
    double ybit=((in-mu).t()*inv_cov*(in-mu)).AsScalar();
    lp=lngamma(vd/2,foo);
    //printf("lp_tpp: %.4lf -> ",lp);
    lp-=(lngamma(v/2,foo)+(d/2)*log(v)+(d/2)*log(M_PI));
    //printf("%.4lf -> ",lp);
    lp-= .5*log_det_cov;
    //printf("%.4lf -> ",lp);
    lp-=(vd/2)*log(1+(1/v)*ybit);
    //printf("%.4lf",lp);
    //printf("\n\tcount = %d, sigma=%.4lf, ldc=%.4lf, lambda=%lf, kn=%lf, v=%lf\n",
    //	   count,Sigma(1,1),log_det_cov,lambda_n(1,1),k_n,v);
    return lp;
  }
  return 0;
}

void ParticleFilter::addM(const Matrix& in, const Matrix& out){
  printf("Adding %d points to the PF\n",in.Ncols());
  for(int n=1;n<=in.Ncols();n++)
    add(in.Column(n),out.Column(n));
}

void ROGER::add_given(const ColumnVector &in,const ColumnVector &out){
  //Get the next given table
  double exp;
  fscanf(given_assignments,"%lf\n",&exp);
  static map<double,int> m_map;
  static int m_tables=0;
  int table=0;
  map<double,int>::iterator itr  = m_map.find(exp);
  if(itr==m_map.end()){
    printf("Mapping expert %lf to table %d\n",exp,m_tables);
    m_map[exp]=m_tables++;
  }
  table=m_map[exp];
  particles[0]->sitat(in,out,table);
}
void ROGER::add_given(const ColumnVector &in,const ColumnVector &out, int exp){
 count++;
  //printf("Adding point %d to a PF (%d=>%d)\n",count,in.Nrows(),out.Nrows());
  if(count==1){
    din=in.Nrows();
    dout=out.Nrows();
  }
  particles[0]->sitat(in,out,exp);
}

void ParticleFilter::add(const ColumnVector &in,const ColumnVector &out){
  count++;
  //printf("Adding point %d to a PF (%d=>%d)\n",count,in.Nrows(),out.Nrows());
  if(count==1){
    din=in.Nrows();
    dout=out.Nrows();
  }
  //This might be a separate learner?
  if(given_assignments){
    add_given(in,out);
    return;
  }
  

  if(count==1){
    //Initialize - first point sits at first table
    for(int p=0;p<NumParticles;p++)
      particles[p]->sitat(in,out,0);
    //printf("First person seated\n");
    new_particles=NumParticles*2;
  }
  else{
    //Verbose flag?
    if(verbose && count%10==0)
      printf("Input #%d, [kplus]=%3lf, mapi is %d (%lf) (mean = 1/%d = %lf) with %d experts\n",count,new_particles/(double)NumParticles,mapi,weights[mapi],NumParticles,1.0/NumParticles,particles[mapi]->experts.size());

    int newpi=0;
    double max_new_prob=0;
    double sum_new_prob=0;
    int next_p_mother[NumParticles];//The Family Tree
    int next_p_child[NumParticles];
    vector<int>other_inds; //Inidices of non-passed particles 

    //How likely are these particles? - Variable size....maybe prealloc?
    double probs[new_particles];
    int mother[new_particles];
    int child[new_particles];

    newpi=0;
    max_new_prob=0;
    //Each particle gives birth
    for(int p=0;p<NumParticles;p++){
      int kp=particles[p]->kplus;
      //One particle per expert
      for(int e=0;e<kp+1;e++,newpi++){
	//Track the lineage
	mother[newpi]=p;
	child[newpi]=e;
	double p1=0,p2=0,p3=0,p4=0;
	bool empty=false;
	if(e<kp){
	  //Weigh particles by CRP probability
	  probs[newpi]=(particles[p]->experts[e].count)/(count+alpha-1);
	  p1 = probs[newpi];
	  //and mother weight
	  probs[newpi]=log(probs[newpi])+log(weights[p]);	
	  p2 = probs[newpi];
	  //Get input weight
	  probs[newpi]+=particles[p]->experts[e].lp_tpp(in,false);
	  p3 = probs[newpi];
	  //And output
	  probs[newpi]+=particles[p]->experts[e].lp(in,out);
	  p4 = probs[newpi];
	}
	else{
	  empty=true;
	  probs[newpi]=log(weights[p])+log(alpha/(count+alpha-1));//CRP+parent
	  p2=probs[newpi];
	  //the empty GP
	  probs[newpi]+=empty_expert->lp_tpp(in,false);//input
	  p3 = probs[newpi];
	  probs[newpi]+=empty_expert->lp(in,out);//output
	  p4=probs[newpi];
	  //printf("Probability of empty expert for particle %d is %lf\n",p,p4);
	}
	if(probs[newpi]>max_new_prob)
	  max_new_prob=probs[newpi];

	if(isnan(probs[newpi])){
	  printf("Prob[%d] (%s) is NaN\n",newpi,empty?"empty":"not empty");
	  printf("Parts are %lf %lf %lf %lf\n",p1,p2,p3,p4);
	  printf("Came from particle %d, expert %d\n",p,e);
	  FILE *inout=fopen("inout_err.txt","w");
	  printCV(in,inout,"input");
	  printCV(out,inout,"output");
	  printf("Problem in/out written to inout_err.txt\n");
	  fclose(inout);
	  this->printTo("pf_err.txt");
	  printf("Problem PF printed to pf_err.txt\n");
	  particles[p]->printTo("part_err.txt");
	  printf("Problem particle printed to part_err.txt\n");	
	  particles[p]->experts[e].printTo("exp_err.txt");//If empty?
	  printf("Problem Expert printed to exp_err.txt\n");
	  exit(-1);
	}
      }
    }

    //normalize
    sum_new_prob=0;
    for(int i=0;i<new_particles;i++){
      //printf("%.4lf ",probs[i]);
      probs[i]=exp(probs[i]-max_new_prob);
      sum_new_prob+=probs[i];
    }
    for(int i=0;i<new_particles;i++){
      probs[i]/=sum_new_prob;
    }
    //printf("\t  And Normalized.  Searching for c\n");
    
    //Find which particles to pass (find_optimal_c)..There's got to be a better way*************
    RowVector Q(new_particles);
    for(int i=0;i<new_particles;i++){
      Q(i+1)=probs[i];
    }
    SortDescending(Q);
    double c = 0;
    int k = 0;
    int M = new_particles;
    double s=0;
    
    double k_old = -1;
    int numpass=0;
    
    while(k_old !=k && numpass <NumParticles){
      k_old = k;
      s = Q.Columns(k+1,M).Sum();
      c = (NumParticles-k)/s;
      int cnt=0;
      numpass=0;
      for(int i=1;i<=M;i++)
	if(Q(i)*c>1){
	  numpass++;
	  if(i>=k+1)
	    cnt++;
	}
      k = k+ cnt;
    }
    //printf("\tOptimal c found %lf (%lf)\n",c,1/c);***********************************************
    
    
    newpi=0;
    other_inds.clear();
    //Reuse the old weights
    //Everything > 1/c gets passed
    for(int i=0;i<new_particles;i++){
      if(probs[i]>1/c){
	next_p_mother[newpi]=mother[i];
	next_p_child[newpi]=child[i];
	weights[newpi]=probs[i];
	newpi++;
      }
      else
	other_inds.push_back(i);
    }

    //how many more to get?
    int more=NumParticles-newpi;
    //printf("\tPassing %d, get %d more\n",newpi,more);

    
    if(more>0){//*************************************************************************
      //Sample from others to get NP  
      //Randomly shuffle the points
      random_shuffle(other_inds.begin(),other_inds.end());
      int M=other_inds.size();
      double rw[M];
      int picked[M];
      double sum=0;
      for(int i=0;i<M;i++){
	picked[i]=0;
	rw[i]=probs[other_inds[i]];
	sum+=rw[i];
      }
      for(int i=0;i<M;i++){
	rw[i]/=sum;
	if(i>0)
	  rw[i]+=rw[i-1];
      }
      if(rw[M-1]!=1){
	//printf("rw = %lf",rw[M-1]);
	rw[M-1]=1;
      }
      double ps[more];
      double base=rand()/((double)RAND_MAX*more);
      ps[0]=base;
      if(more>1){
	double diff=(1-base)/(more-1);
	//printf("Want %d more, base = %lf, diff = %lf\n",more,base,diff);
	for(int i=1;i<more;i++)
	  ps[i]=ps[i-1]+diff;
      }
      int j=0;
      for(int i=0;i<more;i++){
	while(j<(M-1) && rw[j]<ps[i])
	  j=j+1;
	picked[j]++;
	//if(picked[j]>1)
	//printf("Picked %d is %d\n",j,picked[j]);
      }
	
      for(int  i=0;i<M;i++){
	if(picked[i]>0)
	  for(int j=0;j<picked[i];j++){
	    next_p_mother[newpi]=mother[other_inds[i]];
	    next_p_child[newpi]=child[other_inds[i]];
	    weights[newpi]=1/c;
	    newpi++;
	  }
      }
    }
    //printf("\tSampled.  Now propogate forward\n");******************************
    

    //Propogate
    //Who needs copying?
    int counts[NumParticles];
    for(int i=0;i<NumParticles;i++)
      counts[i]=0;
    for(int i=0;i<NumParticles;i++)
      counts[next_p_mother[i]]++;
    //Delete the old ones
    for(int i=0;i<NumParticles;i++)
      if(counts[i]==0){
	delete particles[i];
	particles[i]=NULL;
      }
    
    Particle *newp[NumParticles];
    for(int i=0;i<NumParticles;i++){
      if(counts[next_p_mother[i]]==1) //Just move this one
	newp[i]=particles[next_p_mother[i]];
      else{
	newp[i]=new Particle(particles[next_p_mother[i]]);
      }
      counts[next_p_mother[i]]--;
    }
    
    for(int i=0;i<NumParticles;i++){
      newp[i]->sitat(in,out,next_p_child[i]);
    }
    
    mapi=0;
    new_particles=NumParticles;
    for(int i=0;i<NumParticles;i++){
      particles[i]=newp[i];
      new_particles+=particles[i]->kplus;
      if(weights[i]>weights[mapi])
	mapi=i;
    }
  }
}

//These should be in Particles, no?
ReturnMatrix ParticleFilter::predict(const ColumnVector &in, double &sigconf, SAMPLE_SCHEME part_sample, SAMPLE_SCHEME expert_sample,int& part,int& expert,bool conf){

  //Pick out particle
  switch(part_sample){
  case MAX: part = mapi;break;
  case SAMPLE:{
    double sumw=0;
    for(int p=0;p<NumParticles;p++)
      sumw+=weights[p];
    double plimit=0;
    double randp=rand()/(double)(RAND_MAX);
    for(int p=0;p<NumParticles;p++){
      plimit+=weights[p]/sumw;
      if(randp<plimit){
	part=p;
	break;
      }
    }
  }
    break;
  case AVERAGE: printf("This particle case not written\n"); part=mapi; break;
  case FIXED: break;
  }
  Particle *P = particles[part];
  int numexp=P->experts.size();
  //double ew[numexp+1];
  ColumnVector m_out;  
  //Pick an expert
  double sume=0;
  //weight the experts - consider CRP and lp_tpp
  ew.clear();
  for(int e=0;e<numexp;e++){
    ew.push_back(0);
    //printf("calculating expert %d weight\n",e);
    ew[e]=log((P->experts[e].count)/(count+alpha-1));
    ew[e]+=P->experts[e].lp_tpp(in,false);
    ew[e]=exp(ew[e]);
    sume+=ew[e];
  }
  //Null expert
  //printf("Calculating null expert weight\n");
  ew.push_back(0);
  ew[numexp]=log(alpha/(count+alpha-1))+empty_expert->lp_tpp(in,false);//input
  ew[numexp]=exp(ew[numexp]);
  sume+=ew[numexp];
  for(int e=0;e<=numexp;e++)
    ew[e]/=sume;

  switch(expert_sample){
  case FIXED: break;
  case SAMPLE:{
    double elimit=0;
    double rande=rand()/(double)(RAND_MAX);
    for(int e=0;e<=numexp;e++){
      elimit+=ew[e];
      if(rande<elimit){
	expert=e;
	break;
      }
    }
  }
    break;
  case MAX:{
    expert = 0;
    for(int e=0;e<=numexp;e++)
      if(ew[e]>ew[expert])
	expert=e;
  }
    break;
  case AVERAGE: printf("ROGER::This expert case not written\n"); expert=0;break;
  }
  
  //Predict
  if(verbose)
    printf("ROGER::Predicting from expert %d (%lf) (numexp = %d)\n",expert,ew[expert]/sume,numexp);
  if(expert>numexp){
    printf("ROGER::Attempted to predict from expert %d, which does not exist (%d)",expert,numexp);
    expert=numexp;
  }
  if(expert==numexp){
    //Should return NULL...but we want the Sigma
    expert=numexp;
    m_out=empty_expert->predict(in,sigconf,conf);
    //printf("Predicting from empty expert, %d\n",m_out.Nrows());
    if(m_out.Nrows()!=dout){//always true?
      m_out.ReSize(dout);
      for(int d=1;d<=dout;d++)
	m_out(d)=0;
    }
  }
  else{
    m_out=P->experts[expert].predict(in,sigconf,conf);
  }
  //printf("predicted\n");
  m_out.Release();
  return m_out;
}



#define PVER 2
bool ParticleFilter::printTo(FILE *fp){
  if(!fp){
    printf("PF::printTo, bad fp\n");
    return false;
  }

  fprintf(fp,"PF save file version %d\n",PVER);
  fprintf(fp,"NP %d alpha %lf count %d mapi %d din %d dout %d\n",NumParticles,alpha,count,mapi,din,dout);
  fprintf(fp,"weights: ");
  //for(int i=0;i<NumParticles;i++)
  //fprintf(fp,"%lf ",weights[i]);
  fwrite(weights,sizeof(double),NumParticles,fp);
  fprintf(fp,"\n");
  for(int i=0;i<NumParticles;i++)
    particles[i]->printTo(fp);
  empty_expert->printTo(fp);
  fprintf(fp,"new_particles: %d\n",new_particles);
  return true;
}
bool ParticleFilter::readFrom(FILE *fp){
  if(!fp){
    printf("PF::readFrom, bad fp\n");
    return false;
  }

  int ver;
  fscanf(fp,"PF save file version %d\n",&ver);
  if(ver!=PVER){
    printf("PF save version mismatch.  %d != %d\n",ver,PVER);
    return false;
  }
  printf("ROGER::Reading in a PF save of version %d\n",ver);
  fscanf(fp,"NP %d alpha %lf count %d mapi %d din %d dout %d\n",&NumParticles,&alpha,&count,&mapi,&din,&dout);
  fscanf(fp,"weights: ");
  if(weights)
    delete weights;
  weights = new double[NumParticles];
  fread(weights,sizeof(double),NumParticles,fp);
  //for(int i=0;i<NumParticles;i++)
  //  fscanf(fp,"%lf ",&weights[i]);
  fscanf(fp,"\n");
  if(particles)
    delete particles;//Will this delete them?
  particles = new Particle*[NumParticles];
  //printf("Reading in %d particles\n",NumParticles);
  for(int i=0;i<NumParticles;i++){
    particles[i]=new Particle();
    particles[i]->readFrom(fp);
  }
  //printf("Reading in empty expert\n");
  empty_expert=new Expert();
  empty_expert->readFrom(fp);
  fscanf(fp,"new_particles: %d\n",&new_particles);
  //printf("PF read\n");
  return true;
}


bool Particle::printTo(FILE *fp){
  if(!fp){
    printf("Particle::printTo, bad fp\n");
    return false;
  }
  
  fprintf(fp,"Particle save file version 1\n");
  fprintf(fp,"k_0 %lf v_0 %lf kplus %d\n",k_0,v_0,kplus);
  //printRV(gpkp,fp,"gpkp");
  sogpparam.printTo(fp);
  printCV(mu_0,fp,"mu_0");
  printMatrix(lambda_0,fp,"lambda_0");
  fprintf(fp,"experts (%d)\n",experts.size());
  for(unsigned int i=0;i<experts.size();i++)
    experts[i].printTo(fp);
  fprintf(fp,"assignments (%d)\n",assignments.size());
  for(unsigned int i=0;i<assignments.size();i++)
    fprintf(fp,"%d ",assignments[i]);
  return true;
}
bool Particle::readFrom(FILE *fp){
  if(!fp){
    printf("Particle::readFrom, bad fp\n");
    return false;
  }
  
  int ver;
  fscanf(fp,"Particle save file version %d\n",&ver);
  //printf("Reading in a Particle save ver %d\n",ver);
  fscanf(fp,"k_0 %lf v_0 %lf kplus %d\n",&k_0,&v_0,&kplus);
  //readRV(gpkp,fp,"gpkp");
  sogpparam.readFrom(fp);
  readCV(mu_0,fp,"mu_0");
  readMatrix(lambda_0,fp,"lambda_0");
  int tmp;
  fscanf(fp,"experts (%d)\n",&tmp);
  experts.resize(tmp);
  //printf("Reading in %d experts\n",experts.size());
  for(unsigned int i=0;i<experts.size();i++)
    experts[i].readFrom(fp);
  fscanf(fp,"assignments (%d)\n",&tmp);
  assignments.resize(tmp);
  for(unsigned int i=0;i<assignments.size();i++)
    fscanf(fp,"%d ",&assignments[i]);
  //printf("Particle read\n");
  return true;
}

bool Expert::printTo(FILE *fp){
  if(!fp){
    printf("Expert::printTo fails, bad fp\n");
    return false;
  }
  fprintf(fp,"Expert save file version 1\n");
  fprintf(fp,"%d\n",count);
  m_SOGP->printTo(fp);
  printCV(mean,fp,"mean");
  printMatrix(sumsq,fp,"sumsq");
  printMatrix(inv_cov,fp,"inv_cov");
  fprintf(fp,"%lf %lf %lf\n",log_det_cov,k_0,v_0);
  printCV(mu_0,fp,"mu_0");
  printMatrix(lambda_0,fp,"lambda_0");
  return true;
}
bool Expert::readFrom(FILE *fp){
  if(!fp){
    printf("Expert::readFrom fails, bad fp\n");
    return false;
  }
  int ver;
  fscanf(fp,"Expert save file version %d\n",&ver);
  //printf("Reading in an expert save ver %d\n",ver);
  fscanf(fp,"%d\n",&count);
  m_SOGP = new SOGP();
  m_SOGP->readFrom(fp);
  readCV(mean,fp,"mean");
  readMatrix(sumsq,fp,"sumsq");
  readMatrix(inv_cov,fp,"inv_cov");
  fscanf(fp,"%lf %lf %lf\n",&log_det_cov,&k_0,&v_0);
  readCV(mu_0,fp,"mu_0");
  readMatrix(lambda_0,fp,"lambda_0");
  //printf("Expert read\n");
  return true;
}
