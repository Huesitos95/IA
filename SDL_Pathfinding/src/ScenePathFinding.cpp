#include "ScenePathFinding.h"

using namespace std;

ScenePathFinding::ScenePathFinding()
{
	draw_grid = false;

	num_cell_x = SRC_WIDTH / CELL_SIZE;
	num_cell_y = SRC_HEIGHT / CELL_SIZE;
	initMaze();
	initGraph();
	loadTextures("../res/maze.png", "../res/coin.png");

	srand((unsigned int)time(NULL));

	Agent *agent = new Agent;
	agent->loadSpriteTexture("../res/soldier.png", 4);
	agents.push_back(agent);


	// set agent position coords to the center of a random cell
	Vector2D rand_cell(-1,-1);
	while (!isValidCell(rand_cell)) 
		rand_cell = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));
	agents[0]->setPosition(cell2pix(rand_cell));

	
	// set the coin in a random cell (but at least 3 cells far from the agent)
	coinPosition = Vector2D(-1,-1);
	while ((!isValidCell(coinPosition)) || (Vector2D::Distance(coinPosition, rand_cell)<3)) 
		coinPosition = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));
	
	BFS();
	// PathFollowing next Target
	currentTarget = Vector2D(0, 0);
	currentTargetIndex = -1;

	
}

ScenePathFinding::~ScenePathFinding()
{
	if (background_texture)
		SDL_DestroyTexture(background_texture);
	if (coin_texture)
		SDL_DestroyTexture(coin_texture);

	for (int i = 0; i < (int)agents.size(); i++)
	{
		delete agents[i];
	}

	for (int i = 0; i < num_cell_x; i++)
	{
		for (int j = 0; j < num_cell_y; j++)
		{
			delete nodes[i][j];
		}
	}
}

void ScenePathFinding::update(float dtime, SDL_Event *event)
{
	/* Keyboard & Mouse events */
	switch (event->type) {
	case SDL_KEYDOWN:
		if (event->key.keysym.scancode == SDL_SCANCODE_SPACE)
			draw_grid = !draw_grid;
		break;
	case SDL_MOUSEMOTION:
	case SDL_MOUSEBUTTONDOWN:
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			Vector2D cell = pix2cell(Vector2D((float)(event->button.x), (float)(event->button.y)));
			if (isValidCell(cell))
			{
				if (path.points.size() > 0)
					if (path.points[path.points.size() - 1] == cell2pix(cell))
						break;

				path.points.push_back(cell2pix(cell));
			}
		}
		break;
	default:
		break;
	}
	if ((currentTargetIndex == -1) && (path.points.size()>0))
		currentTargetIndex = 0;

	if (currentTargetIndex >= 0)
	{	
		float dist = Vector2D::Distance(agents[0]->getPosition(), path.points[currentTargetIndex]);
		if (dist < path.ARRIVAL_DISTANCE)
		{
			if (currentTargetIndex == path.points.size() - 1)
			{
				if (dist < 3)
				{
					path.points.clear();
					currentTargetIndex = -1;
					agents[0]->setVelocity(Vector2D(0,0));
					// if we have arrived to the coin, replace it ina random cell!
					if (pix2cell(agents[0]->getPosition()) == coinPosition)
					{
						coinPosition = Vector2D(-1, -1);
						while ((!isValidCell(coinPosition)) || (Vector2D::Distance(coinPosition, pix2cell(agents[0]->getPosition()))<3))
							coinPosition = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));
						
					}
				}
				else
				{
					//Vector2D steering_force = agents[0]->Behavior()->Arrive(agents[0], currentTarget, path.ARRIVAL_DISTANCE, dtime);
					Vector2D algorithm = agents[0]->Behavior()->Arrive(agents[0], currentTarget, path.ARRIVAL_DISTANCE, dtime);
					agents[0]->update(algorithm, dtime, event);
				}
				return;
			}
			currentTargetIndex++;
		}

		currentTarget = path.points[currentTargetIndex];
		Vector2D steering_force = agents[0]->Behavior()->Seek(agents[0], currentTarget, dtime);
		agents[0]->update(steering_force, dtime, event);
	} 
	else
	{
		agents[0]->update(Vector2D(0,0), dtime, event);
		/*Vector2D algorithm = agents[0]->Behavior()->Seek(agents[0], coinPosition, dtime);
		agents[0]->update(algorithm, dtime, event);*/

	}



}

void ScenePathFinding::draw()
{
	drawMaze();
	drawCoin();


	if (draw_grid)
	{
		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 255, 255, 255, 127);
		for (int i = 0; i < SRC_WIDTH; i+=CELL_SIZE)
		{
			SDL_RenderDrawLine(TheApp::Instance()->getRenderer(), i, 0, i, SRC_HEIGHT);
		}
		for (int j = 0; j < SRC_HEIGHT; j = j += CELL_SIZE)
		{
			SDL_RenderDrawLine(TheApp::Instance()->getRenderer(), 0, j, SRC_WIDTH, j);
		}
	}

	for (int i = 0; i < (int)path.points.size(); i++)
	{
		draw_circle(TheApp::Instance()->getRenderer(), (int)(path.points[i].x), (int)(path.points[i].y), 15, 255, 255, 0, 255);
		if (i > 0)
			SDL_RenderDrawLine(TheApp::Instance()->getRenderer(), (int)(path.points[i - 1].x), (int)(path.points[i - 1].y), (int)(path.points[i].x), (int)(path.points[i].y));
	}

	draw_circle(TheApp::Instance()->getRenderer(), (int)currentTarget.x, (int)currentTarget.y, 15, 255, 0, 0, 255);

	agents[0]->draw();
}

const char* ScenePathFinding::getTitle()
{
	return "SDL Steering Behaviors :: BFS Demo";
}

void ScenePathFinding::drawMaze()
{
	if (draw_grid)
	{

		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 0, 0, 255, 255);
		for (unsigned int i = 0; i < maze_rects.size(); i++)
			SDL_RenderFillRect(TheApp::Instance()->getRenderer(), &maze_rects[i]);
	}
	else
	{
		SDL_RenderCopy(TheApp::Instance()->getRenderer(), background_texture, NULL, NULL );
	}
}

void ScenePathFinding::drawCoin()
{
	Vector2D coin_coords = cell2pix(coinPosition);
	int offset = CELL_SIZE / 2;
	SDL_Rect dstrect = {(int)coin_coords.x-offset, (int)coin_coords.y - offset, CELL_SIZE, CELL_SIZE};
	SDL_RenderCopy(TheApp::Instance()->getRenderer(), coin_texture, NULL, &dstrect);
}

void ScenePathFinding::initMaze()
{

	// Initialize a list of Rectagles describing the maze geometry (useful for collision avoidance)
	SDL_Rect rect = { 0, 0, 1280, 32 };
	maze_rects.push_back(rect);
	rect = { 608, 32, 64, 32 };
	maze_rects.push_back(rect);
	rect = { 0, 736, 1280, 32 };
	maze_rects.push_back(rect);
	rect = { 608, 512, 64, 224 }; 
	maze_rects.push_back(rect);
	rect = { 0,32,32,288 };
	maze_rects.push_back(rect);
	rect = { 0,416,32,320 };
	maze_rects.push_back(rect);
	rect = { 1248,32,32,288 };
	maze_rects.push_back(rect);
	rect = { 1248,416,32,320 };
	maze_rects.push_back(rect);
	rect = { 128,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 288,128,96,32 };
	maze_rects.push_back(rect);
	rect = { 480,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 736,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 896,128,96,32 };
	maze_rects.push_back(rect);
	rect = { 1088,128,64,32 };
	maze_rects.push_back(rect);
	rect = { 128,256,64,32 };
	maze_rects.push_back(rect);
	rect = { 288,256,96,32 };
	maze_rects.push_back(rect);
	rect = { 480, 256, 320, 32 };
	maze_rects.push_back(rect);
	rect = { 608, 224, 64, 32 }; 
	maze_rects.push_back(rect);
	rect = { 896,256,96,32 };
	maze_rects.push_back(rect);
	rect = { 1088,256,64,32 };
	maze_rects.push_back(rect);
	rect = { 128,384,32,256 };
	maze_rects.push_back(rect);
	rect = { 160,512,352,32 };
	maze_rects.push_back(rect);
	rect = { 1120,384,32,256 };
	maze_rects.push_back(rect);
	rect = { 768,512,352,32 };
	maze_rects.push_back(rect);
	rect = { 256,640,32,96 };
	maze_rects.push_back(rect);
	rect = { 992,640,32,96 };
	maze_rects.push_back(rect);
	rect = { 384,544,32,96 };
	maze_rects.push_back(rect);
	rect = { 480,704,32,32 };
	maze_rects.push_back(rect);
	rect = { 768,704,32,32 };
	maze_rects.push_back(rect);
	rect = { 864,544,32,96 };
	maze_rects.push_back(rect);
	rect = { 320,288,32,128 };
	maze_rects.push_back(rect);
	rect = { 352,384,224,32 };
	maze_rects.push_back(rect);
	rect = { 704,384,224,32 };
	maze_rects.push_back(rect);
	rect = { 928,288,32,128 };
	maze_rects.push_back(rect);

	// Initialize the terrain matrix (for each cell a zero value indicates it's a wall)
	
	// (1st) initialize all cells to 1 by default
	for (int i = 0; i < num_cell_x; i++)
	{
		vector<int> terrain_col(num_cell_y, 1); 
		terrain.push_back(terrain_col);
	}
	// (2nd) set to zero all cells that belong to a wall
	int offset = CELL_SIZE / 2;
	for (int i = 0; i < num_cell_x; i++)
	{
		for (int j = 0; j < num_cell_y; j++)
		{
			Vector2D cell_center ((float)(i*CELL_SIZE + offset), (float)(j*CELL_SIZE + offset));
			for (unsigned int b = 0; b < maze_rects.size(); b++)
			{
				if (Vector2DUtils::IsInsideRect(cell_center, (float)maze_rects[b].x, (float)maze_rects[b].y, (float)maze_rects[b].w, (float)maze_rects[b].h))
				{
					terrain[i][j] = 0;
				    break;
				}  
			}
			
		}
	}

}


void ScenePathFinding::initGraph() {

	for (int i = 0; i < num_cell_x; i++)
	{
		vector<Node*> node_col(num_cell_y, nullptr);
		nodes.push_back(node_col);
	}

	for (int i = 0; i < num_cell_x; i++)
	{

		for (int j = 0; j < num_cell_y; j++)
		{


			if (terrain[i][j] != 0) {

				Node *node = new Node(Vector2D(i, j));
				nodes[i][j] = node;

			}

			if (nodes[i][j] != nullptr) // Que el node no sigui una paret o no existeixi
			{
				if (i > 0)
				{
					if (nodes[i - 1][j] != nullptr) // Que el node ESQUERRA no sigui una paret o no existeixi
					{
						nodes[i][j]->veiEsquerra = nodes[i - 1][j]; // VINCULEM ELS 2 NODES, COM A VEI
					}
				}

				if (i < num_cell_x - 1)
				{
					if (nodes[i + 1][j] != nullptr) // Que el node DRETA no sigui una paret o no existeixi
					{
						nodes[i][j]->veiDreta = nodes[i + 1][j]; // VINCULEM ELS 2 NODES, COM A VEI
					}
				}

				if (j < num_cell_y - 1)
				{
					if (nodes[i][j + 1] != nullptr) // Que el node ABAIX no sigui una paret o no existeixi
					{
						nodes[i][j]->veiAbaix = nodes[i][j + 1]; // VINCULEM ELS 2 NODES, COM A VEI
					}
				}

				if (j > 0)
				{
					if (nodes[i][j - 1] != nullptr) // Que el node ADALT no sigui una paret o no existeixi
					{
						nodes[i][j]->veiAbaix = nodes[i][j - 1]; // VINCULEM ELS 2 NODES, COM A VEI
					}
				}
			}
		}
	}
	// TUNNELS
	nodes[0][10]->veiEsquerra = nodes[num_cell_x - 1][10]; // Primer tunels esquerra-dreta
	nodes[0][11]->veiEsquerra = nodes[num_cell_x - 1][11];
	nodes[0][12]->veiEsquerra = nodes[num_cell_x - 1][12];

	nodes[num_cell_x - 1][10]->veiDreta = nodes[0][10]; // Tunels dreta-esquerra
	nodes[num_cell_x - 1][11]->veiDreta = nodes[0][11];
	nodes[num_cell_x - 1][12]->veiDreta = nodes[0][12];
	
}

//void ScenePathFinding::Bridge() {
//
//	if (currentTarget == cell2pix(Vector2D{ 0,10 })) {
//		if (path.points[currentTargetIndex - 1] == cell2pix(Vector2D{ 39,10 }))
//			agents[0]->setPosition(cell2pix(Vector2D{ 0,10 }));
//	}
//	else if (currentTarget == cell2pix(Vector2D{ 0,11 })) {
//		if (path.points[currentTargetIndex - 1] == cell2pix(Vector2D{ 39,11 }))
//			agents[0]->setPosition(cell2pix(Vector2D{ 0,11 }));
//	}
//	else if (currentTarget == cell2pix(Vector2D{ 0,12 })) {
//		if (path.points[currentTargetIndex - 1] == cell2pix(Vector2D{ 39,12 }))
//			agents[0]->setPosition(cell2pix(Vector2D{ 0,12 }));
//	}
//	else if (currentTarget == cell2pix(Vector2D{ 39,10 })) {
//		if (path.points[currentTargetIndex - 1] == cell2pix(Vector2D{ 0,10 }))
//			agents[0]->setPosition(cell2pix(Vector2D{ 39,10 }));
//	}
//	else if (currentTarget == cell2pix(Vector2D{ 39,11 })) {
//		if (path.points[currentTargetIndex - 1] == cell2pix(Vector2D{ 0,11 }))
//			agents[0]->setPosition(cell2pix(Vector2D{ 39,11 }));
//	}
//	else if (currentTarget == cell2pix(Vector2D{ 39,12 })) {
//		if (path.points[currentTargetIndex - 1] == cell2pix(Vector2D{ 0,12 }))
//			agents[0]->setPosition(cell2pix(Vector2D{ 39,12 }));
//	}
//}

bool ScenePathFinding::loadTextures(char* filename_bg, char* filename_coin)
{
	SDL_Surface *image = IMG_Load(filename_bg);
	if (!image) {
		cout << "IMG_Load: " << IMG_GetError() << endl;
		return false;
	}
	background_texture = SDL_CreateTextureFromSurface(TheApp::Instance()->getRenderer(), image);

	if (image)
		SDL_FreeSurface(image);

	image = IMG_Load(filename_coin);
	if (!image) {
		cout << "IMG_Load: " << IMG_GetError() << endl;
		return false;
	}
	coin_texture = SDL_CreateTextureFromSurface(TheApp::Instance()->getRenderer(), image);

	if (image)
		SDL_FreeSurface(image);

	return true;
}

Vector2D ScenePathFinding::cell2pix(Vector2D cell)
{
	int offset = CELL_SIZE / 2;
	return Vector2D(cell.x*CELL_SIZE + offset, cell.y*CELL_SIZE + offset);
}

Vector2D ScenePathFinding::pix2cell(Vector2D pix)
{
	return Vector2D((float)((int)pix.x/CELL_SIZE), (float)((int)pix.y / CELL_SIZE));
}

bool ScenePathFinding::isValidCell(Vector2D cell)
{
	if ((cell.x < 0) || (cell.y < 0) || (cell.x >= terrain.size()) || (cell.y >= terrain[0].size()) )
		return false;
	return !(terrain[(unsigned int)cell.x][(unsigned int)cell.y] == 0);
}

//funci�n para checkear si un nodo concreto est� contenido en el vector
bool ScenePathFinding::CheckVector(Node* node, std::vector<Node*> vec) {
	for (unsigned int i = 0; i < vec.size(); i++) {
		if (vec[i] == node)
		{
			return true;
		}
	}
	return false;
}

void ScenePathFinding::BFS()
{

	//Iniciem la frontera amb el node del agent
	frontera.push_back(nodes[pix2cell(agents[0]->getPosition()).x][pix2cell(agents[0]->getPosition()).y]);

	while (frontera.size() != 0)
	{
		// agafem el node de la frontera
		Node * actual = frontera.front();

		//Comprobar si estem a la meta de la moneda
		if (actual->position == coinPosition)
		{
			// hem arribat, tanquem el bucle
			break;
		}
		else // NO estem al objectiu, recorrem veins
		{
			if (CheckVector(actual,visited))
			{
				frontera.erase(frontera.begin());
			}
			else
			{
				frontera.erase(frontera.begin());

				if (actual->veiAdalt != nullptr && !CheckVector(actual->veiAdalt, visited))
				{
					actual->veiAdalt->fromNode = actual;
					frontera.push_back(actual->veiAdalt);
					
				}

				if (actual->veiDreta != nullptr && !CheckVector(actual->veiDreta, visited))
				{
					actual->veiDreta->fromNode = actual;
					frontera.push_back(actual->veiDreta);
					
				}

				if (actual->veiEsquerra != nullptr && !CheckVector(actual->veiEsquerra, visited))
				{
					actual->veiEsquerra->fromNode = actual;
					frontera.push_back(actual->veiEsquerra);
					
				}

				if (actual->veiAbaix != nullptr && !CheckVector(actual->veiAbaix, visited))
				{
					actual->veiAbaix->fromNode = actual;
					frontera.push_back(actual->veiAbaix);
					
				}

				visited.push_back(actual);
			}
		}
	}

	if (!frontera.empty())
	{
		//montem el cami a seguir a la inversa.
		Node * actual = frontera.front();


		reversepath.push_back(actual);
		Vector2D pos = pix2cell(agents[0]->getPosition());
		while (actual->position != pos)
		{
			// Tornar al fromNode i posarlo com a objectiu actual
			actual = actual->fromNode;
			// Afegirlo al reversePath que hem de seguir
			reversepath.push_back(actual);
		}

	}
		
	for (unsigned int i = 0; i < reversepath.size(); i++)
	{
		path.points.insert(path.points.begin(), cell2pix(reversepath[i]->position));
	}
}


