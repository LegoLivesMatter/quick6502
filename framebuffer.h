#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#ifdef SDL_h_
#error "Don't include <SDL2/SDL.h>, "framebuffer.h" does that for you."
#endif

#ifdef __TINYC__
#define SDL_DISABLE_IMMINTRIN_H
#endif

#ifdef _WIN32
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>

static const SDL_Color vga_palette[16] = {
	[0x0] = { .r = 0x0, .g = 0x0, .b = 0x0, .a = 0xFF },
	[0x1] = { .r = 0x0, .g = 0x0, .b = 0xAA, .a = 0xFF },
	[0x2] = { .r = 0x0, .g = 0xAA, .b = 0x0, .a = 0xFF },
	[0x3] = { .r = 0x0, .g = 0xAA, .b = 0xAA, .a = 0xFF },
	[0x4] = { .r = 0xAA, .g = 0x0, .b = 0x0, .a = 0xFF },
	[0x5] = { .r = 0xAA, .g = 0x0, .b = 0xAA, .a = 0xFF },
	[0x6] = { .r = 0xAA, .g = 0x55, .b = 0x0, .a = 0xFF },
	[0x7] = { .r = 0xAA, .g = 0xAA, .b = 0xAA, .a = 0xFF },
	[0x8] = { .r = 0x55, .g = 0x55, .b = 0x55, .a = 0xFF },
	[0x9] = { .r = 0x55, .g = 0x55, .b = 0xFF, .a = 0xFF },
	[0xA] = { .r = 0x55, .g = 0xFF, .b = 0x55, .a = 0xFF },
	[0xB] = { .r = 0x55, .g = 0xFF, .b = 0xFF, .a = 0xFF },
	[0xC] = { .r = 0xFF, .g = 0x55, .b = 0x55, .a = 0xFF },
	[0xD] = { .r = 0xFF, .g = 0x55, .b = 0xFF, .a = 0xFF },
	[0xE] = { .r = 0xFF, .g = 0xFF, .b = 0x55, .a = 0xFF },
	[0xF] = { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF }
};

struct framebuffer {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_PixelFormat *format;
};

extern int init_framebuffer( struct framebuffer *framebuffer );
extern void draw_pixel( struct framebuffer *framebuffer, int x, int y, int color );
extern void update_framebuffer( struct framebuffer *framebuffer, uint8_t *fbmem );
extern void destroy_framebuffer( struct framebuffer *framebuffer );

#define MAP_COLOR(FORMAT, COLOR) SDL_MapRGBA( FORMAT, vga_palette[COLOR].r, vga_palette[COLOR].g, vga_palette[COLOR].b, vga_palette[COLOR].a )

#endif /* #ifndef FRAMEBUFFER_H */

