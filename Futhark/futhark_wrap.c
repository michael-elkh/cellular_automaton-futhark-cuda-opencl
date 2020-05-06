#include <stdint.h>
#include <stdbool.h>

#include "ac_futhark.h"
#include "../backend.h"

static struct futhark_context_config *f_cfg = NULL;
static struct futhark_context *f_ctx = NULL;

static struct futhark_u32_2d *in = NULL;
static struct futhark_u32_2d *out = NULL;
static int32_t f_width = 0;
static int32_t f_height = 0;
static bool f_parity = true;
static uint32_t f_max_value = 0;

void init()
{
    f_cfg = futhark_context_config_new();
    f_ctx = futhark_context_new(f_cfg);
}
void set_args(bool parity, uint32_t *matrix, int32_t width, int32_t length, uint32_t max_value)
{
    f_parity = parity;
    f_width = width;
    f_height = length/width;
    f_max_value = max_value;
    if (in)
    {
        futhark_free_u32_2d(f_ctx, in);
    }
    in = futhark_new_u32_2d(f_ctx, matrix, f_height, f_width);
}
void iterate(uint32_t iteration)
{
    if (out != NULL)
    {
        futhark_free_u32_2d(f_ctx, out);
    }
    futhark_entry_iterate(f_ctx, &out, f_parity, iteration, in, f_max_value);
}
void get_result(uint32_t *matrix)
{
    futhark_context_sync(f_ctx);
    futhark_values_u32_2d(f_ctx, out, matrix);
}
void destroy()
{
    futhark_free_u32_2d(f_ctx, out);
    out = NULL;
    futhark_free_u32_2d(f_ctx, in);
    in = NULL;
    futhark_context_config_free(f_cfg);
    futhark_context_free(f_ctx);
}