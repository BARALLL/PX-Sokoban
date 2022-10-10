#### VARS

GUI_OBJ = src/graphics/gameDisplay.c src/graphics/glad/src/glad.c

NO_GUI_OBJ = src/graphics/gameDisplay_nogui.c

CFLAGS = -fsanitize=address -Wall -g

GUI_FLAGS = -lglfw -lm 

SRC_PATH = src/

MAIN_PATH = src/main.c

SUBDIRS = solution gameManager pathFind playerPosition movement tileMap readLevels show levelEditor input_keyboard globalInput

SRCS = $(wildcard *.c $(foreach fd, $(SUBDIRS), $(SRC_PATH)$(fd)/*.c))

OBJS = $(SRCS:c=o)

### RULES

all: nogui

nogui: $(MAIN_PATH) $(OBJS) $(NO_GUI_OBJ)
	 gcc $(CFLAGS) -o sokoban_nogui.out $^ 

gui : $(MAIN_PATH) $(OBJS) $(GUI_OBJ)
	 gcc $(CFLAGS) -o sokoban_gui.out $^ $(GUI_FLAGS) 

%.o : %.c
	gcc $(CFLAGS) -o $(OBJ_PATH)$@ -c $^

clean:
	rm *.o $(OBJS) *.out

echo:
	echo SRCS : $(SRCS)
	echo OBJS : $(OBJS)
