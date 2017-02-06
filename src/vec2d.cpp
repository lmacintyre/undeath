#include <math.h>

class Vec2d
{
	private:
		float a, b;
	
	public:
		Vec2d( void );
		Vec2d( float a, float b );

		float get_a( void );
		float get_b( void );

		void set( float a, float b );
		void set_a( float a );
		void set_b( float b );

		void translate( Vec2d o );
		void scale( Vec2d o );

		Vec2d add( Vec2d o );
		Vec2d multiply( Vec2d o );
		
		float length( void );
		float dot( Vec2d o );

		Vec2d unit( void );
		Vec2d get_normal( void );
		Vec2d proj( Vec2d o );
};

Vec2d::Vec2d( void )
{
	this->a = 0.0;
	this->b = 0.0;
}

Vec2d::Vec2d( float a, float b )
{
	this->a = a;
	this->b = b;
}

float Vec2d::get_a( void )
{
	return a;
}

float Vec2d::get_b( void )
{
	return b;
}

void Vec2d::set( float a, float b )
{
	this->a = a;
	this->b = b;
}

void Vec2d::set_a( float a )
{
	this->a = a;
}

void Vec2d::set_b( float b )
{
	this->b = b;
}

Vec2d Vec2d::add( Vec2d o )
{
	float a = this->a + o.get_a();
	float b = this->b + o.get_b();

	return Vec2d( a, b );
}

Vec2d Vec2d::multiply( Vec2d o )
{
	float a = this->a * o.get_a();
	float b = this->b * o.get_b();

	return Vec2d( a, b );
}

void Vec2d::translate( Vec2d o )
{
	float a = this->a + o.get_a();
	float b = this->b + o.get_b();

	set( a, b );
}

void Vec2d::scale( Vec2d o )
{
	float a = this->a * o.get_a();
	float b = this->b * o.get_b();
	
	set( a, b );
}

float Vec2d::length( void )
{
	return sqrt( pow( a, 2 ) + pow( b, 2 )  );
}

float Vec2d::dot( Vec2d o )
{
	return ( a*o.a + b*o.b );
}

Vec2d Vec2d::unit( void )
{
	float an = 0.f, bn = 0.f;
	if( length() != 0.f )
	{
		an = a / length();
		bn = b / length();
	}

	return Vec2d( an, bn );
}

Vec2d Vec2d::get_normal( void )
{
	return Vec2d( this->b * -1, this->a );
}

Vec2d Vec2d::proj( Vec2d o )
{
	Vec2d r;
	float c = dot( o ) / pow( o.length(), 2 );
	r.set_a( c * o.get_a() );
	r.set_b( c * o.get_b() );
	
	return r;
}
