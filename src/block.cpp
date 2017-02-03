#include "geo2d.h"
#include "col2d.h"
#include "texture.h"

enum BLOCK_TYPE { BLOCK_TYPE_PLATFORM_LARGE, BLOCK_TYPE_PLATFORM_MEDIUM, BLOCK_TYPE_PLATFORM_SMALL,
								  BLOCK_TYPE_WALL };

class Block
{
	Rect renderbox, clip;

	public:
		int type;

		bool render_hitbox;
	
		Texture* sheet;
	
		Rect hitbox;

		Block( void );
		Block( int type, Vec2d pos, int sprite_id );
		CollisionResult collision( Rect r );
		void render( void );
};

Block::Block( void ) {
	sheet = NULL;
}

Block::Block( int type, Vec2d pos, int sprite_id )
{
	render_hitbox = false;
	sheet = NULL;
	
	this->type = type;

	switch( type )
	{
		case BLOCK_TYPE_PLATFORM_LARGE:
		hitbox = Rect( pos.add( Vec2d( -0.5f, 0.05f ) ), 1.f, 0.1f );
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
		hitbox =	Rect( pos.add( Vec2d( -0.35f, 0.05f ) ), 0.7f, 0.1f );
		renderbox =	Rect( pos.add( Vec2d( -0.5f, -0.25f ) ), 1.f, 0.5f );
		clip = Rect( Vec2d( 0.f, 0.75f ), 0.5f, 0.25f );		
		break;

		case BLOCK_TYPE_PLATFORM_SMALL:
		hitbox = Rect( pos.add( Vec2d( -0.225f, 0.05f ) ), 0.45f, 0.1f );
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
	}
}
