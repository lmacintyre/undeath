#ifndef __MENU_H__
#define __MENU_H__

#include "vec2d.h"
#include "geo2d.h"
#include "textsurface.h"

enum MENU_CHOICE { MENU_EXIT, MENU_RETURN };

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

		float font_height( void );

		void render( Vec2d where );
		void render( Rect where );

		void (MyGame::*action)( void );
};

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
		
		void act( MyGame* game );
		void next( void );
		void prev( void );

		void render( Vec2d where );
};

#endif
