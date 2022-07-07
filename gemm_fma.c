#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <immintrin.h>

//# define DEBUG

#ifdef DEBUG
    #define N 8
#else
    #define N 1024
#endif

#define BLOCK 8

// aligned, what we dod
float A[N*N] __attribute__ ((aligned (32)));
float B[N*N] __attribute__ ((aligned (32)));
float C[N*N] __attribute__ ((aligned (32)));
float val[N*N] __attribute__ ((aligned (32)));

__m256 *Am = (__m256*)A;
__m256 *Bm = (__m256*)B;
__m256 *Cm = (__m256*)C;


uint64_t nanos(){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    return start.tv_sec * 1e9 + start.tv_nsec;
}

#define BLOCK 4
int main(){

#ifdef DEBUG
    for(int i=0; i < N*N) A[i] = i;
    for(int i=0; i < N*N) B[i] = i;
#else
    FILE *f = fopen("/tmp/matmul", "rb");
    fread(A, 1, sizeof(float)*N*N, f);
    fread(B, 1, sizeof(float)*N*N, f);
    fread(val,1, sizeof(float)*N*N,f);
    fclose(f);
#endif
    assert(N%BLOCK == 0);
    uint64_t start = nanos();
    for(int by=0; by< N; by+=BLOCK){
        for(int bx=0; bx<N; bx+=BLOCK){
            float tc[BLOCK][BLOCK];
            for (int y = 0; y < BLOCK; y++){
                for (int x = 0; x < BLOCK; x++){
                    __m256 tmp = {};
                    for (int k = 0; k < N; k+=8){
                        tmp = _mm256_fmadd_ps(
                            Am[((by+y)*N + k)/8],
                            Bm[((bx+x)*N + k)/8],
                            tmp);
                    }

                    float ftmp = 0.0;
                    for (int i =0; i < 8; i++) ftmp += tmp[i];
                    tc[y][x] = ftmp;
                }
            }

            for (int y = 0; y < BLOCK; y++){
                for (int x = 0; x < BLOCK; x++){
                    C[(by+y)*N+bx+x] = tc[y][x];
                }
            }
        }
    }

    uint64_t end = nanos();
    double gflop = (N*N*2.0*N)*1e-9;
    double s = (end-start)*1e-9;
    printf("%f GFLOPs\n",gflop/s);

    // for (int y = 0; y < N; y++){
    //     for(int x = 0; x < N; x++){
    //         if(C[y][x] != val[y][x]){
    //             printf("MISMATCH AT %dx%d, %f != %f", y, x, C[y][x], val[y][x]);
    //             return -1;
    //         }
    //     }
    // }
    for (int k = 0; k < N*N; k++) {
        if (fabsf(C[k] - val[k]) > 1e-3) {
            printf("MISMATCH AT %d, %f != %f\n", k, C[k], val[k]);
            return -1;
            }
    }
    printf("match\n");

    return 0;
}