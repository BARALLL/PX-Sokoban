/**
 * @file readLevels.h
 *
 * */
#ifndef READLEVELS_H
#define READLEVELS_H
#include "readLevels.h"
#endif

static struct tile charToTile(char c)
{
	struct tile t;
            if (c == '#')
                t = (struct tile){ false, false, false };
            else if (c == ' ' || c == '@')
                t = (struct tile){ true, false, false };
            else if (c == '$')
                t = (struct tile){ true, true, false };
            else if (c == '.')
                t = (struct tile){ true, false, true };
	return t;
}

static bool strIsLEVEL(char str[6]){
	/**
	 * tests if input str is equal to "LEVEL "
	 * @param[in] str size 6 string
	 * @param[out] isLEVEL
	 * */
	char target[] = "LEVEL ";
	int index = 0;
	while(str[index] == target[index]){
		index ++;
		if(index == 6) return true;
	}
	return false;
}

int seekLevelAscii(int level, FILE *levelFile){
	/**
	 * Places file cursor at requested level map
	 * @param[in] level
	 * @param[in] levelFile
	 * @param[out] success
	 * */
	fseek(levelFile, 0, SEEK_SET);
	char comment[6];
	char levelName[16];
	while(atoi(levelName) != level){
		do{
			char next = fgetc(levelFile);
			while(next != ';') {
				next = fgetc(levelFile);
				if(next == EOF){
					fprintf(stderr, "Couldn't find level...\n");
					return -1;
				}
			}
			fread(comment, 1, sizeof(char) * 6, levelFile);
		} while(!strIsLEVEL(comment));	
	
		fgets(levelName, 16, levelFile);
	}

	char commentBurn = fgetc(levelFile);
	while(commentBurn == ';'){
		char burnLine = fgetc(levelFile);
		do{
			burnLine = fgetc(levelFile);
		} while(burnLine != '\n');
		commentBurn = fgetc(levelFile);
	}

	return 0;
}

tileMap getLevelMap(int level, FILE *levelFile){
	/*gets the tileMap for a level
	 * @param[in] level
	 * @param[in] levelFile
	 * @param[out] tileMap
	 * @param[out] rows
	 * @param[out] cols
	 * */
	int er = seekLevelAscii(level, levelFile);
	if(er == -1) fprintf(stderr, "level does not exist !");
	fpos_t levelPos;
	fgetpos(levelFile,&levelPos);
	//dynamically finding size of map
	int width = 0;
	int height = 0;

	char read = fgetc(levelFile);
	while(read != ';' && !feof(levelFile))
	{
		int tempWidth = 0;
		do {
			tempWidth += 1;
			read = fgetc(levelFile);
		} while(read != '\n');
		height++;
		if(tempWidth > width) width = tempWidth;
		read = fgetc(levelFile);
	}
	
	
	tileMap tm = newTileMap(width, height);
	fsetpos(levelFile, &levelPos); //back to start of level string ! Let's read
		
	read = ' ';
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			read = fgetc(levelFile);
			if(read == '\n') {
				//fill with empty
				for(int xx = x; xx < width; xx++)
					*getTile(tm,xx,y) = charToTile(' ');
				break;
			}
			*getTile(tm, x, y) = charToTile(read);
			if(read == '@')
				setPlayerPosition(x, y);
		}
		//if reached max boundary, next char will be \n (out of map), BURN IT !
		if(read != '\n') read = fgetc(levelFile);
	}
	return tm;
}

bool testStrIsLevel(){
	char testStr1[] = "LEVEL ";
	char testStr2[] = "LEVAL ";
	return (strIsLEVEL(testStr1) && !strIsLEVEL(testStr2));
}
