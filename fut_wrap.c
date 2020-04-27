#include <stdint.h>
#include <stdbool.h>
#include "ac.h"

static struct futhark_context_config *f_cfg;
static struct futhark_context *f_ctx;

void init_futhark()
{
    f_cfg = futhark_context_config_new();
    f_ctx = futhark_context_new(f_cfg);
}

void render(bool *pixels, uint32_t width, uint32_t height)
{
    struct futhark_bool_2d* in;
    struct futhark_bool_2d* out;

    in = futhark_new_bool_2d(f_ctx, pixels, height, width);
    futhark_entry_ac_parity(f_ctx, &out, in);
    futhark_values_bool_2d(f_ctx, out, pixels);
    
    futhark_free_bool_2d(f_ctx, in);
    futhark_free_bool_2d(f_ctx, out);
}

void destroy_futhark()
{
    futhark_context_config_free(f_cfg);
    futhark_context_free(f_ctx);
}