#include <stdint.h>
#include <stdbool.h>
#include "ac.h"
#include <time.h>
#include <stdio.h>

static struct futhark_context_config *f_cfg;
static struct futhark_context *f_ctx;

void init_futhark()
{
    f_cfg = futhark_context_config_new();
    f_ctx = futhark_context_new(f_cfg);
}

void render(bool *pixels, uint32_t width, uint32_t height)
{
    struct futhark_bool_2d *in;
    struct futhark_bool_2d *out;
    
    
    clock_t start,stop;
    double res = 0.0;
    #pragma unroll 1000
    for (int i = 0; i < 1000; i++)
    {
        in = futhark_new_bool_2d(f_ctx, pixels, height, width);
        start = clock();
        futhark_entry_ac_parity(f_ctx, &out, in);
        stop = clock();
        futhark_values_bool_2d(f_ctx, out, pixels);
        res += (double)(stop-start) / (double) CLOCKS_PER_SEC;
    }
    res/=1000;
    printf("Res: %lf\n", res);
    exit(0);
    
    futhark_free_bool_2d(f_ctx, in);
    futhark_free_bool_2d(f_ctx, out);
}

void destroy_futhark()
{
    futhark_context_config_free(f_cfg);
    futhark_context_free(f_ctx);
}