#ifndef __LEVELBG_H__
#define __LEVELBG_H__

#include "geo2d.h"
#include "texture.h"

#include <vector>
using std::vector;

class LevelBG
{
	public:
		Texture* sky_texture;
		Rect sky_clip;
		
		Texture* ground_texture;
		Rect ground_clip;
		
		Texture* level_texture;
		vector<vector<Rect>> level_tiles;
		
		LevelBG( void );
		LevelBG( Texture* sky_texture, Texture* ground_texture, Texture* level_texture );
		LevelBG( Texture* sky_texture, Texture* ground_texture, Texture* level_texture, int M, int N );
		
		void render( Rect where, Vec2d scroll );
};

#endif
