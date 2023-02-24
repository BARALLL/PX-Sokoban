#ifndef TILEMAP_H
#define TILEMAP_H
#include "../public/tileMap.h" 
#endif
#ifndef TILE_H
#define TILE_H
#include "../public/Tile.h"
#endif
#include "../globalInput/globalInput.h"
#ifndef PLAYERPOSITION_H
#define PLAYERPOSITION_H
#include "../playerPosition/playerPosition.h"
#endif

void moveObject(int x, int y, int dir);

void renderGUI();

int initGUI();

int updateGUIMap(tileMap tm);

void setPlayerDirection(int direction);

typedef struct mobileObject{
        //shader, vertex will hold screen pos
        unsigned int shaderHandle;
        enum type {TYPE_CRATE, TYPE_PLAYER, TYPE_NULL} type;
} mobileObject;

typedef struct mobileObjectNode{
	struct mobileObjectNode *next;
	mobileObject *mobileObject;
} mobileObjectNode;

typedef struct mobileObjectCol{
	mobileObjectNode *col;
	struct mobileObjectCol *next;
} mobileObjectCol;

struct mobileObjectMovement{
        mobileObject mobj;
        int direction;
        int targetX;
        int targetY;
        int countDown;
        struct mobileObjectMovement *next;
};

typedef struct {
	float sizeX;
	float sizeY;
	float centerX;
	float centerY;
	bool enabled;
	char *pathToImg;
	void (*method)();
	unsigned int VBOHandle, shaderHandle, texHandle;
} button;

typedef struct chunk {

	int posX;
	int posY;
	
	unsigned int VBOHandle;

	struct chunk *next;
} chunk;
