#ifndef __BACKEND__
#define __BACKEND__
#include <stdint.h>
#include <stdbool.h>

extern void init();
extern void set_args(bool parity, uint32_t *matrix, int32_t width, int32_t length, uint32_t max_value);
extern void iterate(uint32_t iteration);
extern void get_result(uint32_t *matrix);
extern void destroy();

#endif