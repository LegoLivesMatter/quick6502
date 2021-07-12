#include <stdio.h>
#include "framebuffer.h"

#ifdef ENABLE_DEBUG
#define DEBUG(MSG) printf( "DEBUG: %s\n", MSG )
#endif

void clear_framebuffer( struct framebuffer *framebuffer ) {
	Uint32 *pixels;
	int pitch;
	if( SDL_LockTexture( framebuffer->texture, NULL, (void **) &pixels, &pitch ) != 0 ) {
		fprintf( stderr, "Failed to lock texture: %s\n", SDL_GetError() );
	}

	int i;
	for( i = 0; i < 160 * 160; i++ ) {
		pixels[i] = MAP_COLOR( framebuffer->format, 0x0 );
	}

	SDL_UnlockTexture( framebuffer->texture );
	pixels = NULL;
	if( SDL_RenderCopy( framebuffer->renderer, framebuffer->texture, NULL, NULL ) != 0 ) {
		fprintf( stderr, "Failed to copy texture: %s\n", SDL_GetError() );
	}
	SDL_RenderPresent( framebuffer->renderer );
}

void draw_pixel( struct framebuffer *framebuffer, int x, int y, int color ) {
	Uint32 *pixels;
	int pitch;

	SDL_Rect pixel;
	pixel.x = x;
	pixel.y = y;
	pixel.w = 1;
	pixel.h = 1;

	if( SDL_LockTexture( framebuffer->texture, &pixel, (void **) &pixels, &pitch ) != 0 ) {
		fprintf( stderr, "Failed to lock texture: %s\n", SDL_GetError() );
		return;
	}

	pixels[0] = MAP_COLOR( framebuffer->format, color );

	SDL_UnlockTexture( framebuffer->texture );
	pixels = NULL;
}

void update_framebuffer( struct framebuffer *framebuffer, uint8_t *fbmem ) {
	int x;
	for( x = 0; x < 160 * 160; x++ ) {
		draw_pixel( framebuffer, x % 160, x / 160, fbmem[x] );
	}
	SDL_RenderCopy( framebuffer->renderer, framebuffer->texture, NULL, NULL );
	SDL_RenderPresent( framebuffer->renderer );
}

int init_framebuffer( struct framebuffer *framebuffer) {
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

	new_fb.texture = SDL_CreateTexture( new_fb.renderer, SDL_GetWindowPixelFormat( new_fb.window ), SDL_TEXTUREACCESS_STREAMING, 160, 160 );

	if ( new_fb.texture == NULL ) {
		fprintf( stderr, "Failed to create rendering texture: %s\n", SDL_GetError() );
		return 1;
	}

	clear_framebuffer( &new_fb );

	*framebuffer = new_fb;
	return 0;
}

void destroy_framebuffer( struct framebuffer *framebuffer ) {
	SDL_DestroyTexture( framebuffer->texture );
	SDL_DestroyRenderer( framebuffer->renderer );
	SDL_DestroyWindow( framebuffer->window );
	SDL_FreeFormat( framebuffer->format );
	free( framebuffer );
}
