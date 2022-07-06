#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#define N 1024

float A[N][N];
float B[N][N];
float C[N][N];
float val[N][N];

uint64_t nanos(){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    return start.tv_sec * 1e9 + start.tv_nsec;
}


#define BLOCK 16
int main(){

    FILE *f = fopen("/tmp/matmul", "rb");
    fread(A, 1, sizeof(float)*N*N, f);
    fread(B, 1, sizeof(float)*N*N, f);
    fread(val,1, sizeof(float)*N*N,f);
    fclose(f);


    assert(N%BLOCK == 0);
    uint64_t start = nanos();
     for(int by=0; by< N; by+=BLOCK){
        for(int bx=0; bx<N; bx+=BLOCK){
            // compute
            float tc[BLOCK][BLOCK];
            for(int y=0; y < BLOCK; y++){
                for(int x=0; x<BLOCK;x++){
                    float acc = 0;
                    for(int k=0; k<N; k++){
                        acc += A[by+y][k] * B[bx+x][k];
                    }
                    tc[y][x] = acc;
                }
            }

            //store
            for(int y = 0; y<BLOCK; y++){
                for (int x =0; x< BLOCK; x++){
                    C[by+y][bx+x] = tc[y][x];
                }
            }
        }
    }
    uint64_t end = nanos();
    double gflop = (N*N*2.0*N)*1e-9;
    double s = (end-start)*1e-9;
    printf("%f GFLOPs\n",gflop/s);

    for (int y = 0; y < N; y++){
        for(int x = 0; x < N; x++){
            if(fabs(C[y][x]-val[y][x]) > 1e-3){
                printf("MISMATCH AT %dx%d, %f != %f", y, x, C[y][x], val[y][x]);
                return -1;
            }
        }
    }

    return 0;
}