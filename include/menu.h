#ifndef __MENU_H__
#define __MENU_H__

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

		void render( Vec2d where );
		void render( Rect where );

		void (*action)( void );
};

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

#endif
