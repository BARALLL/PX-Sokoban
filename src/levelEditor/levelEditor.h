#ifndef TILE_H                                                                
#define TILE_H                                                               
#include "../public/Tile.h"                     
#endif                 
#ifndef TILEMAP_H
#define TILEMAP_H                                                               
#include "../public/tileMap.h"                                                  
#endif
#ifndef GLOBALINPUT_H
#define GLOBALINPUT_H
#include "../globalInput/globalInput.h"
#endif
#ifndef READLEVELS_H
#define READLEVELS_H
#include "../readLevels/readLevels.h"
#endif
#include "../show/show.h"
#ifndef GAMEDISPLAY_H
#define GAMEDISPLAY_H
#include "../graphics/gameDisplay.h"
#endif
#ifndef MOVEMENT_H
#define MOVEMENT_H
#include "../movement/movement.h"
#endif

void openLevelEditor(int level, char* filepath, bool gui);

tileMap newLevel(int width, int height);
