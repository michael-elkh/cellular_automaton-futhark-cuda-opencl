#include <stdint.h>
#include <stdbool.h>
#include "ac.h"

static struct futhark_context_config *f_cfg;
static struct futhark_context *f_ctx;

static struct futhark_bool_2d *in;
static struct futhark_bool_2d *out;
static bool *data;

void init_futhark()
{
    f_cfg = futhark_context_config_new();
    f_ctx = futhark_context_new(f_cfg);
}

void prepare_args(bool *pixels, uint32_t width, uint32_t height)
{
    data = pixels;
    in = futhark_new_bool_2d(f_ctx, data, height, width);
}

void execute()
{
    futhark_entry_ac_parity(f_ctx, &out, in);
}

void get_result()
{
    futhark_context_sync(f_ctx);
    futhark_values_bool_2d(f_ctx, out, data);
}

void clean_args()
{
    futhark_free_bool_2d(f_ctx, in);
    futhark_free_bool_2d(f_ctx, out);
}

void render(bool *pixels, uint32_t width, uint32_t height)
{
    prepare_args(pixels, width, height);
    execute();
    get_result();
    clean_args();
}

void destroy_futhark()
{
    futhark_context_config_free(f_cfg);
    futhark_context_free(f_ctx);
}