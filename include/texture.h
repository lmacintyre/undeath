#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "SDL2/SDL_opengl.h"

#include "geo2d.h"

class Texture
{
	private:
		GLuint texture_id;
		GLuint texture_width;
		GLuint texture_height;
	
	public:
		Texture( void );
		~Texture( void );

		bool load( char* path );
		bool load( GLuint* pixels, GLuint width, GLuint height );
		void render( Rect clip, Rect where, bool flip=false );

		void free_texture( void );

		void bind( void );
		
		GLuint get_id( void );
		GLuint get_width( void );
		GLuint get_height( void );
};

#endif
