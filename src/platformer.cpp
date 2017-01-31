////
//
// A Game in progress
//
/// Code - Liam MacIntyre
/// Art - Ray Nalzaro
//
// Todo:
//
/// Menu
/// Text
//
/// Enemy interaction
//
////

//// including SDL headers
//
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_opengl.h"

//// including opengl headers
//
#include "GL/gl.h"

//// including my headers :o)
//
#include "texture.h"
#include "animation.h"
#include "vec2d.h"
#include "col2d.h"
#include "geo2d.h"

#include "display.h"
#include "event.h"
#include "block.h"
#include "player.h"
#include "enemy.h"
#include "levelbg.h"

//// utilities from std
//
#include <stdio.h>
#include <vector>

using std::vector;

const int FPS = 60;	//unused as of now

class Game
{
	private:
		bool finished;

		vector<Actor*> players;
		vector<Actor*> enemies;
		
		vector<Block> platforms;
		vector<Block> walls;
		vector<Block> ground_set;
		
		SDL_Surface* load_surface;
		SDL_Surface* blit_surface;
		
		float perspective_width, perspective_height;
		Rect screen_rect;

		bool fps_cap;
		Uint32 last_tick;
		long delta_t;
		
		SDL_Window* window;
		SDL_Event event;
		
		//Temporary! Put this into some kind of level object!
		LevelBG background;
		
	public:
		Game( void );
		
		bool init( void );
		bool initSDL( void );
		bool initGL( void );

		void load_textures();
		
		void start_loop( void );

		void render( void );
		void render_hud( Player* p );

		void poll_input( bool* keys );
		void logic( long d );
		void quit( void );
};

Game::Game( void )
{
	window = NULL;
	
	finished = false;
}

bool Game::initSDL( void )
{
	SDL_Init( SDL_INIT_VIDEO );
	
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	
	window =
	SDL_CreateWindow( "SKELETON'S DAY OUT", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

	if( WINDOW_WIDTH > WINDOW_HEIGHT )
	{
		perspective_width = (float) WINDOW_WIDTH / WINDOW_HEIGHT;
		perspective_height = 1.f;
	} else
	{
		perspective_height = (float) WINDOW_HEIGHT / WINDOW_WIDTH;
		perspective_width = 1.f;
	}

	glOrtho( -1* perspective_width, perspective_width,
			 -1* perspective_height, perspective_height,
			 0.f, 1.f );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	return true;
}

bool Game::init( void )
{
	printf( "Init SDL..." );
	if( !initSDL() ) return false;
	printf( "successful.\nInit opengl..." );
	if( !initGL() ) return false;
	printf( "successful.\n" );

	screen_rect = Rect( Vec2d( -1* perspective_width, -1* perspective_height ),
												2* perspective_width, 2* perspective_height );

	players.push_back( new Player( Vec2d( 0.f, 0.f ) ) );
	enemies.push_back( new EnemySkeleton( Vec2d( 1.f, 1.f ) ) );

	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_LARGE, Vec2d( 0.f, -0.5f ), 1 ) );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_LARGE, Vec2d( -1.f, 0.f ), 2 ) );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_LARGE, Vec2d( 1.f, 0.f ), 3 ) );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_MEDIUM, Vec2d( 0.f, 0.5f ), 0 ) );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_SMALL, Vec2d( 0.f, 1.f ), 0 ) );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_MEDIUM, Vec2d( 0.f, -1.f ), 0 ) );
/*
	walls.push_back( Block( BLOCK_TYPE_WALL, Vec2d( -1.f, -2.f ), 0 ) );
	walls.push_back( Block( BLOCK_TYPE_WALL, Vec2d( 0.f, -2.f ), 0 ) );
	walls.push_back( Block( BLOCK_TYPE_WALL, Vec2d( 1.f, -2.f ), 0 ) );
	walls.push_back( Block( BLOCK_TYPE_WALL, Vec2d( -2.f, -2.f ), 0 ) );
	walls.push_back( Block( BLOCK_TYPE_WALL, Vec2d( -2.f, -1.f ), 0 ) );
	walls.push_back( Block( BLOCK_TYPE_WALL, Vec2d(  2.f, -2.f ), 0 ) );
	walls.push_back( Block( BLOCK_TYPE_WALL, Vec2d(  2.f, -1.f ), 0 ) );
	walls.push_back( Block( BLOCK_TYPE_WALL, Vec2d(  2.f,  0.f ), 0 ) );
*/
	//TODO -- build level class
	background = LevelBG( NULL, NULL, NULL );

	printf( "load textures...\n" );
	load_textures();	
	printf( "successful.\n" );

	//Build ground set
	ground_set.insert( ground_set.begin(), platforms.begin(), platforms.end() );
	ground_set.insert( ground_set.end(), walls.begin(), walls.end() );

	last_tick = SDL_GetTicks();
	fps_cap = false;
}

void Game::load_textures()
{
	printf( "LOAD PLAYER\n" );
	load_surface = IMG_Load( "res/skele_sheet.png" ); 
	players[0]->sheet = new Texture();
	players[0]->sheet->load( (GLuint*)load_surface->pixels, load_surface->w, load_surface->h );
	
	printf( "LOAD ENEMY\n" );
	enemies[0]->sheet = new Texture();
	enemies[0]->sheet->load( (GLuint*)load_surface->pixels, load_surface->w, load_surface->h );

	printf( "LOAD PLATFORM\n" );
	Texture* plat_sheet = new Texture();
	load_surface = IMG_Load( "res/plat_sheet.png" );
	plat_sheet->load( (GLuint*) load_surface->pixels, load_surface->w, load_surface->h );
	for( int i=0; i<platforms.size(); i++ ) platforms[i].sheet = plat_sheet;
	
	printf( "LOAD WALL\n" );
	Texture* wall_sheet = new Texture();
	load_surface = IMG_Load( "res/wall_sheet.png" );
	wall_sheet->load( (GLuint*) load_surface->pixels, load_surface->w, load_surface->h );
	for( int i=0; i<walls.size(); i++ ) walls[i].sheet = wall_sheet;

	//TODO -- Again, level object
	background.ground_texture = wall_sheet;

	printf( "DONE\n" );
}

void Game::start_loop( void )
{
	while( !finished )
	{
		poll_input( ((Player*) players[0])->keys_down );
		
		delta_t = SDL_GetTicks() - last_tick;
		last_tick = SDL_GetTicks();
			
		logic( delta_t );				
		render();
		SDL_GL_SwapWindow( window );
	} quit();
}

void Game::render( void )
{
	glClear( GL_COLOR_BUFFER_BIT );
	
	glPushMatrix();
	
	/*
	//Render a simple background
	glBegin( GL_QUADS );
		glColor3f( 0.f, 0.f, 0.f );
		glVertex2f( -1 * perspective_width, -1 * perspective_height );
		glVertex2f(      perspective_width, -1 * perspective_height );
		
		glColor3f( 0.5f, 0.f, 0.f );
		glVertex2f(      perspective_width,      perspective_height );
		glVertex2f( -1 * perspective_width,      perspective_height );
	glEnd();
	*/
	
	background.render( screen_rect, players[0]->get_position() );

	glTranslatef( players[0]->get_position().get_a() * -1, players[0]->get_position().get_b() * -1, 0.f );

	//Render platforms	
	glColor3f( 1.f, 1.f, 1.f );
	for( int i=0; i<ground_set.size(); i++ ) ground_set [i].render();

	//Render enemies
	glColor3f( 0.8f, 0.2f, 0.2f );

	for( int i=0; i<enemies.size(); i++ )
		enemies[i]->render();

	glPushMatrix();
	glLoadIdentity();
	glColor3f( 1.f, 1.f, 1.f );
	players[0]->render();
	glPopMatrix();
	glPopMatrix();
	
	render_hud( (Player*) players[0] );
}

void Game::render_hud( Player* p )
{
	//render health blocks
	glPushMatrix();
	glLoadIdentity();
	for( int i=0; i<p->maxhp; i++ )
	{
		if( i < p->hp ) glColor3f( 0.f, 1.f, 0.f );
		else glColor3f( 1.f, 0.f, 0.f );
	
		float start_x = -1 * perspective_width, start_y = perspective_height;
		float hp_size = perspective_width / 15;
		float margin = perspective_width / 15;
		float spacing = hp_size / 3;
		
		glBegin( GL_QUADS );
			glVertex2f( start_x + margin + spacing*i + hp_size*i    , start_y - margin           );
			glVertex2f( start_x + margin + spacing*i + hp_size*(i+1), start_y - margin           );
			glVertex2f( start_x + margin + spacing*i + hp_size*(i+1), start_y - margin - hp_size );
			glVertex2f( start_x + margin + spacing*i + hp_size*i    , start_y - margin - hp_size );
		glEnd();
	}
	glPopMatrix();
}

//TODO -- Move into event file?
void Game::poll_input( bool* keys )
{
	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_QUIT:
			finished = true;
			break;

			case SDL_KEYDOWN:
			switch( event.key.keysym.sym )
			{
				case SDLK_ESCAPE:
				finished = true;
				break;

				case SDLK_h:
				for( int i=0; i<players.size(); i++ ) players[i]->render_hitbox = !players[i]->render_hitbox;
				for( int i=0; i<enemies.size(); i++ ) enemies[i]->render_hitbox = !enemies[i]->render_hitbox;
				break;
				
				case SDLK_UP:
				keys[KEY_UP] = true;
				break;

				case SDLK_LEFT:
				keys[KEY_LEFT] = true;
				break;

				case SDLK_DOWN:
				keys[KEY_DOWN] = true;
				break;
				
				case SDLK_RIGHT:
				keys[KEY_RIGHT] = true;
				break;

				case SDLK_SPACE:
				keys[KEY_SPACE] = true;
				break;
			} break;

			case SDL_KEYUP:
			switch( event.key.keysym.sym )
			{
				case SDLK_UP:
				keys[KEY_UP] = false;
				break;

				case SDLK_LEFT:
				keys[KEY_LEFT] = false;
				break;

				case SDLK_DOWN:
				keys[KEY_DOWN] = false;
				break;
				
				case SDLK_RIGHT:
				keys[KEY_RIGHT] = false;
				break;

				case SDLK_SPACE:
				keys[KEY_SPACE] = false;
				break;
			} break;
		}
	}
}

void Game::logic( long d )
{
	float dt = (float) d / 100;
	
	for( int i=0; i<players.size(); i++ )
		players[i]->update( ground_set, enemies, dt );
	
	for( int i=0; i<enemies.size(); i++ )
		enemies[i]->update( ground_set, players, dt );
}

void Game::quit( void )
{
	SDL_DestroyWindow( window );
	SDL_Quit();
}

int main( int argc, char* argv[] )
{
	Game game;
	printf("Game created. Initializing...\n");
	if( game.init() )
	{
		printf("Initialization successful. Starting game loop.\n");
		game.start_loop();
	}
}
