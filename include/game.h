#ifndef __GAME_H__
#define __GAME_H__

#include "SDL2/SDL.h"

#include "geo2d.h"

enum GAME_MODE { GAME_MODE_NONE, GAME_MODE_MENU, GAME_MODE_LOAD, GAME_MODE_PLAY, GAME_MODE_QUIT };

class Game
{
	public:
		bool debug;
		GAME_MODE mode;
		
		bool finished;
		bool fps_cap;
		bool fullscreen;

		SDL_Window* window;
		Rect display;

		SDL_Event event;

		Game( void );
		virtual bool initSDL( char* title );
		virtual bool initGL( void );

		virtual void resize( int w, int h );
		virtual void toggle_fullscreen();
		
		virtual void quit( void );
};

#endif