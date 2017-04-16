#ifndef __ENEMY_H_INCLUDED__
#define __ENEMY_H_INCLUDED__

#include "vec2d.h"

#include "animation.h"
#include "block.h"
#include "actor.h"
#include "combat.h"
#include "controlzone.h"

#include <vector>

enum ENEMY_TYPE { ENEMY_TYPE_SPIKES, ENEMY_TYPE_SKELETON, ENEMY_TYPE_PALADIN };

class Enemy: public Actor
{
	public:
		int type;

		CONTROL_MODE control_mode = CONTROL_MODE_FREE;

		vector<ControlZone> control_zones;

		Enemy( void );
		Enemy( int type, Vec2d pos );
		
		virtual void hit_by( Attack* attack, bool from_right );
		virtual void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );
		virtual void animate( long t );
};

class MobileEnemy: public Enemy
{
	protected:
		Animation* walk_anim;
		Animation* idle_anim;
		Animation* jump_anim;
		Animation* idle_to_walk_anim;
		Animation* damage_anim;
		Animation* death_anim;

		Attack* active_attack;

		Vec2d attack_origin_low, attack_origin_high;

	bool walk_right;

	public:
		MobileEnemy( void );
		MobileEnemy( int type, Vec2d pos );

		virtual void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );
		virtual void animate( long t );
		virtual void render( void );
};

class EnemySkeleton: public MobileEnemy
{
	public:
		EnemySkeleton( void );
		EnemySkeleton( Vec2d pos );
};

class EnemyPaladin: public MobileEnemy
{
	Attack* heavy_swing;

	public:
		EnemyPaladin( void );
		EnemyPaladin( Vec2d pos );

		void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );

};

#endif