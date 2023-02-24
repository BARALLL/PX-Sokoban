#ifndef TILE_H
#define TILE_H
#include "Tile.h"
#endif

typedef struct mapNode {
	
	unsigned int VBOHandle;
	tile tile;
	struct mapNode *next;

} mapNode;

typedef struct mapCol{
	
	struct mapCol *next;
	mapNode *node;
	

} mapCol;



typedef struct {
	
	int width;
	int height;
	mapCol *map; 
} tileMap;


void freeTileMap(tileMap *tm);
tileMap newTileMap(int width, int height);
unsigned int *getTileVBOHandle(tileMap tm, int x, int y);
tile *getTile(tileMap tm, int x, int y);
