#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "SDL2/SDL_ttf.h"

#include "display.h"

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
		~TextSurface( void );
		TextSurface( char* data, TTF_Font* font );

		void render( Vec2d where );
		void render( Rect where );
};

TextSurface::TextSurface( void )
{
	load_surface = NULL;
	font = NULL;
	text_image = NULL;

	data = NULL;
}

TextSurface::~TextSurface( void )
{

}

TextSurface::TextSurface( char* data, TTF_Font* font )
{

	this->data = data;
	this->font = font;

	SDL_Color color = { 0, 0, 0 };
	load_surface = TTF_RenderText_Blended( font, data, color );

	text_image = new Texture();
	text_image->load( (GLuint*) load_surface->pixels, load_surface->w, load_surface->h );
}

void TextSurface::render( Vec2d where )
{
	float width = (float) (text_image->get_width()) / WINDOW_WIDTH;
	float height = (float) (text_image->get_height()) / WINDOW_HEIGHT;

	printf( "w:%f\th:%f\n", width, height );

	text_image->render( Rect( Vec2d( 0.f, 0.f ), 1.f, 1.f ), Rect( where, width, height ) );
}

void TextSurface::render( Rect where )
{
	text_image->render( Rect( Vec2d( 0.f, 0.f ), 1.f, 1.f ), where );
}
