/* Name: Benjamin Challe
 * CS login: challe
 * Section(s): Lecture 2 Spring 2018
 *
 * csim.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss plus a possible eviction.
 *  2. Instruction loads (I) are ignored.
 *  3. Data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus a possible eviction.
 *
 * The function printSummary() is given to print output.
 * Please use this function to print the number of hits, misses and evictions.
 * This is crucial for the driver to evaluate your work. 
 */

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/****************************************************************************/
/***** DO NOT MODIFY THESE VARIABLE NAMES ***********************************/

/* Globals set by command line args */
int s = 0; /* set index bits */
int E = 0; /* associativity */
int b = 0; /* block offset bits */
int verbosity = 0; /* print trace if set */
char* trace_file = NULL;

/* Derived from command line args */
int B; /* block size (bytes) B = 2^b */
int S; /* number of sets S = 2^s In C, you can use the left shift operator */

/* Counters used to record cache statistics */
int hit_cnt = 0;
int miss_cnt = 0;
int evict_cnt = 0;
/*****************************************************************************/


/* Type: Memory address 
 * Use this type whenever dealing with addresses or address masks
 */
typedef unsigned long long int mem_addr_t;

/* Type: Cache line
 * TODO 
 * 
 * NOTE: 
 * You might (not necessarily though) want to add an extra field to this struct
 * depending on your implementation
 * 
 * For example, to use a linked list based LRU,
 * you might want to have a field "struct cache_line * next" in the struct 
 */
 
 int max = 0; //variable for counting evict status
 
typedef struct cache_line {                     
    char valid;
    mem_addr_t tag;
    int victim_number;
} cache_line_t;

typedef cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;


/* The cache we are simulating */
cache_t cache;  

/* TODO - COMPLETE THIS FUNCTION
 * initCache - 
 * Allocate data structures to hold info regrading the sets and cache lines
 * use struct "cache_line_t" here
 * Initialize valid and tag field with 0s.
 * use S (= 2^s) and E while allocating the data structures here
 */
void initCache() {                      
  //bit shift to get s and b bits
      S = 1<<s; 
      B = 1<<b;
      //allocate for sets
  cache = malloc(sizeof(cache_line_t*) * S);
    //allocates for lines
  for(int x = 0; x<S; x++){
    cache[x] = malloc(sizeof(cache_line_t) * E);
  }
  //for each set and line initialize to 0
  for(int x = 0; x<S; x++){
    for(int y =0; y <E; y++){
     cache[x][y].valid = 0; 
     cache[x][y].tag = 0;
     cache[x][y].victim_number = 0;
    }
  }
  }
  
/* TODO - COMPLETE THIS FUNCTION 
 * freeCache - free each piece of memory you allocated using malloc 
 * inside initCache() function
 */
void freeCache() {   
   //free each set
    for(int x =0; x<S; x++){
      free(cache[x]);
    }           
    //then free the cache
    free(cache); 
}

/* TODO - COMPLETE THIS FUNCTION 
 * accessData - Access data at memory address addr.
 *   If it is already in cache, increase hit_cnt
 *   If it is not in cache, bring it in cache, increase miss count.
 *   Also increase evict_cnt if a line is evicted.
 *   you will manipulate data structures allocated in initCache() here
 */
void accessData(mem_addr_t addr) { 
    //get the t bits and calculate the T number
    int t = (64 - s - b);
   int T = 1<<t;
   //find the mask for both S and T by bit shifting
   mem_addr_t maskS = (S-1) << b;
   mem_addr_t maskT = (T-1) << (s+b);
   //calculate the set number and the address
   int set_number = (maskS & addr) >> b;
   int address = (maskT & addr) >> (s+b);
   //for each line in the found set
   for (int x = 0; x<E; x++){
   //see if the cache is valid and the tag matches if so increment the hit count and the victim number
   if((cache[set_number][x].tag == address) && (cache[set_number][x].valid == 1)){
       hit_cnt++;
       cache[set_number][x].victim_number = max++;
       return;
   }
   }
   //if not return increase the miss count because it wasnt a hit
   miss_cnt++;
   //for each line in the set
   for(int x = 0; x<E; x++){
   //cache not full
   //see if one line isnt used if found update the line to addr
     if(cache[set_number][x].valid == 0){
       cache[set_number][x].valid = 1;
       cache[set_number][x].tag = address;
       cache[set_number][x].victim_number = max++;
       return;
     }
   }
   //capacity miss
   int index_of_evicted =0;
   //for each line find the line to be evicted
 for(int x = 0; x<E; x++){
   if (cache[set_number][x].victim_number < cache[set_number][index_of_evicted].victim_number){
       index_of_evicted = x;
   }
 }
 //evict the line by replacing its data with the tag and the victim number and increment the evicted count
   cache[set_number][index_of_evicted].tag = address;
   cache[set_number][index_of_evicted].victim_number = max++;
   evict_cnt++;
   return;
}

/* TODO - FILL IN THE MISSING CODE
 * replayTrace - replays the given trace file against the cache 
 * reads the input trace file line by line
 * extracts the type of each memory access : L/S/M
 * YOU MUST TRANSLATE one "L" as a load i.e. 1 memory access
 * YOU MUST TRANSLATE one "S" as a store i.e. 1 memory access
 * YOU MUST TRANSLATE one "M" as a load followed by a store i.e. 2 memory accesses 
 */
void replayTrace(char* trace_fn) {                      
    char buf[1000];
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if (!trace_fp) {
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while (fgets(buf, 1000, trace_fp) != NULL) {
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);
      
            if (verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

            // TODO - MISSING CODE
            // now you have: 
            // 1. address accessed in variable - addr 
            // 2. type of acccess(S/L/M)  in variable - buf[1] 
            if(buf[1] == 'S'){
              accessData(addr);
            }else if(buf[1] == 'L'){
              accessData(addr);
            }else if(buf[1] == 'M'){
              accessData(addr);
              accessData(addr);
            }
            // call accessData function here depending on type of access

            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[]) {                 
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * printSummary - Summarize the cache simulation statistics. Student cache simulators
 *                must call this function in order to be properly autograded.
 */
void printSummary(int hits, int misses, int evictions) {                        
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

/*
 * main - Main routine 
 */
int main(int argc, char* argv[]) {                      
    char c;
    
    // Parse the command line arguments: -h, -v, -s, -E, -b, -t 
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'h':
                printUsage(argv);
                exit(0);
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            default:
                printUsage(argv);
                exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

    /* Initialize cache */
    initCache();

    replayTrace(trace_file);

    /* Free allocated memory */
    freeCache();

    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_cnt, miss_cnt, evict_cnt);
    return 0;
}
