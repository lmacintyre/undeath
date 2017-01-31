#include "SDL2/SDL_opengl.h"
#include "vec2d.h"
#include <vector>

using std::vector;

class Rect
{
	public:
		vector<Vec2d> vertices;
		float w, h;

		Rect( void );
		Rect( Vec2d p, float w, float h );
		Rect( vector<Vec2d> vertices );
		
		void translate( Vec2d t );
		void scale( Vec2d s );

		Rect add( Vec2d o );
		Rect multiply( Vec2d o );

		void draw( float r, float g, float b, bool fill );
};

Rect::Rect( void )
{
	w = 0; h = 0;
}

Rect::Rect( Vec2d p, float w, float h )
{
	float pa = p.get_a(), pb = p.get_b();
	this->w = w; this->h = h;

	vertices.clear();
	vertices.push_back( p );
	vertices.push_back( Vec2d( pa + w, pb	  ) );
	vertices.push_back( Vec2d( pa + w, pb + h ) );	
	vertices.push_back( Vec2d( pa	 , pb + h ) );

}

void Rect::translate( Vec2d t )
{
	for( int i=0; i<vertices.size(); i++ ) vertices[i].translate( t );
}

void Rect::scale( Vec2d s )
{
	for( int i=0; i<vertices.size(); i++ ) vertices[i].scale( s );
}

Rect Rect::add( Vec2d o )
{
	return Rect( vertices[0].add( o ), w, h );
}

Rect Rect::multiply( Vec2d o )
{
	return Rect( vertices[0].multiply( o ), w * o.get_a(), h * o.get_b() );
}

void Rect::draw( float r, float g, float b, bool fill )
{
	glColor3f( r, g, b );
	if( fill )
	{
		glBegin( GL_QUADS );
		for( int i=0; i<4; i++ ) glVertex2f( vertices[i].get_a(), vertices[i].get_b() );
		glEnd();
	} else {
		glBegin( GL_LINES );
		for( int i=0; i<3; i++ )
		{
			glVertex2f( vertices[i].get_a(), vertices[i].get_b() );
			glVertex2f( vertices[i+1].get_a(), vertices[i+1].get_b() );
		}
		glVertex2f( vertices[3].get_a(), vertices[3].get_b() );
		glVertex2f( vertices[0].get_a(), vertices[0].get_b() );
		glEnd();
	}
}

class Polygon
{
	vector<Vec2d> vertices;

	public:
		Polygon( void );
		Polygon( vector<Vec2d> vertices ); 

		void set_vertices( vector<Vec2d> vertices );
		vector<Vec2d> get_vertices( void );
		
		int size( void );
		void add( Vec2d o );
		void scale( Vec2d o );
};

Polygon::Polygon( void ) {}

Polygon::Polygon( vector<Vec2d> vertices )
{
	this->vertices = vertices;
}

void Polygon::set_vertices( vector<Vec2d> vertices )
{
	this->vertices = vertices;
}

vector<Vec2d> Polygon::get_vertices( void )
{
	return this->vertices;
}

int Polygon::size( void )
{
	return vertices.size();
}

void Polygon::add( Vec2d o )
{
	for( int i=0; i<vertices.size(); i++ )
	{
		vertices[i].add( o );
	}
}


void Polygon::scale( Vec2d o )
{
	for( int i=0; i<size(); i++ )
	{
		vertices[i].set( vertices[i].get_a() * o.get_a(), vertices[i].get_b() * o.get_b() );
	}
}
