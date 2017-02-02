#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
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

Texture::Texture( void )
{
	texture_id = 0;

	texture_width = 0;
	texture_height = 0;
}

Texture::~Texture( void )
{
	free_texture();
}

bool Texture::load( char* path )
{
	SDL_Surface* load_surface = IMG_Load( path );
	load( (GLuint*) load_surface->pixels, load_surface->w, load_surface->h );
}

bool Texture::load( GLuint* pixels, GLuint width, GLuint height )
{
	free_texture();

	texture_width = width;
	texture_height = height;

	glGenTextures( 1, &texture_id );
	glBindTexture( GL_TEXTURE_2D, texture_id );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	glBindTexture( GL_TEXTURE_2D, 0 );

	GLenum error = glGetError();
	if( error != GL_NO_ERROR )
	{
		return false;
	}

	return true;
}

void Texture::render( Rect clip, Rect where, bool flip )
{
	glEnable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, texture_id );

	glBegin( GL_QUADS );
		
		if( flip ) 	glTexCoord2f( clip.vertices[2].get_a(), clip.vertices[2].get_b() );
		else 		glTexCoord2f( clip.vertices[3].get_a(), clip.vertices[3].get_b() );
		glVertex2f( where.vertices[0].get_a(), where.vertices[0].get_b() );

		if( flip ) 	glTexCoord2f( clip.vertices[3].get_a(), clip.vertices[3].get_b() );
		else		glTexCoord2f( clip.vertices[2].get_a(), clip.vertices[2].get_b() );
		glVertex2f( where.vertices[1].get_a(), where.vertices[1].get_b() );

		if( flip )	glTexCoord2f( clip.vertices[0].get_a(), clip.vertices[0].get_b() );
		else		glTexCoord2f( clip.vertices[1].get_a(), clip.vertices[1].get_b() );
		glVertex2f( where.vertices[2].get_a(), where.vertices[2].get_b() );

		if( flip )	glTexCoord2f( clip.vertices[1].get_a(), clip.vertices[1].get_b() );
		else		glTexCoord2f( clip.vertices[0].get_a(), clip.vertices[0].get_b() );
		glVertex2f( where.vertices[3].get_a(), where.vertices[3].get_b() );
	glEnd();

	glDisable( GL_TEXTURE_2D );
}

void Texture::free_texture( void )
{
	if( texture_id != 0 )
	{
		glDeleteTextures( 1, &texture_id );
		texture_id = 0;
	}

	texture_width = 0;
	texture_height = 0;
}

void Texture::bind( void )
{
	glBindTexture( GL_TEXTURE_2D, texture_id );
}

GLuint Texture::get_id( void ) { return texture_id; }

GLuint Texture::get_width( void ) { return texture_width; }

GLuint Texture::get_height( void ) { return texture_height; }
