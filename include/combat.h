#ifndef __COMBAT_H_DEFINED
#define __COMBAT_H_DEFINED

#include "geo2d.h"
#include "animation.h"

enum ATTACK_HEIGHT { ATTACK_HEIGHT_HIGH, ATTACK_HEIGHT_LOW };

class Attack
{
	public:
		Rect hitbox;
		Animation* animation;

		ATTACK_HEIGHT height;

		int damage;
		int frames;

		Vec2d speed;
		bool speed_relative;

		Attack( void );
		Attack( Rect hitbox, ATTACK_HEIGHT height, Animation* animation, int damage, Vec2d speed = Vec2d(0.f, 0.f), bool speed_relative = true );
};

class Moveset
{
	public:
		Attack light_punch;
		Attack heavy_punch;
		Attack light_kick;
		Attack heavy_kick;

		Moveset( void );
		Moveset( Attack light_punch, Attack heavy_punch, Attack light_kick, Attack heavy_kick );
};

#endif