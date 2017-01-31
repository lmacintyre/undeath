#ifndef __PLAYER_H_DEFINED__
#define __PLAYER_H_DEFINED__

#include "animation.h"
#include "vec2d.h"
#include "geo2d.h"

#include "actor.h"

#include <vector>

using std::vector;

enum CONTROL_MODE { CONTROL_MODE_FREE, CONTROL_MODE_HIT };

class Player: public Actor
{
	bool anim_transition;

	long last_hit;
	long hit_delay;

	CONTROL_MODE control_mode;

	int jumpjuice;

	public:
		bool* keys_down;

		int hp, maxhp;

		Animation* walk_anim;
		Animation* idle_anim;
		Animation* jump_anim;
		Animation* idle_to_walk_anim;

		Player( void );
		Player( Vec2d pos );
		
		void render( void );
		void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );
};

#endif
