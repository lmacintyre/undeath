#include "block.h"
#include "levelbg.h"

#include <vector>

class Level
{
	private:

	public:
		vector<Block> ground_set;
		LevelBG background;

		vector<Actor*> enemies;

		Level( void );
		Level( char* path );
};

Level::Level( void ) {}

Level::Level( char* path )
{
	
}