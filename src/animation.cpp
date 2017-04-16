#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_opengl.h"

#include "texture.h"
#include "geo2d.h"

#include <vector>

using std::vector;

class Animation
{	
	int frame_count;
	int current_frame;
	vector<Rect> clip_rects;
	
	public:
		bool loop;
		bool finished = false;
		
		long frame_ticks;

		Animation( bool loop=false );
		Animation( vector<Rect> clip_rects, bool loop=false );
		~Animation( void );

		Rect get_clip( int i );

		int get_framecount( void );
		int get_current( void );

		void add_frame( Rect clip );
		void goto_frame( int f );

		void render( Texture* sheet, Rect where, bool flip=false );
		void increment( void );
};

Animation::Animation( bool loop )
{
	this->loop = loop;

	frame_count = 0;
	current_frame = 0;
	
	frame_ticks = 50;
}

Animation::Animation( vector<Rect> clip_rects, bool loop )
{
	this->clip_rects = clip_rects;
	this->loop = loop;

	frame_count = clip_rects.size();
	current_frame = 0;
}

Animation::~Animation( void ) {}

int Animation::get_framecount( void )
{
	return frame_count;
}

int Animation::get_current( void )
{
	return current_frame;
}

Rect Animation::get_clip( int i )
{
	return clip_rects[i];
}

void Animation::add_frame( Rect clip )
{
	frame_count++;
	clip_rects.push_back( clip );	
}

void Animation::goto_frame( int f )
{
	current_frame = f % frame_count;
	if( current_frame != frame_count - 1 ) finished = false;
}

void Animation::render( Texture* sheet, Rect where, bool flip )
{
	sheet->render( clip_rects[current_frame], where, flip );
}

void Animation::increment( void )
{
	if( !finished )
	{
		if( current_frame < frame_count - 1 ) current_frame++;
		else if( loop ) current_frame = 0;
		else finished = true;
	}
}

Animation* build_animation( int w, int h, int x0, int y0, int x1, int y1, bool loop=false )
{
	Animation* result = new Animation(loop);

	int loop_x = w;

	float frame_width = 1.0 / w;
	float frame_height = 1.0 / h;

	for( int i = y0; i <= y1; i++ )
	{
		int j;

		if( i == y0 ) {
			j = x0;
		} else j = 0;

		if( i == y1 ) {
			loop_x = x1+1;
		} else loop_x = w;

		while( j < loop_x )
		{
			result->add_frame( Rect( Vec2d( frame_width*j, frame_height*i ), frame_width, frame_height ) );

			j++;
		}
	}

	return result;
}