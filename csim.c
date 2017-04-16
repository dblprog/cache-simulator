#include "cachelab.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

void usage(char** argv){
    fprintf(stderr,"Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n\
Options:\n\
  -h         Print this help message.\n\
  -v         Optional verbose flag.\n\
  -s <num>   Number of set index bits.\n\
  -E <num>   Number of lines per set.\n\
  -b <num>   Number of block offset bits.\n\
  -t <file>  Trace file.\n\
\n\
Examples:\n\
  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n\
  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n\
",argv[0],argv[0],argv[0]);
exit(0);
}

struct arguments
{
  int verbose;
  int s;
  int E;
  int b;
  char* trace;
};

typedef struct line{
    int valid;
    int tag;
    int usedrow;
} line;

line* row;

struct arguments args;

line* getCacheLine(int set, int tag){
    int min = 0, minrow = row[set*args.E].usedrow;
    for(int i = 0; i < args.E; i++){
        if(row[set*args.E+i].usedrow < minrow){
            minrow = row[set*args.E+i].usedrow;
            min = i;
        }
        if(row[set*args.E+i].tag == tag){
            min = i;
            break;
        }
    }
    //printf("%d",min);
    return &row[set*args.E+min];
}
int hits=0,misses=0,evict=0;

void simulate(){
    //int* cache = calloc(sizeof(int),1<<args.s*args.E*1<<args.b);
    row = calloc(sizeof(line),1<<args.s*args.E);
    
    FILE* trace = fopen(args.trace,"r");
    
    if(!trace){
        printf("Invalid trace filename\n");
        exit(1);
    }
    
    char t;
    long addr;
    int size;
    int row = 0;
    while(fscanf(trace, " %c %lx,%d\n", &t, &addr, &size) != EOF){
        if(t=='I'){
            continue;
        }
        int set = (addr>>args.b)&((1<<args.s) - 1);
        //int offset = (addr)&((1<<args.b) - 1);
        int tag = (addr>>args.b>>args.s);
        line* ln = getCacheLine(set,tag);
        if(args.verbose){
            printf("%c %lx,%d ",t,addr,size);
        }
        if(ln->valid && (ln->tag == tag)){
            if(args.verbose){
                printf("hit");
            }
            hits++;
        } else if (ln->valid){
            if(args.verbose){
                printf("miss eviction");
            }
            evict++;
            misses++;
            ln->tag = tag;
        } else {
            if(args.verbose){
                printf("miss");
            }
            misses++;
            ln->valid = 1;
            ln->tag = tag;
        }
        if(t=='M'){
            if(args.verbose){
                printf(" hit \n");
            }
            hits++;
        } else {
            if(args.verbose){
                printf(" \n");
            }
        }
        ln->usedrow = row;
        row++;
    }
    
}

int main(int argc, char **argv)
{
    args.verbose = 0;
    int c;
    
      while ((c = getopt (argc, argv, "hvt:s:E:b:")) != -1)
    switch (c)
      {
        case 'v':
          args.verbose = 1;
          break;
        case 't':
          args.trace = optarg;
          break;
        case 's':
          args.s = atoi(optarg);
          break;
        case 'E':
          args.E = atoi(optarg);
          break;
        case 'b':
          args.b = atoi(optarg);
          break;
        case 'h':
          usage(argv);
        default:
          usage(argv);
      }
    if (args.s == 0 || args.E == 0 || args.b == 0 || args.trace == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        usage(argv);
    }
    
    simulate();
    
    printSummary(hits, misses, evict);
    exit(0);
}
