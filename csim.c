//517030910116    Jingwei Xi
//email: jingweixi@sjtu.edu.cn
//This is the program for simulating the behavior of a cache
#include "cachelab.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <memory.h>

typedef struct{
    int valid;
    long unsigned int tag;
    int timeRef;
}line;   // The line in cache array

typedef struct{
    int helpFlag;           //-h 
    int verboseFlag;        //-v
    int setBit;             //-s 
    int linePerSet;         //-e
    int blockBit;           //-b
    char *fileName;         //-t
}argument;     //The parameters of instruction

line *cache;
argument mainArg;

int setCount;
int blockSize;
int cacheSize;
int hit;
int miss;
int eviction;
int timeClock = 0;

void printHelp(){
	printf("Usage: ./csim-wrc [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
	printf("-h: Optional help flag that prints usage info\n");
    printf("-v: Optional verbose flag that displays trace info\n");
    printf("-s <s>: Number of set index bits (S = 2^s is the number of sets)\n");
    printf("-E <E>: Associativity (number of lines per set)\n");
    printf("-b <b>: Number of block bits (B = 2^b is the block size)\n");
    printf("-t <tracefile>: Name of the valgrind trace to replay\n");
}

void initMainArg(){
    mainArg.helpFlag = 0;
    mainArg.verboseFlag = 0;
    mainArg.setBit = 0;
    mainArg.linePerSet = 0;
    mainArg.blockBit = 0;
    mainArg.fileName = NULL;
}

void cacheAccess(char type, long unsigned int addr, int size){
    int hitFlag = 0, hitId = -1, emptyLine = -1, minTime = INT_MAX, evicId;
    int setIndex = 0;
    int dataTag;
    int i;
    
    //Address: |tag|setIndex|block_offset|
    setIndex = (addr / (blockSize)) % (setCount);
    dataTag = addr / (blockSize * setCount);

    for(i = setIndex * mainArg.linePerSet; i < (setIndex + 1) * mainArg.linePerSet; i++){
        if(cache[i].tag == dataTag){   //Hit
            hitFlag = 1;
            hitId = i;
            break;
        }
        if(cache[i].valid == 0 && emptyLine == -1){  //Record the empty line id
            emptyLine = i;
        }
        if(cache[i].timeRef < minTime){   //Find the block line with the least timeRef number
            minTime = cache[i].timeRef;
            evicId = i;
        }
    }

    if (mainArg.verboseFlag){
        printf("%c %lx,%x ",type,addr,size);
    }
    if(hitFlag == 1){    //Hit
        cache[hitId].timeRef = timeClock;
        hit++;
        if(type == 'M'){
            hit++;
        }
        if(mainArg.verboseFlag){
            if(type == 'S' || type == 'L'){
                printf("hit\n");
            }
            else{
                printf("hit hit\n");
            }
        }
        
    }
    else{
        if(emptyLine != -1){    //Miss but there is empty line
            cache[emptyLine].valid = 1;
            cache[emptyLine].tag = dataTag;
            cache[emptyLine].timeRef = timeClock;
            miss++;
            if(type == 'M'){
                hit++;
            }
            if(mainArg.verboseFlag){
                if(type == 'S' || type == 'L'){
                    printf("miss\n");
                }
                else{
                    printf("miss hit\n");
                }
            }
        }
        else{    //Miss and no empty line, need to evict
            cache[evicId].valid = 1;
            cache[evicId].tag = dataTag;
            cache[evicId].timeRef = timeClock;
            miss++;
            eviction++;
            if(type == 'M'){
                hit++;
            }
            if(mainArg.verboseFlag){
                if(type == 'S' || type == 'L'){
                    printf("miss evition\n");
                }
                else{
                    printf("miss eviction hit\n");
                }
            }           
        }
    }
}

int main(int argc, char* argv[])
{
    int opt;
    int i;
    char type;
    int size;
    long unsigned int addr;

    initMainArg();   //init each variable in argument struct 

    opt = getopt(argc, argv, "s:E:b:t:hv");
    if(opt == -1){     //Invalid arguments
        printHelp();
        return -1;
    }
    while(opt != -1) {
        switch(opt){
            case 'v':
                mainArg.verboseFlag = 1; /* true */
                break;          
            case 's':
                mainArg.setBit = atoi(optarg);
                break;               
            case 'E':
                mainArg.linePerSet = atoi(optarg);
                break;                 
            case 'b':
                mainArg.blockBit = atoi(optarg);
                break;            
            case 't':   
                mainArg.fileName = optarg;
                break;
            default:
                printHelp();
                break;
        }     
        opt = getopt(argc, argv, "s:E:b:t:hv");
    }

    setCount = 1 << (mainArg.setBit);
    blockSize = 1 << (mainArg.blockBit);

    FILE *file = fopen(mainArg.fileName, "r");
	if (file == NULL){   //Error: File not found
		printf("File not found.");
		return -1;
	}

    cache = (line *) malloc(setCount * mainArg.linePerSet * sizeof(line));
	if (cache == NULL){   //Error: Cache space allocated failed
		printf("Fail to allocate cache.");
		return -1;
	}

    cacheSize = setCount * mainArg.linePerSet;
    //Initialize the cache
	for (i = 0; i < cacheSize; i++) {
		cache[i].valid = 0;
		cache[i].timeRef = 0;
        cache[i].tag = -1;
	}

    while (!feof(file)){
		int tmp = fscanf(file, " %c %lx,%x", &type, &addr, &size);
		if (tmp != 3) continue;
		if (type == 'I') continue;
		cacheAccess(type, addr, size);
		timeClock++;
	}

	free(cache);    //Free the cache space malloced
	cache = NULL;
	printSummary(hit, miss, eviction);
    return 0;
}
