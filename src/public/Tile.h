#include <stdbool.h>

typedef struct tile {
	bool isWalkable;
	bool isPushable;
	bool isTarget;
} tile;

//Wall = !isWalkable && !isPushable && !isTarget
//Floor = isWalkable && !isPushable && !isTarget
//Box = !isWalkable && isPushable && !isTarget
//Target = isTarget
