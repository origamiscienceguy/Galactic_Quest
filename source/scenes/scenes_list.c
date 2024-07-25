#include "scenes.h"

extern Scene gameplayScene;
extern Scene introScene;
extern Scene mainMenuScene;

const Scene *sceneList [] = {
	&gameplayScene, &introScene, &mainMenuScene
};