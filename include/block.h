#ifndef __BLOCK_H_DEFINED__
#define __BLOCK_H_DEFINED__

#include "geo2d.h"
#include "col2d.h"
#include "texture.h"

enum BLOCK_TYPE { BLOCK_TYPE_PLATFORM_LARGE, BLOCK_TYPE_PLATFORM_MEDIUM, BLOCK_TYPE_PLATFORM_SMALL, BLOCK_TYPE_WALL };

class Block
{
	Rect renderbox, clip;
	bool render_hitbox;

	public:
		int type;

		Texture* sheet;

		Rect hitbox;

		Block( void );
		Block( int type, Vec2d pos, int sprite_id );
		CollisionResult collision( Rect r );
		void render( void );
};

#endif
