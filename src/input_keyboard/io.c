#ifndef IO_H
#define IO_H
#include "io.h"
#endif
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAXSIZE 10


int configureTerminal(int n) //n=0 => Terminal freeze
{
        static struct termios *new = NULL;
	if(new == NULL) new = malloc(sizeof(struct termios));
        if (tcgetattr(0,new)==-1) {
                perror("tcgetattr");
                return -1;
        }
        if (n==0){
        new->c_lflag &= ~(ICANON); // Met le terminal en mode canonique.
        new->c_lflag &= ~(ECHO);   // les touches tapées ne s'inscriront plus dans le terminal
        new->c_cc[VMIN]=1;
        new->c_cc[VTIME]=0;}
        else{
        new->c_lflag = 35387;   // Config de base
        new->c_cc[VMIN]=1;
        new->c_cc[VTIME]=0;}
        if (tcsetattr(0,TCSANOW,new)==-1) {
                perror("tcsetattr");
                return false;
        }
        return true;
}

int retour(int val){
        switch (val) {
                case 3: return 1; break;
                case 2: return 3; break;
                case 1: return 2; break;
                default: return val; break;
        }
}

int litClavier()
{
        char r[MAXSIZE];
        int c;

        if ((c=read(0,r,3)) == - 1 ) return false;
        switch (r[0]) {
                case 27 :  if ((c==3) && (r[1]==91)) return (retour(r[2]-65)); else return false; break;
                default: return r[0]; break;
        }
}

/* Creer la structure avec malloc avant !!
int main() {
  struct termios* term = malloc(sizeof(struct termios));
  configureTerminal(term,true);
  configureTerminal(term,1);
  free(term);
  return 0;
}
*/
