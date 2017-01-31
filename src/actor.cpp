#include "geo2d.h"
#include "vec2d.h"

#include "texture.h"
#include "animation.h"
#include "block.h"

#include <stdio.h>
#include <vector>
using std::vector;

class Actor
{
	public:
		Texture* sheet;
		Animation* active_anim;

		long frame_tick;
		
		bool render_hitbox;
		Rect renderbox;
		vector<Rect> hitbox;

		bool facing_right;
		bool grounded;
		
		Vec2d position;
		Vec2d velocity;

		virtual void update( vector<Block> ground_set, vector<Actor*> enemy_set, float dt ) = 0;
		virtual void move( Vec2d v, float dt );
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

void Actor::render( void )
{	
	glPushMatrix();
	glTranslatef( get_position().get_a(), get_position().get_b(), 0.f );	
	active_anim->render( sheet, renderbox, !facing_right );
	glPopMatrix();

	if( render_hitbox )
		for( int i=0; i<hitbox.size(); i++) hitbox[i].draw( 0.f, 0.f, 1.f, false );
}
