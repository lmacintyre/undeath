#include "geo2d.h"

enum directive { DIRECTIVE_NONE, DIRECTIVE_STOP, DIRECTIVE_WALK_LEFT, DIRECTIVE_WALK_RIGHT, DIRECTIVE_JUMP };

class ControlZone
{
	public:
		Rect zone;
		directive command;

		ControlZone( void ) {};
		ControlZone( Rect zone, directive command );
};

ControlZone::ControlZone( Rect zone, directive command )
{
	this->zone = zone;
	this->command = command;
}