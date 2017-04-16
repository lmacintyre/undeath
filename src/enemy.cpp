#include "SDL2/SDL.h"

#include "vec2d.h"

#include "event.h"
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
		virtual void animate( long t ) {}
};

Enemy::Enemy( void )
{
	type = -1;
	sheet = NULL;
	active_anim = NULL;

	position = Vec2d( 0, 0 );
	printf("!SET POS TO ZERO!\n");
	velocity = Vec2d( 0, 0 );
}

Enemy::Enemy( int type, Vec2d pos )
{
	this->type = type;
	
	printf("Enemy constructor...\n");
	set_position( pos );
	printf("Enemy constructor done. Enemy position is (%f,\t%f)\n", get_position().get_a(), get_position().get_b());
}

void Enemy::hit_by( Attack* attack, bool from_right )
{
	if( from_right ) velocity = Vec2d( -0.1f, 0.1f );
	else velocity = Vec2d( 0.1f, 0.1f );

	control_mode = CONTROL_MODE_HIT;
}

void Enemy::update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt ) {}

class MobileEnemy: public Enemy
{
	protected:
		Animation* walk_anim;
		Animation* idle_anim;
		Animation* jump_anim;
		Animation* idle_to_walk_anim;
		Animation* damage_anim;
		Animation* death_anim;

		Attack* active_attack = NULL;

		Vec2d attack_origin_low, attack_origin_high;

	bool walk_right;

	public:
		MobileEnemy( void );
		MobileEnemy( int type, Vec2d pos );

		virtual void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );
		virtual void animate( long t );
		virtual void render( void );
};

MobileEnemy::MobileEnemy( void )
{
	Enemy();

	active_anim = NULL;
	walk_anim = NULL;
	idle_anim = NULL;
	jump_anim = NULL;
	idle_to_walk_anim = NULL;
	damage_anim = NULL;
	death_anim = NULL;
	frame_tick = 0;
}

MobileEnemy::MobileEnemy( int type, Vec2d pos )
{
	printf("MobileEnemy constructor...\n");
	Enemy( type, pos );
	printf("MobileEnemy constructor done. Enemy position is (%f,\t%f)\n", get_position().get_a(), get_position().get_b());
}

void MobileEnemy::update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt )
{
	printf("Enemy position is (%f,\t%f)\n", position.get_a(), position.get_b());

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
		if( control_mode == CONTROL_MODE_FREE || control_mode == CONTROL_MODE_INVULN )
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
		case CONTROL_MODE_INVULN:
		if( !walk_right && get_velocity().get_a() > -0.03 ) dx -= 0.01;
		else if( walk_right && get_velocity().get_a() < 0.03 ) dx += 0.01;
		break;

		case CONTROL_MODE_HIT:
		if( grounded ) control_mode = CONTROL_MODE_FREE;
		break;

		case CONTROL_MODE_ATTACKING:
		if( active_anim->finished )
		{
			active_attack = NULL;
			control_mode = CONTROL_MODE_FREE;
		} else {
			if( active_attack->on_spd_frame() )
			{
				Vec2d spd = active_attack->speed;
				if( !facing_right ) spd.scale( Vec2d( -1.f, 1.f ) );
				if( active_attack->speed_relative ) velocity.translate( spd );
				else velocity = spd;
			}
		}
		break;
	}

	//Gravity
	if( !grounded ) dy -= 0.01f;

	set_velocity( get_velocity().add( Vec2d( dx, dy ) ) );
	move( velocity, dt );

	if( get_velocity().get_a() > 0.f ) facing_right = true;
	else if( get_velocity().get_a() < 0.f ) facing_right = false;

	animate( SDL_GetTicks() );


}

void MobileEnemy::animate( long t )
{
	if( (t - frame_tick) > ( 1000 / 8 ) )
	//if( false )
	{
		frame_tick = t;
		active_anim->increment();

		switch( control_mode )
		{
			case CONTROL_MODE_FREE:
			case CONTROL_MODE_INVULN:
			if( grounded )
			{
				if( velocity.get_a() != 0 )
				{
					if( active_anim == idle_to_walk_anim ) set_animation( walk_anim );
					else if( active_anim != walk_anim ) set_animation( idle_to_walk_anim );
				}
				else set_animation( idle_anim );
			}
			else set_animation( jump_anim );
			break;

			case CONTROL_MODE_ATTACKING:
			if( active_attack != NULL ) set_animation( active_attack->animation );
			break;

			case CONTROL_MODE_HIT:
			set_animation( damage_anim );
			break;

			case CONTROL_MODE_DEAD:
			set_animation( death_anim );
			break;
		}
	}
}

void MobileEnemy::render( void )
{
	Actor::render();

	if( render_hitbox && active_attack != NULL )
	{
		Vec2d scale;
		if( facing_right )
			scale = Vec2d(1.f, 1.f);
		else
			scale = Vec2d(-1.f, 1.f);

		Vec2d offset;
		if( active_attack->height == ATTACK_HEIGHT_LOW )
			offset = position.add( attack_origin_low.multiply(scale) );
		else
			offset = position.add( attack_origin_high.multiply(scale) );

		printf( "(%f,\t%f)\n", position.get_a(), position.get_b() );

		//active_attack->hitbox.multiply(scale).add(offset).draw(1.f, 0.f, 1.f, false);
		active_attack->hitbox.draw(1.f, 0.f, 1.f, false);
	}
}

class EnemySkeleton: public MobileEnemy
{
	public:
		EnemySkeleton( void );
		EnemySkeleton( Vec2d pos );
};

EnemySkeleton::EnemySkeleton( void )
{
	MobileEnemy();
}

EnemySkeleton::EnemySkeleton( Vec2d pos )
{
	MobileEnemy( ENEMY_TYPE_SKELETON, pos );

	walk_right = false;
	
	hitbox.push_back( Rect( pos.add( Vec2d( -0.05, -0.15 ) ), 0.1, 0.05 ) );
	hitbox.push_back( Rect( pos.add( Vec2d( -0.075, -0.15) ), 0.15, 0.30 ) );
	renderbox = Rect( pos.add( Vec2d( -0.25, -0.25 ) ), 0.5, 0.5 );
	
	frame_tick = 0;

	//LOAD ANIMATIONS //
	walk_anim = build_animation( 8, 8, 0, 0, 6, 0, true );
	idle_anim = build_animation( 8, 8, 0, 1, 6, 1, true );
	jump_anim = build_animation( 8, 8, 0, 0, 0, 0, false );
	idle_to_walk_anim = build_animation( 8, 8, 0, 2, 0, 2, false );
	damage_anim = build_animation( 8, 8, 0, 3, 0, 3 );
	death_anim = build_animation( 8, 8, 0, 3, 5, 3 );

	active_anim = idle_anim;
}

class EnemyPaladin: public MobileEnemy
{
	Attack* heavy_swing;

	public:
		EnemyPaladin( void );
		EnemyPaladin( Vec2d pos );

		void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );

};

EnemyPaladin::EnemyPaladin( void )
{
	MobileEnemy();

	heavy_swing = NULL;
}

EnemyPaladin::EnemyPaladin( Vec2d pos )
{
	MobileEnemy( ENEMY_TYPE_PALADIN, pos );

	walk_right = false;
	
	hitbox.push_back( Rect( pos.add( Vec2d( -0.05, -0.3 ) ), 0.1, 0.05 ) );
	hitbox.push_back( Rect( pos.add( Vec2d( -0.075, -0.15) ), 0.15, 0.30 ) );
	renderbox = Rect( pos.add( Vec2d( -0.5, -0.5 ) ), 1.f, 1.f );
	
	frame_tick = 0;

	// Create attacks
	Animation* heavy_swing_anim = build_animation(4, 4, 0, 0, 0, 1, false );
	heavy_swing = new Attack( Rect( Vec2d( 0.f, 0.f), 0.5f, 0.5f ), ATTACK_HEIGHT_HIGH, heavy_swing_anim, 4, 0, 2, Vec2d( 0.f, 0.f), false ) ;

	attack_origin_low = Vec2d( 0.f, 0.f );
	attack_origin_high = Vec2d( 0.f, 0.f );

	// Build animations
	walk_anim = build_animation( 4, 4, 0, 2, 1, 3, true );
	idle_anim = build_animation( 4, 4, 0, 0, 1, 0, true );
	jump_anim = build_animation( 4, 4, 0, 3, 0, 3, false );
	idle_to_walk_anim = build_animation( 4, 4, 0, 2, 0, 2, false );
	damage_anim = build_animation( 4, 4, 2, 0, 2, 0 );
	death_anim = build_animation( 4, 4, 0, 0, 2, 0 );

	active_anim = idle_anim;
}

void EnemyPaladin::update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt )
{
	//control_mode = CONTROL_MODE_ATTACKING;
	//active_attack = heavy_swing;

	MobileEnemy::update( ground_set, enemy_set, dt );

	// Look for player
		// Perform attack
}
