#ifndef __ACTOR_H_DEFINED__
#define __ACTOR_H_DEFINED__

#include "geo2d.h"
#include "vec2d.h"

#include "texture.h"
#include "animation.h"
#include "block.h"

#include <vector>
using std::vector;

enum CONTROL_MODE { CONTROL_MODE_FREE, CONTROL_MODE_ATTACKING, CONTROL_MODE_HIT, CONTROL_MODE_INVULN, CONTROL_MODE_DEAD };

class Actor
{
	public:
		Texture* sheet;
		Animation* active_anim;

		long frame_tick;
		
		bool render_hitbox = false;
		Rect renderbox;
		vector<Rect> hitbox;

		bool facing_right = true;		
		bool grounded = false;

		Vec2d position;
		Vec2d velocity;

		virtual void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );
		virtual void move( Vec2d v, float dt );
		virtual void slow( void );
		virtual void render( void );

		virtual Vec2d get_position( void );
		virtual Vec2d get_velocity( void );
		virtual void set_position( Vec2d p );
		virtual void set_velocity( Vec2d v );
		virtual void set_animation( Animation* a );
};

#endif
