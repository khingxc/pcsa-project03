#define SIZEX 1000
#define SIZEY 1000

long * huge_matrixA;
long * huge_matrixB;
long * huge_matrixC;

FILE *fin1, *fin2, *fout, *ftest;

void flush_all_caches();
void load_matrix_base();
void free_all();
void multiply_base();
void compare_results();

// Your job is to finish these three functions in the way that it is fast
void write_results();
void load_matrix();
void multiply();

// Any additional variables needed go here
#define MAXBUF 256
#define NUM_THREAD 10;
long row = SIZEX;	    //row
long col = SIZEY;	    //column
long BLOCK_SIZE = 100;


// Any helper functions go here

