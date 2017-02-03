#ifndef __PLAYER_H_DEFINED__
#define __PLAYER_H_DEFINED__

#include "animation.h"
#include "vec2d.h"
#include "geo2d.h"

#include "actor.h"

#include <vector>

using std::vector;

enum CONTROL_MODE { CONTROL_MODE_FREE, CONTROL_MODE_HIT, CONTROL_MODE_INVULN, CONTROL_MODE_DEAD };

class Player: public Actor
{
	bool anim_transition = false;
	bool dead = false;

	long hit_delay;
	long last_hit;

	CONTROL_MODE control_mode = CONTROL_MODE_FREE;

	int jumpjuice = 0;

	Rect dead_box;

	public:
		bool* keys_down;

		int hp, maxhp;

		Animation* walk_anim;
		Animation* idle_anim;
		Animation* jump_anim;
		Animation* idle_to_walk_anim;
		Animation* death_anim;

		Player( void );
		Player( Vec2d pos );
		
		void render( void );
		void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );
};

#endif
