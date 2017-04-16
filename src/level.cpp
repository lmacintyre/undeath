#include "block.h"
#include "levelbg.h"
#include "actor.h"
#include "enemy.h"

#include <vector>

class Level
{
	private:
		
	public:
		Texture* platform_sheet;
		Texture* background_sheet;

		vector<Block*> ground_set;
		LevelBG background;

		vector<Actor*> enemies;

		vector<ControlZone> control_zones;

		Level( void );
		Level( Texture* platform_sheet, Texture* background_sheet );

		void add_platform( Block* block );
		void add_enemy( Enemy* enemy );
		void add_zone( ControlZone zone );
};

Level::Level( void ) {}

Level::Level( Texture* platform_sheet, Texture* background_sheet )
{
	this->platform_sheet = platform_sheet;
	this->background_sheet = background_sheet;
}

void Level::add_platform( Block* block )
{
	ground_set.push_back( block );
}

void Level::add_enemy( Enemy* enemy )
{
	enemies.push_back( (Actor*) enemy );
}