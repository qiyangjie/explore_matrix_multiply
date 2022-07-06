#include <stdint.h>
#include <time.h>
#include <stdio.h>
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

int main(){
    FILE *f = fopen("/tmp/matmul", "rb");
    fread(A, 1, sizeof(float)*N*N, f);
    fread(B, 1, sizeof(float)*N*N, f);
    fread(val,1, sizeof(float)*N*N,f);
    fclose(f);

    // printf("%f\n", A[10][10]);


    uint64_t start = nanos();
    for(int y=0; y< N; y++){
        for(int x=0; x<N; x++){
            float acc = 0;
            for(int k=0; k<N; k++){
                acc += A[y][k] * B[x][k];
            }
            C[y][x] = acc;
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