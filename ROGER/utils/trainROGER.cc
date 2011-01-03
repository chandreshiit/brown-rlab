/*Train a roger from a file

*/

#include "ROGER.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc,const char **argv){
	if(argc<2){
		printf("Usage:  trainROGER -i inputfile indim -o outputfile outdim -s savefile\n");
		printf("Or help for more detailed Roger parameters\n");
		exit(-1);
	}

	int pdim=-1,adim=-1;
	FILE *ifile=NULL,*ofile=NULL,*sfile=NULL;
	ROGER *m_roger=NULL;
	//const char **argvL=NULL;
	//int argcL=0;
	unsigned int seed = time(NULL);
	  //from args
	//m_expert = SAMPLE_EXPERT;
	double w=.1;
	double n=.1;
	int cap = 300;
	ColumnVector mu(1);
	Matrix lambda(1,1);
	mu(1)=0;
	lambda(1,1)=1;
	int P=10;
	double alpha=0.5;
	double k_0=.1;
	bool verbose=false;
	double v_0=pdim+1;
	const char *given_file=NULL;
	double tmp = -1;

	for(int argi=1;argi<argc;argi++){
		if(!strcmp(argv[argi],"-i")){
			if(!(ifile = fopen(argv[++argi],"r")))
				perror("Error opening inputfile:\n");
			printf("Reading input from %s, with dimensionality ",argv[argi]);
			pdim = atoi(argv[++argi]);
			printf("%d\n",pdim);
		}
		if(!strcmp(argv[argi],"-o")){
			if(!(ofile = fopen(argv[++argi],"r")))
				perror("Error opening outputfile:\n");
			printf("Reading output from %s, with dimensionality ",argv[argi]);
			adim = atoi(argv[++argi]);
			printf("%d\n",adim);
		}
		if(!strcmp(argv[argi],"-s")){
			if(!(sfile = fopen(argv[++argi],"w")))
				perror("Error opening savefile:\n");
		}

		
		if(!strcmp(argv[argi],"help")){
			printf("ROGER [-p particles -h alpha -l lambda_0 -k kappa_0 -u nu -v -g file -x expert (-1 to sample, -2 for MAP)]\n");
			printf("SOGP options are: [-c capacity -w width -n noise]\n");
			return false;
		}
		if(!strcmp(argv[argi],"-d"))
			seed=atoi(argv[++argi]);
		if(!strcmp(argv[argi],"-p"))
			P=atoi(argv[++argi]);
		if(!strcmp(argv[argi],"-h"))
			alpha=atof(argv[++argi]);
		if(!strcmp(argv[argi],"-l"))
			lambda(1,1)=atof(argv[++argi]);
		if(!strcmp(argv[argi],"-c"))
			cap=atoi(argv[++argi]);
		if(!strcmp(argv[argi],"-w"))
			w=atof(argv[++argi]);
		if(!strcmp(argv[argi],"-n"))
			n=atof(argv[++argi]);
		if(!strcmp(argv[argi],"-k"))
			k_0=atof(argv[++argi]);
		if(!strcmp(argv[argi],"-v"))
			verbose=true;
		if(!strcmp(argv[argi],"-g"))
			given_file=argv[++argi];
		if(!strcmp(argv[argi],"-u"))
			tmp=atof(argv[++argi]);
		//if(!strcmp(argv[argi],"-x"))
			//m_expert=atoi(argv[++argi]);
	}
	if(tmp<0)
		v_0 = pdim+1;
	else if(tmp<=pdim+1)
		printf("v_0 must be >= num_dim+1 (%lf)\n",v_0);
	else
		v_0=tmp;
	printf("Making ROGER SOGP Params %lf %lf %d\n",w,n,cap);
	RBFKernel kern(w);
	SOGPParams params(&kern);
	params.s20=n;
	params.capacity=cap;
 
	//Should this be tunable?


	printf("Starting ROGER with %d particles, alpha = %lf, lambda = %lf (%lf), kappa = %lf\n",
			 P,alpha,lambda(1,1),v_0,k_0);
	
	
	m_roger = new ROGER(P,alpha,params,k_0,mu,v_0,lambda);
	m_roger->verbose=verbose;
	if(given_file){
		printf("Using given experts from %s\n",given_file);
		m_roger->use_given(given_file);
	}
	ColumnVector ins(pdim),outs(adim);
	double p;
	double a;

	srand(seed);
	int cnt=0;
	time_t start=time(NULL);
	while(fscanf(ifile,"%lf ",&p)!=EOF){
		ins(1) = p;
		for(int i=1;i<pdim;i++){
			fscanf(ifile,"%lf ",&(p));
			ins(i+1) = p;
		}
		for(int i=0;i<adim;i++){
			fscanf(ofile,"%lf ",&(a));
			outs(i+1) = a;
		}
		m_roger->add(ins,outs);
		fflush(stdout);//For when using Tee
		cnt++;
	}
	time_t end = time(NULL);
	long int hour=0,min=0,sec=end-start;
	min = sec/60;
	sec = sec%60;
	hour = min/60;
	min = min%60;
  printf("Trained on %d points with seed %d in %2ld:%2ld:%2ld time\n",cnt,seed,hour,min,sec);

  m_roger->printTo(sfile);
  fclose(sfile);
  fclose(ifile);
  fclose(ofile);
}
