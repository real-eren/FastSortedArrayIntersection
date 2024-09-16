# Fast Sorted Int Array Intersection
A simple benchmark comparing several implementations for finding the intersection of two sorted arrays of unique ints.
This can be used as the core routine of a larger data structure designed for bulk set operations.

Considerable speed-ups were achieved by interleaving the processing of several arrays to
improve instruction-level parallelism.

Similar techniques can be used for `union` and similar operations.

## Build Instructions
`clang -mavx512f -mavx512bw -mavx512vl -mavx512cd -O3 main.c`  
`gcc -mavx512f -mavx512bw -mavx512vl -mavx512cd -O3 main.c`  

## CPU compatibility
### x86-64
Only the 'conflict' algorithms use AVX512, so `0`-`4` should work on any x86-64 processor.
### Other
Sorry, I haven't added the necessary preprocessor directives to conditionally define the AVX512 functions.
Your best bet is to modify the file by deleting those functions and removing the avx512 build flags.

## Profiling 
Most of the execution time is actually spent just preparing the input,
so `perf stat` and similar commands won't be very helpful.
You'll have better luck with profilers that can attribute metrics to specific instructions / functions,
such as `perf record -e ...` + `perf report`.

## Results
<details>
  <summary>R7 7700x branchy</summary>
  
```
algorithm 	avg time (ms)	avg thrpt (MB/s)
same range, more left: [0,1500]*900, [0,1500]*500, 10000 iters
branchy      	0.003767	1486.554688
csum:3000584

same range, fewer left: [0,1500]*500, [0,1500]*900, 10000 iters
branchy      	0.002662	2103.839600
csum:3000036

all left < right: [0,1500]*500, [1500,3500]*500, 10000 iters
branchy      	0.000191	20898.640625
csum:0

all left > right: [1500,3500]*500, [0,1500]*500, 10000 iters
branchy      	0.000279	14352.349609
csum:0

dense: [0,1500]*1400, [0,1500]*1200, 10000 iters
branchy      	0.002259	4602.992188
csum:11199671

sparse: [0,1500]*90, [0,1500]*50, 10000 iters
branchy      	0.000299	1672.800293
csum:29964

big: [0,15000]*9000, [0,15000]*9000, 1000 iters
branchy      	0.037584	1915.708862
csum:5399317

huge: [0,150000]*90000, [0,150000]*90000, 100 iters
branchy      	0.375270	1918.618652
csum:5400186

yomama: [0,1500000]*900000, [0,1500000]*900000, 10 iters
branchy      	3.703000	1944.369385
csum:5399859

yomama2: [0,9500000]*1200000, [0,9500000]*1500000, 10 iters
branchy      	6.375900	1693.878540
csum:1893647
```
</details>

<details>
  <summary>R7 7700x branchless</summary>

```
algorithm 	avg time (ms)	avg thrpt (MB/s)
same range, more left: [0,1500]*900, [0,1500]*500, 10000 iters
branchless   	0.001663	3367.408447
csum:2999860

same range, fewer left: [0,1500]*500, [0,1500]*900, 10000 iters
branchless   	0.001632	3432.213623
csum:2999926

all left < right: [0,1500]*500, [1500,3500]*500, 10000 iters
branchless   	0.000743	5382.131348
csum:0

all left > right: [1500,3500]*500, [0,1500]*500, 10000 iters
branchless   	0.000656	6094.773926
csum:0

dense: [0,1500]*1400, [0,1500]*1200, 10000 iters
branchless   	0.002205	4715.697754
csum:11200156

sparse: [0,1500]*90, [0,1500]*50, 10000 iters
branchless   	0.000211	2368.545654
csum:29867

big: [0,15000]*9000, [0,15000]*9000, 1000 iters
branchless   	0.018576	3875.968994
csum:5399375

huge: [0,150000]*90000, [0,150000]*90000, 100 iters
branchless   	0.185850	3874.092041
csum:5401112

yomama: [0,1500000]*900000, [0,1500000]*900000, 10 iters
branchless   	1.863700	3863.282715
csum:5399200

yomama2: [0,9500000]*1200000, [0,9500000]*1500000, 10 iters
branchless   	3.705000	2914.979736
csum:1894573
```
</details>

<details>
  <summary>R7 7700x branchless unrolled</summary>
  
```
algorithm 	avg time (ms)	avg thrpt (MB/s)
same range, more left: [0,1500]*900, [0,1500]*500, 10000 iters
b-less unroll	0.001656	3381.642578
csum:3000329

same range, fewer left: [0,1500]*500, [0,1500]*900, 10000 iters
b-less unroll	0.001629	3437.691895
csum:3001100

all left < right: [0,1500]*500, [1500,3500]*500, 10000 iters
b-less unroll	0.000736	5434.044434
csum:0

all left > right: [1500,3500]*500, [0,1500]*500, 10000 iters
b-less unroll	0.000647	6180.469727
csum:0

dense: [0,1500]*1400, [0,1500]*1200, 10000 iters
b-less unroll	0.002196	4735.667969
csum:11199622

sparse: [0,1500]*90, [0,1500]*50, 10000 iters
b-less unroll	0.000212	2359.603516
csum:29847

big: [0,15000]*9000, [0,15000]*9000, 1000 iters
b-less unroll	0.018592	3872.633301
csum:5400478

huge: [0,150000]*90000, [0,150000]*90000, 100 iters
b-less unroll	0.187370	3842.664307
csum:5402549

yomama: [0,1500000]*900000, [0,1500000]*900000, 10 iters
b-less unroll	1.857100	3877.012451
csum:5400524

yomama2: [0,9500000]*1200000, [0,9500000]*1500000, 10 iters
b-less unroll	3.703200	2916.396484
csum:1894396
```
</details>

<details>
  <summary>R7 7700x 'dual-wield'</summary>

```
algorithm 	avg time (ms)	avg thrpt (MB/s)
same range, more left: [0,1500]*900, [0,1500]*500, 10000 iters
dual-wield   	0.000985	5684.125488
csum:2999418

same range, fewer left: [0,1500]*500, [0,1500]*900, 10000 iters
dual-wield   	0.000958	5847.953125
csum:3000756

all left < right: [0,1500]*500, [1500,3500]*500, 10000 iters
dual-wield   	0.000460	8691.873047
csum:0

all left > right: [1500,3500]*500, [0,1500]*500, 10000 iters
dual-wield   	0.000408	9806.325195
csum:0

dense: [0,1500]*1400, [0,1500]*1200, 10000 iters
dual-wield   	0.001288	8073.907715
csum:11200327

sparse: [0,1500]*90, [0,1500]*50, 10000 iters
dual-wield   	0.000112	4484.305176
csum:29812

big: [0,15000]*9000, [0,15000]*9000, 1000 iters
dual-wield   	0.009879	7288.187500
csum:5401139

huge: [0,150000]*90000, [0,150000]*90000, 100 iters
dual-wield   	0.097930	7352.189941
csum:5401321

yomama: [0,1500000]*900000, [0,1500000]*900000, 10 iters
dual-wield   	0.977800	7363.468750
csum:5399679

yomama2: [0,9500000]*1200000, [0,9500000]*1500000, 10 iters
dual-wield   	1.911400	5650.308594
```
</details>

<details>
  <summary>R7 7700x 'tri-wield'</summary>
  
```
algorithm 	avg time (ms)	avg thrpt (MB/s)
same range, more left: [0,1500]*900, [0,1500]*500, 10000 iters
tri-wield    	0.000826	6779.661133
csum:3000937

same range, fewer left: [0,1500]*500, [0,1500]*900, 10000 iters
tri-wield    	0.000773	7246.376465
csum:2999878

all left < right: [0,1500]*500, [1500,3500]*500, 10000 iters
tri-wield    	0.000357	11204.482422
csum:0

all left > right: [1500,3500]*500, [0,1500]*500, 10000 iters
tri-wield    	0.000361	11086.474609
csum:0

dense: [0,1500]*1400, [0,1500]*1200, 10000 iters
tri-wield    	0.001040	10000.000000
csum:11202031

sparse: [0,1500]*90, [0,1500]*50, 10000 iters
tri-wield    	0.000093	5393.743164
csum:29969

big: [0,15000]*9000, [0,15000]*9000, 1000 iters
tri-wield    	0.008031	8965.258789
csum:5401165

huge: [0,150000]*90000, [0,150000]*90000, 100 iters
tri-wield    	0.080960	8893.280273
csum:5399488

yomama: [0,1500000]*900000, [0,1500000]*900000, 10 iters
tri-wield    	0.917600	7846.556152
csum:5401616

yomama2: [0,9500000]*1200000, [0,9500000]*1500000, 10 iters
tri-wield    	1.789800	6034.193359
csum:1894597
```
</details>

<details>
  <summary>R7 7700x conflict</summary>

```
algorithm 	avg time (ms)	avg thrpt (MB/s)
same range, more left: [0,1500]*900, [0,1500]*500, 10000 iters
conflict     	0.000503	11135.415039
csum:2998624

same range, fewer left: [0,1500]*500, [0,1500]*900, 10000 iters
conflict     	0.000450	12447.209961
csum:2969638

all left < right: [0,1500]*500, [1500,3500]*500, 10000 iters
conflict     	0.000231	17316.017578
csum:0

all left > right: [1500,3500]*500, [0,1500]*500, 10000 iters
conflict     	0.000296	13522.650391
csum:0

dense: [0,1500]*1400, [0,1500]*1200, 10000 iters
conflict     	0.000672	15483.102539
csum:11198590

sparse: [0,1500]*90, [0,1500]*50, 10000 iters
conflict     	0.000067	7507.507324
csum:29811

big: [0,15000]*9000, [0,15000]*9000, 1000 iters
conflict     	0.004746	15170.670898
csum:5398153

huge: [0,150000]*90000, [0,150000]*90000, 100 iters
conflict     	0.048030	14990.630859
csum:5400018

yomama: [0,1500000]*900000, [0,1500000]*900000, 10 iters
conflict     	0.478700	15040.735352
csum:5400369

yomama2: [0,9500000]*1200000, [0,9500000]*1500000, 10 iters
conflict     	0.771200	14004.149414
csum:1895597
```
</details>

<details>
  <summary>R7 7700x conflict2</summary>

```
algorithm 	avg time (ms)	avg thrpt (MB/s)
same range, more left: [0,1500]*900, [0,1500]*500, 10000 iters
conflict2    	0.000448	12494.421875
csum:2999524

same range, fewer left: [0,1500]*500, [0,1500]*900, 10000 iters
conflict2    	0.000424	13220.018555
csum:2969445

all left < right: [0,1500]*500, [1500,3500]*500, 10000 iters
conflict2    	0.000199	20060.179688
csum:0

all left > right: [1500,3500]*500, [0,1500]*500, 10000 iters
conflict2    	0.000218	18323.408203
csum:0

dense: [0,1500]*1400, [0,1500]*1200, 10000 iters
conflict2    	0.000556	18718.501953
csum:11199033

sparse: [0,1500]*90, [0,1500]*50, 10000 iters
conflict2    	0.000053	9398.497070
csum:30141

big: [0,15000]*9000, [0,15000]*9000, 1000 iters
conflict2    	0.002887	24939.382812
csum:5399516

huge: [0,150000]*90000, [0,150000]*90000, 100 iters
conflict2    	0.028000	25714.287109
csum:5398880

yomama: [0,1500000]*900000, [0,1500000]*900000, 10 iters
conflict2    	0.279000	25806.451172
csum:5401377

yomama2: [0,9500000]*1200000, [0,9500000]*1500000, 10 iters
conflict2    	0.437900	24663.164062
csum:1895484
```
</details>

<details>
  <summary>R7 7700x conflict3</summary>

```
algorithm 	avg time (ms)	avg thrpt (MB/s)
same range, more left: [0,1500]*900, [0,1500]*500, 10000 iters
conflict3    	0.000341	16407.853516
csum:3000280

same range, fewer left: [0,1500]*500, [0,1500]*900, 10000 iters
conflict3    	0.000333	16837.041016
csum:2969448

all left < right: [0,1500]*500, [1500,3500]*500, 10000 iters
conflict3    	0.000163	24539.878906
csum:0

all left > right: [1500,3500]*500, [0,1500]*500, 10000 iters
conflict3    	0.000157	25445.292969
csum:0

dense: [0,1500]*1400, [0,1500]*1200, 10000 iters
conflict3    	0.000437	23798.625000
csum:11198175

sparse: [0,1500]*90, [0,1500]*50, 10000 iters
conflict3    	0.000052	9523.809570
csum:29574

big: [0,15000]*9000, [0,15000]*9000, 1000 iters
conflict3    	0.002331	30888.029297
csum:5398239

huge: [0,150000]*90000, [0,150000]*90000, 100 iters
conflict3    	0.022800	31578.949219
csum:5399685

yomama: [0,1500000]*900000, [0,1500000]*900000, 10 iters
conflict3    	0.267800	26885.736328
csum:5400954

yomama2: [0,9500000]*1200000, [0,9500000]*1500000, 10 iters
conflict3    	0.401100	26925.953125
csum:1894314
```
</details>
