/*
 * vim: ai ts=4 sts=4 sw=4 cinoptions=>4 expandtab
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/* how many runs to average by default */
#define DEFAULT_NR_LOOPS 10

/* we have 3 tests at the moment */
#define MAX_TESTS 3

/* default number of workers */
#define DEFAULT_WORKERS 8

/* default block size for test 2, in bytes */
#define DEFAULT_BLOCK_SIZE 262144

/* test types */
#define TEST_MEMCPY 0
#define TEST_DUMB 1
#define TEST_MCBLOCK 2

/* version number */
#define VERSION "1.6"

/*
 * MBW memory bandwidth benchmark
 *
 * 2006, 2012 Andras.Horvath@gmail.com
 * 2013 j.m.slocum@gmail.com
 * (Special thanks to Stephen Pasich)
 *
 * http://github.com/raas/mbw
 *
 * compile with:
 *			gcc -O -o mbw mbw.c -lpthread
 *
 * run with eg.:
 *
 *			./mbw 300
 *
 * or './mbw -h' for help
 *
 * watch out for swap usage (or turn off swap)
 */

void usage()
{
    printf("mbw memory benchmark v%s, https://github.com/raas/mbw\n", VERSION);
    printf("Usage: mbw [options] array_size_in_MiB\n");
    printf("Options:\n");
    printf("	-n: number of runs per test (0 to run forever)\n");
    printf("	-w: number of workers (threads, default: %d)\n", DEFAULT_WORKERS);
    printf("	-a: Don't display average\n");
    printf("	-t%d: memcpy test\n", TEST_MEMCPY);
    printf("	-t%d: dumb (b[i]=a[i] style) test\n", TEST_DUMB);
    printf("	-t%d: memcpy test with fixed block size\n", TEST_MCBLOCK);
    printf("	-b <size>: block size in bytes for -t2 (default: %d)\n", DEFAULT_BLOCK_SIZE);
    printf("	-q: quiet (print statistics only)\n");
    printf("(will then use two arrays, watch out for swapping)\n");
    printf("'Bandwidth' is amount of data copied over the time this operation took.\n");
    printf("\nThe default is to run all tests available.\n");
}

/* ------------------------------------------------------ */

typedef struct {
    unsigned long long asize;
    long *a;
    long *b;
    int type;
    unsigned long long block_size;
} thread_param_t;

void *thread_routine(void *p)
{
    thread_param_t *params = (thread_param_t *)p;
    unsigned long long t;
    unsigned int long_size = sizeof(long);
    unsigned long long array_bytes = params->asize * long_size;

    if (params->type == TEST_MEMCPY) { /* memcpy test */
        memcpy(params->b, params->a, array_bytes);
    } else if (params->type == TEST_MCBLOCK) { /* memcpy block test */
        char* src = (char*)params->a;
        char* dst = (char*)params->b;
        for (t = array_bytes; t >= params->block_size; t -= params->block_size, src += params->block_size) {
            dst = (char *) memcpy(dst, src, params->block_size) + params->block_size;
        }
        if (t) {
            dst = (char *) memcpy(dst, src, t) + t;
        }
    } else if (params->type == TEST_DUMB) { /* dumb test */
        for (t = 0; t < params->asize; t++) {
            params->b[t] = params->a[t];
        }
    }
    return NULL;
}

/* allocate a test array and fill it with data
 * so as to force Linux to _really_ allocate it */
long *make_array(unsigned long long asize)
{
    unsigned long long t;
    unsigned int long_size=sizeof(long);
    long *a;

    a=calloc(asize, long_size);

    if(NULL==a) {
        perror("Error allocating memory");
        exit(1);
    }

    /* make sure both arrays are allocated, fill with pattern */
    for(t=0; t<asize; t++) {
        a[t]=0xaa;
    }
    return a;
}

/* actual benchmark */
/* asize: number of type 'long' elements in test arrays
 * long_size: sizeof(long) cached
 * type: 0=use memcpy, 1=use dumb copy loop (whatever GCC thinks best)
 *
 * return value: elapsed time in seconds
 */
double worker(unsigned long long asize, long *a, long *b, int type, unsigned long long block_size, int num_workers)
{
    struct timeval starttime, endtime;
    double te;
    int i;

    pthread_t *threads = malloc(num_workers * sizeof(pthread_t));
    thread_param_t *params = malloc(num_workers * sizeof(thread_param_t));
    unsigned long long chunk_asize = asize / num_workers;

    if (NULL == threads || NULL == params) {
        perror("Error allocating memory for threads");
        exit(1);
    }

    /* timer starts */
    gettimeofday(&starttime, NULL);

    for (i = 0; i < num_workers; i++) {
        params[i].a = a + i * chunk_asize;
        params[i].b = b + i * chunk_asize;
        params[i].type = type;
        params[i].block_size = block_size;
        /* give last worker the remainder */
        params[i].asize = (i == num_workers - 1) ? (asize - i * chunk_asize) : chunk_asize;
        pthread_create(&threads[i], NULL, thread_routine, &params[i]);
    }

    for (i = 0; i < num_workers; i++) {
        pthread_join(threads[i], NULL);
    }

    /* timer stops */
    gettimeofday(&endtime, NULL);

    free(threads);
    free(params);

    te=((double)(endtime.tv_sec*1000000-starttime.tv_sec*1000000+endtime.tv_usec-starttime.tv_usec))/1000000;

    return te;
}

/* ------------------------------------------------------ */

/* pretty print worker's output in human-readable terms */
/* te: elapsed time in seconds
 * mt: amount of transferred data in MiB
 * type: see 'worker' above
 *
 * return value: -
 */
void printout(double te, double mt, int type)
{
    switch(type) {
        case TEST_MEMCPY:
            printf("Method: MEMCPY\t");
            break;
        case TEST_DUMB:
            printf("Method: DUMB\t");
            break;
        case TEST_MCBLOCK:
            printf("Method: MCBLOCK\t");
            break;
    }
    printf("Elapsed: %.5f\t", te);
    printf("MiB: %.5f\t", mt);
    printf("Copy: %.3f MiB/s\n", mt/te);
    return;
}

/* ------------------------------------------------------ */

int main(int argc, char **argv)
{
    unsigned int long_size=0;
    double te, te_sum; /* time elapsed */
    unsigned long long asize=0; /* array size (elements in array) */
    int i;
    long *a, *b; /* the two arrays to be copied from/to */
    int o; /* getopt options */
    unsigned long testno;

    /* options */

    /* how many runs to average? */
    int nr_loops=DEFAULT_NR_LOOPS;
    /* how many workers to use? */
    int num_workers=DEFAULT_WORKERS;
    /* fixed memcpy block size for -t2 */
    unsigned long long block_size=DEFAULT_BLOCK_SIZE;
    /* show average, -a */
    int showavg=1;
    /* what tests to run (-t x) */
    int tests[MAX_TESTS];
    double mt=0; /* MiBytes transferred == array size in MiB */
    int quiet=0; /* suppress extra messages */

    tests[0]=0;
    tests[1]=0;
    tests[2]=0;

    while((o=getopt(argc, argv, "haqn:t:b:w:")) != EOF) {
        switch(o) {
            case 'h':
                usage();
                exit(1);
                break;
            case 'a': /* suppress printing average */
                showavg=0;
                break;
            case 'n': /* no. loops */
                nr_loops=strtoul(optarg, (char **)NULL, 10);
                break;
            case 't': /* test to run */
                testno=strtoul(optarg, (char **)NULL, 10);
                if(testno>MAX_TESTS-1) {
                    printf("Error: test number must be between 0 and %d\n", MAX_TESTS-1);
                    exit(1);
                }
                tests[testno]=1;
                break;
            case 'b': /* block size in bytes*/
                block_size=strtoull(optarg, (char **)NULL, 10);
                if(0>=block_size) {
                    printf("Error: what block size do you mean?\n");
                    exit(1);
                }
                break;
            case 'w': /* number of workers */
                num_workers=strtoul(optarg, (char **)NULL, 10);
                if(0>=num_workers) {
                    printf("Error: number of workers must be > 0\n");
                    exit(1);
                }
                break;
            case 'q': /* quiet */
                quiet=1;
                break;
            default:
                break;
        }
    }

    /* default is to run all tests if no specific tests were requested */
    if( (tests[0]+tests[1]+tests[2]) == 0) {
        tests[0]=1;
        tests[1]=1;
        tests[2]=1;
    }

    if( nr_loops==0 && ((tests[0]+tests[1]+tests[2]) != 1) ) {
        printf("Error: nr_loops can be zero if only one test selected!\n");
        exit(1);
    }

    if(optind<argc) {
        mt=strtoul(argv[optind++], (char **)NULL, 10);
    } else {
        printf("Error: no array size given!\n");
        exit(1);
    }

    if(0>=mt) {
        printf("Error: array size wrong!\n");
        exit(1);
    }

    /* ------------------------------------------------------ */

    long_size=sizeof(long); /* the size of long on this platform */
    asize=1024*1024/long_size*mt; /* how many longs then in one array? */

    if(asize*long_size < block_size) {
        printf("Error: array size larger than block size (%llu bytes)!\n", block_size);
        exit(1);
    }

    if(!quiet) {
        printf("Long uses %d bytes. ", long_size);
        printf("Allocating 2*%lld elements = %lld bytes of memory.\n", asize, 2*asize*long_size);
        printf("Using %d worker threads.\n", num_workers);
        if(tests[2]) {
            printf("Using %lld bytes as blocks for memcpy block copy test.\n", block_size);
        }
    }

    a=make_array(asize);
    b=make_array(asize);

    /* ------------------------------------------------------ */
    if(!quiet) {
        printf("Getting down to business... Doing %d runs per test.\n", nr_loops);
    }

    /* run all tests requested, the proper number of times */
    for(testno=0; testno<MAX_TESTS; testno++) {
        te_sum=0;
        if(tests[testno]) {
            for (i=0; nr_loops==0 || i<nr_loops; i++) {
                te=worker(asize, a, b, testno, block_size, num_workers);
                te_sum+=te;
                printf("%d\t", i);
                printout(te, mt, testno);
            }
            if(showavg) {
                printf("AVG\t");
                printout(te_sum/nr_loops, mt, testno);
            }
        }
    }

    free(a);
    free(b);
    return 0;
}

