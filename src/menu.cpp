#include "SDL2/SDL_ttf.h"

#include "vec2d.h"
#include "geo2d.h"
#include "textsurface.h"

class MyGame;

class MenuItem
{
	private:
		char* text;
		TextSurface* texture;
		
	public:
		bool selected = false;
		
		MenuItem( void );
		~MenuItem( void );
		MenuItem( char* text, TTF_Font* font, void(MyGame::*action)( void ) );

		float font_height( void ) { return texture->size_on_screen(); }

		void render( Vec2d where ) { texture->render( where ); }
		void render( Rect where ) { texture->render( where ); }

		void (MyGame::*action)( void );
};

MenuItem::MenuItem( void )
{
	delete( texture );
	texture = NULL;
}

MenuItem::~MenuItem( void ) {}


MenuItem::MenuItem( char* text, TTF_Font* font, void(MyGame::*action)( void ) )
{
	this->text = text;
	this->texture = new TextSurface( text, font );
	this->action = action;
}

class Menu
{
	private:
		TTF_Font* font;
		
	public:
		vector<MenuItem> items;
		int selection;
		
		Menu( void );
		~Menu( void );
		Menu( vector<MenuItem> items );
		
		void set_font( TTF_Font* font );
		void add_item( MenuItem item );
		void add_item( char* text, void(MyGame::*menu_action)( void ) );

		void next( void );
		void prev( void );		
		void act( MyGame* game );

		void render( Vec2d where );
};

Menu::Menu( void )
{
	font = NULL;
	selection = 0;
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

void Menu::add_item( char* text, void(MyGame::*action)( void ) )
{
	items.push_back( MenuItem( text, font, action ) );
}

void Menu::next( void )
{
	selection++;
	if( selection >= items.size() ) selection = 0;
}

void Menu::prev( void )
{
	selection--;
	if( selection < 0 ) selection = items.size() - 1;
}

void Menu::act( MyGame* game )
{
	(game->*(items[selection].action))();
}

void Menu::render( Vec2d where )
{
	float font_height = items[0].font_height();

	float spacing = 0.5f;

	glColor3f( 1.f, 0.f, 0.f );
	for( int i=0; i<items.size(); i++ )
	{
		if( i == selection ) glColor3f( 1.f, 1.f, 1.f );
		items[i].render( where.add( Vec2d( 0, 0-(i+1)*font_height - (i*spacing)*font_height ) ) );
		if( i == selection ) glColor3f( 1.f, 0.f, 0.f );
	}
}
