/*Train a roger from a file

*/

#include "ROGER.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
enum MODE{TRANS,MM,MS,F};

int main(int argc,const char **argv){
	if(argc<2){
		printf("Usage:  predictRoger -r ROGERfile -i inputFile indim [-m {trans | mm | ms | f stateFile}] [-o outputfile outdim] -s savefile\n");
		printf("Or help for more detailed Roger parameters\n");
		exit(-1);
	}
	
	int mode = TRANS;
	int pdim=-1,adim=-1;
	FILE *rfile=NULL,*ifile=NULL,*ofile=NULL,*sfile=NULL,*gfile=NULL;
	ROGER *m_roger=NULL;

	for(int argi=1;argi<argc;argi++){
		if(!strcmp(argv[argi],"-r")){
			if(!(rfile = fopen(argv[++argi],"r")))
				perror("Error opening ROGERfile:\n");
			printf("Reading ROGER from %s\n",argv[argi]);
			m_roger = new ROGER();
			bool read = m_roger->readFrom(rfile);
			if(!read)
				perror("Error loading ROGERfile:\n");
		}
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
		if(!strcmp(argv[argi],"-m")){
			if(!strcmp(argv[argi+1],"trans"))
				mode = TRANS;
			else if(!strcmp(argv[argi+1],"mm"))
				mode = MM;
			else if(!strcmp(argv[argi+1],"ms"))
				mode = MS;
			else if(!strcmp(argv[argi+1],"f")){
				mode = F;
				if(!(gfile = fopen(argv[++argi+1],"r")))
					perror("Error opening state file:\n");
			}
			argi++;
		}
	}
	
	Particle* part = m_roger->particles[m_roger->mapi];
	const int kplus = part->kplus;
	double trans[kplus][kplus];
	if(mode == TRANS){
		for(uint i = 0; i < part->assignments.size()-1; i++){
			trans[part->assignments[i]][part->assignments[i+1]]++;
		}
		double sum;
		for(int i = 0; i < kplus; i++){
			sum = 0;
			for(int j = 0; j < kplus; j++){
				sum+= trans[i][j];
			}
			for(int j = 0; j < kplus; j++){
				trans[i][j]= log(trans[i][j]/sum);
			}
		}
	}
	int e;
	int laste = 0;
	double p = 0;
	int pt = 0;
	double sig = 0;
	double weight;
	double max_weight;
	int cnt = 0;
	ColumnVector ins(pdim),outs;
	while(fscanf(ifile,"%lf ",&p)!=EOF){
		e = 0;
		ins(1) = p;
		for(int i=1;i<pdim;i++){
			fscanf(ifile,"%lf ",&(p));
			ins(i+1) = p;
		}
		switch(mode){
			case TRANS:
				if(cnt==0){
					max_weight = part->experts[0]->lp_tpp(ins,false);
					for(int i = 0; i < kplus; i++){
						weight = part->experts[i]->lp_tpp(ins,false);
						if(weight > max_weight){
							max_weight = weight;
							e = i;
						}
					}
				}
				else{
					max_weight = trans[laste][0]+part->experts[0]->lp_tpp(ins,false);
					for(int i = 1; i < kplus; i++){
						weight = trans[laste][i]+part->experts[i]->lp_tpp(ins,false);
						if(weight > max_weight){
							max_weight = weight;
							e = i;
						}
					}
				}
				outs = m_roger->predict(ins, sig, MAX, FIXED,pt,e);
				break;
			case MM:
				outs = m_roger->predict(ins, sig, MAX, MAX,pt,e);
				break;
			case MS:
				outs = m_roger->predict(ins, sig, MAX, SAMPLE,pt,e);
				break;
			case FIXED:
				fscanf(gfile,"%d ",&e);
				outs = m_roger->predict(ins, sig, MAX, FIXED,pt,e);
		}
		for(int i = 1; i <= outs.Nrows(); i++){
			fprintf(sfile,"%lf ",outs(i));
		}
		fprintf(sfile,"\n");
		laste = e;
		cnt++;
	}
	fclose(sfile);
	fclose(ifile);
	//fclose(ofile);
}
