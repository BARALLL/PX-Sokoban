#ifndef TILEMAP_H
#define TILEMAP_H
#include "../public/tileMap.h"
#endif
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

tile *getTile(tileMap tm, int x, int y){
	if(x >= tm.width || x < 0 || y >= tm.height || y < 0){
		fprintf(stderr, "invalid tile coordinates\n");
		return NULL;
	}
	
	mapCol *currentCol = tm.map;
	for(int i = 0; i < x; i++) currentCol = currentCol->next;

	mapNode *currentNode = currentCol->node;
	for(int i = 0; i < y; i++) currentNode = currentNode->next;

	return &(currentNode->tile);
}

tileMap newTileMap(int width, int height){
        
        mapCol *map = malloc(sizeof(mapCol));
        
        mapCol *currentCol = map;
        //create map
        for(int x = 0; x < width; x++){
		currentCol->node = malloc(sizeof(mapNode));
        	mapNode *currentNode = currentCol->node;
                for(int y = 0; y < height; y++){
			tile t = {true, false, false};
			if(x == 0 || x == width-1 || y == 0 || y == height - 1) //border walls
					t.isWalkable = false;
			currentNode->tile = t;
                        currentNode->next = malloc(sizeof(mapCol));
                        currentNode = currentNode->next;
                }
		currentNode->next = NULL;
                currentCol->next = malloc(sizeof(mapCol));
                currentCol = currentCol->next;
        }
	currentCol->next = NULL;
        
        tileMap tm = {width, height, map};
	return tm;
}

void freeTileMap(tileMap *tm){
        mapCol *currentCol = tm->map;
	while(currentCol->next != NULL){
        	mapNode *currentNode = currentCol->node;
		while(currentNode->next != NULL){
			mapNode *toFree = currentNode;
			currentNode = currentNode->next;
			free(toFree);
		}
		free(currentNode);
		mapCol *toFree = currentCol;
		currentCol = currentCol->next;
		free(toFree);
	}
	free(currentCol);
}

