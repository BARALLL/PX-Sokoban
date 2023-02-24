#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#ifndef TILEMAP_H
#define TILEMAP_H
#include "../public/tileMap.h"
#endif
#ifndef TILE_H
#define TILE_H
#include "../public/Tile.h"
#endif
#ifndef GLOBALINPUT_H
#define GLOBALINPUT_H
#include "../globalInput/globalInput.h"
#endif
#ifndef PLAYERPOSITION_H
#define PLAYERPOSITION_H
#include "../playerPosition/playerPosition.h"
#endif


typedef struct aTile {
	int x;
	int y;
	struct aTile* parent;
	int cost;
	int costDist;

} aTile;

typedef struct ListAT
{
	struct ListAT* next;
	aTile data;
} listAT;


aTile tileToATile(int x, int y);
int getDistance(aTile point1, aTile point2);
bool pathfinding(tileMap map, aTile player, aTile finish, bool isCrate, aTile *blackList, aTile *whiteList, bool registerPath);
void quicksort(listAT* tab, int first, int last);

aTile* getATile(listAT* list, int index);
listAT *addLAT(listAT** list, aTile item);
tileMap newTileMap(int width, int height);
