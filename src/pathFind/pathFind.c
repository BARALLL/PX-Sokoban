#include "pathFind.h"


/*Transforme une tile en aTile*/
aTile tileToATile(int x, int y)
{
	return (aTile) { x, y, NULL, 0, 0 };
}

/*Retourne la distance "en ligne droite" entre 2 aTiles*/
int getDistance(aTile point1, aTile point2)
{
	return abs(point1.x - point2.x) + abs(point1.y - point2.y);
}

static aTile *getBestaTileAtCoords(listAT *list, int x, int y, aTile *checkParent){
	listAT *itter = list;
	aTile *best = NULL;
	while(itter != NULL){
		aTile *data = &(itter->data);
		if(data->x == x && data->y == y && (!checkParent || data->parent == checkParent) && (!best || data->cost < best->cost)) {
			best = data;
		}
		itter = itter->next;
	}
	return best;
}

static void freeList(listAT* list){
	listAT *buffer = list;
	while(list != NULL){
		list = list->next;
		free(buffer);
		buffer = list;
	}
}


static void registerMove(aTile tile){
	printf("(%d,%d)\n", tile.x, tile.y);
	if(!tile.parent) {
		printf("Tried to register original pathfind aTile\n");
		exit(1);
	}
	if(tile.parent->parent) registerMove(*tile.parent);
	int in = 0;
	switch (tile.x - tile.parent->x) {
		case 1 : in = 3; break;
		case -1 : in = 1; break;
		default : in = (tile.y - tile.parent->y) == 1 ? 2 : 0; 
	}
	registerInput(in);
}

static aTile getPlayerParent(aTile current){
	int posX;
	int posY;

	if(!current.parent) getPlayerPosition(&posX, &posY);
	else {
		posX = current.parent->x;
		posY = current.parent->y;
	}
	return tileToATile(posX, posY);
}

static void registerCrateMove(tileMap map, aTile tile){
	printf("crate (%d,%d)\n", tile.x, tile.y);
	if(!tile.parent){
		printf("Tried to register original pathfind aTile : crate\n");
		exit(1);
	}
	if(tile.parent->parent) registerCrateMove(map, *tile.parent);
	int oppositeX = tile.parent->x + (tile.parent->x - tile.x);
	int oppositeY = tile.parent->y + (tile.parent->y - tile.y);
	pathfinding(map, getPlayerParent(*tile.parent), tileToATile(oppositeX, oppositeY), false, tile.parent,NULL,true); 
	int in = 0;
	switch (tile.x - tile.parent->x) {
		case 1 : in = 3; break;
		case -1 : in = 1; break;
		default : in = (tile.y - tile.parent->y) == 1 ? 2 : 0; 
	}
	registerInput(in);
	
}

/*Retourne la liste des tiles atteignables autour de currentTile*/
static listAT* getWalkableTiles(tileMap map, aTile *currentTile, aTile targetTile, aTile start, bool isCrate, aTile *blackList, aTile *whiteList)
{
	listAT* possibleTiles = NULL;		//ensemble de cases pouvant être atteinte avec 1 coup
	listAT* finalPossibleTiles = NULL;
	aTile* atile;
	
	//distCosts will be set later
	addLAT(&possibleTiles, (aTile) { currentTile->x + 1, currentTile->y, currentTile, currentTile->cost + 1, 0 });
	addLAT(&possibleTiles, (aTile) { currentTile->x - 1, currentTile->y, currentTile, currentTile->cost + 1, 0 });
	addLAT(&possibleTiles, (aTile) { currentTile->x, currentTile->y + 1, currentTile, currentTile->cost + 1, 0 });
	addLAT(&possibleTiles, (aTile) { currentTile->x, currentTile->y - 1, currentTile, currentTile->cost + 1, 0});
	
	for(int i = 0; i < 4; i++)
	{	
		atile = getATile(possibleTiles, i);
		tile *t = getTile(map, atile->x, atile->y);
		atile->costDist = getDistance(*atile, targetTile) + currentTile->cost + 1;
		
		if ((!blackList || atile->x != blackList->x || atile->y != blackList->y) 
				&& atile->x < map.width && atile->x >= 0 && atile->y < map.height && atile->y >= 0 
					&& t && t->isWalkable && (!t->isPushable 
								|| ( whiteList && (atile->x == whiteList->x && atile->y == whiteList->y) ))) //if tile is valid
		{
			if(!isCrate)
				addLAT(&finalPossibleTiles, *atile);
			else{
				int oppositeX = currentTile->x + (currentTile->x - atile->x);
				int oppositeY = currentTile->y + (currentTile->y - atile->y);
				if(pathfinding(map, getPlayerParent(*currentTile), tileToATile(oppositeX, oppositeY), false, currentTile, &start, false)) //if player can path find 
					addLAT(&finalPossibleTiles, *atile);
			}
		}
	
	}
	freeList(possibleTiles);
	return finalPossibleTiles;
}


/*Trouve le chemin entre player et finish dans la map*/
bool pathfinding(tileMap map, aTile start, aTile finish, bool isCrate, aTile *blackList, aTile *whiteList, bool registerPath)
{
	listAT* activeTiles = NULL;
	listAT* visitedTiles = NULL;
	listAT* walkableTiles;
	int walkTileLength = 0;
	aTile *checkTile;

	bool completed = false;

	int k = 0;
	int count = 0;
	listAT* index = NULL;

	
	addLAT(&activeTiles, start);

	while(activeTiles != NULL || completed)
	{
		index = activeTiles;
		if (!index){
			printf("ON SCASSE \n");
			exit(EXIT_FAILURE);
		}

		k = 0;
		while (index)
		{
			index = index->next;
			k++;
		}

		quicksort(activeTiles, 0, k - 1);
		checkTile = getATile(activeTiles, 0);
		count++;

		if (checkTile->x == finish.x && checkTile->y == finish.y){
			completed = true;
			break;
		}
		else
		{	
			checkTile = &(addLAT(&visitedTiles, *checkTile)->data);
			listAT *buffer = activeTiles;
			activeTiles = activeTiles->next;
			free(buffer);

			walkableTiles = getWalkableTiles(map, checkTile, finish, start,isCrate, blackList, whiteList);
			
			walkTileLength = 0;
			index = walkableTiles;
			while (index)
			{
				index = index->next;
				walkTileLength++;
			}
			
			for(int i = 0; i < walkTileLength; i++)
			{
				bool found = false;
				aTile *current = getATile(walkableTiles,i);
				aTile *checkParent = isCrate ? current->parent : NULL;
				if (getBestaTileAtCoords(visitedTiles, current->x, current->y, checkParent) != NULL)
					found = true;				//si on a deja visite cette case, on a pas besoin d'y retourner
				
				aTile *active = getBestaTileAtCoords(activeTiles, current->x, current->y, checkParent);
				
				if(active != NULL){ 
					found = true;
					if(active->costDist > current->costDist){
						active->costDist -= (active->cost - current->cost);
						active->cost = current->cost + 1;
						active->parent = current->parent;
					}
				}
				

				//si on a jamais vu cette case, on l'ajoute a la liste de case a voir
				if(!found)
					addLAT(&activeTiles, *current);
				
			}	
			freeList(walkableTiles);
		}

	}
	if(!completed) return false;
	else
	{						//Grace au tri par nombre de coups, le chemin trouve est le plus rapide
				
				//enregistre les mouvements
				if(registerPath && checkTile->parent){
					if(!isCrate)
						registerMove(*checkTile);
					else{
						registerCrateMove(map, *checkTile);
					}
				}
	}
	freeList(activeTiles);
	freeList(visitedTiles);
	//printf("%d",count);
	return true;
}


/*Retourne aTile data situé à l'indice index de la liste chainée liste*/
aTile* getATile(listAT* list, int index) {
	aTile emptyNode = (aTile){ -1, -1, NULL, 0, 0 };
	listAT* currentNode = list;

	if (!currentNode){
		currentNode = malloc(sizeof(listAT));
		*currentNode = (listAT){ NULL, emptyNode };
	}
	
	for (int j = 0; j < index; j++)
	{
		if (!currentNode->next)
		{
			for (size_t i = j; i < index; i++)
			{
				currentNode->next = calloc(1, sizeof(listAT));
				*currentNode->next = (listAT) {NULL, emptyNode };
				currentNode = currentNode->next;
			}	
		}
		currentNode = currentNode->next;
	}

	return &currentNode->data;
}


/* Insertion en "queue de liste" */
listAT *addLAT(listAT** list, aTile item) {
	listAT* index = *list;
	listAT* newNode = malloc(sizeof(listAT));
	newNode->data = item;
	newNode->next = NULL;

	if (index)
	{
		while (index->next)
			index = index->next;
		index->next = newNode;
	}
	else
			*list = newNode;
	return newNode;
}

/*Tri de la liste chainée tab*/
void quicksort(listAT* tab, int first, int last)
{
	int i, j, pivot, temp;
	pivot = first;
	i = first+1;
	j = last;

	if (first < last) {

		
		while (i < j) {
			while (getATile(tab, i)->costDist <= getATile(tab, pivot)->costDist && i < last)
				i++;

			while (getATile(tab, j)->costDist > getATile(tab, pivot)->costDist)
				j--;

			if (i < j) 
			{
				temp = getATile(tab, i)->costDist;
				getATile(tab, i)->costDist = getATile(tab, j)->costDist;
				getATile(tab, j)->costDist = temp;
			}
		}

		temp = getATile(tab, pivot)->costDist;
		getATile(tab, pivot)->costDist = getATile(tab, j)->costDist;
		getATile(tab, j)->costDist = temp;

		quicksort(tab, first, j - 1);
		quicksort(tab, j + 1, last);
	}
}


	
