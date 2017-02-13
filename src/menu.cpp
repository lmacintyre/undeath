#include "SDL2/SDL_ttf.h"

#include "vec2d.h"
#include "geo2d.h"
#include "textsurface.h"

enum MENU_CHOICE { MENU_EXIT, MENU_RETURN };

class MenuItem
{
	private:
		char* text;
		TextSurface* texture;
		
	public:
		bool selected = false;
		
		MenuItem( void );
		~MenuItem( void );
		MenuItem( char* text, TTF_Font* font, void(*action)( void ) );

		void render( Vec2d where ) { texture->render( where ); }
		void render( Rect where ) { texture->render( where ); }

		void (*action)( void );
};

MenuItem::MenuItem( void )
{
	delete( texture );
	texture = NULL;
}

MenuItem::~MenuItem( void ) {}


MenuItem::MenuItem( char* text, TTF_Font* font, void(*action)( void ) )
{
	this->text = text;
	this->texture = new TextSurface();
	this->action = action;
}

class Menu
{
	private:
		vector<MenuItem> items;
		int selection;
		TTF_Font* font;
		
	public:
		Menu( void );
		~Menu( void );
		Menu( vector<MenuItem> items );
		
		void set_font( TTF_Font* font );
		void add_item( MenuItem item );
		void add_item( char* text, void(*menu_action)( void ) );
		
		void render( Vec2d where );
};

Menu::Menu( void )
{
	font = NULL;
}

Menu::~Menu( void ) {}

Menu::Menu( vector<MenuItem> items )
{
	this->items = items;
	selection = 0;
}

void Menu::set_font( TTF_Font* font )
{
	this->font = font;
}

void Menu::add_item( MenuItem item )
{
	items.push_back( item );
}

void Menu::add_item( char* text, void(*action)( void ) 
)
{
	items.push_back( MenuItem( text, font,  action ) );
}

void Menu::render( Vec2d where )
{
	for( int i=0; i<items.size(); i++ )
	{
		int font_height = TTF_FontHeight( font );
		items[i].render( where.add( Vec2d( 20, 
		20+i*font_height ) ) );
	}
}
