#ifndef __GAME_H__
#define __GAME_H__

#include "SDL2/SDL.h"

#include "geo2d.h"

class Game
{
	public:
		bool debug;

		bool finished;
		bool fps_cap;

		SDL_Window* window;
		Rect display;

		SDL_Event event;

		Game( void );
		virtual bool initSDL( char* title );
		virtual bool initGL( void );
		virtual void quit( void );
};

#endif