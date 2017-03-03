#include "SDL2/SDL.h"

#include "vec2d.h"

#include "event.h"
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

Enemy::Enemy( void )
{
	type = -1;
	sheet = NULL;
	active_anim = NULL;

	position = Vec2d( 0, 0 );
	velocity = Vec2d( 0, 0 );
}

Enemy::Enemy( int type, Vec2d pos )
{
	this->type = type;
	
	set_position( pos );
}

void Enemy::hit_by( Attack* attack, bool from_right )
{
	if( from_right ) velocity = Vec2d( -0.1f, 0.1f );
	else velocity = Vec2d( 0.1f, 0.1f );

	control_mode = CONTROL_MODE_HIT;
}

void Enemy::update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt ) {}

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

EnemySkeleton::EnemySkeleton( void )
{
	Enemy();

	active_anim = NULL;
	walk_anim = NULL;
	idle_anim = NULL;
	frame_tick = 0;
}

EnemySkeleton::EnemySkeleton( Vec2d pos )
{
	Enemy( ENEMY_TYPE_SKELETON, pos );

	walk_right = false;
	
	hitbox.push_back( Rect( pos.add( Vec2d( -0.05, -0.2 ) ), 0.1, 0.05 ) );
	hitbox.push_back( Rect( pos.add( Vec2d( -0.1, -0.2 ) ), 0.2, 0.4 ) );
	renderbox = Rect( pos.add( Vec2d( -0.25, -0.25 ) ), 0.5, 0.5 );
	
	frame_tick = 0;

	//LOAD WALK ANIMATION //
	walk_anim = new Animation();
	
	walk_anim->add_frame( Rect( Vec2d( 0.f	, 0.f	), 0.25f, 0.25f ) );
	walk_anim->add_frame( Rect( Vec2d( 0.25f, 0.f	), 0.25f, 0.25f ) );
	walk_anim->add_frame( Rect( Vec2d( 0.5f	, 0.f	), 0.25f, 0.25f ) );
	walk_anim->add_frame( Rect( Vec2d( 0.75f, 0.f	), 0.25f, 0.25f ) );
	walk_anim->add_frame( Rect( Vec2d( 0.f	, 0.25f	), 0.25f, 0.25f ) );
	walk_anim->add_frame( Rect( Vec2d( 0.25f, 0.25f	), 0.25f, 0.25f ) );
	walk_anim->add_frame( Rect( Vec2d( 0.5f	, 0.25f	), 0.25f, 0.25f ) );

	//LOAD IDLE ANIMATION
	idle_anim = new Animation();

	idle_anim->add_frame( Rect( Vec2d( 0.f	, 0.5f	 ), 0.25f, 0.25f ) );
	idle_anim->add_frame( Rect( Vec2d( 0.25f, 0.5f	 ), 0.25f, 0.25f ) );
	idle_anim->add_frame( Rect( Vec2d( 0.5f	, 0.5f	 ), 0.25f, 0.25f ) );
	idle_anim->add_frame( Rect( Vec2d( 0.75f, 0.5f	 ), 0.25f, 0.25f ) );	
	idle_anim->add_frame( Rect( Vec2d( 0.5f	, 0.5f	 ), 0.25f, 0.25f ) );
	idle_anim->add_frame( Rect( Vec2d( 0.25f, 0.5f	 ), 0.25f, 0.25f ) );	

	//LOAD JUMP ANIM
	jump_anim = new Animation();

	jump_anim->add_frame( Rect( Vec2d( 0.f, 0.f ), 0.25f, 0.25f ) );

	active_anim = idle_anim;
}

void EnemySkeleton::update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt )
{
	float dx = 0.f, dy = 0.f;
	grounded = false;

	// For each element of the ground set
	for( int i=0; i<ground_set.size(); i++ )
	{
		//Check for collision with the ground
		CollisionResult CR = ground_set[i].collision( hitbox[0] );
		if( CR.result )
		{
			switch( ground_set[i].type )
			{
				case BLOCK_TYPE_PLATFORM_LARGE:
				case BLOCK_TYPE_PLATFORM_MEDIUM:
				case BLOCK_TYPE_PLATFORM_SMALL:
				if( get_velocity().get_b() <= 0.f )
				{
					if( CR.out.get_b() >= 0.f && CR.axis.get_b() != 0.f )
					{
						grounded = true;
						set_velocity( Vec2d( get_velocity().get_a(), 0.f ) );
						move( CR.out , 1.f );
					}
				} break;

				case BLOCK_TYPE_WALL:
				//If horizontal collision, switch walk direction
				if( abs(CR.out.get_a()) > abs(CR.out.get_b()) ) walk_right = !walk_right;
				
				if( CR.out.get_a() == 0.f && CR.axis.get_b() != 0.f )
				{
					grounded = true;
					set_velocity( Vec2d( get_velocity().get_a(), 0.f ) );
				} else if( CR.out.get_a() != 0 )
				{
					set_velocity( Vec2d( 0.f, get_velocity().get_b() ) );
				}
				move( CR.out, 1 );
				break;
			}
		}

		//Check for collision with the control zones
		if( control_mode == CONTROL_MODE_FREE )
			for( int j=0; j<ground_set[i].zones.size(); j++ )
			{
				CR = collision_test( hitbox[0], ground_set[i].zones[j].zone );
				if( CR.result )
				{
					switch( ground_set[i].zones[j].command )
					{
						case DIRECTIVE_WALK_LEFT:
						if( walk_right ) walk_right = false;
						break;

						case DIRECTIVE_WALK_RIGHT:
						if( !walk_right ) walk_right = true;
						break;

						case DIRECTIVE_JUMP:
						if( grounded ) set_velocity( Vec2d( get_velocity().get_a(),  0.3f ) );
						break;
					}
				}
			}
	}

	switch( control_mode )
	{
		case CONTROL_MODE_FREE:
		if( !walk_right && get_velocity().get_a() > -0.03 ) dx -= 0.01;
		else if( walk_right && get_velocity().get_a() < 0.03 ) dx += 0.01;
		break;

		case CONTROL_MODE_HIT:
		if( grounded ) control_mode = CONTROL_MODE_FREE;
		break;
	}

	//Gravity
	if( !grounded ) dy -= 0.01f;

	set_velocity( get_velocity().add( Vec2d( dx, dy ) ) );
	move( velocity, dt );

	if( get_velocity().get_a() > 0.f ) facing_right = true;
	else if( get_velocity().get_a() < 0.f ) facing_right = false;

	//Animate

	long t = SDL_GetTicks();
	
	if( (t - frame_tick) > ( 1000 / 8 ) )
	{
		frame_tick = t;
		if( get_velocity().get_a() == 0 && active_anim != idle_anim )
		{
			active_anim = idle_anim;
			active_anim->goto_frame( 0 );
		} else if( get_velocity().get_a() != 0 && grounded && active_anim != walk_anim )
		{
			active_anim = walk_anim;
			active_anim->goto_frame( 0 );
		} else if( !grounded && active_anim != jump_anim ) active_anim = jump_anim;
		active_anim->increment();
	}
}
 