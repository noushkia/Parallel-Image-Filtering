# Parallel-Image-Filtering
The third project of OS course offered at the University of Tehran

# Serial Runtime
Time taken for reading image: 2.62 ms
Time taken for blur filter: 27.16 ms
Time taken for sepia filter: 6.38 ms
Time taken for mean filter: 6.91 ms
Time taken for X filter: 0.37 ms
Total time taken for applying filters and writing images: 45.92 ms


# Parallel Runime
Time taken for reading image: 1.01 ms (Speedup = $\frac{2.62}{1.2} \approx 2.60$)
    For 40 threads : 1.5
    FOR 16 threads : 1.2 -> best approach
    For 10 threads : 1.3
    For 8  threads : 1
    For 4  threads : 1.3
Time taken for  Time taken for blur filter : 8.02 ms
Time taken to write on real_pixel: 3.16 ms -> new hotspot (could optimize by using dynamic arrays instead of vectors)
Time taken for sepia filter: 2.36 ms
Time taken to write on real_pixel: 3.12 ms
Time taken for mean filter: 5.05 ms
Time taken to write on real_pixel: 3.31 ms
Time taken for X filter: 0.32 ms
Time taken to write on real_pixel: 0.00 ms
Total time taken for applying filters and writing images: 28.43 ms