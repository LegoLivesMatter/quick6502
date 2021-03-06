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

#include <stdio.h>
#include "framebuffer.h"

#ifdef ENABLE_DEBUG
#define DEBUG(MSG) printf( "DEBUG: %s\n", MSG )
#else
#define DEBUG(MSG) 
#endif

#ifdef ENABLE_FRAMEBUFFER
void clear_framebuffer( struct framebuffer *framebuffer ) {
	uint8_t *pixels;
	int pitch;
	if( SDL_LockTexture( framebuffer->texture, NULL, (void **) &pixels, &pitch ) != 0 ) {
		fprintf( stderr, "Failed to lock texture: %s\n", SDL_GetError() );
	}

	int i;
	for( i = 0; i < 32 * 32; i++ ) {
		pixels[i] = 0;
	}

	SDL_UnlockTexture( framebuffer->texture );
	pixels = NULL;
	if( SDL_RenderCopy( framebuffer->renderer, framebuffer->texture, NULL, NULL ) != 0 ) {
		fprintf( stderr, "Failed to copy texture: %s\n", SDL_GetError() );
	}
	SDL_RenderPresent( framebuffer->renderer );
}

void update_framebuffer( struct framebuffer *framebuffer, const uint8_t *fbmem, uint8_t *prev_fbmem, int fb_size ) {
	if( memcmp( fbmem, prev_fbmem, fb_size ) != 0 ) {
		uint8_t *pixels;
		int pitch, x, y;

		if( SDL_LockTexture( framebuffer->texture, NULL, (void **) &pixels, &pitch ) ) {
			fprintf( stderr, "Failed to lock texture: %s\n", SDL_GetError() );
			return;
		}

		for( y = 0; y < 32; y++ ) {
			for( x = 0; x < 32; x++ ) {
				pixels[x + (y * 32)] = fbmem[x + (y * 32)];
			}
		}

		SDL_UnlockTexture( framebuffer->texture );
		SDL_RenderCopy( framebuffer->renderer, framebuffer->texture, NULL, NULL );
		SDL_RenderPresent( framebuffer->renderer );

		memcpy( prev_fbmem, fbmem, fb_size );
	}
}

int init_framebuffer( struct framebuffer **framebuffer) {
	*framebuffer = malloc( sizeof( struct framebuffer ) );
	struct framebuffer new_fb;

	new_fb.window = SDL_CreateWindow( "Quick 6502", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 640, 0 );

	if ( new_fb.window == NULL ) {
		fprintf( stderr, "Failed to initialize framebuffer: %s\n", SDL_GetError() );
		return 1;
	}

	new_fb.format = SDL_AllocFormat( SDL_GetWindowPixelFormat( new_fb.window ) );

	new_fb.renderer = SDL_CreateRenderer( new_fb.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

	if ( new_fb.renderer == NULL ) {
		fprintf( stderr, "Failed to create hardware renderer, falling back to software: %s\n", SDL_GetError() );

		new_fb.renderer = SDL_CreateRenderer( new_fb.window, -1, SDL_RENDERER_SOFTWARE );

		if ( new_fb.renderer == NULL ) {
			fprintf( stderr, "Failed to create software renderer: %s\n", SDL_GetError() );
			return 1;
		}
	}

	new_fb.texture = SDL_CreateTexture( new_fb.renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, 32, 32 );

	if ( new_fb.texture == NULL ) {
		fprintf( stderr, "Failed to create rendering texture: %s\n", SDL_GetError() );
		return 1;
	}

	clear_framebuffer( &new_fb );

	**framebuffer = new_fb;
	return 0;
}

void destroy_framebuffer( struct framebuffer *framebuffer ) {
	SDL_DestroyTexture( framebuffer->texture );
	SDL_DestroyRenderer( framebuffer->renderer );
	SDL_DestroyWindow( framebuffer->window );
	SDL_FreeFormat( framebuffer->format );
	free( framebuffer );
}

void shutdown_framebuffer()
{
	SDL_Quit();
}
#else /* #ifdef ENABLE_FRAMEBUFFER */
void update_framebuffer( struct framebuffer *framebuffer, const uint8_t *fbmem, uint8_t *prev_fbmem, int fb_size )
{
}

void destroy_framebuffer( struct framebuffer *framebuffer )
{
	free( framebuffer );
}

void shutdown_framebuffer()
{
}

int init_framebuffer( struct framebuffer **framebuffer )
{
	return 0;
}
#endif /* #ifdef ENABLE_FRAMEBUFFER */
