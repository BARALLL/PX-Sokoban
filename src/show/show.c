#ifndef SHOW_H
#define SHOW_H
#include "show.h"
#endif
#ifndef TILEMAP_H
#define TILEMAP_H
#include "../public/tileMap.h"
#endif
#ifndef PLAYERPOSITION_H
#define PLAYERPOSITION_H
#include "../playerPosition/playerPosition.h"
#endif


char tileToChar(tile t){
	   /**
	    *converts tile to char
	    *@param[in] tile tile to convert
	    *@param[out] char
	    **/
	    if (!t.isWalkable && !t.isPushable && !t.isTarget)
                return'#';
	    else if (t.isPushable && t.isTarget){
		return '*';
	    }
	    else if (t.isWalkable && !t.isPushable && !t.isTarget)
                return' ';
            else if (t.isWalkable && t.isPushable && !t.isTarget)
                return'$';
            else if (t.isTarget)
                return'.';
	    else return 'x';
}

void show(tileMap tm){
	/**
	 *Shows given tilemap
	 * */
	printf("\e[1;1H\e[2J");

	int playerPosX;
	int playerPosY;

	getPlayerPosition(&playerPosX, &playerPosY);
	for (int y = 0; y < tm.height; y++) {
        for (int x = 0; x < tm.width; x++)
        {
     		if(x == playerPosX && y == playerPosY) printf("%c", '@');
		else printf("%c", tileToChar(*getTile(tm, x,y)));    
        }
	printf("\n");
    }
}
