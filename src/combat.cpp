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

		Attack( void );
		Attack( Rect hitbox, ATTACK_HEIGHT height, Animation* animation, int damage );
};

Attack::Attack( void ) {}

Attack::Attack(  Rect hitbox, ATTACK_HEIGHT height, Animation* animation, int damage )
{
	this->hitbox = hitbox;
	this->height = height;
	this->animation = animation;
	this->damage = damage;

	frames = animation->get_framecount();
}

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

Moveset::Moveset( void ) {}

Moveset::Moveset( Attack light_punch, Attack heavy_punch, Attack light_kick, Attack heavy_kick )
{
	this->light_punch = light_punch;
	this->heavy_punch = heavy_punch;
	this->light_kick = light_kick;
	this->heavy_kick = heavy_kick;
}