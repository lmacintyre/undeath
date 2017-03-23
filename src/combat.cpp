#include "geo2d.h"
#include "animation.h"
#include "event.h"

enum ATTACK_HEIGHT { ATTACK_HEIGHT_HIGH, ATTACK_HEIGHT_LOW };

class Attack
{
	public:
		Rect hitbox;
		Animation* animation;

		ATTACK_HEIGHT height;

		int damage;
		int frames;
		int hit_frame;
		int spd_frame;

		Vec2d speed;
		bool speed_relative;

		KEY key = KEY_NULL;

		Attack( void );
		Attack( Rect hitbox, ATTACK_HEIGHT height, Animation* animation, int hit_frame, int spd_frame, int damage, Vec2d speed = Vec2d(0.f, 0.f), bool speed_relative = true );

		bool on_hit_frame( void );
		bool on_spd_frame( void );
};

Attack::Attack( void ) {}

Attack::Attack(  Rect hitbox, ATTACK_HEIGHT height, Animation* animation, int hit_frame, int spd_frame, int damage, Vec2d speed, bool speed_relative )
{
	this->hitbox = hitbox;
	this->height = height;
	this->animation = animation;
	this->hit_frame = hit_frame;
	this->spd_frame = spd_frame;

	this->damage = damage;

	this->speed = speed;
	this->speed_relative = speed_relative;

	frames = animation->get_framecount();
}

bool Attack::on_hit_frame( void )
{
	return animation->get_current() == hit_frame;
}

bool Attack::on_spd_frame( void )
{
	return animation->get_current() == spd_frame;
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