#ifndef __CTRLZONE_H__
#define __CTRLZONE_H__

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

#endif