#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "display.h"

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

Game::Game( void )
{
	debug = false;

	finished = false;
	window = NULL;

	display = Rect( Vec2d( -1.f, -1.f ), 2.f, 2.f);
}

bool Game::initSDL( char* title )
{
	SDL_Init( SDL_INIT_VIDEO );

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	
	window =
	SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
												WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL );

	if( window == NULL )
	{
		printf( "Could not create window: %s", SDL_GetError() );
		return false;
	}
	
	SDL_GL_CreateContext( window );
	SDL_GL_SetSwapInterval( 1 );
	
	return true;
}

bool Game::initGL( void )
{

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	return true;
}


void Game::quit( void )
{
	SDL_DestroyWindow( window );
	SDL_Quit();
}