#include "SDL2/SDL_opengl.h"

#include "display.h"

#include "vec2d.h"
#include "geo2d.h"
#include "texture.h"

#include <math.h>
#include <stdio.h>
#include <vector>
using std::vector;

class LevelBG
{
	public:
		Texture* sky_texture;
		Rect sky_clip;
		
		Texture* ground_texture;
		Rect ground_clip;
		
		Texture* level_texture;
		vector<vector<Rect>> level_tiles;
		
		LevelBG( void );
		LevelBG( Texture* sky_texture, Texture* ground_texture, Texture* level_texture );
		LevelBG( Texture* sky_texture, Texture* ground_texture, Texture* level_texture, int M, int N );
		
		void render( Rect where, Vec2d scroll );
};

LevelBG::LevelBG( void )
{
	this->sky_texture = NULL;
	this->ground_texture = NULL;
	this->level_texture = NULL;
}

LevelBG::LevelBG( Texture* sky_texture, Texture* ground_texture, Texture* level_texture )
{
	this->sky_texture = sky_texture;
	this->ground_texture = ground_texture;
	this->level_texture = level_texture;

	this->sky_clip = Rect( Vec2d( 0.f, 0.f ), 1.f, 1.f);
	this->ground_clip = Rect( Vec2d( 0.f, 0.f ), 0.5f, 1.f );

	vector<Rect> level_clip0;
	level_clip0.push_back( Rect( Vec2d( 0.f, 0.f ), 0.5f, 1.f ) );
	level_clip0.push_back( Rect( Vec2d( 0.f, 0.f ), 0.5f, 1.f ) );
	level_clip0.push_back( Rect( Vec2d( 0.f, 0.f ), 0.5f, 1.f ) );
	level_tiles.push_back( level_clip0 );
	vector<Rect> level_clip1;
	level_clip1.push_back( Rect( Vec2d( 0.f, 0.f ), 0.5f, 1.f ) );
	level_clip1.push_back( Rect( Vec2d( 0.5f, 0.f ), 0.5f, 1.f ) );
	level_clip1.push_back( Rect( Vec2d( 0.f, 0.f ), 0.5f, 1.f ) );
	level_tiles.push_back( level_clip1 );
	level_tiles.push_back( level_clip0 );
}

void LevelBG::render( Rect where, Vec2d scroll )
{
	int x_tiles, y_tiles;
	float x_start, y_start;
	float tile_size = 0.75f;
	
	float half_height;
	
	Vec2d scroll_by;
	scroll.scale( Vec2d( 1.1f, 1.1f ) );
	
	//Render sky
	if( sky_texture == NULL )
	{
		//If no sky texture provided, use a simple gradient for the sky
		glPushMatrix();
		glLoadIdentity();
		glBegin( GL_QUADS );
			glColor3f( 0.8f, 0.f, 0.f );
			glVertex2f( where.vertices[0].get_a(), where.vertices[0].get_b() );
			glVertex2f( where.vertices[1].get_a(), where.vertices[1].get_b() );
			
			glColor3f( 0.2f, 0.f, 0.f );
			glVertex2f( where.vertices[2].get_a(), where.vertices[2].get_b() );
			glVertex2f( where.vertices[3].get_a(), where.vertices[3].get_b() );
		glEnd();
		glPopMatrix();
	}
	else
	{
		//tile sky_texture within sky_clip across screen
		y_tiles = 4;
		x_tiles = (int) ( y_tiles * ( where.w / where.h ) );
		
		x_start = where.vertices[0].get_a();
		y_start = where.vertices[0].get_b();
				
		scroll_by = Vec2d( scroll.get_a(), scroll.get_b() * -1/2 );
		
		glColor3f( 0.6f, 0.6f, 0.6f );
		
		for( int i=0; i<x_tiles; i++)
			for( int j=0; j<y_tiles; j++)
			{
				sky_texture->render( sky_clip.add( scroll_by ),
														 Rect( Vec2d( x_start + i*tile_size, y_start + j*tile_size ),
														 tile_size,
														 tile_size ) );
			}
	}
	
	//Render ground
	//tile ground_texture within ground_clip below y=0
	
	if( ground_texture != NULL )
	{
		glColor3f( 0.6f, 0.6f, 0.6f );
	
		//Start from screen left? start from first tiling before screen left...
		// Player.x / total tile width - total tile width?

		//Then repeat for screen width / total tiles width + 1

		x_start = (where.w/2)*-1 - fmod(scroll.get_a(), tile_size) - tile_size;
		y_start = scroll.get_b() * -1 - tile_size;

		half_height = where.h/2;

		y_tiles = ( half_height - scroll.get_b() ) / tile_size + 1;
		x_tiles = ceil( where.w / tile_size ) + 2;
		
		scroll_by = Vec2d( scroll.get_a(), scroll.get_b()/2 );
		
		for( int i=0; i<x_tiles; i++ )
			for( int j=0; j<y_tiles; j++ )
			{
				ground_texture->render( ground_clip, Rect( Vec2d( x_start + i*tile_size, y_start - j*tile_size ),
																tile_size, tile_size ) );
			}
	}
		
	//Render level
	if( level_texture != NULL )
	{
		glColor3f( 0.6f, 0.6f, 0.6f );
		
		y_tiles = level_tiles.size();
		x_tiles = level_tiles[0].size();

		float repeat_width = ( x_tiles * tile_size );
		int reps = ceil( where.w / repeat_width ) + 2;

		x_start = (where.w/2) * -1 - fmod(scroll.get_a(), repeat_width) - repeat_width;
		y_start = scroll.get_b() * -1 - tile_size;

		float rep_start;
		for( int r=0; r<reps; r++ )
		{
			rep_start = x_start + r*repeat_width;
			for( int i=0; i<y_tiles; i++ )
				for( int j=0; j<x_tiles; j++ )
				{
					level_texture->render( level_tiles[i][j],
											 Rect( Vec2d( rep_start + j*tile_size, y_start + i*tile_size ),
											 tile_size,
											 tile_size ) );
				}
		}
	}
}
