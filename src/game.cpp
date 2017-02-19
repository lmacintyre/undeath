#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "SDL2/SDL_ttf.h"

#include "display.h"

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

Game::Game( void )
{
	debug = true;
	mode = GAME_MODE_NONE;

	finished = false;
	window = NULL;

	display = Rect( Vec2d( -1.f, -1.f ), 2.f, 2.f);
}

bool Game::initSDL( char* title )
{
	SDL_Init( SDL_INIT_VIDEO );

	if( TTF_Init() == -1 )
	{
		printf( "TTF encountered an error and could not initialize:\n%s", TTF_GetError() );
		return false;
	}

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	
	window =
	SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
												WINDOW_WIDTH, WINDOW_HEIGHT,
												SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );

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

void Game::resize( int w, int h )
{
	SDL_SetWindowSize( this->window, w, h );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glViewport(0, 0, w, h);

	float perspective_width, perspective_height;

	if( w > h )
	{
		perspective_width = (float) w/h;
		perspective_height = 1.f;
	} else
	{
		perspective_height = (float) h/w;
		perspective_width = 1.f;
	}

	glOrtho( -1* perspective_width, perspective_width,
			 -1* perspective_height, perspective_height,
			 0.f, 1.f );

	display = Rect( Vec2d( -1* perspective_width, -1* perspective_height ), perspective_width * 2, perspective_height * 2 );

}

void Game::toggle_fullscreen()
{

	if( fullscreen )
	{
		fullscreen = false;

		SDL_SetWindowFullscreen( window, 0 );

		resize( WINDOW_WIDTH, WINDOW_HEIGHT );

	} else {

		fullscreen = true;

		SDL_SetWindowFullscreen( window, SDL_WINDOW_FULLSCREEN );

		SDL_DisplayMode dm;
		SDL_GetDesktopDisplayMode( 0, &dm );

		resize( dm.w, dm.h );
	}
}

void Game::quit( void )
{
	SDL_DestroyWindow( window );
	TTF_Quit();
	SDL_Quit();
}