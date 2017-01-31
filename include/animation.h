#ifndef __ANIMATION_H_DEF__
#define __ANIMATION_H_DEF__

#include "texture.h"

#include <vector>

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

#endif
