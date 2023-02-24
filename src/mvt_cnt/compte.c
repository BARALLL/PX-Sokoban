#include <stdlib.h> //STOPPED
#include <stdbool.h>
typedef struct Movement
{
  enum direction {UP = 0, LEFT = 1, DOWN = 2, RIGHT = 3};
  struct Movement* next;
  bool boxMoved;
} Movement;

struct tile{
	bool isWalkable;
	bool isPushable;
	bool isTarget;
};

int count(Movement* listeMvt, int nombreMvt, struct tile** map, int entreeClavier, int x, int y){
  switch (entreeClavier) {
    case 0: if (!*map[x][y+1].isWalkable) return nombreMvt; Movement* new; new = malloc(sizeof(Movement)); new->boxMoved = false; listeMvt->next = new;
    case 1: if (!*map[x-1][y].isWalkable) return nombreMvt;
    case 2: if (!*map[x][y-1].isWalkable) return nombreMvt;
    case 3: if (!*map[x+1][y].isWalkable) return nombreMvt;
    default: break;
  }
}

int main(int argc, char const *argv[]) {
  return 0;
}
