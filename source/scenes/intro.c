#include "intro.h"

Scene introScene = {
	.initialize = &introInitialize,
	.intro = 0,
	.normal = &introNormal,
	.pause = 0,
	.outro = 0,
	.end = &introEnd,
};

void introInitialize(){
	
	//disable every sprite
	
}

void introNormal(){

}

void introEnd(){
	currentScene.scenePointer = sceneList[GAMEPLAY];
	currentScene.state = INITIALIZE;
}