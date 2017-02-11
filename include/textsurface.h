#ifndef __TEXTSURFACE_H__
#define __TEXTSURFACE_H__

#include "vec2d.h"
#include "geo2d.h"
#include "texture.h"

class TextSurface
{
	private:
		SDL_Surface* load_surface;
		TTF_Font* font;
		Texture* text_image;
		char* data;
	
	public:
		TextSurface( void );
		TextSurface( char* data, TTF_Font* font );

		void render( Vec2d where );
		void render( Rect where );
};

#endif
