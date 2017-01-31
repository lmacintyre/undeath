CC 	= g++
ENG	= src/vec2d.cpp src/geo2d.cpp src/col2d.cpp src/texture.cpp
OBJS 	= src/platformer.cpp src/animation.cpp src/actor.cpp src/player.cpp src/enemy.cpp src/block.cpp src/levelbg.cpp
LINKS 	= -lSDL2 -lSDL2_image -lGL
FLAGS	= -I include

stable:
	$(CC) $(ENG) $(OBJS) $(LINKS) $(FLAGS) -o exe/skeleton.out

unstable:
	$(CC) $(ENG) $(OBJS) $(LINKS) $(FLAGS) -o exe/skeleton-unstable.out
