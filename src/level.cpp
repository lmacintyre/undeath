#include "block.h"
#include "levelbg.h"

#include <vector>

class Level
{
	private:
		
	public:
		Texture* platform_sheet;
		Texture* background_sheet;

		vector<Block> ground_set;
		LevelBG background;

		vector<Actor*> enemies;

		Level( void );
};

Level::Level( void ) {}

Level::Level( char* path )
{
	
}