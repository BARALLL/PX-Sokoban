
#include "glad/glad.h" //opengl function definitions
#include <GLFW/glfw3.h> //GLFW, dependency 
#include <stdio.h> 
#define STB_IMAGE_IMPLEMENTATION //activating stb image
#include "stb_image.h" //png reading
#ifndef GAMEDISPLAY_H
#define GAMEDISPLAY_H
#include "gameDisplay.h" //self
#endif

#define MOVEMENT_DURATION 60
#define AMMOUNT_OF_BUTTONS 16

// SHADER SOURCES -------------------------------------------------------------------------------

const char *standardVertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec2 texCoord;\n"
"void main(){\n"
"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"texCoord = aTexCoord;\n"
"}\n";

const char *transformedVertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec2 texCoord;\n"
"uniform vec3 posTransform;\n"
"void main(){\n"
"vec4 pos = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"pos.xyz += posTransform;\n"
"gl_Position = pos;\n"
"texCoord = aTexCoord;\n"
"}\n";

const char *standardFragmentShaderSource = "#version 460 core \n"
"out vec4 color;\n"
"in vec2 texCoord;\n"
"uniform sampler2D tex;\n"
"void main(){\n"
"color = texture(tex, texCoord);\n"
"}\n";

const char *animatedFragmentShaderSource = "#version 460 core \n"
"out vec4 color;\n"
"in vec2 texCoord;\n"
"uniform sampler2D tex;\n"
"uniform int animFrame;\n"
"void main(){\n"
"	vec2 coords = texCoord + vec2(0.5* animFrame,0);\n"
"	color = texture(tex, coords);\n"
"}\n";

const char *playerFragmentShaderSource = "#version 460 core \n"
"out vec4 color;\n"
"in vec2 texCoord;\n"
"uniform sampler2D tex;\n"
"uniform int animFrame;\n"
"uniform int direction;\n"
"void main(){\n"
"	vec2 coords = texCoord + vec2(0.5* animFrame,0.25*direction);\n"
"	color = texture(tex, coords);\n"
"}\n";

// LOCAL ENUMS -------------------------------------------------------------------------------

enum textureIndex {INDEX_TILESET=0, INDEX_CURSOR=1, INDEX_CRATE=2, INDEX_PLAYER,INDEX_BUTTONS};

enum buttonIndex {BUTTON_PLAY_STORY, BUTTON_PLAY_CUSTOM, BUTTON_EDITOR};

static enum gameMode {PLAY, EDITOR, MENU} currentGameMode = MENU;

// (local) GLOBALS ---------------------- sorry I really need them ----------------------------

static mobileObjectCol *mobileObjectMap;

static struct mobileObjectMovement *mobileObjectMovementQueue = NULL;

static GLFWwindow* window;

static unsigned int VAOHandle, cursorVBOHandle, chunkEBOHandle, tileEBOHandle, VAOHandle, cursorShaderHandle, crateVBOHandle, playerVBOHandle, screenOverlayVBOHandle, tileShaderHandle;

static unsigned int texHandle[4];

static tileMap currentTileMap;

static button *allButtons[AMMOUNT_OF_BUTTONS];

static chunk *chunkList;

//GENERAL PURPOSE -----------------------------------------------------------------------------

static button *getButtonAt(float posX, float posY){
	for(int i = 0; i < AMMOUNT_OF_BUTTONS; i++){
		button *b = allButtons[i];
		if(b != NULL && b->enabled){
			if(posX > b->centerX - b->sizeX/2
			&& posX < b->centerX + b->sizeX/2
			&& posY > b->centerY - b->sizeY/2
			&& posY < b->centerY + b->sizeY/2){
				return b;
			}
		}
	}
	return NULL;
}

static float ratioedHeight(int width, int height) {
	return height*16/9 > width ? height : (int)((float)width * 9/16);
}

static float ratioedWidth(int width, int height) {
	return height *16/9 < width ? width : (int)((float)height * 16/9);
}

static void tileToWorld(int x, int y, float *outx, float *outy, tileMap tm){
/**
 *turns the tile coords to openGL world coords
 * */
	float rh = ratioedHeight(tm.width, tm.height);
        float rw = ratioedWidth(tm.width, tm.height); 
	
	*outx = (x + (int)(rw - tm.width)/2)*2/rw;
	*outy = -(y - (int)(rh - tm.height)/2)*2/rh;
}
//Mobile objects ------------------------------------------------------------------------

static mobileObject *getMobileObject(int x, int y){
	mobileObjectCol *currentCol = mobileObjectMap;
	for(int i = 0; i < x; i++) currentCol = currentCol->next;

	mobileObjectNode *currentNode = currentCol->col;
	for(int i = 0; i < y; i++) currentNode = currentNode->next;

	return currentNode->mobileObject;
}


static void freeMobileObjectMap(tileMap tm){        
	mobileObjectCol *currentCol = mobileObjectMap;
        for(int x = 0; x < (tm.width-1); x++){
                mobileObjectNode *currentNode = currentCol->col;
                for(int y = 0; y < (tm.height)-1; y++){
                        mobileObjectNode *toFree = currentNode;
                        currentNode = currentNode->next;
                        free(toFree);
                }
                free(currentNode->next);
                free(currentNode);
                mobileObjectCol *toFree = currentCol;
                currentCol = currentCol->next;
                free(toFree);
        }
        mobileObjectNode *currentNode = currentCol->col;
        for(int y = 0; y < (tm.height)-1; y++){
                mobileObjectNode *toFree = currentNode;
                currentNode = currentNode->next;
                free(toFree);
        }
        free(currentCol->next);
        free(currentCol);

}

static void initMobileObjectMap(tileMap tm){
	if(mobileObjectMap != NULL) freeMobileObjectMap(tm);
        
	mobileObjectMap = malloc(sizeof(mobileObjectCol));
        
        mobileObjectCol *currentCol = mobileObjectMap;

	mobileObject emptyObj = {-1, TYPE_NULL};

        //create map
        for(int x = 0; x < tm.width; x++){
                currentCol->col = malloc(sizeof(mapNode));
                mobileObjectNode *currentNode = currentCol->col;
                for(int y = 0; y < tm.height; y++){
			currentNode->mobileObject = malloc(sizeof(mobileObject));
			*(currentNode->mobileObject) = emptyObj;
                        currentNode->next = malloc(sizeof(mapNode));
                        currentNode = currentNode->next;
                }
                currentCol->next = malloc(sizeof(mapCol));
                currentCol = currentCol->next;
        }
}

static void makeTransformedAnimatedShader(int unitIndex, unsigned int *shaderHandle){
	//create shader program
	int success;
        char infoLog[512];
        unsigned int vertexHandle;
        vertexHandle = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(vertexHandle, 1, &transformedVertexShaderSource, NULL);
        glCompileShader(vertexHandle);

        glGetShaderiv(vertexHandle, GL_COMPILE_STATUS, &success);
        if(!success){
                glGetShaderInfoLog(vertexHandle, 512, NULL, infoLog);
                printf("Vertex is fucked : %s\n", infoLog);
        }


        unsigned int fragmentHandle;
        fragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentHandle, 1, &animatedFragmentShaderSource, NULL);
        glCompileShader(fragmentHandle);

        glGetShaderiv(fragmentHandle, GL_COMPILE_STATUS, &success);
        if(!success){
                glGetShaderInfoLog(fragmentHandle, 512, NULL, infoLog);
                printf("Fragment is fucked : %s\n", infoLog);
        }
        
        *shaderHandle = glCreateProgram();
        glAttachShader(*shaderHandle, vertexHandle);

        glAttachShader(*shaderHandle, fragmentHandle);
        glLinkProgram(*shaderHandle);
	
	glUseProgram(*shaderHandle);
        
	glGetProgramiv(*shaderHandle, GL_LINK_STATUS, &success);
        if(!success){
                glGetProgramInfoLog(*shaderHandle, 512, NULL, infoLog);
                printf("Program is fucked : %s\n", infoLog);
        }
	
	glUniform3f(glGetUniformLocation(*shaderHandle, "posTransform"),0,0,0);
	
	glUniform1i(glGetUniformLocation(*shaderHandle, "tex"),unitIndex);
	
	GLenum err = glGetError();
	if(err != GL_NO_ERROR){
                printf("buggy bug, animated transformed shader: %d\n", err);
        }
	//end shader
}
static void createCrate(tileMap tm, int x, int y){
	mobileObject crate;
	crate.type = TYPE_CRATE;
	
	
	makeTransformedAnimatedShader(INDEX_CRATE, &(crate.shaderHandle));
	glUseProgram(crate.shaderHandle);

	float worldX;
	float worldY;
	
	tileToWorld(x,y,&worldX,&worldY,tm);

	glUniform3f(glGetUniformLocation(crate.shaderHandle, "posTransform"), worldX,worldY,0);


	*getMobileObject(x,y) = crate;
	

	GLenum err = glGetError();
	if(err != GL_NO_ERROR){
		printf("buggy bug, crate creation : %d\n", err);
	}


}

static void initCrateVBO(tileMap tm){
	glGenBuffers(1, &crateVBOHandle);

	float rh = ratioedHeight(tm.width, tm.height);
        float rw = ratioedWidth(tm.width, tm.height); 

	float vertexData[4*2*2] = { // at 0 0
		-1,1,0,0,
		-1+2/rw, 1, 0.5, 0,
		-1+2/rw, 1-2/rh, 0.5,1,
		-1, 1-2/rh,0,1
	};
	glBindBuffer(GL_ARRAY_BUFFER, crateVBOHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
}


void moveObject(int posx, int posy, int direction){
	/**
	 *move object at posx,posy one tile in direction. posx,posy must be in range (please). if direction is invalid, will not move
	 **/
	if(mobileObjectMap == NULL){
		printf("Attempted to draw crates but mobileObjectMap is not initialized, we're done for\n");
		return;
	}

	mobileObject mobj = *getMobileObject(posx,posy);
	
	if(mobj.type == TYPE_NULL) 
	{
		printf("error, no object at %d, %d\n", posx,posy);
		return;
	}

	int targetX = posx;
	int targetY = posy;
	switch (direction){
		case 0 : targetY--; break;
		case 2 : targetY++; break;
		case 3 : targetX++; break;
		case 1 : targetX--; break;
	}
		

	*getMobileObject(targetX, targetY) = (mobj);
	getMobileObject(posx, posy)->type = TYPE_NULL;

	//file up
	struct mobileObjectMovement mobjMov = {mobj, direction, targetX, targetY, MOVEMENT_DURATION, mobileObjectMovementQueue};
	mobileObjectMovementQueue = malloc(sizeof(struct mobileObjectMovement));
	*mobileObjectMovementQueue = mobjMov;
}

static void processMobileObjects(tileMap tm){
	struct mobileObjectMovement *mobjMov = mobileObjectMovementQueue;
	struct mobileObjectMovement *prev = NULL;

	float rh = ratioedHeight(tm.width, tm.height);
        float rw = ratioedWidth(tm.width, tm.height); 
	while(mobjMov != NULL){
		
		float moveX;
		float moveY;

		switch (mobjMov->direction){
			case 0: moveY = 1; moveX = 0; break;
			case 2: moveY = -1; moveX = 0; break;
			case 1: moveY = 0; moveX = -1; break;
			case 3: moveY = 0; moveX = 1; break;
		}
		
		float initialX = mobjMov->targetX - moveX;
		float initialY = mobjMov->targetY + moveY;

		moveX*=2/(rw*MOVEMENT_DURATION); //tile
		moveY*=2/(rh*MOVEMENT_DURATION);

		float worldX;
		float worldY;

		tileToWorld(initialX, initialY,&worldX,&worldY,tm);
		
		
		mobjMov->countDown--;
		worldX += (MOVEMENT_DURATION - mobjMov->countDown)*moveX;
		worldY += (MOVEMENT_DURATION - mobjMov->countDown)*moveY;

		glUseProgram((mobjMov->mobj).shaderHandle);
		glUniform3f(glGetUniformLocation((mobjMov->mobj).shaderHandle, "posTransform"),worldX,worldY,0);
		
	GLenum err = glGetError();
	if(err != GL_NO_ERROR){
                printf("buggy bug, mobj proc : %d\n", err);
        }

		struct mobileObjectMovement *next = mobjMov->next;
		if(mobjMov->countDown == 0){
			mobileObject mobj = mobjMov->mobj;
			if(mobj.type == TYPE_CRATE){
				glUseProgram(mobj.shaderHandle);
				int frame = 0;
				if(getTile(tm, mobjMov->targetX, mobjMov->targetY)->isTarget) frame = 1;
				glUniform1i(glGetUniformLocation(mobj.shaderHandle, "animFrame"), frame);
			}	

			if(mobjMov == mobileObjectMovementQueue){
				free(mobileObjectMovementQueue);
				mobileObjectMovementQueue = NULL;
				mobjMov = NULL;
			}
			else{
				free(mobjMov);
				if(prev != NULL) prev->next = NULL;
			}
		}
		
		prev = mobjMov;
		mobjMov = next;
	}
	GLenum err = glGetError();
	if(err != GL_NO_ERROR){
                printf("buggy bug, mobj proc : %d\n", err);
        }

}
//PLAYER ------------------------------------------------------------------------

void setPlayerDirection(int direction){
		int px,py;
		getPlayerPosition(&px, &py);
		
		mobileObject player = *getMobileObject(px,py);
		glUseProgram(player.shaderHandle);
		glUniform1i(glGetUniformLocation(player.shaderHandle, "direction"), direction);
}


static void initPlayer(tileMap tm){

	mobileObject player;
	player.type = TYPE_PLAYER;
	
	int playerPosX, playerPosY;
	getPlayerPosition(&playerPosX, &playerPosY);


//shader creation :
	int success;
        char infoLog[512];
        unsigned int vertexHandle;
        vertexHandle = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(vertexHandle, 1, &transformedVertexShaderSource, NULL);
        glCompileShader(vertexHandle);

        glGetShaderiv(vertexHandle, GL_COMPILE_STATUS, &success);
        if(!success){
                glGetShaderInfoLog(vertexHandle, 512, NULL, infoLog);
                printf("Vertex is fucked : %s\n", infoLog);
        }


        unsigned int fragmentHandle;
        fragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentHandle, 1, &playerFragmentShaderSource, NULL);
        glCompileShader(fragmentHandle);

        glGetShaderiv(fragmentHandle, GL_COMPILE_STATUS, &success);
        if(!success){
                glGetShaderInfoLog(fragmentHandle, 512, NULL, infoLog);
                printf("Fragment is fucked : %s\n", infoLog);
        }
        
        player.shaderHandle = glCreateProgram();
        glAttachShader(player.shaderHandle, vertexHandle);

        glAttachShader(player.shaderHandle, fragmentHandle);
        glLinkProgram(player.shaderHandle);
	
	glUseProgram(player.shaderHandle);
        
	glGetProgramiv(player.shaderHandle, GL_LINK_STATUS, &success);
        if(!success){
                glGetProgramInfoLog(player.shaderHandle, 512, NULL, infoLog);
                printf("Program is fucked : %s\n", infoLog);
        }
	
	
	glUniform1i(glGetUniformLocation(player.shaderHandle, "tex"),INDEX_PLAYER);
	
	GLenum err = glGetError();
	if(err != GL_NO_ERROR){
                printf("buggy bug, player shader: %d\n", err);
        }

	float worldX;
	float worldY;
	
	tileToWorld(playerPosX,playerPosY,&worldX,&worldY,tm);

	glUniform3f(glGetUniformLocation(player.shaderHandle, "posTransform"), worldX,worldY,0);
	glUniform1i(glGetUniformLocation(player.shaderHandle, "direction"), 0);
	//end shader
	
	//VBO
	glGenBuffers(1,&playerVBOHandle);
	
	float rh = ratioedHeight(tm.width, tm.height);
        float rw = ratioedWidth(tm.width, tm.height); 
	
	float vertexData[4*2*2] = {
		-1, 1-2/rh, 0, 0.25,	
		2/rw-1, 1-2/rh, 0.5, 0.25,	
		2/rw-1, 1, 0.5, 0,	
		-1, 1, 0, 0	

	};

	glBindBuffer(GL_ARRAY_BUFFER, playerVBOHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	//texture
	
	glUseProgram(player.shaderHandle);
	glUniform1i(glGetUniformLocation(player.shaderHandle, "tex"), INDEX_PLAYER);

	//done
	
	*getMobileObject(playerPosX,playerPosY) = player;
	
	err = glGetError();
	if(err != GL_NO_ERROR){
		printf("buggy bug, player creation : %d\n", err);
	}


}




//move this somewhere else ---------------------------------------------------------------
static void loadTile(tileMap tm, int posx, int posy, float *tx, float *ty){
	/**
	 *tile at position and returns texture pos in tx and ty
	 * */
	
	tile t = *getTile(tm, posx,posy);;


	if(t.isPushable){
		createCrate(tm,posx, posy); 
	}

	if(t.isTarget) {
		*tx = 0;
		*ty = 0;
	}
	else if(!t.isWalkable && !t.isPushable) {
		*tx = 0.5;
		*ty = 0.5;
	}
	else{
		*tx = 0;
		*ty = 0.5;
	}
}
static void loadTexture(const char *pathToImg, int texIndex){

	int width, height, chans;
	unsigned char *data = stbi_load(pathToImg, &width, &height, &chans, 0);
	glActiveTexture(GL_TEXTURE0 + texIndex);
	glBindTexture(GL_TEXTURE_2D, texHandle[texIndex]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	stbi_image_free(data);
}



//inputs ----------------------------------------------------------------------------------

static void setGUICursorPos(int x, int y, tileMap tm){

	float rh = ratioedHeight(tm.width, tm.height);
        float rw = ratioedWidth(tm.width, tm.height); 
	glUseProgram(cursorShaderHandle);
	glUniform3f(glGetUniformLocation(cursorShaderHandle, "posTransform"),(float)x*2/rw,(float)-y*2/rh,0);

}

static void doInputs(tileMap tm){
	if(currentGameMode != MENU){
		if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) registerInput(0);
		else if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) registerInput(1);
		else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) registerInput(2);
		else if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) registerInput(3);
		else if(glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) registerInput(8);
		if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) registerInput(119);
		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) registerInput(113);
		if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) registerInput(99);
		if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) registerInput(116);
		if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) registerInput(115);
		if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) registerInput(120);
		if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) registerInput(10);
	}

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	mouseX /= width;
	mouseY /= height;

	float rh = ratioedHeight(tm.width, tm.height);
	float rw = ratioedWidth(tm.width, tm.height);
	
	if(mouseX > 0 && mouseX < 1 && mouseY > 0 && mouseY < 1){ //mouse on window
		int tileX = (float)mouseX*rw;
		int tileY = (float)mouseY*rh;

		setGUICursorPos(tileX, tileY, tm);
		tileX = tileX-(rw-tm.width)/2+1; //compensating for water
		tileY = tileY-(rh-tm.height)/2;
		if(tileX >= 0 && tileX < tm.width && tileY >= 0 && tileY < tm.height)
			setCursor(tileX,tileY);
		//button hover 
		static button *last;
		button *b = getButtonAt((mouseX*2)-1, -((mouseY*2)-1));
		if(b != last){
			if(b != NULL){ //set 
				glUseProgram(b->shaderHandle);
				glUniform1i(glGetUniformLocation(b->shaderHandle, "animFrame"), 1);
			}
			if(last != NULL){ //reset
				glUseProgram(last->shaderHandle);
				glUniform1i(glGetUniformLocation(last->shaderHandle, "animFrame"), 0);
			}
		}
		last = b;
		

		//clickity click
		if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
			if(b != NULL){
				b->method();
			}
		}
	}
}

//RENDER ---------------------------------------------------------------------------

static void setVAO(){
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE, 4*sizeof(float), (void*)0);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
}

static void drawChunks(tileMap tm){
	/**
	 * Refreshes tilemap
	 *
	 **/
//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkEBOHandle);
	glUseProgram(tileShaderHandle);
	
	chunk *c = chunkList;
	while(c->next != NULL){
		
		glBindBuffer(GL_ARRAY_BUFFER, c->VBOHandle);
		setVAO();
		glDrawElements(GL_TRIANGLES,16*16*6,GL_UNSIGNED_INT,0);
		c = c->next;
	}


	GLenum err = glGetError();
	if(err != GL_NO_ERROR){
                printf("buggy bug, tilemap draw : %d\n", err);
        }

}

static void drawCursor(){

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	
	glBindBuffer(GL_ARRAY_BUFFER, cursorVBOHandle);
	setVAO();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tileEBOHandle);
	glUseProgram(cursorShaderHandle);

	GLenum err = glGetError();
	if(err != GL_NO_ERROR){
                printf("buggy bug, cursor draw start : %d\n", err);
        }

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

     	err = glGetError();
        if(err != GL_NO_ERROR){
                printf("buggy bug, cursor drew elements : %d\n", err);
        }
}

static void drawCrates(tileMap tm){
	if(mobileObjectMap == NULL){
		printf("Attempted to draw crates but mobileObjectMap is not initialized, we're done for\n");
		return;
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, crateVBOHandle);
	setVAO();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tileEBOHandle);

	for(int x = 0; x < tm.width; x++){
		for(int y = 0; y < tm.height; y++){
			mobileObject mobj = *getMobileObject(x,y); 
			if(mobj.type != TYPE_CRATE) continue;

			glUseProgram(mobj.shaderHandle);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);
			
     			GLenum err = glGetError();
        		if(err != GL_NO_ERROR){
                		printf("buggy bug, crate drew elements : %d\n", err);
        		}
		}
	}
}

static void drawButtons(){
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tileEBOHandle);
	glActiveTexture(GL_TEXTURE0 + INDEX_BUTTONS);	
	for(int i = 0; i < AMMOUNT_OF_BUTTONS; i++){
		button *b = allButtons[i];
		if(b != NULL && b->enabled){
			glBindBuffer(GL_ARRAY_BUFFER, b->VBOHandle);
			setVAO();
	
			glUseProgram(b->shaderHandle);
			glBindTexture(GL_TEXTURE_2D, b->texHandle);
			glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
     			GLenum err = glGetError();
        		if(err != GL_NO_ERROR){
                		printf("buggy bug, buttons drew elements : %d\n", err);
        		}
		}
	}
}

static void drawPlayer(){
	if(mobileObjectMap == NULL){
		printf("Attempted to draw crates but mobileObjectMap is not initialized, we're done for\n");
		return;
	}
	
	int playerPosX, playerPosY;
	getPlayerPosition(&playerPosX, &playerPosY);
	
	mobileObject player = *getMobileObject(playerPosX,playerPosY);

	glUseProgram(player.shaderHandle);

	glBindBuffer(GL_ARRAY_BUFFER, playerVBOHandle);
	setVAO();
	glBindBuffer(GL_ARRAY_BUFFER, tileEBOHandle);

	glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
	GLenum err = glGetError();
	if(err != GL_NO_ERROR){
                	printf("buggy bug, player drew elements : %d\n", err);
        	}
	}


static void drawAll(tileMap tm){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.5f,0.3f,0.5f, 5.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	if(currentGameMode == MENU){
		
	}
	else{
		drawChunks(tm);
		drawCrates(tm);
		drawPlayer();
		drawCursor();
	}
	drawButtons();
	glfwSwapBuffers(window);
	
	glDisable(GL_BLEND);
}



static void framebuffer_size_callback(GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
}


//TILEMAP ----------------------------------------------------------------------------------
static void initChunkEBO(){
	
	int indices[16*16*6];
	int index = 0;
	int indice = 0;
	for(int x = 0; x < 16; x++){
		for(int y = 0; y < 16; y++){
			indices[index] = indice+0;
			indices[index+1] = indice+1;
			indices[index+2] = indice+2;
			indices[index+3] = indice+0;
			indices[index+4] = indice+2;
			indices[index+5] = indice+3;
			index+=6;
			indice+=4;
		}
	}

	glGenBuffers(1, &chunkEBOHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkEBOHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),indices, GL_STATIC_DRAW);

}


static void initTileShader(){

 	int success;
        char infoLog[512];
        unsigned int vertexHandle;
        vertexHandle = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(vertexHandle, 1, &standardVertexShaderSource, NULL);
        glCompileShader(vertexHandle);

        glGetShaderiv(vertexHandle, GL_COMPILE_STATUS, &success);
        if(!success){
                glGetShaderInfoLog(vertexHandle, 512, NULL, infoLog);
                printf("Vertex is fucked : %s\n", infoLog);
        }


        unsigned int fragmentHandle;
        fragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentHandle, 1, &standardFragmentShaderSource, NULL);
        glCompileShader(fragmentHandle);

        glGetShaderiv(fragmentHandle, GL_COMPILE_STATUS, &success);
        if(!success){
                glGetShaderInfoLog(fragmentHandle, 512, NULL, infoLog);
                printf("Fragment is fucked : %s\n", infoLog);
        }
        
        tileShaderHandle = glCreateProgram();
        glAttachShader(tileShaderHandle, vertexHandle);
        glAttachShader(tileShaderHandle, fragmentHandle);
        glLinkProgram(tileShaderHandle);
}


static float tileToNorm(int tilePos, int size) {return ((float)tilePos*2/(float)size) - 1;}

static void initChunksVBOs(tileMap tm){

	int rh = ratioedHeight(tm.width, tm.height);
	int rw = ratioedWidth(tm.width, tm.height);
	
	//TODO - free chunklist correctly
	chunk *newList = malloc(sizeof(chunk));
	chunk *c = newList;
	for(int cy = 0; cy < rh; cy+=16){
		for(int cx = 0; cx < rw; cx +=16){
			float vertexData[4*2*2*16*16];
			int i = 0;
			for(int x = cx; x < (cx+16); x++){
				for(int y = cy; y < (cy+16); y++){	
					float tx;
					float ty;

						if(x < tm.width && x >= 0 && y < tm.height && y >= 0)
						loadTile(tm,x,tm.height - y - 1, &tx,&ty);
					else{
						tx = 0.5;
						ty = 0;
					}	
					
					//centering and cycling around if not already out
					int centeredX = ((rw == tm.width || x >= rw) ? x : (int)((x + (int)((float)(rw-tm.width)/2))% rw ));
		
					int centeredY = ((rh == tm.height || y < rh) ? y : (int)(y+ (float)tm.height*(16*2)/9) % rh );

						//bot left 
						 vertexData[i] = tileToNorm(centeredX, rw);
						 vertexData[i+1] =tileToNorm(centeredY, rh);
						 vertexData[i+2] =tx;
						 vertexData[i+3] =ty;
						 i+=4;
						 
						 //bot right
						 vertexData[i] =tileToNorm(centeredX+1, rw);
						 vertexData[i+1] =tileToNorm(centeredY, rh);
						 vertexData[i+2] =tx+0.5;
						 vertexData[i+3] =ty;
						 i+=4;;

						 //top right
						 vertexData[i] =tileToNorm(centeredX+1, rw);
						 vertexData[i+1] =tileToNorm(centeredY+1, rh);
						 vertexData[i+2] =tx+0.5;
						 vertexData[i+3] =ty-0.5;
						 i+=4;	

						 //top left
						 vertexData[i] =tileToNorm(centeredX, rw);
						 vertexData[i+1] =tileToNorm(centeredY+1, rh);
						 vertexData[i+2] =tx;
						 vertexData[i+3] = ty-0.5;
						 i+=4;
				}
			}	
			glGenBuffers(1, &c->VBOHandle);
			glBindBuffer(GL_ARRAY_BUFFER, c->VBOHandle);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
				c->next = malloc(sizeof(chunk));
				c = c->next;
		}
	}
	c->next = NULL;
	chunkList = newList;
}
//Cursor------------------------------------------------------------------------------------

static void initCursorVBO(tileMap tm){
	glGenBuffers(1, &cursorVBOHandle);

	float rh = ratioedHeight(tm.width, tm.height);
        float rw = ratioedWidth(tm.width, tm.height); 

	float vertexData[4*2*2] = { // at 0 0
		-1,1,0,0,
		-1+2/rw, 1, 0.5, 0,
		-1+2/rw, 1-2/rh, 0.5,1,
		-1, 1-2/rh,0,1
	};
	glBindBuffer(GL_ARRAY_BUFFER, cursorVBOHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
}

static void initTileEBO(){
	glGenBuffers(1, &tileEBOHandle);                                      
        unsigned int indices[6] = {0,1,2,0,2,3};
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tileEBOHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),indices, GL_STATIC_DRAW);

}

// MENU ----------------------------------------------------------------------------------

static void initScreenOverlay(){
	//VBO
	glGenBuffers(1, &screenOverlayVBOHandle);

	float vertexData[4*2*2] = {
		0,0,0,1,
		1,0,1,1,
		1,1,1,0,
		0,1,0,0
	};

	glBindBuffer(GL_ARRAY_BUFFER, screenOverlayVBOHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexData),vertexData, GL_STATIC_DRAW);
	
	
}



static void setButton(button *b, int index) {allButtons[index] = b;}

static void initButton(button *b){
        /**
         *creates GL handles for button
         * */



        //VBO
        glGenBuffers(1, &(b->VBOHandle));

        float vertexData[4*2*2] = {
                -(b->sizeX)/2,-(b->sizeY/2), 0,1,
                (b->sizeX)/2,-(b->sizeY)/2, 0.5,1,
                (b->sizeX)/2,(b->sizeY)/2, 0.5,0,
                -(b->sizeX)/2,(b->sizeY)/2, 0,0
        };        
        glBindBuffer(GL_ARRAY_BUFFER, b->VBOHandle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	
	
	makeTransformedAnimatedShader(INDEX_BUTTONS, &(b->shaderHandle));

	glActiveTexture(GL_TEXTURE0 + INDEX_BUTTONS);
	glGenTextures(1, &(b->texHandle));
	glBindTexture(GL_TEXTURE_2D, b->texHandle);

	int width, height, chans;
	unsigned char *data = stbi_load(b->pathToImg, &width, &height, &chans, 0);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	stbi_image_free(data);
	
	glUseProgram(b->shaderHandle);
	glUniform1i(glGetUniformLocation(b->shaderHandle, "animFrame"),0);
	glUniform3f(glGetUniformLocation(b->shaderHandle, "posTransform"),b->centerX, b->centerY, 0);
}

static void closeMainMenu(){
	allButtons[BUTTON_PLAY_STORY]->enabled = false;
	allButtons[BUTTON_PLAY_CUSTOM]->enabled = false;
}

static void playStoryButtonPress(){
	currentGameMode = PLAY;
	closeMainMenu();
}

static void playCustomButtonPress(){
	printf("PLAY CUSTOM !\n");
}


static void initMainMenu(){
	button *playStory = malloc(sizeof(button));
	playStory->sizeX = 0.75;
	playStory->sizeY = 0.3;
	playStory->centerX = 0;
	playStory->centerY = 0.6;
	playStory->pathToImg = "src/graphics/playStoryButton.png";
	playStory->method = &playStoryButtonPress;
	initButton(playStory);
	setButton(playStory, BUTTON_PLAY_STORY);
	
	button *playCustom = malloc(sizeof(button));
	playCustom->sizeX = 0.75;
	playCustom->sizeY = 0.3;
	playCustom->centerX = 0;
	playCustom->centerY = 0;
	playCustom->pathToImg = "src/graphics/playCustomButton.png";
	playCustom->method = &playCustomButtonPress;
	initButton(playCustom);
	setButton(playCustom, BUTTON_PLAY_CUSTOM);
	
}

static void openMainMenu(){
	currentGameMode = MENU;
	allButtons[BUTTON_PLAY_STORY]->enabled = true;
	allButtons[BUTTON_PLAY_CUSTOM]->enabled = true;
}


// GLOBAL INITS----------------------------------------------------------------------------

static void initVAO(){
	glGenVertexArrays(1, &VAOHandle);
	glBindVertexArray(VAOHandle);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}


static void initTextures(){

	glGenTextures(4, texHandle);
	loadTexture("src/graphics/tileset1.png",INDEX_TILESET);
	loadTexture("src/graphics/cursor.png", INDEX_CURSOR);
	loadTexture("src/graphics/crate.png", INDEX_CRATE);
	loadTexture("src/graphics/snail.png", INDEX_PLAYER);
	glUseProgram(tileShaderHandle);
	glUniform1i(glGetUniformLocation(tileShaderHandle, "tex"),INDEX_TILESET);
	glUseProgram(cursorShaderHandle);
	glUniform1i(glGetUniformLocation(cursorShaderHandle, "tex"),INDEX_CURSOR);
}

static void animateFrame(){
	static int tileFrame = 0;
	static int cursorFrame = 0;
	static char *tilesets[] = {"src/graphics/tileset1.png","src/graphics/tileset2.png","src/graphics/tileset3.png"};
	//cursor	
	glUseProgram(cursorShaderHandle);
	glUniform1i(glGetUniformLocation(cursorShaderHandle, "animFrame"), cursorFrame++);
	
	//player
	int playerPosX, playerPosY;
	getPlayerPosition(&playerPosX, &playerPosY);
	mobileObject *player = getMobileObject(playerPosX,playerPosY);
	if(player != NULL){
		glUseProgram(player->shaderHandle);
		glUniform1i(glGetUniformLocation(player->shaderHandle, "animFrame"), cursorFrame);
		
	}


	if(cursorFrame == 2) cursorFrame = 0;
	loadTexture(tilesets[tileFrame++], INDEX_TILESET);
	if(tileFrame == 3) tileFrame = 0;
}

void renderGUI(){
	/**
	 *renders a frame
	 * */
	static int animCount = 0;
	
	glfwPollEvents();
	if(mobileObjectMovementQueue == NULL) doInputs(currentTileMap);


	drawAll(currentTileMap);
	processMobileObjects(currentTileMap);
	

	if(animCount++ == 30){
		animCount = 0;
		animateFrame();
	}
	return;
}


int initGUI(){
	/**
	 *inits window (glfw) and context
	 * */
	glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        window = glfwCreateWindow(1920,1080, "Projet PX", NULL, NULL);
        if(window == NULL){
                printf("Yo what, window failed to create");
                glfwTerminate();
                return -1;
        }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
                printf("Glad is borked");
                return -1;
        }   

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	        
	initVAO();
        initTileShader();
	makeTransformedAnimatedShader(INDEX_CURSOR, &cursorShaderHandle);
	
	initMainMenu();
	openMainMenu();

        GLenum err = glGetError();
        if(err != GL_NO_ERROR){
                printf("buggy bug, init: %d\n", err);
        	return -1;
	}
        
	return 0;

}

int updateGUIMap(tileMap tm){
	/**
	 * Sends tileMap to gui
	 * */
        if(!window){
		printf("initGUI() before please\n");
	}
	
	initMobileObjectMap(tm);
	
	initChunksVBOs(tm);
	initChunkEBO();

        initCursorVBO(tm);
        initTileEBO();
       
	initCrateVBO(tm);
	initPlayer(tm);

	initTextures();
        
	GLenum err = glGetError();
        if(err != GL_NO_ERROR){
                printf("buggy bug, map init: %d\n", err);
        	return -1;
	}
        
        currentTileMap = tm;
	
	return 0;

}

void closeGUI(){
	freeMobileObjectMap(currentTileMap);
	for(int i = 0; i < AMMOUNT_OF_BUTTONS; i++){
		free(allButtons[i]);
	}
}

