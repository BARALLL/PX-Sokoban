#include "solution.h"

//___AFFICHAGE___

int affiche(movement* liste){
  while (liste != NULL){
    switch (liste->direction) {
      case UP: printf("UP "); break;
      case LEFT: printf("LEFT "); break;
      case DOWN: printf("DOWN "); break;
      case RIGHT: printf("RIGHT "); break;
    }
    if (liste->boxMoved) printf("BoxMoved !\n");
    else printf("NoBoxMoved !\n");
    liste = liste->prev;
  }
  return 0;
}



// ___ECRITURE___

enum direction intToDirection(int n){
  switch (n) {
    case 0: return UP;
    case 1: return LEFT;
    case 2: return DOWN;
    case 3: return RIGHT;
  }
  return -1;
}

char* levelName(int n, bool custom){ //Cree le nom du niveau en char[] à partir d'un int.
    char level[10] = "Sol_Level_";
    char cust[5] = "Custo";
    int i;
    int j=0; // Indice pour numero
    char numero[10];
    int len = sprintf(numero,"%d",n);
    char* output = malloc(sizeof(char)*(9+len+1));
    for (i = 0; i < 10; i++) {
      output[i] = level[i];
    }
    for (; i < 10+len; i++) {
      output[i]=numero[j++];
    }
    j=0;
    for (i = 4; (i < 9) && custom; i++) {
      output[i]=cust[j++];
      printf("e\n");
    }
    printf("%c\n", output);
    return output; //Penser à free()
}

int ecritureSolution(movement* liste, int n, bool custom, bool fini) {
    char *nomLevel = levelName(n, custom);
    FILE *f = NULL;
    f=fopen(nomLevel,"w");
    if (f==NULL) return -1;
    if (fini) fprint(f,"%d",8);
    while(liste->prev != NULL){
    fprintf(f,"%d", (*liste).direction);
    liste = liste->prev;
  }
    fprintf(f,"%d", (*liste).direction);
    fclose(f);
    return 0;
}


//___LECTURE___ -> registerInput

void lectureSolution(char* nomLevel){
  FILE *f;
  int i;
  char c; //c-48
  f=fopen(nomLevel,"r");
  if (f == NULL) return NULL;
  movement* head = malloc(sizeof(movement)); //First movement
  fseek(f,0,SEEK_END); //Taille
  long int len = ftell(f); //Taille
  fseek(f,-1,SEEK_END); //Pour prendre le dernier carac
  c=fgetc(f);
  if (c-48 > 3) {c=c-4;head->boxMoved = true;}
  else head->boxMoved = false;
  head->direction = intToDirection(c-48);
  registerInput(c-48);
  head->prev = NULL;
  //printf("TAILLE: %d \n", len);
  for (i = 0; i < len-1; i++) {
    fseek(f,-2,SEEK_CUR);
    c=fgetc(f);
    registerInput(c-48);
  }
  fclose(f);
}

void levelName2(int n, bool custom, char* output){ //Cree le nom du niveau en char[] à partir d'un int. output fait la bonne longueur.
    char level00[] = "Sol_Level_";
    char cust[] = "Custom_";
    char numero[10];
    int i = 10;
    sprintf(numero,"%d",n);
    strcpy(output,level00);
    if (custom) {strcat(output,cust);i+=7;}
    strcat(output,numero);
    return;
  }

int tailleInt(int n, bool cust){
  char temp[20];
  int offset = 0;
  int ret = sprintf(temp,"%d",n);
  if(cust) offset = 7;
  return ret+offset;
}
