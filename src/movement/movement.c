#include <stdio.h>
#include <stdlib.h>
#include "movement.h"

static movement* listMvt = NULL;


static void addMvt(movement mvt)
{
	/**
	 * Insertion en tête d'un movement a la liste des movements
	 */
        movement* pNode = calloc(1, sizeof(movement));
        if (pNode)
        {
                *pNode = mvt;
                pNode->prev = listMvt;
                listMvt = pNode;
        }
}

void removeMvt(tileMap map, bool gui) 
{
	/**
	 * Suppression en tête d'un movement a la liste des movements
	 */
	if(listMvt == NULL) return;
        executeMovement(map, *listMvt, true, gui);
        movement* prevP = listMvt->prev;
        free(listMvt);
        listMvt = prevP;
}


void printMvt(movement* mvt) {
	/**
	 * Affichage de la liste
	 */
        if (!mvt)
                printf("Liste vide!");
        else
                printf("Contenu de la liste : ");
        while (mvt) {
                printf("%d", (*mvt).direction);
                printf("%c", (*mvt).boxMoved);
                mvt = (*mvt).prev;
        }
        printf("\n");
}


void getNbMvt(int* nb, int* boxMove)
{
	/**
	 * Calcule le nombre de coup et de caisse poussées dans ce niveau
	 */
	movement* pIndex = listMvt;
	if (pIndex != NULL)
	{
		while ((pIndex->prev)) {
			pIndex = pIndex->prev;
			(*nb)++;
			if (pIndex->boxMoved) (*boxMove)++;
		}
		(*nb)++;
	}
}


static bool canMoveInDir(tileMap tm, int x, int y, enum direction dir, bool push, bool *willPush){
	/**
	 * can an object move from x y in dir
	 */
	switch (dir){
		case UP: y--;break;
		case LEFT: x--;break;
		case DOWN: y++;break;
		case RIGHT: x++;break;
	}
	
	if(x < 0 || x >= tm.width || y < 0 || y > tm.height) return false;
	tile t = *getTile(tm, x, y);
	
	if(!t.isWalkable || (t.isPushable && !push)) return false;
	if(push && t.isPushable) {
		*willPush = true;
		bool nulled;
		return canMoveInDir(tm, x, y, dir, false, &nulled);
	}
	
	*willPush = false;
	return true;

}


static int readMovement(tileMap tm, bool gui, movement *move, int in){
	/**
	 * convert input to movement, will make a mess in move if invalid movement
	*/
	if(in == 8) removeMvt(tm, gui); //back

	if(in < 0 || in > 3) return -1;
	
	if(gui) setPlayerDirection(in);
	int playerPosX;
	int playerPosY;
	getPlayerPosition(&playerPosX, &playerPosY);
	
	move->prev = NULL;
	move->boxMoved = false;
	move->direction = in;
		
	if(!canMoveInDir(tm, playerPosX, playerPosY, move->direction, true, &(move->boxMoved))) return -1;

	return 1;
}

void executeMovement(tileMap tm, movement mov, bool reversed, bool gui){
	
	printf("movement %d\n", mov.direction);

	int playerPosX;
	int playerPosY;

	getPlayerPosition(&playerPosX, &playerPosY);
	int newPlayerPosX = playerPosX;
	int newPlayerPosY = playerPosY;

	int mod = reversed ? -1 : 1;

	int cratePosX = playerPosX;
	int cratePosY = playerPosY;

	int newCratePosX = playerPosX;
	int newCratePosY = playerPosY;

	switch (mov.direction){
		case UP: 
			newPlayerPosY = playerPosY - mod; 
			cratePosY --;
			newCratePosY = cratePosY - mod;
			break;
		case LEFT: 
			newPlayerPosX = playerPosX - mod;
			cratePosX --;
			newCratePosX = cratePosX - mod;
			break;
		case DOWN: 
			newPlayerPosY = playerPosY + mod;
			cratePosY ++;
			newCratePosY = cratePosY + mod;
			break;
		case RIGHT: 
			newPlayerPosX = playerPosX + mod;
			cratePosX ++;
			newCratePosX = cratePosX + mod;
			break;
	}
	
	if(mov.boxMoved){
		getTile(tm, cratePosX, cratePosY)->isPushable = false;
		getTile(tm, newCratePosX, newCratePosY)->isPushable = true;
		isLevelFinish(tm, gui);
	}
	
	setPlayerPosition(newPlayerPosX, newPlayerPosY);
	
	if(gui) {

		int finalDirection = mov.direction;
		if(reversed) {
			setPlayerDirection(mov.direction);
			finalDirection = ((finalDirection + 2) % 4) ;
			moveObject(playerPosX, playerPosY, finalDirection);
			if(mov.boxMoved)moveObject(cratePosX, cratePosY, finalDirection);
		}
		else{
			if(mov.boxMoved)moveObject(cratePosX, cratePosY, finalDirection);
			moveObject(playerPosX, playerPosY, finalDirection);
		}
	}
	else show(tm);

}

void readMovementInputAndExecute(tileMap tm, bool gui, int dir){
	movement m;
	if(readMovement(tm, gui, &m, dir) == -1) return;
	addMvt(m);
	executeMovement(tm, m, false, gui);
}

movement* getMovementList(){
	return listMvt; 
}

int freeMovementList(){
  movement* list2;
  while (listMvt != NULL){
    list2 = listMvt->prev;
    free(listMvt);
    listMvt = list2;
  }
  return 0;
}

void resetMovementList(){
	freeMovementList();
	listMvt = NULL;
}

movement* getMoveList(){
	return listMvt;
}
