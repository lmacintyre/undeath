#include "geo2d.h"
#include "col2d.h"
#include "texture.h"
#include "controlzone.h"

#include <vector>
using std::vector;

enum BLOCK_TYPE { BLOCK_TYPE_PLATFORM_LARGE, BLOCK_TYPE_PLATFORM_MEDIUM, BLOCK_TYPE_PLATFORM_SMALL, BLOCK_TYPE_WALL };

class Block
{
	Rect renderbox, clip;

	public:
		int type;

		bool render_hitbox;
	
		Texture* sheet;
	
		Rect hitbox;
		vector<ControlZone> zones;

		Block( void );
		Block( int type, Vec2d pos, int sprite_id, vector<directive> bounds );
		CollisionResult collision( Rect r );
		void render( void );
};

Block::Block( void ) {
	sheet = NULL;
}

Block::Block( int type, Vec2d pos, int sprite_id, vector<directive> bounds )
{
	render_hitbox = false;
	sheet = NULL;
	
	this->type = type;

	Vec2d left( 0.f, 0.f );
	Vec2d right( 0.f, 0.f );

	switch( type )
	{
		case BLOCK_TYPE_PLATFORM_LARGE:
		left = pos.add( Vec2d( -0.5f, 0.f ) );
		right = pos.add( Vec2d( 0.5f, 0.f ) );

		hitbox = Rect( pos.add( Vec2d( -0.5f, 0.075f ) ), 1.f, 0.05f );
		renderbox = Rect( pos.add( Vec2d( -1.f, -0.25f ) ), 2.f, 0.5f );
		switch( sprite_id )
		{
			case 1:
			clip = Rect( Vec2d( 0.f, 0.f ), 1.f, 0.25f );
			break;

			case 2:
			clip = Rect( Vec2d( 0.f, 0.25f ), 1.f, 0.25f );
			break;

			case 3:
			clip = Rect( Vec2d( 0.f, 0.5f ), 1.f, 0.25f );
			break;
		}
		break;

		case BLOCK_TYPE_PLATFORM_MEDIUM:
		left = pos.add( Vec2d( -0.35f, 0.f ) );
		right = pos.add( Vec2d( 0.35f, 0.f ) );

		hitbox =	Rect( pos.add( Vec2d( -0.35f, 0.075f ) ), 0.7f, 0.05f );
		renderbox =	Rect( pos.add( Vec2d( -0.5f, -0.25f ) ), 1.f, 0.5f );
		clip = Rect( Vec2d( 0.f, 0.75f ), 0.5f, 0.25f );		
		break;

		case BLOCK_TYPE_PLATFORM_SMALL:
		left = pos.add( Vec2d( -0.225f, 0.f ) );
		right = pos.add( Vec2d( 0.225f, 0.f ) );

		hitbox = Rect( pos.add( Vec2d( -0.225f, 0.075f ) ), 0.45f, 0.05f );
		renderbox = Rect( pos.add( Vec2d( -0.5f, -0.25f ) ), 1.f, 0.5f );
		clip = Rect( Vec2d( 0.5f, 0.75f ), 0.5f, 0.25f );		
		break;

		case BLOCK_TYPE_WALL:
		hitbox = Rect( pos.add( Vec2d( -0.5f, -0.5f ) ), 1.f, 1.f );
		renderbox = Rect( pos.add( Vec2d( -0.5f, -0.5f ) ), 1.f, 1.f );
		//renderbox = Rect( pos.add( Vec2d( -1.f, -1.f ) ), 2.f, 2.f );
		clip = Rect( Vec2d( 0.f, 0.f ), 1.f, 1.f );
		break;
	}

	if( bounds[0] != DIRECTIVE_NONE ) zones.push_back( ControlZone( Rect( left.add( Vec2d( -0.05f, 0.f ) ), 0.05f, 0.2f ), bounds[0] ) );
	if( bounds[1] != DIRECTIVE_NONE ) zones.push_back( ControlZone( Rect( right, 0.05f, 0.2f ), bounds[1] ) );
}

CollisionResult Block::collision( Rect entity )
{
	switch( type )
	{
		case BLOCK_TYPE_PLATFORM_LARGE:
		case BLOCK_TYPE_PLATFORM_MEDIUM:
		case BLOCK_TYPE_PLATFORM_SMALL:
		return platform_test( entity, hitbox );
		break;
		
		case BLOCK_TYPE_WALL:
		return collision_test( entity, hitbox );
		break;
	}
}

void Block::render( void )
{

	glColor3f( 1.f, 1.f, 1.f );
	sheet->render( clip, renderbox );

	if( render_hitbox )
	{
		glColor3f( 0.f, 0.f, 1.f );
		glBegin( GL_LINES );
		for( int i=0; i<3; i++ )
		{
			glVertex2f( hitbox.vertices[i].get_a(), hitbox.vertices[i].get_b() );
			glVertex2f( hitbox.vertices[i+1].get_a(), hitbox.vertices[i+1].get_b() );
		}
		glVertex2f( hitbox.vertices[3].get_a(), hitbox.vertices[3].get_b() );
		glVertex2f( hitbox.vertices[0].get_a(), hitbox.vertices[0].get_b() );
		glEnd();

		for( int i=0; i<zones.size(); i++ )
		{
			glColor3f( 1.f, 0.f, 1.f );
			glBegin( GL_LINES );
			for( int j=0; j<3; j++ )
			{
				glVertex2f( zones[i].zone.vertices[j].get_a(), zones[i].zone.vertices[j].get_b() );
				glVertex2f( zones[i].zone.vertices[j+1].get_a(), zones[i].zone.vertices[j+1].get_b() );
			}
			glVertex2f( zones[i].zone.vertices[3].get_a(), zones[i].zone.vertices[3].get_b() );
			glVertex2f( zones[i].zone.vertices[0].get_a(), zones[i].zone.vertices[0].get_b() );
			glEnd();
		}
	}
}
