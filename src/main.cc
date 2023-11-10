#include "world.h"

int main(int argc, char *argv[]) {
    srand(time(NULL));
	/*run the cache simulator by typing commands with six parameters:
	sim_cache    <L1_BLOCKSIZE>    <L1_SIZE>    <L1_ASSOC>     
	<L1_REPLACEMENT_POLICY> <L1_WRITE_POLICY> <trace_file>
	*/
	if (argc != 7) {
        printf("Usage: %s <param1> <param2> <param3> <param4> <param5> <filename>\n", argv[0]);
        return 1;
    }

    int L1_BLOCKSIZE = atoi(argv[1]);
    int C = atoi(argv[2]);
    int E = atoi(argv[3]);
    int L1_REPLACEMENT_POLICY = atoi(argv[4]);
    int L1_WRITE_POLICY = atoi(argv[5]);

    char * pre = "../traces/";
     
    char * filename = (char*)malloc(30);
    strcpy(filename,pre);
    strcat(filename,argv[6]);

     printf("  ===== Simulator configuration ===== \n");

    printf("L1_BLOCKSIZE: %d\n", L1_BLOCKSIZE);
    printf("L1_SIZE: %d\n", C);
    printf("L1_ASSOC: %d\n", E);
    printf("L1_REPLACEMENT_POLICY: %d\n", L1_REPLACEMENT_POLICY);
    printf("L1_WRITE_POLICY: %d\n", L1_WRITE_POLICY);
    printf("trace_file: %s\n", argv[6]);
    printf("===================================\n");
    printf("\n");
    int b = (int)(log(L1_BLOCKSIZE)/log(2))+1;//bit偏移量
    int S;
    if(E==0)S = 1;
    else S = (C/L1_BLOCKSIZE)/E;//set
    int s = (int)(log(S)/log(2))+1;//set bit

     

	/*
		initalize the cache simulator with the above parameters
	*/
	struct cache_* cache = Initiate_Cache(L1_BLOCKSIZE, C, E, S, L1_WRITE_POLICY);
     
    
    get_trace(cache, L1_BLOCKSIZE, E, C,b,s, L1_REPLACEMENT_POLICY, L1_WRITE_POLICY, filename);
    printL1Contents(cache,L1_WRITE_POLICY);
    printf("\n");
    double mr = (double)(result.b+result.d)/(load_cnt+store_cnt);

    printf("====== Simulation results (raw) ======\n");
    printf("a. number of L1 reads: %d\n",load_cnt);
    printf("b. number of L1 read misses: %d\n",result.b);
    printf("c. number of L1 writes: %d\n",store_cnt);
    printf("d. number of L1 write misses: %d\n",result.d);
    printf("e. L1 miss rate: %.4f\n", mr);
    printf("f. number of writebacks from L1: %d\n",result.f);
    
    int traffic_WBWA = result.b+result.d+result.f;
    int traffic_WTNA = result.b+store_cnt;
    
    if(L1_WRITE_POLICY == WBWA) printf("g. total memory traffic: %d\n",traffic_WBWA);
    else if(L1_WRITE_POLICY==WTNA)printf("g. total memory traffic: %d\n",traffic_WTNA);
    printf("\n");
    printf("==== Simulation results (performance) ====\n");
    printf("1. average access time: %.4f ns\n",0.25 + 2.5 * (double)( 1.0000*C/ 512 / 1024) + 0.025 * (double)(1.0000*L1_BLOCKSIZE / 16) + (double)(0.025 * E) + (double)(mr*(double)(20.0000 + (double)(1.0000*L1_BLOCKSIZE / 16/2))));

    
    free_cache(cache);
return 0;

}
