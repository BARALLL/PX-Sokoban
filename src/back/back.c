#include "movement.h"


movement* listMvt = NULL;

/* Insertion en tête */
static void addMvt(movement mvt)
{
	movement* pNode = calloc(1, sizeof(movement));
	if (pNode)
	{
		*pNode = mvt;
		pNode->prev = listMvt->prev;
		listMvt->prev = pNode;
	}
}


/* Suppression en tête */
void removeMvt(movement** ph, tileMap map) {
	executeMovement(map, **ph, true);
	movement* prevP = (*ph)->prev;
	free(*ph);
	*ph = prevP;
}


/* Affichage de la liste */
void printMvt(movement* ptr) {
	if (!ptr)
		printf("Liste vide!");
	else
		printf("Contenu de la liste : ");
	while (ptr) {
		printf("%d", (*ptr).direction);
		printf("%c", (*ptr).boxMoved);
		ptr = (*ptr).prev;
	}
	printf("\n");
}
