#include <stdint.h>
#include <stdbool.h>
#include "ac_fut.h"

static struct futhark_context_config *f_cfg;
static struct futhark_context *f_ctx;

static struct futhark_u32_1d *in;
static struct futhark_u32_1d *out;
static uint32_t *data;
static uint32_t f_width;

void init_futhark()
{
    f_cfg = futhark_context_config_new();
    f_ctx = futhark_context_new(f_cfg);
}

void prepare_args(uint32_t *pixels, uint32_t width, uint32_t height)
{
    data = pixels;
    f_width = width;
    in = futhark_new_u32_1d(f_ctx, data, width*height);
}

void execute()
{
    if (out != NULL) { futhark_free_u32_1d(f_ctx, out); }
    futhark_entry_ac_parity(f_ctx, &out, in, f_width);
}

void get_result()
{
    futhark_context_sync(f_ctx);
    futhark_values_u32_1d(f_ctx, out, data);
}

void clean_args()
{
    futhark_free_u32_1d(f_ctx, in);
    in = NULL;
    futhark_free_u32_1d(f_ctx, out);
    out = NULL;
}

void render(uint32_t *pixels, uint32_t width, uint32_t height)
{
    prepare_args(pixels, width, height);
    execute();
    get_result(pixels);
    clean_args();
}

void destroy_futhark()
{
    futhark_context_config_free(f_cfg);
    futhark_context_free(f_ctx);
}