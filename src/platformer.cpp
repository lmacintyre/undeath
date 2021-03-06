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
/// Improve text rendering?
///		-text boxes
///		-loop w/ delay (costly)
//
//
/// More enemy interaction
//		-Enemy attacks
//
/// Improve control zones?
//
////

#define GL_GLEXT_PROTOTYPES

//// including SDL headers
//
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_opengl.h"
#include "SDL2/SDL_opengl_glext.h"

//// including opengl headers
//
#include "GL/gl.h"

//// including my headers :o)
//
#include "game.h"
#include "texture.h"
#include "textsurface.h"
#include "animation.h"
#include "vec2d.h"
#include "col2d.h"
#include "geo2d.h"
#include "menu.h"

#include "display.h"
#include "event.h"
#include "block.h"
#include "player.h"
#include "enemy.h"
#include "level.h"
#include "levelbg.h"

//// utilities from std
//
#include <stdio.h>
#include <vector>

using std::vector;

typedef void (MyGame::*GameFunction)( void );

class MyGame: public Game
{
	private:
		vector<Actor*> players;
		vector<Actor*> enemies;
		
		vector<Block> platforms;
		vector<Block> walls;
		vector<Block> ground_set;
		
		SDL_Surface* load_surface;
		SDL_Surface* blit_surface;

		TTF_Font* font;

		Uint32 last_tick;
		long delta_t;
		
		//The level object... it's happening...
		Level level0;
		LevelBG background;

		//And this into some kind of HUD?
		Texture* hp_tex = NULL;

		//My GOD this is so temporary, build a lighting engine!
		GLuint lightmap_fbo;
		Texture* lightsrc_tex = NULL;

		Menu main_menu;
		
	public:
		MyGame( void ) {};
		
		bool init( void );
		bool initSDL( void ) { return Game::initSDL( "Undeath" ); }
		bool initGL( void );

		void load_textures();
		
		void start_loop( void );

		void menu_action( void );

		void render( void );
		void render_hud( Player* p );

		void poll_input( bool* keys );
		void logic( long d );

		void soft_quit( void );
		void quit( void );
};

bool MyGame::initGL( void )
{
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glMatrixMode( GL_PROJECTION );

	float perspective_width, perspective_height;

	if( WINDOW_WIDTH > WINDOW_HEIGHT )
	{
		perspective_width = (float) WINDOW_WIDTH / WINDOW_HEIGHT;
		perspective_height = 1.f;
	} else
	{
		perspective_height = (float) WINDOW_HEIGHT / WINDOW_WIDTH;
		perspective_width = 1.f;
	}

	glGenBuffers(1, &lightmap_fbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightmap_fbo);

	glOrtho( -1* perspective_width, perspective_width,
			 -1* perspective_height, perspective_height,
			 0.f, 1.f );

	display = Rect( Vec2d( -1* perspective_width, -1* perspective_height ), perspective_width * 2, perspective_height * 2 );

	glMatrixMode( GL_MODELVIEW );
	
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	return true;
}

bool MyGame::init( void )
{
	mode = GAME_MODE_LOAD;

	if( debug ) printf( "Init SDL..." );
	if( !initSDL() ) return false;
	if( debug ) printf( "successful.\nInit opengl..." );
	if( !initGL() ) return false;
	if( debug ) printf( "successful.\n" );


	if( debug) printf( "Load font..." );
	font = TTF_OpenFont( "res/Xanadu.ttf", 50 );
	printf( "Successful\n" );

	main_menu.set_font( font );
	main_menu.add_item( MenuItem( "toggle fullscreen", font, &MyGame::toggle_fullscreen ) );
	main_menu.add_item( MenuItem( "main menu", font, &MyGame::soft_quit ) );
	main_menu.add_item( MenuItem( "exit", font, &MyGame::soft_quit ) );
	
	players.push_back( new Player( Vec2d( 0.f, 0.f ) ) );
	
	//enemies.push_back( new EnemySkeleton( Vec2d( 0.f, 2.f ) ) );
	EnemyPaladin* ep = new EnemyPaladin( Vec2d( 0.f, 2.8f ) );
	printf("Returned. OBJ position is (%f,\t%f)\n", ep->get_position().get_a(), ep->get_position().get_b());
	enemies.push_back( (Actor*) ep );
	printf("Returned. Enemy position is (%f,\t%f)\n", enemies[0]->get_position().get_a(), enemies[0]->get_position().get_b());
	

	vector<directive> bounds;
	//Bottom middle
	bounds.push_back( DIRECTIVE_WALK_RIGHT );bounds.push_back( DIRECTIVE_WALK_LEFT );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_LARGE, Vec2d( 0.f, -0.5f ), 1, bounds ) );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_MEDIUM, Vec2d( 0.f, -1.f ), 0, bounds ) );

	//Left
	bounds.clear();
	bounds.push_back( DIRECTIVE_NONE ); bounds.push_back( DIRECTIVE_JUMP );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_LARGE, Vec2d( -1.f, 0.f ), 2, bounds ) );

	bounds.clear();
	bounds.push_back( DIRECTIVE_WALK_RIGHT ); bounds.push_back( DIRECTIVE_NONE );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_LARGE, Vec2d( -2.f, 0.f ), 1, bounds ) );

	//Right
	bounds.clear();
	bounds.push_back( DIRECTIVE_JUMP ); bounds.push_back( DIRECTIVE_NONE );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_LARGE, Vec2d( 1.f, 0.f ), 3, bounds ) );

	bounds.clear();
	bounds.push_back( DIRECTIVE_NONE ); bounds.push_back( DIRECTIVE_WALK_LEFT );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_LARGE, Vec2d( 2.f, 0.f ), 1, bounds ) );

	//Top middle
	bounds.clear();
	bounds.push_back( DIRECTIVE_NONE );bounds.push_back( DIRECTIVE_NONE );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_MEDIUM, Vec2d( 0.f, 0.5f ), 0, bounds ) );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_SMALL, Vec2d( 0.f, 1.f ), 0, bounds ) );

	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_SMALL, Vec2d( 3.f, 0.f ), 0, bounds ) );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_SMALL, Vec2d( 4.f, 0.f ), 0, bounds ) );
	platforms.push_back( Block( BLOCK_TYPE_PLATFORM_SMALL, Vec2d( 5.f, 0.f ), 0, bounds ) );

	//TODO -- build level class
	background = LevelBG( NULL, NULL, NULL );

	if( debug ) printf( "load textures...\n" );
	load_textures();	
	if( debug ) printf( "successful.\n" );

	//Build ground set
	ground_set.insert( ground_set.begin(), platforms.begin(), platforms.end() );
	ground_set.insert( ground_set.end(), walls.begin(), walls.end() );

	last_tick = SDL_GetTicks();
	fps_cap = false;
}

void MyGame::load_textures()
{
	if( debug ) printf( "LOAD PLAYER\n" );
	load_surface = IMG_Load( "res/skele_sheet.png" ); 
	players[0]->sheet = new Texture();
	players[0]->sheet->load( (GLuint*)load_surface->pixels, load_surface->w, load_surface->h, GL_RGBA );

	if( debug ) printf( "LOAD ENEMIES\n" );
	Texture* enemy_skel_tex = new Texture();
	enemy_skel_tex->load( (GLuint*)load_surface->pixels, load_surface->w, load_surface->h, GL_RGBA );
	//enemies[0]->sheet = enemy_skel_tex;

	Texture* enemy_paladin_tex = new Texture;
	load_surface = IMG_Load( "res/paladin_sheet.png" );
	enemy_paladin_tex->load( (GLuint*)load_surface->pixels, load_surface->w, load_surface->h, GL_RGBA );
	enemies[0]->sheet = enemy_paladin_tex;
	

	if( debug ) printf( "LOAD PLATFORM\n" );
	Texture* plat_sheet = new Texture();
	load_surface = IMG_Load( "res/plat_sheet.png" );
	plat_sheet->load( (GLuint*) load_surface->pixels, load_surface->w, load_surface->h, GL_RGBA );
	for( int i=0; i<platforms.size(); i++ ) platforms[i].sheet = plat_sheet;

	if( debug ) printf( "LOAD WALL\n" );
	Texture* wall_sheet = new Texture();
	load_surface = IMG_Load( "res/wall_sheet.png" );
	wall_sheet->load( (GLuint*) load_surface->pixels, load_surface->w, load_surface->h, GL_RGBA );
	for( int i=0; i<walls.size(); i++ ) walls[i].sheet = wall_sheet;

	if( debug ) printf( "LOAD HP\n" );
	load_surface = IMG_Load( "res/skele_head.png" ); 
	hp_tex = new Texture();
	hp_tex->load( (GLuint*) load_surface->pixels, load_surface->w, load_surface->h, GL_RGBA );

	if( debug ) printf( "LOAD LIGHTSRC\n" );
	load_surface = SDL_LoadBMP( "res/lightsrc.bmp" );
	lightsrc_tex = new Texture();
	lightsrc_tex->load( (GLuint*) load_surface->pixels, load_surface->w, load_surface->h, GL_LUMINANCE );
	SDL_FreeSurface( load_surface );

	//TODO -- Again, level object
	//background.ground_texture = wall_sheet;
	background.level_texture = wall_sheet;
	background.ground_texture = wall_sheet;

	if( debug ) printf( "DONE\n" );
}

void MyGame::start_loop( void )
{
	mode = GAME_MODE_PLAY;
	while( !finished )
	{
		poll_input( ((Player*) players[0])->keys_down );
		
		delta_t = SDL_GetTicks() - last_tick;
		last_tick = SDL_GetTicks();
		
		if( delta_t < 1000 ) logic( delta_t );

		render();
		SDL_GL_SwapWindow( window );
	} quit();
}

void MyGame::menu_action( void )
{
	printf("Action!\n");
}

void MyGame::render( void )
{
	glClear( GL_COLOR_BUFFER_BIT );
	
	glPushMatrix();
	
	if( mode == GAME_MODE_MENU ) glColor3f( 0.3f, 0.3f, 0.3f );

	background.render( display, players[0]->get_position() );

	glTranslatef( players[0]->get_position().get_a() * -1, players[0]->get_position().get_b() * -1, 0.f );

	//Render platforms	
	glColor3f( 1.f, 1.f, 1.f );
	for( int i=0; i<ground_set.size(); i++ ) ground_set[i].render();

	//Render enemies
	for( int i=0; i<enemies.size(); i++ )
	{
		enemies[i]->render();
	}

	glPopMatrix();
	glPushMatrix();

	glColor3f( 1.f, 1.f, 1.f );
	players[0]->render();	

	glPopMatrix();
	
	/*
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	lightsrc_tex->render( Rect( Vec2d(0.f, 0.f), 1.f, 1.f ), display );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	*/

	if( mode == GAME_MODE_MENU )
	{
		glColor3f( 1.f, 1.f, 1.f );
		main_menu.render( Vec2d( -1*display.w*0.5, display.h*0.5 ) );
	}

	render_hud( (Player*) players[0] );
}

void MyGame::render_hud( Player* p )
{
	//render health blocks
	glPushMatrix();

	Rect clip( Vec2d( 0.f, 0.f), 1.f, 1.f );

	for( int i=0; i<p->maxhp; i++ )
	{
		if( i < p->hp ) glColor3f( 0.f, 1.f, 0.f );
		else glColor3f( 1.f, 0.f, 0.f );
	
		float start_x = display.vertices[0].get_a(), start_y = display.vertices[0].get_b()*-1;
		float hp_size = display.w / 15;
		float margin =  display.w / 20;
		float spacing = 0.f;

		Rect where( Vec2d( start_x + margin + (i*spacing) + (i*hp_size), start_y - hp_size - margin ), hp_size, hp_size );

		hp_tex->render( clip, where );
	}

	glPopMatrix();
}

//TODO -- Move into event file? Game class?
void MyGame::poll_input( bool* keys )
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
				if( mode == GAME_MODE_PLAY ) mode = GAME_MODE_MENU;
				else mode = GAME_MODE_PLAY;
				break;

				case SDLK_a:
				keys[KEY_a] = true;
				break;

				case SDLK_h:
				for( int i=0; i<players.size(); i++ ) players[i]->render_hitbox = !players[i]->render_hitbox;
				for( int i=0; i<enemies.size(); i++ ) enemies[i]->render_hitbox = !enemies[i]->render_hitbox;
				for( int i=0; i<ground_set.size(); i++ ) ground_set[i].render_hitbox = !ground_set[i].render_hitbox;
				break;

				case SDLK_f:
				this->toggle_fullscreen();
				break;

				case SDLK_q:
				soft_quit();
				break;

				case SDLK_s:
				keys[KEY_s] = true;
				break;

				case SDLK_x:
				keys[KEY_x] = true;
				break;

				case SDLK_z:
				keys[KEY_z] = true;
				break;

				case SDLK_UP:
				if( mode == GAME_MODE_MENU ) main_menu.prev();
				keys[KEY_UP] = true;
				break;

				case SDLK_LEFT:
				keys[KEY_LEFT] = true;
				break;

				case SDLK_DOWN:
				if( mode == GAME_MODE_MENU ) main_menu.next();
				keys[KEY_DOWN] = true;
				break;
				
				case SDLK_RIGHT:
				keys[KEY_RIGHT] = true;
				break;

				case SDLK_SPACE:
				keys[KEY_SPACE] = true;
				break;

				case SDLK_RETURN:
				if( mode == GAME_MODE_MENU ) main_menu.act( this );
				keys[KEY_RETURN] = true;
				break;

			} break;

			case SDL_KEYUP:
			switch( event.key.keysym.sym )
			{
				case SDLK_a:
				keys[KEY_a] = false;
				break;

				case SDLK_s:
				keys[KEY_s] = false;
				break;

				case SDLK_z:
				keys[KEY_z] = false;
				break;

				case SDLK_x:
				keys[KEY_x] = false;
				break;

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

				case SDLK_RETURN:
				keys[KEY_RETURN] = false;
				break;
			} break;
		}
	}
}

void MyGame::logic( long d )
{
	float dt = (float) d / 100;

	switch( mode )
	{	
		case GAME_MODE_PLAY: //================================

		for( int i=0; i<players.size(); i++ )
			players[i]->update( ground_set, enemies, dt );
		
		for( int i=0; i<enemies.size(); i++ )
			enemies[i]->update( ground_set, players, dt );

		break;



		case GAME_MODE_MENU: //================================
		/*if( ((Player*) players[0])->keys_down[KEY_RETURN] )
		{
			main_menu.act( this );
			(this->*(main_menu.items[0].action))();
			((Player*) players[0])->keys_down[KEY_RETURN] = false;
		}*/
		break;
	}
}

void MyGame::soft_quit( void )
{
	finished = true;
}

void MyGame::quit( void )
{
	Game::quit();

	for( int i=0; i<players.size(); i++ ) delete players[i];
	for( int i=0; i<enemies.size(); i++ ) delete enemies[i];
}

int main( int argc, char* argv[] )
{
	MyGame game;
	if( game.debug ) printf("Game created. Initializing...\n");
	if( game.init() )
	{
		if( game.debug ) printf("Initialization successful. Starting game loop.\n");
		game.start_loop();
	} else printf( "init failed...?\n" );
}
