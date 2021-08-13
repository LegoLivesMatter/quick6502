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

struct framebuffer {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_PixelFormat *format;
};

extern int init_framebuffer( struct framebuffer **framebuffer );
extern void update_framebuffer( struct framebuffer *framebuffer, const uint8_t *fbmem, uint8_t *prev_fbmem, int fb_size );
extern void destroy_framebuffer( struct framebuffer *framebuffer );
extern void shutdown_framebuffer();

#endif /* #ifndef FRAMEBUFFER_H */

