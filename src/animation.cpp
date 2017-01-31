#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_opengl.h"

#include "texture.h"
#include "geo2d.h"

#include <vector>

using std::vector;

class Animation
{	
	int frames, current;
	vector<Rect> clip_rects;
	
	public:
		Animation( void );
		~Animation( void );

		vector<Rect> get_clip_rects( void );

		int get_framecount( void );
		int get_current( void );

		void add_frame( Rect clip );
		void goto_frame( int f );

		void render( Texture* sheet, Rect where, bool flip=false );
		void increment( void );
};

Animation::Animation( void )
{
	frames = 0;
	current = 0;
}

Animation::~Animation( void )
{
	
}

int Animation::get_framecount( void ) { return frames; }

int Animation::get_current( void ) { return current; }

vector<Rect> Animation::get_clip_rects( void )
{
	return clip_rects;
}

void Animation::add_frame( Rect clip )
{
	frames++;
	clip_rects.push_back( clip );	
}

void Animation::goto_frame( int f )
{
	if( f < frames ) current = f;
}

void Animation::render( Texture* sheet, Rect where, bool flip )
{
	sheet->render( clip_rects[current], where, flip );
}

void Animation::increment( void )
{
	if( current < frames-1 ) current++;
	else current = 0;
}
