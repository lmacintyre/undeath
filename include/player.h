#ifndef __PLAYER_H_DEFINED__
#define __PLAYER_H_DEFINED__

#include "animation.h"
#include "vec2d.h"
#include "geo2d.h"

#include "actor.h"
#include "combat.h"

#include <vector>

using std::vector;

class Player: public Actor
{
	bool anim_transition = false;
	bool dead = false;

	long hit_delay;
	long last_hit;

	CONTROL_MODE control_mode = CONTROL_MODE_FREE;

	Moveset moveset;
	Vec2d attack_origin_low, attack_origin_high;
	long attack_timer;

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

		Attack* active_attack = NULL;

		Player( void );
		~Player( void );
		Player( Vec2d pos );

		void use_attack( Attack* attack );

		void render( void );
		void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );
};

#endif
