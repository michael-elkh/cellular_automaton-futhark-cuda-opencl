#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

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
void set_start_state(bool parity, uint32_t *screen, uint width, uint height, uint value)
{
	if (!parity)
	{
		srand(time(NULL));
		for (uint i = 0; i < width * height; i++)
		{
			screen[i] = rand() % (value + 1);
		}
	}
	else
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
		uint sqs = 50;
		uint sx = (width - 5*sqs)>>1;
		uint sy = (width- 5*sqs)>>1;
		

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
}

/// Program entry point.
/// @return the application status code (0 if success).
int main(int argc, const char *argv[])
{
	uint32_t iteration, max_value;
	int32_t width, length;
	bool parity;

	if (argc < 5 || argc > 6)
	{
		printf("Usage:\n    %s iteration width height automaton [max_value]\n\
            automaton: parity or cyclic\n\
            max_value: uint, mandatory for cyclic automation\n",
			   argv[0]);
		exit(1);
	}
	else
	{
		iteration = atoi(argv[1]);
		width = atoi(argv[2]);
		length = width * atoi(argv[3]);

		if (!strcmp(argv[4], "parity"))
		{
			parity = true;
			max_value = 1;
		}
		else if (!strcmp(argv[4], "cyclic") && argc == 6)
		{
			parity = false;
			max_value = atoi(argv[5]);
		}
		else
		{
			printf("Automaton not implemented\n");
			exit(1);
		}
	}

	struct gfx_context_t *ctxt = gfx_create("Cellular automaton", width, length / width);
	if (!ctxt)
	{
		fprintf(stderr, "Graphics initialization failed!\n");
		return EXIT_FAILURE;
	}
	uint32_t *matrix = malloc(length * sizeof(uint32_t));

	init();

	gfx_clear(ctxt, COLOR_BLUE);
	set_start_state(parity, matrix, width, length / width, max_value);
	apply_color_map(ctxt->pixels, matrix, length, max_value);
	gfx_present(ctxt);
	while (gfx_keypressed() != SDLK_ESCAPE && iteration--)
	{
		set_args(parity, matrix, width, length, max_value);
		iterate(1);
		get_result(matrix);
		apply_color_map(ctxt->pixels, matrix, length, max_value);
		gfx_present(ctxt);
	}
	destroy();
	free(matrix);
	gfx_destroy(ctxt);
	return EXIT_SUCCESS;
}