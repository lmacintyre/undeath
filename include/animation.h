////
//
//  Author : Liam MacIntyre
//  Version : 0.1
//
////

#ifndef __ANIMATION_H_DEF__
#define __ANIMATION_H_DEF__

#include "texture.h"
#include <vector>

////
//
// Animation : structure for building animations
//
// Defines a series of clipping rectangles that can be applied
// to an arbitrary sprite sheet stores as a Texture (see texture.h)
//
// Animation also defines methods for iterating over these clipping
// rects (frames) and managing the animation's state.
//
////

class Animation
{
	// number of clipping rectangles (frames) in the animation
	//
	int frame_count;

	// tracks the index of the current frame to be rendered
	//
	int current_frame;

	// the set of frames
	//
	vector<Rect> clip_rects;
	
	public:

		// if loop is false, increment will not reset current_frame to 0 after the final
		// frame. It will instead signal that the animation is over by setting finished = true
		//
		bool loop;
		bool finished = false;

		// default constructor, creates an Animation with no clipping Rects
		//
		Animation( bool loop = false );

		// constuctor for pre-defined clipping rectangle set, useful for implementing a
		// general set of clipping rects for use over multiple sprite sheets
		//
		Animation( vector<Rect> clip_rects, bool loop = false );

		// destructor... maybe this will do something eventually?
		//
		~Animation( void );

		// returns the clipping rectangle at index i
		//
		Rect get_clip( int i );

		// getters for framecount and current frame
		//
		int get_framecount( void );
		int get_current( void );

		// pushes a new frame to the end of the animation
		//
		void add_frame( Rect clip );

		// jumps to a specific frame at index (f % frame_count)
		//
		void goto_frame( int f );
		
		// renders a portion of the sprite sheet 'sheet' defined by the current clipping
		// rectangle to the portion of the rendering space defined by 'where'. Setting
		// flip to true will horizontally flip the rendering order of points in the
		// clipping rect.
		//
		void render( Texture* sheet, Rect where, bool flip=false );
		
		// increments the 'current_frame' iterator within the range 0 <= current_frame < frame_count
		//
		void increment( void );
};

// Builds an Animation given the dimensions of a sprite sheet and coordinates of the relevant frames
//
// w, h 	:	Dimensions of the sprite sheet
// x0, y0	: 	Coordinates of the first fame
// x1, y1 	: 	Coordinates of the last frame
//
// loop 	: 	Should the animation loop after the last frame? (default false)
//
Animation* build_animation( int w, int h, int x0, int y0, int x1, int y1, bool loop=false );

#endif
