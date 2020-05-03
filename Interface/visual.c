#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "gfx/gfx.h"
#include "../backend.h"

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

/// Program entry point.
/// @return the application status code (0 if success).
int main()
{
	int32_t width = 1280, height = 720;
    int32_t length = width * height;
    uint32_t max_value = 1;
	struct gfx_context_t *ctxt = gfx_create("Cellular automaton", width, height);
	if (!ctxt)
	{
		fprintf(stderr, "Graphics initialization failed!\n");
		return EXIT_FAILURE;
	}
    uint32_t *matrix = malloc(length * sizeof(uint32_t)); 

	init();

	gfx_clear(ctxt, COLOR_BLUE);
	set_start_state(matrix, width, height, 1);
	apply_color_map(ctxt->pixels, matrix, length, max_value);
	gfx_present(ctxt);
	while (gfx_keypressed() != SDLK_ESCAPE)
	{
		set_args(true, matrix, width, length, max_value);
        iterate(1);
        get_result(matrix);
        apply_color_map(ctxt->pixels, matrix, length, max_value);
		gfx_present(ctxt);
		usleep(20000);
	}
	destroy();
    free(matrix);
	gfx_destroy(ctxt);
	return EXIT_SUCCESS;
}