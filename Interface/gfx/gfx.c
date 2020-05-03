/// @file gfx.c
/// @author Florent Gluck
/// @date November 6, 2016
/// Helper routines to render pixels in fullscreen graphic mode.
/// Uses the SDL2 library.

#include "gfx.h"

/// Create a fullscreen graphic window.
/// @param title Title of the window.
/// @param width Width of the window in pixels.
/// @param height Height of the window in pixels.
/// @return a pointer to the graphic context or NULL if it failed.
struct gfx_context_t *gfx_create(char *title, uint width, uint height)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		goto error;
	SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
										  SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
											 SDL_TEXTUREACCESS_STREAMING, width, height);
	uint32_t *pixels = malloc(width * height * sizeof(uint32_t));
	struct gfx_context_t *ctxt = malloc(sizeof(struct gfx_context_t));

	if (!window || !renderer || !texture || !pixels || !ctxt)
		goto error;

	ctxt->renderer = renderer;
	ctxt->texture = texture;
	ctxt->window = window;
	ctxt->width = width;
	ctxt->height = height;
	ctxt->pixels = pixels;

	SDL_ShowCursor(SDL_DISABLE);
	gfx_clear(ctxt, COLOR_BLACK);
	return ctxt;

error:
	return NULL;
}

/// Draw a pixel in the specified graphic context.
/// @param ctxt Graphic context where the pixel is to be drawn.
/// @param x X coordinate of the pixel.
/// @param y Y coordinate of the pixel.
/// @param color Color of the pixel.
void gfx_putpixel(struct gfx_context_t *ctxt, int x, int y, uint32_t color)
{
	if (x < ctxt->width && y < ctxt->height)
		ctxt->pixels[ctxt->width * y + x] = color;
}

/// Clear the specified graphic context.
/// @param ctxt Graphic context to clear.
/// @param color Color to use.
void gfx_clear(struct gfx_context_t *ctxt, uint32_t color)
{
	int n = ctxt->width * ctxt->height;
	while (n)
		ctxt->pixels[--n] = color;
}

/// Display the graphic context.
/// @param ctxt Graphic context to clear.
void gfx_present(struct gfx_context_t *ctxt)
{
	SDL_UpdateTexture(ctxt->texture, NULL, ctxt->pixels, ctxt->width * sizeof(uint32_t));
	SDL_RenderCopy(ctxt->renderer, ctxt->texture, NULL, NULL);
	SDL_RenderPresent(ctxt->renderer);
}

/// Destroy a graphic window.
/// @param ctxt Graphic context of the window to close.
void gfx_destroy(struct gfx_context_t *ctxt)
{
	SDL_ShowCursor(SDL_ENABLE);
	SDL_DestroyTexture(ctxt->texture);
	SDL_DestroyRenderer(ctxt->renderer);
	SDL_DestroyWindow(ctxt->window);
	free(ctxt->pixels);
	ctxt->texture = NULL;
	ctxt->renderer = NULL;
	ctxt->window = NULL;
	ctxt->pixels = NULL;
	SDL_Quit();
	free(ctxt);
}

/// If a key was pressed, returns its key code (non blocking call).
/// List of key codes: https://wiki.libsdl.org/SDL_Keycode
/// @return the key that was pressed or 0 if none was pressed.
SDL_Keycode gfx_keypressed()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN)
			return event.key.keysym.sym;
	}
	return 0;
}

typedef struct color
{
	float r;
	float g;
	float b;
} color_t;

void groundColorMix(color_t *color, float x, float min, float max)
{
	float posSlope = (max - min) / 60;
	float negSlope = (min - max) / 60;
	if (x < 60)
	{
		color->r = max;
		color->g = posSlope * x + min;
		color->b = min;
		return;
	}
	else if (x < 120)
	{
		color->r = negSlope * x + 2 * max + min;
		color->g = max;
		color->b = min;
		return;
	}
	else if (x < 180)
	{
		color->r = min;
		color->g = max;
		color->b = posSlope * x - 2 * max + min;
		return;
	}
	else if (x < 240)
	{
		color->r = min;
		color->g = negSlope * x + 4 * max + min;
		color->b = max;
		return;
	}
	else if (x < 300)
	{
		color->r = posSlope * x - 4 * max + min;
		color->g = min;
		color->b = max;
		return;
	}
	else
	{
		color->r = max;
		color->g = min;
		color->b = negSlope * x + 6 * max;
		return;
	}
}

void apply_color_map(uint32_t *pixels, uint32_t *matrix, uint32_t length, uint32_t max_value)
{
	float gradient;
	color_t col;
		for (uint32_t i = 0; i < length; i++)
		{
			gradient = (((float)matrix[i]) / ((float)max_value)) * 250.0;
			groundColorMix(&col, 250.0 - gradient, 0.0, 250.0);
			pixels[i] = MAKE_COLOR(col.r, col.g, col.b);
		}
}