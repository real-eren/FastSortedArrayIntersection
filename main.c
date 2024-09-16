#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

#define u32 uint32_t
// Comparison of branchy and branchless impls of 'in-place intersection'.
// inputs are assumed to be arrays of sorted, unique ints.
// You configure the benchmark by modifying an array in the main function
__attribute__((noinline)) size_t intersect(u32 *left, size_t left_len, const u32 *right, size_t right_len) {
    size_t l_i = 0;
    size_t l_write_i = 0;
    size_t r_i = 0;
    while (l_i < left_len && r_i < right_len) {
        u32 l = left[l_i];
        u32 r = right[r_i];
        if (l < r) {
            l_i++;
        } else if (l == r) {
            left[l_write_i] = l;
            l_i++;
            r_i++;
            l_write_i++;
        } else {
            r_i++;
        }
    }
    return l_write_i;
}
__attribute__((noinline)) size_t intersect_branchless(u32 *left, size_t left_len, const u32 *right, size_t right_len) {
    size_t l_i = 0;
    size_t l_write_i = 0;
    size_t r_i = 0;
    while (l_i < left_len && r_i < right_len) {
        u32 l = left[l_i];
        u32 r = right[r_i];
        u32 l_le = l <= r;
        u32 eq = l == r;
        u32 l_ge = l >= r;
        l_i += l_le;
        r_i += l_ge;
        left[l_write_i] = l;
        l_write_i += eq;
    }
    return l_write_i;
}

#define UNROLL 2
__attribute__((noinline)) size_t intersect_branchless_unroll(u32 *left, size_t left_len, const u32 *right,
                                                             size_t right_len) {
    size_t l_i = 0;
    size_t l_write_i = 0;
    size_t r_i = 0;
    while (l_i + UNROLL < left_len && r_i + UNROLL < right_len) {
        for (int i = 0; i < UNROLL; i++) {
            u32 l = left[l_i];
            u32 r = right[r_i];
            u32 l_le = l <= r;
            u32 eq = l == r;
            u32 l_ge = l >= r;
            l_i += l_le;
            r_i += l_ge;
            left[l_write_i] = l;
            l_write_i += eq;
        }
    }
    while (l_i < left_len && r_i < right_len) {
        u32 l = left[l_i];
        u32 r = right[r_i];
        u32 l_le = l <= r;
        u32 eq = l == r;
        u32 l_ge = l >= r;
        l_i += l_le;
        r_i += l_ge;
        left[l_write_i] = l;
        l_write_i += eq;
    }
    return l_write_i;
}
__attribute__((noinline)) size_t intersect_branchless_dual_wield(u32 *left1, size_t left1_len, const u32 *right1,
                                                                 size_t right1_len, u32 *left2, size_t left2_len,
                                                                 const u32 *right2, size_t right2_len) {
    size_t l1_i = 0;
    size_t l1_write_i = 0;
    size_t r1_i = 0;
    size_t l2_i = 0;
    size_t l2_write_i = 0;
    size_t r2_i = 0;
    while (l1_i < left1_len && r1_i < right1_len && l2_i < left2_len && r2_i < right2_len) {
        u32 l1 = left1[l1_i];
        u32 r1 = right1[r1_i];
        u32 l1_le = l1 <= r1;
        u32 eq1 = l1 == r1;
        u32 l1_ge = l1 >= r1;
        l1_i += l1_le;
        r1_i += l1_ge;
        left1[l1_write_i] = l1;
        l1_write_i += eq1;
        u32 l2 = left2[l2_i];
        u32 r2 = right2[r2_i];
        u32 l2_le = l2 <= r2;
        u32 eq2 = l2 == r2;
        u32 l2_ge = l2 >= r2;
        l2_i += l2_le;
        r2_i += l2_ge;
        left2[l2_write_i] = l2;
        l2_write_i += eq2;
    }
    while (l1_i < left1_len && r1_i < right1_len) {
        u32 l = left1[l1_i];
        u32 r = right1[r1_i];
        u32 l_le = l <= r;
        u32 eq = l == r;
        u32 l_ge = l >= r;
        l1_i += l_le;
        r1_i += l_ge;
        left1[l1_write_i] = l;
        l1_write_i += eq;
    }
    while (l2_i < left2_len && r2_i < right2_len) {
        u32 l = left2[l2_i];
        u32 r = right2[r2_i];
        u32 l_le = l <= r;
        u32 eq = l == r;
        u32 l_ge = l >= r;
        l2_i += l_le;
        r2_i += l_ge;
        left2[l2_write_i] = l;
        l2_write_i += eq;
    }

    return l1_write_i + l2_write_i;
}

__attribute__((noinline)) size_t intersect_branchless_tri_wield(u32 *left1, size_t left1_len, const u32 *right1,
                                                                size_t right1_len, u32 *left2, size_t left2_len,
                                                                const u32 *right2, size_t right2_len, u32 *left3,
                                                                size_t left3_len, const u32 *right3,
                                                                size_t right3_len) {
    size_t l1_i = 0;
    size_t l1_write_i = 0;
    size_t r1_i = 0;
    size_t l2_i = 0;
    size_t l2_write_i = 0;
    size_t r2_i = 0;
    size_t l3_i = 0;
    size_t l3_write_i = 0;
    size_t r3_i = 0;
    // these end up being predictable, no need to bother with amortizing the checks via unrolls
    while (l1_i < left1_len && r1_i < right1_len && l2_i < left2_len && r2_i < right2_len) {
        u32 l1 = left1[l1_i];
        u32 r1 = right1[r1_i];
        u32 l1_le = l1 <= r1;
        u32 eq1 = l1 == r1;
        u32 l1_ge = l1 >= r1;
        l1_i += l1_le;
        r1_i += l1_ge;
        left1[l1_write_i] = l1;
        l1_write_i += eq1;
        u32 l2 = left2[l2_i];
        u32 r2 = right2[r2_i];
        u32 l2_le = l2 <= r2;
        u32 eq2 = l2 == r2;
        u32 l2_ge = l2 >= r2;
        l2_i += l2_le;
        r2_i += l2_ge;
        left2[l2_write_i] = l2;
        l2_write_i += eq2;
        u32 l3 = left3[l3_i];
        u32 r3 = right3[r3_i];
        u32 l3_le = l3 <= r3;
        u32 eq3 = l3 == r3;
        u32 l3_ge = l3 >= r3;
        l3_i += l3_le;
        r3_i += l3_ge;
        left3[l3_write_i] = l3;
        l3_write_i += eq3;
    }
    while (l1_i < left1_len && r1_i < right1_len) {
        u32 l = left1[l1_i];
        u32 r = right1[r1_i];
        u32 l_le = l <= r;
        u32 eq = l == r;
        u32 l_ge = l >= r;
        l1_i += l_le;
        r1_i += l_ge;
        left1[l1_write_i] = l;
        l1_write_i += eq;
    }
    while (l2_i < left2_len && r2_i < right2_len) {
        u32 l = left2[l2_i];
        u32 r = right2[r2_i];
        u32 l_le = l <= r;
        u32 eq = l == r;
        u32 l_ge = l >= r;
        l2_i += l_le;
        r2_i += l_ge;
        left2[l2_write_i] = l;
        l2_write_i += eq;
    }
    while (l3_i < left3_len && r3_i < right3_len) {
        u32 l = left3[l3_i];
        u32 r = right3[r3_i];
        u32 l_le = l <= r;
        u32 eq = l == r;
        u32 l_ge = l >= r;
        l3_i += l_le;
        r3_i += l_ge;
        left3[l3_write_i] = l;
        l3_write_i += eq;
    }

    return l1_write_i + l2_write_i + l3_write_i;
}

void shuffle(u32 *arr, size_t n) {
    if (n <= 1)
        return;
    for (size_t i = n - 1; i > 1; i--) {
        size_t j = rand() % i; // biased, but w/e
        u32 tmp = arr[j];
        arr[j] = arr[i];
        arr[i] = tmp;
    }
}

int compare(const void *a, const void *b) {
    u32 va = *(u32 *)a;
    u32 vb = *(u32 *)b;
    if (va == vb)
        return 0;
    return (va < vb) ? -1 : 1;
}
void sort(u32 *arr, size_t len) {
    qsort(arr, len, sizeof(u32), compare);
}

struct Benchmark {
    // min must be less than max
    // count must be less than range
    long left_min;
    long left_max;
    size_t left_count;

    long right_min;
    long right_max;
    size_t right_count;

    size_t iters;

    char *label;
};
void display(struct Benchmark *b) {
    fprintf(stdout, "%s: [%ld,%ld]*%ld, [%ld,%ld]*%ld, %ld iters\n", b->label, b->left_min, b->left_max, b->left_count,
            b->right_min, b->right_max, b->right_count, b->iters);
}

enum Mode {
    Branchy = 0,
    Branchless = 1,
    BranchlessUnroll = 2,
    DualWield = 3,
    TriWield = 4,
};

char *algo_names[] = {"branchy      ", "branchless   ", "b-less unroll",
                      "dual-wield   ", "tri-wield    "};
int main(int argc, char *argv[]) {
    size_t num_algos = sizeof(algo_names) / sizeof(char *);
    if (argc != 2) {
        fprintf(stderr, "expects exactly 1 argument\n");
        for (int i = 0; i < num_algos; i++) {
            fprintf(stderr, "%d: %s\n", i, algo_names[i]);
        }
        return 1;
    }
    size_t algo_arg;
    char b = *argv[1];
    if ('0' <= b && b < ('0' + num_algos)) {
        algo_arg = b - '0';
    } else {
        fprintf(stderr, "need a valid single digit number between 0 and %ld\n", num_algos - 1);
        return 1;
    }

    srand(time(NULL));

    struct Benchmark benches[] = {
        // { .label = "test", .left_min = 0, .left_max = 150, .left_count = 90, .right_min = 0, .right_max = 150,
        // .right_count = 50, .iters = 1 },
        {.label = "same range, more left",
         .left_min = 0,
         .left_max = 1500,
         .left_count = 900,
         .right_min = 0,
         .right_max = 1500,
         .right_count = 500,
         .iters = 10000},
        {.label = "same range, fewer left",
         .left_min = 0,
         .left_max = 1500,
         .left_count = 500,
         .right_min = 0,
         .right_max = 1500,
         .right_count = 900,
         .iters = 10000},
        {.label = "all left < right",
         .left_min = 0,
         .left_max = 1500,
         .left_count = 500,
         .right_min = 1500,
         .right_max = 3500,
         .right_count = 500,
         .iters = 10000},
        {.label = "all left > right",
         .left_min = 1500,
         .left_max = 3500,
         .left_count = 500,
         .right_min = 0,
         .right_max = 1500,
         .right_count = 500,
         .iters = 10000},
        {.label = "dense",
         .left_min = 0,
         .left_max = 1500,
         .left_count = 1400,
         .right_min = 0,
         .right_max = 1500,
         .right_count = 1200,
         .iters = 10000},
        {.label = "sparse",
         .left_min = 0,
         .left_max = 1500,
         .left_count = 90,
         .right_min = 0,
         .right_max = 1500,
         .right_count = 50,
         .iters = 10000},
        {.label = "big",
         .left_min = 0,
         .left_max = 15000,
         .left_count = 9000,
         .right_min = 0,
         .right_max = 15000,
         .right_count = 9000,
         .iters = 1000},
        {.label = "huge",
         .left_min = 0,
         .left_max = 150000,
         .left_count = 90000,
         .right_min = 0,
         .right_max = 150000,
         .right_count = 90000,
         .iters = 100},
        {.label = "yomama",
         .left_min = 0,
         .left_max = 1500000,
         .left_count = 900000,
         .right_min = 0,
         .right_max = 1500000,
         .right_count = 900000,
         .iters = 10},
        {.label = "yomama2",
         .left_min = 0,
         .left_max = 9500000,
         .left_count = 1200000,
         .right_min = 0,
         .right_max = 9500000,
         .right_count = 1500000,
         .iters = 10},
    };
    size_t num_benches = sizeof(benches) / sizeof(struct Benchmark);

    // Yes, we could allocate one big buffer once and re-use it across benches. but it doesn't actually matter here.
    // And i'd rather avoid the footgun where one benchmark has a high number of iters and another has a large count
    fprintf(stdout, "algorithm \tavg time (ms)\tavg thrpt (MB/s)\n");
    for (size_t b_i = 0; b_i < num_benches; b_i++) {
        struct Benchmark b = benches[b_i];

        // When benchmarking branchy algos, you don't want to re-use inputs,
        //  as the branch predictor will learn the pattern of the data, yielding unrealistic results.
        u32 *left_arr = malloc(b.left_count * b.iters * sizeof(u32));
        u32 *right_arr = malloc(b.right_count * b.iters * sizeof(u32));
        if (!left_arr || !right_arr) {
            fprintf(stderr, "malloc failed, try smaller values\n");
            return 1;
        }

        long left_range = b.left_max - b.left_min;
        long right_range = b.right_max - b.right_min;

        // If we used rand() % range + min, it'd be harder to know that the vals are unique.
        // shuffle in here, copy into arr
        // names r hard
        u32 *left_pool = malloc(left_range * sizeof(u32));
        u32 *right_pool = malloc(right_range * sizeof(u32));
        for (size_t i = 0; i < left_range; i++) {
            left_pool[i] = i + b.left_min;
        }
        for (size_t i = 0; i < right_range; i++) {
            right_pool[i] = i + b.right_min;
        }
        // Also, you may not want to interleave the input generation, for reasons.
        // So, we do it all before running the fns to be benched
        for (size_t i = 0; i < b.iters; ++i) {
            shuffle(left_pool, left_range);
            memcpy(left_arr + i * b.left_count, left_pool, b.left_count * sizeof(u32));
            sort(left_arr + i * b.left_count, b.left_count);

            shuffle(right_pool, right_range);
            memcpy(right_arr + i * b.right_count, right_pool, b.right_count * sizeof(u32));
            sort(right_arr + i * b.right_count, b.right_count);
        }

        // finally, bench
        display(&b); // creating the input is much slower
                     // so we print this here so that the visual delay matches the actual execution time

        // MB
        size_t input_size = b.iters * (b.left_count + b.right_count) * sizeof(u32) * 0.000001;
        // not a serious checksum, just to prevent the loops from getting optimized out
        size_t checksum = 0;

        clock_t start = clock();
        switch (algo_arg) {
        case Branchy:
            for (size_t i = 0; i < b.iters; i++) {
                checksum +=
                    intersect(left_arr + i * b.left_count, b.left_count, right_arr + i * b.right_count, b.right_count);
            }
            break;

        case Branchless:
            for (size_t i = 0; i < b.iters; i++) {
                checksum += intersect_branchless(left_arr + i * b.left_count, b.left_count,
                                                 right_arr + i * b.right_count, b.right_count);
            }
            break;
        case BranchlessUnroll:
            for (size_t i = 0; i < b.iters; i++) {
                checksum += intersect_branchless_unroll(left_arr + i * b.left_count, b.left_count,
                                                        right_arr + i * b.right_count, b.right_count);
            }
            break;
        case DualWield: {
            size_t i = 0;
            for (; i + 1 < b.iters; i += 2) {
                checksum += intersect_branchless_dual_wield(left_arr + i * b.left_count, b.left_count,
                                                            right_arr + i * b.right_count, b.right_count,
                                                            left_arr + (i + 1) * b.left_count, b.left_count,
                                                            right_arr + (i + 1) * b.right_count, b.right_count);
            }
            for (; i < b.iters; i++) {
                checksum += intersect_branchless(left_arr + i * b.left_count, b.left_count,
                                                 right_arr + i * b.right_count, b.right_count);
            }
            break;
        }
        case TriWield: {
            size_t i = 0;
            for (; i + 2 < b.iters; i += 3) {
                checksum += intersect_branchless_tri_wield(
                    left_arr + i * b.left_count, b.left_count, right_arr + i * b.right_count, b.right_count,
                    left_arr + (i + 1) * b.left_count, b.left_count, right_arr + (i + 1) * b.right_count, b.right_count,
                    left_arr + (i + 2) * b.left_count, b.left_count, right_arr + (i + 2) * b.right_count,
                    b.right_count);
            }
            for (; i < b.iters; i++) {
                checksum += intersect_branchless(left_arr + i * b.left_count, b.left_count,
                                                 right_arr + i * b.right_count, b.right_count);
            }
            break;
        }
        }
        clock_t end = clock();
        float seconds = (float)(end - start) / CLOCKS_PER_SEC;
        float thrpt = input_size / seconds;

        // in a more serious context, we'd collect the times for individual runs, and analyze the stddev and other
        // stats. Also, randomize the order of the tests

        fprintf(stdout, "%s\t%f\t%f\n", algo_names[algo_arg], seconds / b.iters * 1000., thrpt);
        fprintf(stdout, "csum:%ld\n\n", checksum);
        fflush(stdout);
        free(left_arr);
        free(left_pool);
        free(right_arr);
        free(right_pool);
    }
}
