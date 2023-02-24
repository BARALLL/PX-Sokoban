static int posX = -1;
static int posY = -1;

void setPlayerPosition(int x, int y){
	posX = x;
	posY = y;
}

void getPlayerPosition(int *x, int *y){
	*x = posX;
	*y = posY;
}
