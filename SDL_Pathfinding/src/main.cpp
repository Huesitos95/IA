#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

//Render
#include "SDL_SimpleApp.h"

//Scenes
#include "ScenePathFindingBFS.h"
#include "ScenePathFindingDijkstra.h"
#include "ScenePathFindingGreedyBFS.h"
#include "ScenePathFindingAestrella.h"

#define FRAMES_PER_SEC 60

using namespace std;

int main(int argc, char ** argv)
{
	bool quit = false;
	SDL_Event event;
	int this_tick = 0;
	int next_tick = 1000/FRAMES_PER_SEC;

	SDL_SimpleApp *app = SDL_SimpleApp::Instance();

	Scene *curr_scene = new ScenePathFindingBFS;
	app->setWindowTitle(curr_scene->getTitle());

	while (!quit)
	{
		this_tick = SDL_GetTicks();
		//if (this_tick < next_tick)
		//{
		//	SDL_Delay(next_tick - this_tick);  //Podem borrar el Delay si el consum de CPU és massa alt.
		//}
		next_tick = this_tick + (1000 / FRAMES_PER_SEC);

		// run app frame by frame
		event = app->run(curr_scene);

		/* Keyboard events */
		switch (event.type)
		{
		case SDL_KEYDOWN:
			if (event.key.keysym.scancode == SDL_SCANCODE_1)
			{
				delete(curr_scene);
				curr_scene = new ScenePathFindingBFS;
				app->setWindowTitle(curr_scene->getTitle());
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_2)
			{
				delete(curr_scene);
				curr_scene = new ScenePathFindingDijkstra;
				app->setWindowTitle(curr_scene->getTitle());
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_3)
			{
				delete(curr_scene);
				curr_scene = new ScenePathFindingGreedyBFS;
				app->setWindowTitle(curr_scene->getTitle());
			}

			if (event.key.keysym.scancode == SDL_SCANCODE_4)
			{
				delete(curr_scene);
				curr_scene = new ScenePathFindingAestrella;
				app->setWindowTitle(curr_scene->getTitle());
			}
			if ((event.key.keysym.scancode == SDL_SCANCODE_Q) || (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
			{
				quit = true;
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_F)
			{
				app->setFullScreen();
			}
			break;
		case SDL_QUIT:
			quit = true;
			break;
		}

	}

	return 0;
}