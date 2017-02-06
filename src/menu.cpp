
class MenuItem
{
	private:
		char* text;
		TextSurface texture;

	public:
		bool selected;

		MenuItem( void );
		MenuItem( char* text );

		void render( Vec2d where ) { texture.render( where ) }
		void render( Rect where ) { texture.render( where ) }
}

class Menu
{


}