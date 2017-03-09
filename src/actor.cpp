

#include "geo2d.h"
#include "vec2d.h"

#include "texture.h"
#include "animation.h"
#include "block.h"

#include <stdio.h>
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

		virtual void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt ) = 0;
		virtual void move( Vec2d v, float dt );
		virtual void slow( void );
		virtual void render( void );

		virtual Vec2d get_position( void ) { return position; }
		virtual Vec2d get_velocity( void ) { return velocity; }
		virtual void set_position( Vec2d p ) { position = p; }
		virtual void set_velocity( Vec2d v ) { velocity = v; }
		virtual void set_animation( Animation* a ) { active_anim = a; }
};

void Actor::move( Vec2d v, float dt )
{
	Vec2d by = v.multiply( Vec2d( dt, dt ) );

	position.translate( by );
	for( int i=0; i<hitbox.size(); i++) hitbox[i].translate( by );
}

void Actor::slow( void )
{
	if( velocity.get_a() > 0.01 ) velocity.translate( Vec2d( -0.01f, 0.f ) );
	else if( velocity.get_a() < -0.01 ) velocity.translate( Vec2d( 0.01f, 0.f ) );
	else velocity.set_a( 0.f );
}

void Actor::render( void )
{	
	glPushMatrix();
	glTranslatef( get_position().get_a(), get_position().get_b(), 0.f );	
	active_anim->render( sheet, renderbox, !facing_right );
	glPopMatrix();

	if( render_hitbox )
		for( int i=0; i<hitbox.size(); i++) hitbox[i].draw( 0.f, 0.f, 1.f, false );
}
