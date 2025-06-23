# mbw - Memory Bandwidth Benchmark

`mbw` is a simple command-line utility to measure memory bandwidth on a system. It performs memory copy operations and reports the throughput in MiB/s.

## Recent Updates: Multi-threading Support

To better saturate the memory controller and achieve a more accurate measurement of maximum memory bandwidth, this version introduces multi-threading. The memory copy workload is now divided among multiple worker threads.

### Key Changes:
- **Parallel Execution**: The benchmark now spawns multiple POSIX threads (`pthreads`) to perform the memory copy operations in parallel. Each thread works on a separate chunk of the allocated memory arrays.
- **New Command-Line Option**: A new `-w` option has been added to specify the number of worker threads to use.
  - **Usage**: `mbw -w <number_of_threads> ...`
  - **Default**: If not specified, the benchmark defaults to using 8 worker threads.

## Compilation

```sh
cd mbw
make
```

## Performance Differences

Runtime Environment:
AMD EPYC 9965 192-Core Processor with 12 channels 16 GiB DDR5-4800 RAM (192 GiB total)

Previous:
```
./mbw 1024
Long uses 8 bytes. Allocating 2*134217728 elements = 2147483648 bytes of memory.
Using 262144 bytes as blocks for memcpy block copy test.
Getting down to business... Doing 10 runs per test.
0       Method: MEMCPY  Elapsed: 0.05318        MiB: 1024.00000 Copy: 19256.808 MiB/s
1       Method: MEMCPY  Elapsed: 0.05124        MiB: 1024.00000 Copy: 19985.947 MiB/s
2       Method: MEMCPY  Elapsed: 0.05086        MiB: 1024.00000 Copy: 20134.492 MiB/s
3       Method: MEMCPY  Elapsed: 0.05090        MiB: 1024.00000 Copy: 20118.273 MiB/s
4       Method: MEMCPY  Elapsed: 0.05217        MiB: 1024.00000 Copy: 19628.139 MiB/s
5       Method: MEMCPY  Elapsed: 0.06335        MiB: 1024.00000 Copy: 16164.422 MiB/s
6       Method: MEMCPY  Elapsed: 0.05230        MiB: 1024.00000 Copy: 19577.853 MiB/s
7       Method: MEMCPY  Elapsed: 0.05203        MiB: 1024.00000 Copy: 19682.466 MiB/s
8       Method: MEMCPY  Elapsed: 0.05182        MiB: 1024.00000 Copy: 19761.473 MiB/s
9       Method: MEMCPY  Elapsed: 0.05202        MiB: 1024.00000 Copy: 19683.223 MiB/s
AVG     Method: MEMCPY  Elapsed: 0.05299        MiB: 1024.00000 Copy: 19325.860 MiB/s
0       Method: DUMB    Elapsed: 0.11460        MiB: 1024.00000 Copy: 8935.817 MiB/s
1       Method: DUMB    Elapsed: 0.11467        MiB: 1024.00000 Copy: 8929.895 MiB/s
2       Method: DUMB    Elapsed: 0.11390        MiB: 1024.00000 Copy: 8990.106 MiB/s
3       Method: DUMB    Elapsed: 0.11388        MiB: 1024.00000 Copy: 8992.000 MiB/s
4       Method: DUMB    Elapsed: 0.11261        MiB: 1024.00000 Copy: 9093.573 MiB/s
5       Method: DUMB    Elapsed: 0.10991        MiB: 1024.00000 Copy: 9316.629 MiB/s
6       Method: DUMB    Elapsed: 0.11041        MiB: 1024.00000 Copy: 9274.102 MiB/s
7       Method: DUMB    Elapsed: 0.10951        MiB: 1024.00000 Copy: 9350.830 MiB/s
8       Method: DUMB    Elapsed: 0.10971        MiB: 1024.00000 Copy: 9333.273 MiB/s
9       Method: DUMB    Elapsed: 0.10935        MiB: 1024.00000 Copy: 9364.426 MiB/s
AVG     Method: DUMB    Elapsed: 0.11186        MiB: 1024.00000 Copy: 9154.668 MiB/s
0       Method: MCBLOCK Elapsed: 0.08316        MiB: 1024.00000 Copy: 12314.353 MiB/s
1       Method: MCBLOCK Elapsed: 0.08357        MiB: 1024.00000 Copy: 12253.494 MiB/s
2       Method: MCBLOCK Elapsed: 0.08374        MiB: 1024.00000 Copy: 12227.596 MiB/s
3       Method: MCBLOCK Elapsed: 0.08459        MiB: 1024.00000 Copy: 12105.450 MiB/s
4       Method: MCBLOCK Elapsed: 0.08405        MiB: 1024.00000 Copy: 12183.514 MiB/s
5       Method: MCBLOCK Elapsed: 0.08464        MiB: 1024.00000 Copy: 12099.013 MiB/s
6       Method: MCBLOCK Elapsed: 0.08485        MiB: 1024.00000 Copy: 12069.067 MiB/s
7       Method: MCBLOCK Elapsed: 0.08421        MiB: 1024.00000 Copy: 12160.365 MiB/s
8       Method: MCBLOCK Elapsed: 0.08482        MiB: 1024.00000 Copy: 12071.913 MiB/s
9       Method: MCBLOCK Elapsed: 0.08570        MiB: 1024.00000 Copy: 11948.379 MiB/s
AVG     Method: MCBLOCK Elapsed: 0.08433        MiB: 1024.00000 Copy: 12142.470 MiB/s
```

After:
```
./mbw -w 16 1024
Long uses 8 bytes. Allocating 2*134217728 elements = 2147483648 bytes of memory.
Using 16 worker threads.
Using 262144 bytes as blocks for memcpy block copy test.
Getting down to business... Doing 10 runs per test.
0       Method: MEMCPY  Elapsed: 0.01661        MiB: 1024.00000 Copy: 61668.172 MiB/s
1       Method: MEMCPY  Elapsed: 0.01804        MiB: 1024.00000 Copy: 56772.190 MiB/s
2       Method: MEMCPY  Elapsed: 0.01594        MiB: 1024.00000 Copy: 64248.965 MiB/s
3       Method: MEMCPY  Elapsed: 0.01977        MiB: 1024.00000 Copy: 51795.650 MiB/s
4       Method: MEMCPY  Elapsed: 0.01485        MiB: 1024.00000 Copy: 68965.517 MiB/s
5       Method: MEMCPY  Elapsed: 0.02181        MiB: 1024.00000 Copy: 46946.635 MiB/s
6       Method: MEMCPY  Elapsed: 0.01515        MiB: 1024.00000 Copy: 67613.074 MiB/s
7       Method: MEMCPY  Elapsed: 0.01518        MiB: 1024.00000 Copy: 67443.852 MiB/s
8       Method: MEMCPY  Elapsed: 0.01658        MiB: 1024.00000 Copy: 61757.433 MiB/s
9       Method: MEMCPY  Elapsed: 0.01528        MiB: 1024.00000 Copy: 67011.321 MiB/s
AVG     Method: MEMCPY  Elapsed: 0.01692        MiB: 1024.00000 Copy: 60520.095 MiB/s
0       Method: DUMB    Elapsed: 0.02583        MiB: 1024.00000 Copy: 39639.221 MiB/s
1       Method: DUMB    Elapsed: 0.02734        MiB: 1024.00000 Copy: 37459.760 MiB/s
2       Method: DUMB    Elapsed: 0.02280        MiB: 1024.00000 Copy: 44920.161 MiB/s
3       Method: DUMB    Elapsed: 0.02339        MiB: 1024.00000 Copy: 43777.521 MiB/s
4       Method: DUMB    Elapsed: 0.02292        MiB: 1024.00000 Copy: 44677.138 MiB/s
5       Method: DUMB    Elapsed: 0.02358        MiB: 1024.00000 Copy: 43432.158 MiB/s
6       Method: DUMB    Elapsed: 0.02781        MiB: 1024.00000 Copy: 36823.936 MiB/s
7       Method: DUMB    Elapsed: 0.02419        MiB: 1024.00000 Copy: 42338.543 MiB/s
8       Method: DUMB    Elapsed: 0.02783        MiB: 1024.00000 Copy: 36788.216 MiB/s
9       Method: DUMB    Elapsed: 0.02318        MiB: 1024.00000 Copy: 44179.826 MiB/s
AVG     Method: DUMB    Elapsed: 0.02489        MiB: 1024.00000 Copy: 41147.633 MiB/s
0       Method: MCBLOCK Elapsed: 0.03166        MiB: 1024.00000 Copy: 32347.738 MiB/s
1       Method: MCBLOCK Elapsed: 0.02727        MiB: 1024.00000 Copy: 37551.799 MiB/s
2       Method: MCBLOCK Elapsed: 0.01992        MiB: 1024.00000 Copy: 51397.882 MiB/s
3       Method: MCBLOCK Elapsed: 0.02358        MiB: 1024.00000 Copy: 43428.474 MiB/s
4       Method: MCBLOCK Elapsed: 0.03195        MiB: 1024.00000 Copy: 32045.063 MiB/s
5       Method: MCBLOCK Elapsed: 0.02215        MiB: 1024.00000 Copy: 46221.901 MiB/s
6       Method: MCBLOCK Elapsed: 0.02327        MiB: 1024.00000 Copy: 43997.594 MiB/s
7       Method: MCBLOCK Elapsed: 0.02765        MiB: 1024.00000 Copy: 37039.716 MiB/s
8       Method: MCBLOCK Elapsed: 0.02240        MiB: 1024.00000 Copy: 45718.368 MiB/s
9       Method: MCBLOCK Elapsed: 0.02317        MiB: 1024.00000 Copy: 44204.619 MiB/s
AVG     Method: MCBLOCK Elapsed: 0.02530        MiB: 1024.00000 Copy: 40471.269 MiB/s
```

## Usage

Run the benchmark by specifying the amount of memory to use for the test in MiB.

### Basic Example
To run the benchmark with default settings (all tests, 10 runs, 8 worker threads) on a 512 MiB array:
```sh
./mbw 512
```

### Using Multiple Workers
To run the `memcpy` test (`-t0`) with 8 worker threads on a 1024 MiB array:
```sh
./mbw -t0 -w 8 1024
```

This update allows `mbw` to more effectively test the limits of your system's memory subsystem by leveraging multiple CPU cores.





MBW determines the "copy" memory bandwidth available to userspace programs. Its simplistic approach models that of real applications. It is not tuned to extremes and it is not aware of hardware architecture, just like your average software package.

2006, 2012 Andras.Horvath atnospam gmail.com
2013 j.m.slocum atnospam gmail.com
2022 Willian.Zhang

http://github.com/raas/mbw
https://github.com/Willian-Zhang/mbw

'mbw 1000' to run copy memory test on all methods with 1 GiB memory.
'mbw -h' for help

watch out for swap usage (or turn off swap)
