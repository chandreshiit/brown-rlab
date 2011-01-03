#include "ROGER.h"
#include <string.h>

int main(int argc,char **argv){
  if(argc<3){
    printf("Usage: print_ROGER policy [w ass mapi count]\n");
    exit(-1);
  }

	ROGER m_ROGER;
  m_ROGER.readFrom(argv[1]);
	for(int argi=2;argi<argc;argi++){
		//printf("Performing option %s\n",argv[argi]);
		if(!(strcmp(argv[argi],"ass"))){
			//printf("Matched ass!\n");
			m_ROGER.particles[m_ROGER.mapi]->printass(stdout);
		}
		if(!(strcmp(argv[argi],"w"))){
			for(int i=0;i<m_ROGER.NumParticles;i++)
				printf("%Lf ",m_ROGER.weights[i]);
		}
		if(!(strcmp(argv[argi],"mapi"))){
			printf("%d ",m_ROGER.mapi);
		}
		if(!(strcmp(argv[argi],"count"))){
			printf("%d ",m_ROGER.count);
		}

	}
	printf("\n");
}
