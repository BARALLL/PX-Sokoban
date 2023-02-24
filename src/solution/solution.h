#include <stdio.h>
#include <stdlib.h>
#ifndef MOVEMENT_H
#define MOVEMENT_H
#include "../movement/movement.h"
#endif

#ifndef PLAYERPOSITION_H
#define PLAYERPOSITION_H
#include "../playerPosition/playerPosition.h"
#endif

char* levelName(int n, bool custom);
int affiche(movement* liste);
int ecritureSolution(movement* liste, int n, bool custom, bool fini);
void lectureSolution(char* nomLevel);
void levelName2(int n, bool custom, char* output);
int tailleInt(int n, bool cust);
