/* Copyright (C) 2021 Duje Mihanović
 *
 * This file is part of Quick6502.
 *
 * Quick6502 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * Quick6502 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Quick6502. If not, see <https://www.gnu.org/licenses/>.
*/

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

