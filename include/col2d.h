#ifndef __COL_2D_H
#define __COL_2D_H

#include <vector>
#include "geo2d.h"

using std::vector;

class CollisionResult
{
	public:
		bool result;
		Vec2d out, axis;

		CollisionResult( void );		
		CollisionResult( bool result );
		CollisionResult( bool result, Vec2d out, Vec2d axis );
};

vector<Vec2d> get_normals( Polygon shape );

CollisionResult platform_test( Rect A, Rect B );

CollisionResult collision_test( Rect A, Rect B );

CollisionResult collision_test( Polygon A, Polygon B );

#endif
