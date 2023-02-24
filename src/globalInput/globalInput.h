#ifndef IO_H
#define IO_H
#include "../input_keyboard/io.h"
#endif

void setCursor(int x, int y);

void getCursor(int *x, int *y);

void registerInput(int in);

int getInput();

void flushInputBuffer();

void registerTerminal();
