#!/usr/bin/env python3
import time
import numpy as np

N = 1024

if __name__ == "__main__":
    A = np.random.randn(N, N).astype(np.float32)
    B = np.random.randn(N, N).astype(np.float32)

    flop = N*N*2*N
    # print(f"{flop / 1e9:.2f} GFLOP")

    for i in range(10):
        st = time.monotonic()
        C = A @ B
        et = time.monotonic()
        s = et - st

        print(f"{flop/s * 1e-9:.2f} GFLOPS")

    # Compute the theoritical flops
    # Online 267.4 Gflops
    # 8 cores