#ifndef __GEO_2D_H__
#define __GEO_2D_H__

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

#endif
