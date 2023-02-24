#ifndef TILEMAP_H
#define TILEMAP_H
#include "../public/tileMap.h"
#endif
#ifndef GLOBALINPUTS_H
#define GLOBALINPUTS_H
#include "../globalInput/globalInput.h"
#endif
#ifndef PLAYERPOS_H
#define PLAYERPOS_H
#include "../playerPosition/playerPosition.h"
#endif
#ifndef TILE_H
#define TILE_H
#include "../public/Tile.h"
#endif
#ifndef GAMEDISPLAY_H
#define GAMEDISPLAY_H
#include "../graphics/gameDisplay.h"
#endif
#ifndef GAMEDISPLAY_H
#define GAMEDISPLAY_H
#include "../show/show.h"
#endif
#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include "../gameManager/gameManager.h"
#endif

typedef struct movement
{
        enum direction {UP,LEFT,DOWN,RIGHT} direction;
        struct movement* prev;
        bool boxMoved;
} movement;


void removeMvt(tileMap map, bool gui);
void getNbMvt(int* nb, int* boxMove);
void executeMovement(tileMap tm, movement mov, bool reversed, bool gui);  

void readMovementInputAndExecute(tileMap tm, bool gui, int dir);

movement* getMoveList();
