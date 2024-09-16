# Fast Sorted Int Array Intersection
A simple benchmark comparing several implementations for finding the intersection of two sorted arrays of unique ints.
This can be used as the core routine of a larger data structure designed for bulk set operations.

Considerable speed-ups were achieved by interleaving the processing of several arrays to
improve instruction-level parallelism.

Similar techniques can be used for `union` and similar operations.

## Build Instructions
### Non-AVX512
Only includes scalar implementations  
`clang -O3 main.c`  
`gcc -O3 main.c`  
### AVX512
Includes scalar and AVX512 implementations  
`clang -mavx512f -mavx512bw -mavx512vl -mavx512cd -O3 main_avx512.c`  
`gcc -mavx512f -mavx512bw -mavx512vl -mavx512cd -O3 main_avx512.c`  

## Profiling 
Most of the execution time is actually spent just preparing the input,
so `perf stat` and similar commands won't be very helpful.
You'll have better luck with profilers that can attribute metrics to specific instructions / functions,
such as `perf record -e ...` + `perf report`.

## Results
Throughput in MB/s

### R7 7700x
|   Algorithm   |   A   |   B   |   C   |   D   |   E   |   F  |   G   |   H   |   I   |   J   |
| ------------- | ----: | ----: | ----: | ----: | ----: | ---: | ----: | ----: | ----: | ----: |
| branchy       |  1661 |  2119 | 21085 | 14331 |  4617 | 1682 |  1928 |  1931 |  1944 |  1712 |
| branchless    |  3432 |  3466 |  5446 |  6097 |  4764 | 2433 |  3903 |  3859 |  3859 |  2904 |
| b-less unroll |  3429 |  3420 |  5356 |  6177 |  4724 | 2410 |  3863 |  3861 |  3881 |  2926 |
| dual-wield    |  5859 |  5768 |  8673 | 10045 |  8171 | 4508 |  7454 |  7497 |  7350 |  5707 |
| tri-wield     |  6682 |  6802 | 10632 | 11148 |  8980 | 5005 |  8024 |  8041 |  7174 |  5651 |
| conflict      | 10666 | 10432 | 16694 |  8490 | 13379 | 7507 | 11894 | 11879 | 11824 | 12982 |
| conflict2     | 12762 | 13661 | 17929 | 13531 | 18258 | 9433 | 20833 | 22291 | 22065 | 23146 |
| conflict3     | 16696 | 16622 | 23460 | 19333 | 22797 | 9398 | 28560 | 29813 | 24615 | 25447 |

### i7 9750h
|   Algorithm   |  A   |  B   |   C   |  D   |  E   |   F  |  G   |  H   |  I   |  J   |
| ------------- | ---: | ---: | ----: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| branchy       | 1070 | 1198 | 12121 | 7365 | 2152 | 1047 | 1177 | 1170 | 1168 | 1074 |
| branchless    | 2197 | 2228 |  3329 | 3331 | 3024 | 1607 | 2441 | 2471 | 2478 | 1878 |
| b-less unroll | 2254 | 2276 |  3374 | 3358 | 3078 | 1620 | 2489 | 2544 | 2503 | 1910 |
| dual-wield    | 3900 | 3901 |  5899 | 5895 | 5068 | 2762 | 4401 | 4425 | 4395 | 3386 |
| tri-wield     | 3410 | 3370 |  5069 | 5269 | 4688 | 2357 | 3790 | 3619 | 3628 | 2747 |
