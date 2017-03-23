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
		bool attack_held = false;

		Player( void );
		~Player( void );
		Player( Vec2d pos );
		
		void use_attack( Attack* attack, KEY key );
		void set_active_animation( Animation* anim );

		void render( void );
		void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt );
		void control( long t );
		void animate( long t );
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

	Animation* high_punch_anim = build_animation( 8, 8, 0, 5, 5, 5, false );
	Animation* low_kick_anim = build_animation( 8, 8, 0, 6, 4, 6, false );

	moveset = Moveset(
		Attack( Rect( Vec2d(-0.1f, 0.f), 0.2f, 0.3f ), ATTACK_HEIGHT_HIGH, high_punch_anim, 3, 2, 10, Vec2d( 0.2, 0.0 ), false ),
		Attack( Rect( Vec2d(0.f, 0.f), 0.1f, 0.1f ), ATTACK_HEIGHT_HIGH, high_punch_anim, 3, 2, 25, Vec2d( 0.2, 0.0 ), false ),
		Attack( Rect( Vec2d(0.f, 0.f), 0.1f, 0.1f ), ATTACK_HEIGHT_LOW, low_kick_anim, 2, 0, 15, Vec2d( 0.2, 0.0), false ),
		Attack( Rect( Vec2d(0.f, 0.f), 0.2f, 0.1f ), ATTACK_HEIGHT_LOW, low_kick_anim, 2, 0, 30, Vec2d( 0.2, 0.0), false ));
}

void Player::render( void )
{
	long t = SDL_GetTicks();

	glPushMatrix();
	switch( control_mode )
	{
		case CONTROL_MODE_ATTACKING:
		if( active_attack != NULL && active_attack->on_hit_frame() ) glColor3f( 1.f, 0.5f, 0.5f );
		active_anim->render( sheet, renderbox, !facing_right );
		break;

		case CONTROL_MODE_HIT:
		case CONTROL_MODE_INVULN:
		if( t%500 > 250 ) glColor3f( 0.5f, 0.5f, 0.5f );
		case CONTROL_MODE_DEAD:
		case CONTROL_MODE_FREE:
		active_anim->render( sheet, renderbox, !facing_right );
		break;
	}

	if( render_hitbox )
	{

		glTranslatef( -1 * get_position().get_a(), -1 * get_position().get_b(), 0.f );
		
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

void Player::use_attack( Attack* attack, KEY key )
{
	if( control_mode == CONTROL_MODE_FREE || control_mode == CONTROL_MODE_INVULN )
	{
		active_attack = attack;
		active_attack->key = key;
		attack_held = true;

		set_animation( active_attack->animation );

		control_mode = CONTROL_MODE_ATTACKING;
	}
}

void Player::update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt )
{
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
				if( velocity.get_b() <= 0.f )
				{
					grounded = true;
					jumpjuice = 10;
					velocity.set_b( 0.f );
					move( CR.out , 1.f );
				} break;
				
				case BLOCK_TYPE_WALL:
				if( CR.out.get_a() == 0.f && CR.axis.get_b() != 0.f )
				{
					grounded = true;
					jumpjuice = 10;
					velocity.set_b( 0.f );
				} else if( CR.out.get_a() != 0 )
				{
					velocity.set_a( 0.f );
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
	{
		for( int i=0; i<enemy_set.size(); i++)
		{
			if( ((Enemy*) enemy_set[i])->control_mode != CONTROL_MODE_FREE ) continue;

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
			if( active_attack != NULL && active_attack->on_hit_frame() )
			{
				CR = collision_test( attack_box, enemy_set[i]->hitbox[1] );

				if( CR.result )
				{
					bool facing_right = true;
					if( CR.out.get_a() < 0 ) facing_right = false;
					( (Enemy*) enemy_set[i] )->hit_by( active_attack, facing_right );
					velocity.set_a( velocity.get_a() * -1.f );
					velocity.set_b( 0.1f );
				}
			}
		}
	}

	if( !grounded ) velocity.translate( Vec2d( 0.f, -0.01f ) );

	control( t );
	move( velocity, dt );
	animate( t );
}

void Player::control( long t )
{
	switch( control_mode )
	{
		case CONTROL_MODE_INVULN:
		if( t > last_hit + hit_delay ) control_mode = CONTROL_MODE_FREE;

		case CONTROL_MODE_FREE:
		if( hp <= 0 ) control_mode = CONTROL_MODE_DEAD;

		if( keys_down[KEY_UP] )
		{
			if( grounded ) velocity.set_b( 0.2f );
			else if( jumpjuice > 0 )
			{
				jumpjuice--;
				velocity.translate( Vec2d( 0.f, 0.012f ) );
			}
		} else if( !grounded )
		{
			jumpjuice = 0;
		}

		if( keys_down[KEY_LEFT] )
		{
			facing_right = false;
			if( velocity.get_a() > -0.10 )
			{
				if( grounded ) velocity.translate( Vec2d( -0.01f, 0.f ) );
				else velocity.translate( Vec2d( -0.005f, 0.f ) );
			}
		} if( keys_down[KEY_RIGHT] )
		{
			facing_right = true;
			if( velocity.get_a() < 0.10 )
			{
				if( grounded ) velocity.translate( Vec2d( 0.01f, 0.f ) );
				else velocity.translate( Vec2d( 0.005f, 0.f ) );
			}
		} else if( !keys_down[KEY_LEFT] && !keys_down[KEY_RIGHT] ) slow( 0.01f );

		if( keys_down[KEY_a] )
		{
			use_attack(&(moveset.light_punch), KEY_a);
		} else if( keys_down[KEY_s] )
		{
			use_attack(&(moveset.heavy_punch), KEY_s);
		} else if( keys_down[KEY_z] )
		{
			use_attack(&(moveset.light_kick), KEY_z);
		} else if( keys_down[KEY_x] )
		{
			use_attack(&(moveset.heavy_kick), KEY_x);
		}
		break;

		case CONTROL_MODE_ATTACKING:
		if( active_anim->finished )
		{
			active_attack = NULL;
			control_mode = CONTROL_MODE_FREE;
		} else {
			if( !keys_down[active_attack->key] ) attack_held = false;
			
			if( active_attack->on_spd_frame() )
			{
				Vec2d spd = active_attack->speed;
				if( !facing_right ) spd.scale( Vec2d( -1.f, 1.f ) );
				if( active_attack->speed_relative ) velocity.translate( spd );
				else velocity = spd;
			}
		}

		slow( 0.005f );
		break;

		case CONTROL_MODE_HIT:
		if( grounded ) control_mode = CONTROL_MODE_INVULN;
		slow( 0.01f );
		break;

		case CONTROL_MODE_DEAD:
		slow( 0.01f );
		break;
	}
}

void Player::animate( long t )
{
	if( (t - frame_tick) > (1000 / 8) )
	{
		frame_tick = t;

		switch( control_mode )
		{
			case CONTROL_MODE_FREE:
			case CONTROL_MODE_INVULN:
			active_anim->increment();

			if( grounded )
			{
				if( velocity.get_a() != 0 )
				{
					if( active_anim == idle_to_walk_anim ) set_animation( walk_anim );
					else if( active_anim != walk_anim ) set_animation( idle_to_walk_anim );
				}
				else
				{
					if( active_anim == idle_to_walk_anim ) set_animation( idle_anim );
					else if( active_anim != idle_anim ) set_animation( idle_to_walk_anim );
				}
			}
			else set_animation( jump_anim );
			break;

			case CONTROL_MODE_HIT:
			active_anim->increment();
			set_animation( damage_anim );
			break;

			case CONTROL_MODE_ATTACKING:
			if( active_attack != NULL ) 
			{
				if( !(attack_held && active_attack->on_hit_frame()) ) active_anim->increment();
			}
			break;

			case CONTROL_MODE_DEAD:
			active_anim->increment();
			set_animation( death_anim );
			break;
		}
	}
}