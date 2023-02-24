#include <stdio.h>
#include <stdbool.h>
#include "gameManager.h"

int lvl;
bool custom;


/**
 * Charge etat du niveau depuis fichier
 * @param[in] bool gui
 * @param[out] none
 */
static void load(int level, bool gui)
{
	FILE* f = fopen("levels.lvl", "r");
	tileMap tm = getLevelMap(level, f);
	fclose(f);
	if (gui) updateGUIMap(tm);
}

/**
 * Lorsque le niveau est fini, ecrit la solution dans un fichier et charge le niveau suivant
 * @param[in] bool gui
 * @param[out] none
 */
static void levelFinished(bool gui)
{
  printf("%s\n", "Level finished!");
  ecritureSolution(getMoveList(), lvl, custom, true);
  lvl++;
  load(lvl, gui);
}


/**
 * Verifie si le niveau est finie
 * @param[in] tileMap tm, bool gui
 * @param[out] none
 */
void isLevelFinish(tileMap tm, bool gui)
{
	/**
	 * Verifie si le niveau est fini
	 */
	int row = 0;
	int col = 0;
	bool finish = true;

	mapCol* currentCol = tm.map;
	while (row < (tm.width - 1) && finish)
	{
		mapNode* currentNode = currentCol->node;
		while (col < (tm.height - 1) && finish)
		{
			currentNode = currentNode->next;
			if (!currentNode->tile.isPushable || !currentNode->tile.isTarget)
				finish = false;
		}
		currentCol = currentCol->next;
	}
	if (finish)
		levelFinished(gui);
}




static void save()
{
	/**
	 * Enregistre etat actuel du niveau
	 */
	ecritureSolution(getMoveList(), lvl, custom, false);
}


void input(tileMap tm, bool gui)
{
	/**
	 * Centralise toutes les entrées input
	 */
	int in = getInput();
	if (in == -1) {
		registerTerminal();		//terminal input
		in = getInput();
	}

	bool isFinished = false;
	FILE* f;
	int taille_lvl = tailleInt(lvl,false);
	char nom[15];

	if (in == 43) 
	{						//Charge le prochain niveau lorsque on appui sur '+'
		levelName2(lvl,custom,nom);
		if((f = fopen(nom, "r")) != NULL && fgetc(f) == '8'){	//niveau n° lvl deja fini ?
			isFinished = true;
		fclose(f);
		load(lvl + 1, gui);
	}
	else if (in == 45)				//Charge le niveau precedent avec '-'
		load(lvl - 1, gui);
	else if (in == 10 || in == 83 || in == 115)	//'s' /'S'
		save();
	else if (in == 82 || in == 114)		//'r' / 'R'
		load(lvl, gui);					//recharge le niveau (recommencer depuis le debut)
	else
		readMovementInputAndExecute(tm, gui, in);
}
