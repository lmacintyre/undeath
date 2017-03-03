CC = g++
ENG	= bin/vec2d.o bin/geo2d.o bin/col2d.o bin/texture.o bin/game.o bin/textsurface.o bin/menu.o
GAME = bin/platformer.o bin/animation.o bin/actor.o bin/player.o bin/enemy.o bin/block.o bin/levelbg.o bin/ctrlzone.o bin/combat.o
EXEC = exec/*
LINKS = -lSDL2 -lSDL2_image -lSDL2_ttf -lGL
FLAGS	= -I include

all: $(ENG) $(GAME)
	$(CC) $(ENG) $(GAME) $(LINKS) $(FLAGS) -o exe/skeleton.out

engine: $(ENG)
	$(CC) -c $(ENG) $(LINKS) $(FLAGS)

clean-all:
	rm edit $(ENG) $(GAME)

clean-exec:
	rm edit $(EXEC)
 

bin/vec2d.o: src/vec2d.cpp include/vec2d.h
	$(CC) -c -o bin/vec2d.o src/vec2d.cpp $(LINKS) $(FLAGS)

bin/geo2d.o: src/geo2d.cpp include/geo2d.h include/vec2d.h
	$(CC) -c -o bin/geo2d.o src/geo2d.cpp $(LINKS) $(FLAGS)

bin/col2d.o: src/col2d.cpp include/col2d.h include/vec2d.h include/geo2d.h
	$(CC) -c -o bin/col2d.o src/col2d.cpp $(LINKS) $(FLAGS) 

bin/texture.o: src/texture.cpp include/texture.h include/geo2d.h
	$(CC) -c -o bin/texture.o src/texture.cpp $(LINKS) $(FLAGS)

bin/game.o: src/game.cpp include/game.h include/display.h include/geo2d.h
	$(CC) -c -o bin/game.o src/game.cpp $(LINKS) $(FLAGS)

bin/textsurface.o: src/textsurface.cpp include/textsurface.h include/vec2d.h include/geo2d.h include/texture.h
	$(CC) -c -o bin/textsurface.o src/textsurface.cpp $(LINKS) $(FLAGS)

bin/menu.o: src/menu.cpp include/menu.h include/vec2d.h include/geo2d.h include/texture.h include/textsurface.h
	$(CC) -c -o bin/menu.o src/menu.cpp $(LINKS) $(FLAGS)




bin/platformer.o: src/platformer.cpp include/display.h include/game.h include/texture.h include/animation.h include/vec2d.h include/col2d.h include/geo2d.h include/event.h include/block.h include/player.h include/enemy.h include/levelbg.h include/ctrlzone.h
	$(CC) -c -o bin/platformer.o src/platformer.cpp $(LINKS) $(FLAGS)

bin/animation.o: src/animation.cpp include/animation.h include/texture.h include/geo2d.h
	$(CC) -c -o bin/animation.o src/animation.cpp $(LINKS) $(FLAGS)

bin/actor.o: src/actor.cpp include/actor.h include/vec2d.h include/geo2d.h include/texture.h include/animation.h include/block.h
	$(CC) -c -o bin/actor.o src/actor.cpp $(LINKS) $(FLAGS)

bin/player.o: src/player.cpp include/player.h include/vec2d.h include/geo2d.h include/col2d.h include/actor.h include/texture.h include/animation.h include/block.h include/display.h include/event.h
	$(CC) -c -o bin/player.o src/player.cpp $(LINKS) $(FLAGS)

bin/enemy.o: src/enemy.cpp include/enemy.h include/vec2d.h include/event.h include/block.h include/actor.h
	$(CC) -c -o bin/enemy.o src/enemy.cpp $(LINKS) $(FLAGS)

bin/block.o: src/block.cpp include/block.h include/geo2d.h include/col2d.h include/texture.h include/ctrlzone.h
	$(CC) -c -o bin/block.o src/block.cpp $(LINKS) $(FLAGS)

bin/levelbg.o: src/levelbg.cpp include/levelbg.h include/display.h include/vec2d.h include/geo2d.h include/texture.h
	$(CC) -c -o bin/levelbg.o src/levelbg.cpp $(LINKS) $(FLAGS)

bin/ctrlzone.o: src/ctrlzone.cpp include/ctrlzone.h include/geo2d.h
	$(CC) -c -o bin/ctrlzone.o src/ctrlzone.cpp $(LINKS) $(FLAGS)

bin/combat.o: src/combat.cpp include/combat.h include/geo2d.h include/animation.h
	$(CC) -c -o bin/combat.o src/combat.cpp $(LINKS) $(FLAGS)
