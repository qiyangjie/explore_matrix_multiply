#include <stdint.h>
#include <time.h>
#include <stdio.h>
#define N 1024

float A[N][N];
float B[N][N];
float C[N][N];

uint64_t nanos(){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    return start.tv_sec * 1e9 + start.tv_nsec;
}

int main(){
    uint64_t start = nanos();
    for(int y=0; y< N; y++){
        for(int x=0; x<N; x++){
            float acc = 0;
            for(int k=0; k<N; k++){
                acc += A[y][k] * B[k][x];
            }
            C[y][x] = acc;
        }
    }
    uint64_t end = nanos();
    double gflop = (N*N*2.0*N)*1e-9;
    double s = (end-start)*1e-9;
    printf("%f GFLOPs\n",gflop/s);

    return 0;
}