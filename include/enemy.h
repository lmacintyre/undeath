#include "vec2d.h"

#include "animation.h"
#include "block.h"
#include "actor.h"
#include "combat.h"

#include <vector>

enum ENEMY_TYPE { ENEMY_TYPE_SPIKES, ENEMY_TYPE_SKELETON };

class Enemy: public Actor
{
	public:
		int type;

		CONTROL_MODE control_mode = CONTROL_MODE_FREE;
		
		Enemy( void );
		Enemy( int type, Vec2d pos );
		
		virtual void hit_by( Attack* attack, bool from_right );
		virtual void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );
};

class EnemySkeleton: public Enemy
{
	Animation* walk_anim;
	Animation* idle_anim;
	Animation* jump_anim;

	bool walk_right;
	
	public:
		EnemySkeleton( void );
		EnemySkeleton( Vec2d pos );

		void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );
};
