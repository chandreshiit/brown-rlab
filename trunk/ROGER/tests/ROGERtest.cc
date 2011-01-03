/*
A little testing program for ROGER
Dang, 2008

Test sampling schemes?

Need a verbose flag.
*/

#include "ROGER.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

typedef ParticleFilter ROGER;

#define THRESH 1e-4

int main(int argc, char **argv){
  int seed = time(NULL);
  bool valgrind = true;
  
  for(int argi=1;argi<argc;argi++){
    if(!strcmp(argv[argi],"-s"))
      seed = atoi(argv[++argi]);
    if(!strcmp(argv[argi],"-v"))
      valgrind = false;
  }

  printf("Seed is %d\n",seed);
  srand(seed);

  ROGER *m_ROGER = NULL;
  ColumnVector pred;
  double mse,sigma; 
  int part,expert; 
  
  //Params
  SOGPParams params;//Default should be good
  
  ColumnVector mu(1);
  Matrix lambda(1,1);
  mu(1)=0.5;
  lambda(1,1)=2;//?
  double alpha=0.5;
  double k = .01;
  double v = 2;
  int P = 100;

  //Generate data
  int N = 100;
  RowVector inputs(N),outputs(N),outX(N);
  for(int n=1;n<=N;n++){
    inputs(n)= -1 + rand()/(RAND_MAX/2.0);
    outputs(n)=sin(inputs(n));//sinc?
    outX(n)=inputs(n);
    double neg = -1 + rand()/(RAND_MAX/2.0);
    if(neg<0)
      outX(n)=-outX(n);
  }

  //Test regression.  How is ROGER better?
  printf("Testing regression, 1 expert, same as SOGP\n");
  SOGP *m_SOGP = new SOGP(params);
  m_ROGER = new ROGER(P,alpha,params,k,mu,v,lambda);
  for(int n=1;n<=N;n++){
		//printf("%d\n",n);
    m_SOGP->add(inputs.Column(n),outputs.Column(n));
    m_ROGER->add(inputs.Column(n),outputs.Column(n));
  }
  mse=0;
  double mse2=0;
  for(int n=1;n<=N;n++){
    pred = m_ROGER->predict(inputs.Column(n),sigma,MAX,SAMPLE,part,expert);
    double err = outputs(n)-pred(1);
    mse+=(err*err);
    pred = m_SOGP->predict(inputs.Column(n),sigma);
    err = outputs(n)-pred(1);
    mse2+=(err*err);
  }
  mse/=N;
  mse2/=N;
  printf("MAP is %d with weight %Lf and %d experts. \t %s\n",
	 m_ROGER->mapi,
	 m_ROGER->weights[m_ROGER->mapi],
	 m_ROGER->particles[m_ROGER->mapi]->experts.size(),
	 m_ROGER->particles[m_ROGER->mapi]->experts.size()==1?"PASS":"FAIL");
  printf("MSE = %.10lf, SOGP = %.10lf \t %s\n",mse,mse2,mse<THRESH?"PASS":"FAIL");
  delete m_SOGP;

  //Test File IO
  m_ROGER->printTo("savedROGER.txt");
  delete m_ROGER;
  m_ROGER = new ROGER;
  m_ROGER->readFrom("savedROGER.txt");
  m_ROGER->printTo("resavedROGER.txt");
  printf("Save and Load test.  Differences between stars\n******\n");
  system("diff -a savedROGER.txt resavedROGER.txt");
  printf("******\n");
  for(int n=1;n<=N;n++){
    pred = m_ROGER->predict(inputs.Column(n),sigma,MAX,SAMPLE);
    double err = outputs(n)-pred(1);
    mse+=(err*err);
  }
  mse/=N;
  printf("MAP is %d with weight %Lf and %d experts\n",
	 m_ROGER->mapi,
	 m_ROGER->weights[m_ROGER->mapi],
	 m_ROGER->particles[m_ROGER->mapi]->experts.size());
  printf("MSE = %.10lf \t\t\t\t %s\n",mse,mse<THRESH?"PASS":"FAIL");

  delete m_ROGER;
  
  //Test on X data
  printf("Testing on X data, multiple experts necessary\n");
  m_ROGER = new ROGER(P,alpha,params,k,mu,v,lambda);
  for(int n=1;n<=N;n++){
    m_ROGER->add(inputs.Column(n),outX.Column(n));
  }
  printf("MAP is %d with weight %Lf and %d experts. \t %s\n",
	 m_ROGER->mapi,
	 m_ROGER->weights[m_ROGER->mapi],
	 m_ROGER->particles[m_ROGER->mapi]->experts.size(),
	 m_ROGER->particles[m_ROGER->mapi]->experts.size()>1?"PASS":"FAIL");
  mse=0;
  for(int n=1;n<=N;n++){
    pred = m_ROGER->predict(inputs.Column(n),sigma,MAX,SAMPLE);
    double err1 = outX(n)-pred(1);
    double err2 = outX(n)+pred(1);
    mse+=min(err1*err1,err2*err2);
  }
  mse/=N;
  printf("MSE-corrected = %.10lf \t\t\t %s\n",mse,"NONE");//mse<THRESH?"PASS":"FAIL");
  delete m_ROGER;

  //Test empty expert
  expert=5;
  printf("Testing empty expert prediction\n");
  m_ROGER = new ROGER(P,alpha,params,k,mu,v,lambda);
  pred = m_ROGER->predict(inputs.Column(1),sigma,MAX,FIXED,part,expert);
  printf("No data returns expert %d with nrows = %d. \t %s\n",expert,pred.Nrows(),pred.Nrows()==0?"PASS":"FAIL");
  m_ROGER->add(inputs.Column(1),outputs.Column(1));
  pred = m_ROGER->predict(inputs.Column(1),sigma,MAX,FIXED,part,expert);
  printf("Data returns expert %d with nrows = %d. \t\t %s\n",expert,pred.Nrows(),pred.Nrows()==1?"PASS":"FAIL");
  delete m_ROGER;
  m_ROGER = new ROGER(P,alpha,params,k,mu,v,lambda);
  ColumnVector out(10);
  m_ROGER->add(inputs.Column(1),out);
  pred = m_ROGER->predict(inputs.Column(1),sigma,MAX,FIXED,part,expert);
  printf("Multidim data returns expert %d with nrows = %d.  %s\n",expert,pred.Nrows(),pred.Nrows()==10?"PASS":"FAIL");
  delete m_ROGER;

  //Test verbose..redirect output?
  printf("Testing verbose\n");
  m_ROGER = new ROGER(P,alpha,params,k,mu,v,lambda);
  m_ROGER->verbose=true;
  for(int n=1;n<=N;n++)
    m_ROGER->add(inputs.Column(n),outX.Column(n));
  m_ROGER->predict(inputs.Column(1),sigma);

  //Test given experts
  printf("Testing given experts\n");
  delete m_ROGER;
  m_ROGER = new ROGER(1,alpha,params,k,mu,v,lambda);
  FILE *fp=fopen("given_experts.txt","w");
  fprintf(fp,"%lf\n%lf\n%lf\n%lf\n%lf\n%lf",1.0,2.0,5.0,3.0,4.3,1.0);
  fclose(fp);
  m_ROGER->use_given("given_experts.txt");
	m_ROGER->verbose=true;
  for(int i=1;i<=6;i++)
    m_ROGER->add(inputs.Column(i),outputs.Column(i));
  printf("Assignments are:\n");
  m_ROGER->particles[0]->printass(stdout);
  printf("\n");
	printf("Probability of these assignments given params is: %Lf\n",m_ROGER->weights[0]);
	//Test save/read
	m_ROGER->printTo("savedROGER.txt");
  delete m_ROGER;
  m_ROGER = new ROGER;
  m_ROGER->readFrom("savedROGER.txt");
  m_ROGER->printTo("resavedROGER.txt");
  printf("Save and Load test.  Differences between stars\n******\n");
  system("diff -a savedROGER.txt resavedROGER.txt");
  printf("******\n");

  //Test Lp_tpp

  //Test multidimensionality

  //Test sampling
  
  //Test for memory leaks
  if(valgrind){//Doesn't run it?
    printf("Testing for memory leaks with valgrind, errors between stars\n");
    printf("*****\n");
    system("valgrind -q test -v");
    printf("*****\n");
  }

  //Clean up after yourself
  system("rm -f *.txt");

}
