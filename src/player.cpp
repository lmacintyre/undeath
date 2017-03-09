#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "vec2d.h"
#include "geo2d.h"
#include "col2d.h"

#include "actor.h"
#include "enemy.h"
#include "combat.h"

#include "texture.h"
#include "animation.h"
#include "block.h"

#include "display.h"
#include "event.h"

#include <vector>
#include <stdio.h>

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
		Animation* damage_anim;
		Animation* death_anim;

		Attack* active_attack = NULL;

		Player( void );
		~Player( void );
		Player( Vec2d pos );
		
		void use_attack( Attack* attack );
		void set_active_animation( Animation* anim );

		void render( void );
		void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );
		void animate( void );
};

Player::Player( void )
{
	hp = 0; maxhp = 0;

	walk_anim = NULL;
	idle_anim = NULL;
	jump_anim = NULL;
	idle_to_walk_anim = NULL;
	death_anim = NULL;
	active_anim = NULL;

	sheet = NULL;
}

Player::~Player( void )
{
	if( walk_anim != NULL ) delete walk_anim;
	if( idle_anim != NULL ) delete idle_anim;
	if( jump_anim != NULL ) delete jump_anim;
	if( idle_to_walk_anim != NULL ) delete idle_to_walk_anim;
	if( death_anim != NULL ) delete death_anim;

	if( sheet != NULL ) delete sheet;

	if( keys_down != NULL ) delete keys_down;
}

Player::Player( Vec2d pos )
{
	control_mode = CONTROL_MODE_FREE;	
	last_hit = 0;
	hit_delay = 2000;

	keys_down = new bool[KEYCOUNT];
	for( int i=0; i<KEYCOUNT; i++ ) keys_down[i] = false;

	frame_tick = 0;

	set_position( pos );
	set_velocity( Vec2d( 0.f, 0.f ) );

	maxhp = 3; hp = maxhp;
	
	hitbox.push_back( Rect( pos.add( Vec2d( -0.05, -0.25 ) ), 0.1, 0.05 ) );
	hitbox.push_back( Rect( pos.add( Vec2d( -0.1, -0.25) ), 0.2, 0.45 ) );
	renderbox = Rect( pos.add( Vec2d( -0.25, -0.25 ) ), 0.5, 0.5 );

	//BUILD ANIMATIONS //

	walk_anim = build_animation( 8, 8, 0, 0, 6, 0, true );
	idle_anim = build_animation( 8, 8, 0, 1, 6, 1, true );
	jump_anim = build_animation( 8, 8, 0, 0, 0, 0, false );
	idle_to_walk_anim = build_animation( 8, 8, 0, 2, 0, 2, false );
	damage_anim = build_animation( 8, 8, 0, 3, 0, 3 );
	death_anim = build_animation( 8, 8, 0, 3, 5, 3 );

	dead_box = Rect( Vec2d( -0.1f, -0.05f ), 0.2f, 0.05f );

	active_anim = idle_anim;

	//ATTACKS//
	attack_timer = 0;

	attack_origin_high = Vec2d( 0.1f, -0.1f );
	attack_origin_low = Vec2d( 0.1f, -0.2f );

	Animation* high_punch_anim = build_animation( 8, 8, 0, 4, 2, 4, false );

	moveset = Moveset(
		Attack( Rect( Vec2d(0.f, 0.f), 0.1f, 0.1f ), ATTACK_HEIGHT_HIGH, high_punch_anim, 10 ),
		Attack( Rect( Vec2d(0.f, 0.f), 0.1f, 0.1f ), ATTACK_HEIGHT_HIGH, high_punch_anim, 25 ),
		Attack( Rect( Vec2d(0.f, 0.f), 0.1f, 0.1f ), ATTACK_HEIGHT_LOW, high_punch_anim, 15 ),
		Attack( Rect( Vec2d(0.f, 0.f), 0.2f, 0.1f ), ATTACK_HEIGHT_LOW, high_punch_anim, 30 ));
}

void Player::render( void )
{
	long t = SDL_GetTicks();

	glPushMatrix();
	switch( control_mode )
	{
		case CONTROL_MODE_HIT:
		case CONTROL_MODE_INVULN:
		if( t%500 > 250 ) glColor3f( 0.5f, 0.5f, 0.5f );
		case CONTROL_MODE_DEAD:
		case CONTROL_MODE_FREE:
		case CONTROL_MODE_ATTACKING:
		active_anim->render( sheet, renderbox, !facing_right );
		break;
	}
	glTranslatef( -1 * get_position().get_a(), -1 * get_position().get_b(), 0.f );
	if( render_hitbox )
	{
		for( int i=0; i<hitbox.size(); i++) hitbox[i].draw( 0.f, 0.f, 1.f, false );

		if( active_attack != NULL )
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

			active_attack->hitbox.multiply(scale).add(offset).draw(1.f, 0.f, 1.f, false);
		}
	}
	glPopMatrix();
}

void Player::use_attack( Attack* attack )
{
	if( control_mode == CONTROL_MODE_FREE || control_mode == CONTROL_MODE_INVULN )
	{
		active_attack = attack;
		control_mode = CONTROL_MODE_ATTACKING;
		active_anim = attack->animation;
		active_anim->goto_frame( 0 );
	}
}

void Player::set_active_animation( Animation* anim )
{
	if( active_anim != anim )
	{
		active_anim = anim;
		active_anim->goto_frame( 0 );
	}
}

void Player::update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt )
{
	float dx = 0.0f, dy = 0.0f;
	CollisionResult CR;	
	
	long t = SDL_GetTicks();

	// Ground Collision Checks
	grounded = false;
	for( int i=0; i<ground_set.size(); i++ )
	{
		CR = ground_set[i].collision( hitbox[0] );
		if( CR.result )
		{
			switch( ground_set[i].type )
			{
				case BLOCK_TYPE_PLATFORM_LARGE:
				case BLOCK_TYPE_PLATFORM_MEDIUM:
				case BLOCK_TYPE_PLATFORM_SMALL:
				if( get_velocity().get_b() <= 0.f )
				{/*
					if( CR.out.get_b() >= 0.f && CR.axis.get_b() != 0.f )
					{*/
						grounded = true;
						jumpjuice = 10;
						set_velocity( Vec2d( get_velocity().get_a(), 0.f ) );
						move( CR.out , 1.f );
					//}
				} break;
				
				case BLOCK_TYPE_WALL:
				if( CR.out.get_a() == 0.f && CR.axis.get_b() != 0.f )
				{
					grounded = true;
					jumpjuice = 10;
					set_velocity( Vec2d( get_velocity().get_a(), 0.f ) );
				} else if( CR.out.get_a() != 0 )
				{
					set_velocity( Vec2d( 0.f, get_velocity().get_b() ) );
				}
				move( CR.out, 1 );
				break;
			}
		}
	}
	
	// Enemy Collision Checks
	Rect attack_box;

	if( active_attack != NULL )
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

		attack_box = active_attack->hitbox.multiply(scale).add(offset);
	}

	if( control_mode == CONTROL_MODE_FREE || control_mode == CONTROL_MODE_ATTACKING )
		for( int i=0; i<enemy_set.size(); i++)
		{
			CR = collision_test(hitbox[1], enemy_set[i]->hitbox[1]);

			if( CR.result )
			{
				grounded = false;
				if( CR.out.get_a() > 0 ) set_velocity( Vec2d( 0.05f, 0.2f ) );
				else set_velocity( Vec2d( -0.05f, 0.2f ) );
				hp--;

				control_mode = CONTROL_MODE_HIT;
				last_hit = t;

				break;
			}

			//Enemy hit checks
			if( active_attack != NULL )
			{
				CR = collision_test( attack_box, enemy_set[i]->hitbox[1] );

				if( CR.result )
				{
					bool facing_right = true;
					if( CR.out.get_a() < 0 ) facing_right = false;
					( (Enemy*) enemy_set[i] )->hit_by( active_attack, facing_right );
				}
			}
		}


	// Control
	switch( control_mode )
	{
		case CONTROL_MODE_INVULN:
		if( t > last_hit + hit_delay ) control_mode = CONTROL_MODE_FREE;

		case CONTROL_MODE_FREE:
		if( hp <= 0 ) control_mode = CONTROL_MODE_DEAD;

		if( keys_down[KEY_UP] )
		{
			if( grounded ) set_velocity( Vec2d( get_velocity().get_a(),  0.2f ) );
			else if( jumpjuice > 0 )
			{
				jumpjuice--;
				dy += 0.012f;
			}
		} else if( !grounded )
		{
			jumpjuice = 0;
		}

		if( keys_down[KEY_LEFT] )
		{
			facing_right = false;
			if( get_velocity().get_a() > -0.10 )
			{
				if( grounded ) dx -= 0.01;
				else dx -= 0.005;
			}
		} if( keys_down[KEY_RIGHT] )
		{
			facing_right = true;
			if( get_velocity().get_a() < 0.10 )
			{
				if( grounded ) dx += 0.01;
				else dx += 0.005;
			}
		} else if( !keys_down[KEY_LEFT] && !keys_down[KEY_RIGHT] )
		{
			if( get_velocity().get_a() > 0.01 ) dx -= 0.01;
			else if( get_velocity().get_a() < -0.01 ) dx += 0.01;
			else set_velocity( Vec2d( 0, get_velocity().get_b() ) );
		}

		if( keys_down[KEY_a] )
		{
			use_attack(&(moveset.light_punch));
		} else if( keys_down[KEY_s] )
		{
			use_attack(&(moveset.heavy_punch));
		} else if( keys_down[KEY_z] )
		{
			use_attack(&(moveset.light_kick));
		} else if( keys_down[KEY_x] )
		{
			use_attack(&(moveset.heavy_kick));
		}
		break;

		case CONTROL_MODE_ATTACKING:
		if( active_anim->finished )
		{
			control_mode = CONTROL_MODE_FREE;
			active_attack = NULL;
		}
		break;

		case CONTROL_MODE_HIT:
		if( grounded ) control_mode = CONTROL_MODE_INVULN;
		break;

		case CONTROL_MODE_DEAD:
		if( get_velocity().get_a() > 0.01 ) dx -= 0.01;
		else if( get_velocity().get_a() < -0.01 ) dx += 0.01;
		else set_velocity( Vec2d( 0, get_velocity().get_b() ) );
		break;
	}
	if( !grounded ) dy -= 0.01f;

	velocity = velocity.add( Vec2d( dx, dy ) );
	
	move( velocity, dt );
	animate();
}

void Player::animate( void )
{
	long t = SDL_GetTicks();
	if( (t - frame_tick) > (1000 / 8) )
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
					if( active_anim == idle_to_walk_anim ) set_active_animation( walk_anim );
					else if( active_anim != walk_anim ) set_active_animation( idle_to_walk_anim );
				}
				else set_active_animation( idle_anim );
			}
			else set_active_animation( jump_anim );
			break;

			case CONTROL_MODE_HIT:
			set_active_animation( damage_anim );
			break;

			case CONTROL_MODE_ATTACKING:
			set_active_animation( active_attack->animation );
			break;

			case CONTROL_MODE_DEAD:
			set_active_animation( death_anim );
			break;
		}
	}
}