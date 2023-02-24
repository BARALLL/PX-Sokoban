#include "levelEditor.h"
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>

char* levelFilePath;
int level;


static void arrowPress(enum direction d, tileMap *tm, bool gui){
	int tileX;
	int tileY;
	
	getCursor(&tileX,&tileY);

	switch (d){
		case UP :
			if(tileY > 0){
				tileY--;
				if(!gui) printf("\033[1A");
			}
			break;

		case DOWN :
			if(tileY + 1 < tm->height){ 
				tileY++;
				if(!gui) printf("\033[1B");
			}
			break;

		case RIGHT :
			if(tileX + 1 < tm->width){
				tileX++;
				if(!gui) printf("\033[1C");
			}
			break;

		case LEFT :
			if(tileX > 0){
				tileX--;
				if(!gui) printf("\033[1D");
			}
			break;
	}
	if(!gui) fflush(stdout);
	setCursor(tileX,tileY);
}


static void closeLevelEditor(tileMap *tm){
	int tileX;
	int tileY;
	
	getCursor(&tileX,&tileY);
	printf("\033[%dB", tileY);
	printf("\033[%dD", tileX);
	freeTileMap(tm);
	free(tm);
	flushInputBuffer();
	exit(0);
}




static void setWall(tileMap *tm, bool gui){
	int tileX;
	int tileY;
	
	getCursor(&tileX,&tileY);
	tile wall = {false, false, false};
	*getTile(*tm, tileX, tileY) = wall;
	if(!gui){
		printf("#\033[1D");
		fflush(stdout);
	}
	else updateGUIMap(*tm);
}

static void setCrate(tileMap *tm, bool gui){
	int tileX;
	int tileY;
	
	getCursor(&tileX,&tileY);
	tile crate = {true, true, false};
	*getTile(*tm, tileX, tileY) = crate;
	if(!gui){
		printf("$\033[1D");
		fflush(stdout);

	}
	else updateGUIMap(*tm);
}

static void setTarget(tileMap *tm, bool gui){
	int tileX;
	int tileY;
	
	getCursor(&tileX,&tileY);
	tile target = {true, false, true};
	*getTile(*tm, tileX, tileY) = target;
	if(!gui){
		printf(".\033[1D");
		fflush(stdout);
	}	
	else updateGUIMap(*tm);
}


static void setEmpty(tileMap *tm, bool gui){
	int tileX;
	int tileY;
	
	getCursor(&tileX,&tileY);
	tile floor = {true, false, false};
	*getTile(*tm, tileX, tileY) = floor;
	if(!gui){
		printf(" \033[1D");
		fflush(stdout);
	}
	else updateGUIMap(*tm);
}


static void setSpawn(tileMap *tm, bool gui){
	int tileX;
	int tileY;
	
	getCursor(&tileX,&tileY);
	setEmpty(tm,gui);
	if(!gui){
		printf("@\033[1D");
		fflush(stdout);
	}
	else updateGUIMap(*tm);
}

static void resetCursor(tileMap *tm, bool reset){ 
	/**
	 *resets the cursor to 0,0 or tilePos depending on reset
	 * */
	int tileX;
	int tileY;
	
	getCursor(&tileX,&tileY);
	if(reset){
		printf("\033[%dB", tm->height - tileY);		
		printf("\033[%dD", tileX);		
	}
	else{
		printf("\033[%dA", tm->height - tileY);		
		printf("\033[%dC", tileX);		
	}
}

static void saveLevel(tileMap *tm, int level, char *filePath){
	
	resetCursor(tm, true);
	printf("\033[1B");
	fflush(stdout);
	FILE *f = fopen(filePath, "r+");
	if(!f){
		fprintf(stderr, "coulnd't open file");
		return;
	}
	
	int er = seekLevelAscii(level, f);
	if(er == -1) {
		printf("creating new level save...\n");
		fseek(f,0,SEEK_END);
		char *buffer = malloc(sizeof(char)*20);
		sprintf(buffer, ";LEVEL %d", level);
		fwrite(buffer, 1,sizeof(buffer), f);
		fputs("\n", f);
		free(buffer);
		char mark[] = ";Level created by level editor\n";
		fwrite(mark,1,sizeof(mark), f);
	};
	for(int y = 0; y < tm->height;y++){
		for(int x = 0; x < tm->width; x++){
			char c[] = {tileToChar(*getTile(*tm, x, y))};
			fwrite(c, 1,sizeof(char), f);
		}
		fputs("\n",f);
	}
	fclose(f);
    	time_t t;
    	t = time(NULL);
    	struct tm tim;
		tim = *localtime(&t);
    	printf("Saved at : %d:%d:%d\n", tim.tm_hour, tim.tm_min, tim.tm_sec);
	printf("\033[2A");
	resetCursor(tm,false);
	fflush(stdout);
	closeLevelEditor(tm);
}


static void mainLoop(tileMap *tm, bool gui){
	if(!gui) registerTerminal();
	else renderGUI();
	int key = getInput();
	if(key >= 0 && key < 4) arrowPress(key, tm, gui);
	if(key == 113) closeLevelEditor(tm);
	if(key == 119) setWall(tm,gui);
	if(key == 99) setCrate(tm,gui);
	if(key == 116) setTarget(tm,gui);
	if(key == 115) setSpawn(tm,gui);
	if(key == 120) setEmpty(tm,gui);
	if(key == 10 || key == 83 || key == 115) saveLevel(tm,level, levelFilePath);
}

void openLevelEditor(int _level, char *filePath, bool gui){
	/**
	 *Opens level editor on selected level at filepath, will create a new one if level is not found
	 *@param[in] _level
	 *@param[in] filePath
	 *@param[in] width will be filled
	 *@param[in] height will be filled
	 * */

	//loadMap ----------------
	
	level = _level;
	levelFilePath = filePath;
	FILE *f = fopen(filePath, "r");
	if(!f){
		printf("file does not exist.\n");
		return;
	}
	
	tileMap *tm = malloc(sizeof(tileMap));

	if(seekLevelAscii(level, f) != -1)
		*tm = getLevelMap(level, f);
	else
		*tm = newTileMap(50,50);
	
	fclose(f);
		
	if(!gui){
		show(*tm);
		printf("\033[1A"); //get cursor on map
		fflush(stdout);
		configureTerminal();
	}
	else{
		if(initGUI() == -1) return ;
                if(updateGUIMap(*tm) == -1) return ;
	}

	setCursor(0,tm->height-1);
	while(1) mainLoop(tm, gui);
}

