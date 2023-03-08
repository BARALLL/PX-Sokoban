# PX-Sokoban

This is a simple implementation of the Sokoban game in C with a graphical user interface (GUI) using OpenGL.


### How to Play
The objective of the game is to push all the boxes to the target locations. The player can move in four directions (up, down, left, right) and push one box at a time. 
The player cannot push a box into a wall or another box. The game save the solution and load the next level when all boxes are on target locations.

- To move the player, use the arrow keys on your keyboard or ZQSD
- 'S' to save the current state of the level
- 'R' to restart the level
- '+' and '-' to load either the next or the previous level
- To exit the game, press 'Esc'
- You can undo a much as you want using the backspace key
- Left click on a tile to find a path and travel to it

Each character in the terminal represents a cell in the game grid:
- #: wall
- .: target location
- $: box
- @: player
- (space): empty cell
- *: box on target location
- +: player on target location


### Installation
Enter `make all` in the terminal.
You can then play in the terminal with no argurments or using the following arguments:
- `--editor` to enter the editor
- `--guieditor` to enter the graphical version of the editor
- `--gui` to play the gui version of the graphical version of the game
The second argument is the number of the level to edit/play


The game levels are stored in text files in the levels.lvl file. You can create your own levels by modifying this file, or using the built-in editor.
