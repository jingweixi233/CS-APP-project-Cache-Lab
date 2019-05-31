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
    int usedTime;
}line;

typedef struct{
    int helpFlag;
    int verboseFlag;
    int setBit;
    int linePerSet;
    int blockBit;
    char *fileName;
}argument;

line *cache;
argument mainArg;

int setCount;
int blockSize;
int cacheSize;
char type;
int size;
long unsigned int addr;
int hit;
int miss;
int eviction;
int timeClock = 0;

void printHelp(){
	printf("Usage: ./csim-wrc [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
	printf("• -h: Optional help flag that prints usage info\n");
    printf("• -v: Optional verbose flag that displays trace info\n");
    printf("• -s <s>: Number of set index bits (S = 2^s is the number of sets)\n");
    printf("• -E <E>: Associativity (number of lines per set)\n");
    printf("• -b <b>: Number of block bits (B = 2^b is the block size)\n");
    printf("• -t <tracefile>: Name of the valgrind trace to replay\n");
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
    int setId;
    int dataTag;
    int i;
    
    setId = (addr / (blockSize)) % (setCount);
    dataTag = addr / (blockSize * setCount);

    for(i = 0; i < cacheSize; i++){
        if(cache[i].tag == dataTag){
            hitFlag = 1;
            hitId = i;
            break;
        }
        if(cache[i].valid == 0 && emptyLine == -1){
            emptyLine = i;
        }
        if(cache[i].usedTime < minTime){
            minTime = cache[i].usedTime;
            evicId = i;
        }
    }
    
    if (mainArg.verboseFlag){
        printf("%c %lx,%x",type,addr,size);
    }
    if(hitFlag == 1){
        //situation 1
        cache[hitId].usedTime = timeClock;
        hit++;   
        if(mainArg.verboseFlag){
            if(type == 'S' | type == 'L'){
                printf("hit\n");
            }
            else{
                hit++;
                printf("hit hit\n");
            }
        }
        
    }
    else{
        if(emptyLine != -1){
            //situation 2
            cache[emptyLine].valid = 1;
            cache[emptyLine].tag = dataTag;
            cache[emptyLine].usedTime = timeClock;
            miss++;
            if(mainArg.verboseFlag){
                if(type == 'S' | type == 'L'){
                    printf("miss\n");
                }
                else{
                    hit++;
                    printf("miss hit\n");
                }
            }
        }
        else{
            //situation 3
            cache[evicId].valid = 1;
            cache[evicId].tag = dataTag;
            cache[evicId].usedTime = timeClock;
            miss++;
            eviction++;
            if(mainArg.verboseFlag){
                if(type == 'S' | type == 'L'){
                    printf("miss evition\n");
                }
                else{
                    hit++;
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

    initMainArg();

    opt = getopt(argc, argv, "v:s:E:b:t");
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
        opt = getopt(argc, argv, "v:s:E:b:t");
    }

    setCount = pow(2, mainArg.setBit);
    blockSize = pow(2, mainArg.blockBit);

    FILE *file = fopen(mainArg.fileName, "r");
	if (file==NULL){
		printf("file not found.");
		return -1;
	}

    cache = (line *) malloc(setCount * mainArg.linePerSet * sizeof(line));
	if (cache == NULL){
		printf("Fail to allocate cache.");
		return 3;
	}
    cacheSize = setCount * mainArg.linePerSet;
	for (i = 0; i < cacheSize; i++) {
		cache[i].valid = 0;
		cache[i].usedTime = 0;
        cache[i].tag = 0;
	}

    while (!feof(file)){
		int tmp = fscanf(file, " %c %lx,%x", &type, &addr, &size);
		if (tmp != 3) continue;
		if (type =='I') continue;
		cacheAccess(type,addr,size);
		timeClock++;
	}

	free(cache);
	cache = NULL;
	printSummary(hit, miss, eviction);
    return 0;
     
}