#ifndef READLEVELS_H                            
#define READLEVELS_H 
#include "readLevels/readLevels.h"
#endif
#ifndef TILEMAP_H
#define TILEMAP_H
#include "../public/tileMap.h"
#endif
                
#include "show/show.h"
#define LEVEL 85
#define LEVELPATH "../levels.lvl"
#include "public/strCompare.h"
#ifndef GAMEDISPLAY_H
#define GAMEDISPLAY_H
#include "graphics/gameDisplay.h"
#endif
#include "levelEditor/levelEditor.h"

#ifndef IO_H
#define IO_H
#include "input_keyboard/io.h"
#endif

#ifndef PLAYERPOSITION_H
#define PLAYERPOSITION_H
#include "playerPosition/playerPosition.h"
#endif

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include "gameManager/gameManager.h"
#endif 

#include "pathFind/pathFind.h"
#include <unistd.h>

int main(int argc, char *argv[]){
	

	setPlayerPosition(1,1);
	FILE *f = fopen("customLevels.lvl", "r");
	tileMap tm = getLevelMap(2, f);
	fclose(f);

        aTile start = tileToATile(3, 3);
        aTile finish = tileToATile(6, 5);
	pathfinding(tm, start, finish, true, NULL, NULL, true);
	
	if(argc == 3 && strCompare(argv[1], "--editor")) openLevelEditor(atoi(argv[2]), "customLevels.lvl", false);
	if(argc == 3 && strCompare(argv[1], "--guieditor")) openLevelEditor(atoi(argv[2]), "customLevels.lvl", true);
	if(argc == 2 && strCompare(argv[1], "--gui")) {
		if(initGUI() != -1 && updateGUIMap(tm) != -1){
			while(1){
				input(tm, true);
				renderGUI();
				sleep(0.03);
			}
		}
	}
	
	else{
		configureTerminal(0);
		while(1){
			input(tm, false);
		}
		configureTerminal(1);
	}

	
	freeTileMap(&tm);

	return 1;
}
