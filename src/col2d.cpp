#include "vec2d.h"
#include "geo2d.h"
#include <math.h>
#include <stdio.h>
#include <vector>

#include <GL/gl.h>

using std::vector;

////
//
//	Container for results of collision tests
//
//	result: true if objects colliding
//	out:	shortest axis-aligned vector out of collision
//
class CollisionResult
{
	public:
		bool result;
		Vec2d out, axis;
		
		CollisionResult( void );
		CollisionResult( bool result );
		CollisionResult( bool result, Vec2d out, Vec2d axis );
};	

CollisionResult::CollisionResult( void )
{
	result = false;
}

CollisionResult::CollisionResult( bool result )
{
	this->result = result;
}

CollisionResult::CollisionResult( bool result, Vec2d out, Vec2d axis )
{
	this->result = result;
	this->out = out;
	this->axis = axis;
}

vector<Vec2d> get_normals( Polygon shape )
{
	Vec2d n;
	vector<Vec2d> sn;
	
	for( int i=0; i<shape.size()-1; i++ )
	{
		n.set_a( ( shape.get_vertices()[i+1].get_b() - shape.get_vertices()[i].get_b( ) ) * -1 );
		n.set_b( shape.get_vertices()[i+1].get_a() - shape.get_vertices()[i].get_a() );
		
		n = n.unit();
		sn.push_back( Vec2d( n.get_a(), n.get_b() ) );	
	}
	
	n.set_a( ( shape.get_vertices()[0].get_b() - shape.get_vertices()[shape.size()-1].get_b() ) * -1 );
	n.set_b( shape.get_vertices()[0].get_a() - shape.get_vertices()[shape.size()-1].get_a() );
	
	sn.push_back( Vec2d( n.get_a(), n.get_b() ).unit() );
	
	return sn;
}

CollisionResult platform_test( Rect A, Rect B )
{
	float minA = A.vertices[0].get_b();	float minB = B.vertices[0].get_b();
	float maxA = A.vertices[2].get_b();	float maxB = B.vertices[2].get_b();
	
	if( A.vertices[1].get_a() < B.vertices[0].get_a()
		|| B.vertices[1].get_a() < A.vertices[0].get_a() ) return CollisionResult( false );

	if( maxA < minB || maxB < minA ) return CollisionResult( false );
	else if( minA > minB ) return CollisionResult( true, Vec2d( 0.f, maxB-minA ), Vec2d( 0.f, 1.f ) );
	else return CollisionResult( false );
}

CollisionResult collision_test( Rect A, Rect B )
{
	Vec2d result( 0.f, 0.f );
	
	vector<Vec2d> axes;
	axes.push_back( A.vertices[1].add( A.vertices[0].multiply( Vec2d( -1, -1 ) ) ).get_normal() );
	axes.push_back( A.vertices[2].add( A.vertices[1].multiply( Vec2d( -1, -1 ) ) ).get_normal() );
	
	//axes.push_back( B.vertices[1].add( B.vertices[0].multiply( Vec2d( -1, -1 ) ) ).get_normal() );
	//axes.push_back( B.vertices[2].add( B.vertices[1].multiply( Vec2d( -1, -1 ) ) ).get_normal() );

	float sep = -1.f; int sep_axis;
	float chk;
	float minA, maxA, minB, maxB;
	for( int i=0; i<axes.size(); i++ )
	{
		minA = A.vertices[0].dot( axes[i] );
		maxA = minA;

		minB = B.vertices[0].dot( axes[i] );
		maxB = minB;

		for( int j=1; j<4; j++ )
		{
			chk = A.vertices[j].dot( axes[i] );
			if( chk < minA ) minA = chk;
			else if( chk > maxA ) maxA = chk;

			chk = B.vertices[j].dot( axes[i] );
			if( chk < minB ) minB = chk;
			else if( chk > maxB ) maxB = chk;
		}

		if( maxA < minB || maxB < minA ) return CollisionResult( false );
	
		chk = abs( minB - maxA );
		if( sep == -1.f || chk < abs( sep ) ){
			sep = minB - maxA;
			sep_axis = i;
		}
		
		chk = abs( maxB - minA );
		if( chk < abs( sep ) ) {
			sep = maxB - minA;
			sep_axis = i;
		}
	}
 		
	result = axes[sep_axis].multiply( Vec2d( sep, sep ) );
	float p = 1 / pow( axes[sep_axis].length(), 2 );
	result.scale( Vec2d( p, p ) );
	
	return CollisionResult( true, result, axes[sep_axis] );
}

CollisionResult collision_test( Polygon A, Polygon B )
{
	Vec2d result( 0.f, 0.f );
	float seperation = 0.0f;	
	bool seperated = false;
	
	//Calculate axes
	vector<Vec2d> An = get_normals( A ), Bn = get_normals( B );
	
	vector<Vec2d> axes = An;
	axes.insert( axes.end(), Bn.begin(), Bn.end() );
	
	for( int i=0; i<axes.size(); i++ )
	{
		float minA, maxA;
		float minB, maxB;
		
		minA = A.get_vertices()[0].dot( axes[i] );
		maxA = minA;
		
		minB = B.get_vertices()[0].dot( axes[i] );
		maxB = minB;
		
		float check;
		
		for( int j=1; j<A.size(); j++ )
		{
			check = A.get_vertices()[j].dot( axes[i] );
			if( check < minA ) minA = check;
			else if( check > maxA ) maxA = check;
		}
		
		for( int j=1; j<B.size(); j++ )
		{
			check = B.get_vertices()[j].dot( axes[i] );
			if( check < minB ) minB = check;
			else if( check > maxB ) maxB = check;
		}
		
		if( maxB < minA || maxA < minB )
		{
		
		}
	}
	
	return CollisionResult( true, result, axes[0] );
}
