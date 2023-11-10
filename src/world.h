#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<malloc.h>
#include<unistd.h>
#include<time.h>
#define WBWA 0
#define WTNA 1
#define LRU 0
#define LFU 1


struct cache_line{ //Cache行 
	int valid;       
	int dirty;      
	int time_stamp; 
	int count_block;
	int count_set;
	unsigned int tag;   
	int* data;       //指向数据数组的指针
}; 

struct cache_{
    int B; //Block size, B=2^b, b is the number of block bits
	int C; //total size of cache, C=S*B*E, S is the number of sets
	int E; //Associativity (number of lines per set)
    int S; //number of sets, S=2^s, s is the number of set index bits
	struct cache_line **line;  //指向Cache line的二维数组的指针
};


struct cache_* Initiate_Cache(int BB, int CC , int EE, int SS, int allocate_strategy){
    struct cache_* cache; //指向Cache的指针
	cache = (struct cache_*)malloc(sizeof(struct cache_));
	cache->E = EE;//associativity
	cache->C = CC;//cache size
	cache->B = BB; //block size
    cache->S = SS; //number of sets
	cache->line = (struct cache_line **)malloc(sizeof(struct cache_line *) * cache->S); 
	int i,j;
	for(i = 0; i < cache->S; i++){//各组(set)的初始化 
		cache->line[i] = (struct cache_line *)malloc(sizeof(struct cache_line) *EE);
		//cache->line[i].count_set = 0;
		for(j = 0; j < cache->E; j++){//各行(line)的初始化
			cache->line[i][j].valid = 0;
			if(allocate_strategy == 0)
				cache->line[i][j].dirty = 0;
			else cache->line[i][j].dirty = -1;
			cache->line[i][j].time_stamp = 0;
			cache->line[i][j].tag = -1;
			cache->line[i][j].count_block = 0;
			cache->line[i][j].count_set = 0;
			cache->line[i][j].data = (int *)malloc(sizeof(int) * cache->B);
		}
	}
	return cache;
};

void free_cache(struct cache_* cache)
{
	int i,j;
	//将每个cache line对应的数据数组释放掉
        for(i = 0; i < cache->S; i++)
	    for(j = 0; j < cache->E; j++)
	        free(cache->line[i][j].data);
	//将cache line二维数组释放掉
	free(cache->line);
	//将cache释放掉
	free(cache);
};

void printL1Contents(struct cache_* cache,int allocate_strategy){
	printf("===== L1 contents =====\n");
	for(int i = 0; i < cache->S; i++){
		printf("set %d: ",i);
		for(int j = 0; j < cache->E; j++){
			printf("%x ",cache->line[i][j].tag);
			if(cache->line[i][j].dirty==1) printf(" D");
		}
		printf("\n");
	}
};

int get_index(struct cache_* cache, unsigned long op_s, unsigned long op_tag){
    int i;
    for(i = 0; i < cache->E; i++){
        if(cache->line[op_s][i].valid && cache->line[op_s][i].tag == op_tag){
			return i;
		}
	}
    return -1;
};

int find_LRU(struct cache_* cache, unsigned long op_s){ 
	int max_index = 0; 
	int max_stamp = 0;
	int i; 
	for(i = 0; i < cache->E; i++){
	    if(cache->line[op_s][i].time_stamp > max_stamp){
	        max_stamp = cache->line[op_s][i].time_stamp;
	        max_index = i;
	    }
	}
	return max_index;
};

int find_LFU(struct cache_*cache,unsigned long op_s){
	int min_index = 0;
	int min_count_block = 0x7fffffff;
	for(int i = 0 ;i<cache->E;i++){
		if(cache->line[op_s][i].count_block < min_count_block){
			min_count_block = cache->line[op_s][i].count_block;
			min_index = i;
		}
	}
	return min_index;
};


int is_full(struct cache_* cache, unsigned long op_s){
	int i;
	for(i = 0; i < cache->E; i++){
		if(cache->line[op_s][i].valid == 0){
			return i;
		}     
	} 
    return -1;
};

void update(struct cache_* cache, int i, unsigned long op_s, unsigned long op_tag, char operation, char state, int allocate_strategy,int replace_strategy){ 
    if(operation == 'w' && allocate_strategy == WTNA && state == 'm'); 
    else{
		cache->line[op_s][i].valid = 1;
        if(operation == 'w' && allocate_strategy == WBWA) 
	    	cache->line[op_s][i].dirty = 1;
		if(operation == 'r' && allocate_strategy == WBWA && state == 'm')//如果是读操作，不命中，且是写分配\写回策略，那么dirty位置为0; 读命中的情况是不对dirty位进行修改的
	    	cache->line[op_s][i].dirty = 0;
		if(replace_strategy == LRU){
			for(int k = 0; k< cache->E; k++)//遍历组中的各行，对于有效行，时间戳加1 
	    	if(cache->line[op_s][k].valid == 1)
	        	cache->line[op_s][k].time_stamp++;
        	cache->line[op_s][i].time_stamp = 0;//操作行的时间戳置为最小，也就是0
			cache->line[op_s][i].tag = op_tag;
		} 
	
		else if(replace_strategy == LFU){
			if(operation == 'w'){
				
				cache->line[op_s][i].count_block = cache->line[op_s][i].count_set+1;
			}
			else if(operation == 'r'){
				
				cache->line[op_s][i].count_block++;
			}
			for(int j = 0 ;j<cache->E;j++){
				cache->line[op_s][i].count_set = cache->line[op_s][i].count_block;
			}
			cache->line[op_s][i].tag = op_tag;
		}
    }
};

struct Calculate{
	int b=0,d=0;//b读缺失次数，d写缺失次数
	int f=0;//写回次数
}result;

void update_info(struct cache_* cache, unsigned long op_s, unsigned long op_tag, int operation,int E, int replace_strategy, int allocate_strategy){
	char state;
	int index = get_index(cache, op_s, op_tag);
	if(index == -1){
		state = 'm';
		if(operation == 'r') result.b++;
		else if(operation == 'w'){
			result.d++;
		}
	
		int i = is_full(cache, op_s);
		if(i == -1){	
			if (replace_strategy == LRU){
				i = find_LRU(cache, op_s);
				
				if(allocate_strategy == WBWA && cache->line[op_s][i].dirty == 1) result.f++;
			}
			
			else if(replace_strategy == LFU){
				i = find_LFU(cache, op_s);
				if(allocate_strategy == WBWA && cache->line[op_s][i].dirty == 1) result.f++;
			}
		}
		update(cache, i, op_s, op_tag, operation, state, allocate_strategy,replace_strategy); 
	}
	else{
		state = 'h'; //state = hit
		update(cache, index, op_s, op_tag, operation, state, allocate_strategy,replace_strategy);
	}
};
int load_cnt = 0;
	int store_cnt = 0;
int get_trace(struct cache_* cache, int B, int E, int C, int b, int s,int replace_strategy, int allocate_strategy, char* t){   
	/*t为指向追踪文件名字符串的指针*/
	int iteration = 1; //迭代次数,为的是在特定的迭代次数下，输出一定的信息方便调试
	char operation; //每次迭代的操作
	//打开追踪文件
	FILE *pFile;
	pFile = fopen(t,"r");//t为指向追踪文件名字符串的指针
	
	// if(pFile == NULL){   
	// 	//如果文件被没有找到，报错并终止 
	//     printf("Trace File not found\n");
	//     return -1;
	// }   //如果找到了文件则继续 
	// else printf("Trace File exists!Start reading trace...\n");

	unsigned long address;
	unsigned long op_tag;
	unsigned long op_s;
	unsigned long block_offset;
	
	char line[15];
	while(fgets(line,15,pFile)!=NULL){
		sscanf(line,"%s %I64X", &operation, &address);
		int Mask = (1 << s) - 1;
	    //tag是address右移(b+s)位 
        op_tag = address >> (s+b);
	    /*获取s，先右移 b位，再用无符号 0xFF...右移后进行"与"操作将tag抹去
	    为什么要用无符号 0xFF... 右移呢？因为C语言中的右移为算术右移，
		有符号数右移补位的数为符号位
		*/
			op_s = (address >> b) & Mask;
		
	    block_offset = address & ((unsigned int)(-1) >> (32 - b)); //块内偏移量 
	    //printf("address: %I64X, op_s: %u,op_tag: %u,block_offset: %u\n",address,op_s,op_tag,block_offset);
	    switch(operation){
	        case 'r':
			//printf("read: %I32X,tag: %u,op_s: %u",address,op_tag,op_s);
				load_cnt++;
	            update_info(cache, op_s, op_tag, operation, E, replace_strategy, allocate_strategy);
				
		    break;
	        case 'w':
			//printf("Write: %I32X,tag: %u,op_s: %u",address,op_tag,op_s);
				store_cnt++;
		    	update_info(cache, op_s, op_tag, operation, E, replace_strategy, allocate_strategy);
		    break;
	    }
	    iteration++;
	}
	fclose(pFile);
	//printf("Trace reading finished!load:%d store:%d\n",load_cnt,store_cnt);
	//printf("读缺失次数:%d 写缺失次数:%d 写回次数:%d\n",result.b,result.d,result.f);
	return 0;
};