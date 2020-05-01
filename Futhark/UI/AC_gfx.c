#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gfx.h"
#include <unistd.h>

void draw_square(uint32_t *screen, uint size_x, uint x, uint y, uint width, uint value)
{
	for (uint i = y; i < (y + width); i++)
	{
		for (uint j = x; j < (x + width); j++)
		{
			screen[i * size_x + j] = value;
		}
	}
}
//Draw a M in a given position, and a border on a given matrix.
void set_start_state(uint32_t *screen, uint width, uint height, uint value)
{
	uint border = 1;
	for (uint i = 0; i < height; i++)
	{
		for (uint j = 0; j < width; j++)
		{
			if (i < border || i >= (height - border) || j < border || j >= (width - border))
			{
				screen[i * width + j] = value;
			}
		}
	}
	uint sx = 125;
	uint sy = 125;
	uint sqs = 50;

	draw_square(screen, width, sx, sy, sqs, value);
	draw_square(screen, width, sx, sy + (1 * sqs), sqs, value);
	draw_square(screen, width, sx, sy + (2 * sqs), sqs, value);
	draw_square(screen, width, sx, sy + (3 * sqs), sqs, value);
	draw_square(screen, width, sx, sy + (4 * sqs), sqs, value);

	draw_square(screen, width, sx + (1 * sqs), sy + (1 * sqs), sqs, value);
	draw_square(screen, width, sx + (2 * sqs), sy + (2 * sqs), sqs, value);
	draw_square(screen, width, sx + (3 * sqs), sy + (1 * sqs), sqs, value);

	draw_square(screen, width, sx + (4 * sqs), sy, sqs, value);
	draw_square(screen, width, sx + (4 * sqs), sy + (1 * sqs), sqs, value);
	draw_square(screen, width, sx + (4 * sqs), sy + (2 * sqs), sqs, value);
	draw_square(screen, width, sx + (4 * sqs), sy + (3 * sqs), sqs, value);
	draw_square(screen, width, sx + (4 * sqs), sy + (4 * sqs), sqs, value);
}

extern void render(uint32_t *pixels, uint32_t width, uint32_t height);
extern void init_futhark();
extern void destroy_futhark();

void cast_array_bool(uint32_t *in, bool *out, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		out[i] = in[i] == COLOR_RED;
	}
}
void cast_array_uint32(bool *in, uint32_t *out, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		out[i] = in[i] ? COLOR_RED : COLOR_BLUE;
	}
}

/// Program entry point.
/// @return the application status code (0 if success).
int main()
{
	int width = 1000, height = 1000;
	struct gfx_context_t *ctxt = gfx_create("Example", width, height);
	if (!ctxt)
	{
		fprintf(stderr, "Graphics initialization failed!\n");
		return EXIT_FAILURE;
	}
	init_futhark();

	gfx_clear(ctxt, COLOR_BLUE);
	set_start_state(ctxt->pixels, width, height, COLOR_RED);
	while (gfx_keypressed() != SDLK_ESCAPE)
	{
		render(ctxt->pixels, width, height);
		gfx_present(ctxt);
		usleep(20000);
	}

	destroy_futhark();
	gfx_destroy(ctxt);
	return EXIT_SUCCESS;
}
