#ifndef __LEVEL_H_INCLUDED__
#define __LEVEL_H_INCLUDED__

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

		vector<Block> ground_set;
		LevelBG background;
		
		vector<Actor*> enemies;
		
		Level( void );
		Level( Texture* platform_sheet, Texture* background_sheet );

		void add_platform( Block block );
		void add_enemy( Enemy enemy );
};

#endif