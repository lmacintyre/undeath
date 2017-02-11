#ifndef __MENU_H__
#define __MENU_H__

#include "vec2d.h"
#include "geo2d.h"
#include "textsurface.h"

class MenuItem
{
	private:
		char* text;
		TextSurface texture;

		void (*action)( void );

	public:
		bool selected = false;

		MenuItem( void );
		MenuItem( char* text, TTF_Font* font, void(*action)( void ) );

		void render( Vec2d where );
		void render( Rect where );
};

class Menu
{
	private:
		vector<MenuItem> items;
		int selection;
		TTF_Font* font;
		
	public:
		Menu( void );
		Menu( vector<MenuItem> items );
		
		void set_font( TTF_Font* font );
		void add_item( MenuItem item );
		void add_item( char* text, void(*menu_action)( void ) );
		
		void render( Vec2d where );
};

#endif
