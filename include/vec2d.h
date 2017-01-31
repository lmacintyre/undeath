#ifndef __VEC_2D_H__
#define __VEC_2D_H__ 

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

		Vec2d add( Vec2d o );
		Vec2d multiply( Vec2d o );

		void translate( Vec2d o );
		void scale( Vec2d o );

		float length( void );
		float dot( Vec2d o );
		
		Vec2d unit( void );
		Vec2d get_normal( void );
		Vec2d proj( Vec2d o );
};

#endif
