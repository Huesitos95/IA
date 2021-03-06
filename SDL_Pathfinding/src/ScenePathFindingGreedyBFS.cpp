#include "ScenePathFindingGreedyBFS.h"
#include <queue>

using namespace std;

ScenePathFindingGreedyBFS::ScenePathFindingGreedyBFS()
{
	draw_grid = false;
	start = false;

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
	Vector2D rand_cell(-1, -1);
	while (!isValidCell(rand_cell))
		rand_cell = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));
	agents[0]->setPosition(cell2pix(rand_cell));

	// set the coin in a random cell (but at least 3 cells far from the agent)
	coinPosition = Vector2D(-1, -1);
	while ((!isValidCell(coinPosition)) || (Vector2D::Distance(coinPosition, rand_cell)<3))
		coinPosition = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));

	// PathFollowing next Target
	currentTarget = Vector2D(0, 0);
	currentTargetIndex = -1;
}

ScenePathFindingGreedyBFS::~ScenePathFindingGreedyBFS()
{
	if (background_texture)
		SDL_DestroyTexture(background_texture);
	if (coin_texture)
		SDL_DestroyTexture(coin_texture);

	for (int i = 0; i < (int)agents.size(); i++)
	{
		delete agents[i];
	}


}

void ScenePathFindingGreedyBFS::update(float dtime, SDL_Event *event)
{
	/* Keyboard & Mouse events */
	switch (event->type) {
	case SDL_KEYDOWN:
		if (event->key.keysym.scancode == SDL_SCANCODE_SPACE)
			draw_grid = !draw_grid;
		if (event->key.keysym.scancode == SDL_SCANCODE_Z)
		{
			principi = true;
			start = false;
		}

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
	//Per iniciar el GreeedyBFS
	if (principi) {
		inici.position.x = pix2cell(agents[0]->getPosition()).x;
		inici.position.y = pix2cell(agents[0]->getPosition()).y;
		objectiu.position.x = coinPosition.x;
		objectiu.position.y = coinPosition.y;
		std::cout << "start" << endl;
		GBFS(objectiu, inici);
		principi = false;
	}
	if (start) {
		inici.position.x = pix2cell(agents[0]->getPosition()).x;
		inici.position.y = pix2cell(agents[0]->getPosition()).y;
		objectiu.position.x = coinPosition.x;
		objectiu.position.y = coinPosition.y;
		std::cout << "finished" << endl;
		GBFS(objectiu, inici);
		start = false;
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
					agents[0]->setVelocity(Vector2D(0, 0));
					// if we have arrived to the coin, replace it ina random cell!
					if (pix2cell(agents[0]->getPosition()) == coinPosition)
					{
						coinPosition = Vector2D(-1, -1);

						while ((!isValidCell(coinPosition)) || (Vector2D::Distance(coinPosition, pix2cell(agents[0]->getPosition()))<3))

							coinPosition = Vector2D((float)(rand() % num_cell_x), (float)(rand() % num_cell_y));
					}
					start = true;
				}
				else
				{
					Vector2D steering_force = agents[0]->Behavior()->Arrive(agents[0], currentTarget, path.ARRIVAL_DISTANCE, dtime);
					//Vector2D algorithm = agents[0]->Behavior()->Seek(agents[0], coinPosition, dtime);
					agents[0]->update(steering_force, dtime, event);
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
		agents[0]->update(Vector2D(0, 0), dtime, event);
		/*Vector2D algorithm = agents[0]->Behavior()->Seek(agents[0], coinPosition, dtime);
		agents[0]->update(algorithm, dtime, event);*/

	}
}

void ScenePathFindingGreedyBFS::draw()
{
	drawMaze();
	drawCoin();


	if (draw_grid)
	{
		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 255, 255, 255, 127);
		for (int i = 0; i < SRC_WIDTH; i += CELL_SIZE)
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

const char* ScenePathFindingGreedyBFS::getTitle()
{
	return "SDL Steering Behaviors :: BFS Demo";
}

void ScenePathFindingGreedyBFS::drawMaze()
{
	if (draw_grid)
	{

		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 0, 0, 255, 255);
		for (unsigned int i = 0; i < maze_rects.size(); i++)
			SDL_RenderFillRect(TheApp::Instance()->getRenderer(), &maze_rects[i]);
	}
	else
	{
		SDL_RenderCopy(TheApp::Instance()->getRenderer(), background_texture, NULL, NULL);
	}
}

void ScenePathFindingGreedyBFS::drawCoin()
{
	Vector2D coin_coords = cell2pix(coinPosition);
	int offset = CELL_SIZE / 2;
	SDL_Rect dstrect = { (int)coin_coords.x - offset, (int)coin_coords.y - offset, CELL_SIZE, CELL_SIZE };
	SDL_RenderCopy(TheApp::Instance()->getRenderer(), coin_texture, NULL, &dstrect);
}

void ScenePathFindingGreedyBFS::initMaze()
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
			Vector2D cell_center((float)(i*CELL_SIZE + offset), (float)(j*CELL_SIZE + offset));
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

void ScenePathFindingGreedyBFS::initGraph() {

	int mida_x = terrain.size();
	int mida_y = terrain[0].size();
	Edge* temp = new Edge;

	for (int i = 1; i < mida_x - 1; i++) {
		for (int j = 1; j < mida_y - 1; j++) {
			if (terrain[i][j] != 0) {
				if (terrain[i + 1][j] != 0) {
					temp = new Edge;
					temp->fromNode.position.x = i;
					temp->fromNode.position.y = j;
					temp->toNode.position.x = i + 1;
					temp->toNode.position.y = j;
					temp->fromNode.visited = false;
					temp->toNode.visited = false;
					graph.connections.push_back(temp);
				}
				if (terrain[i - 1][j] != 0) {
					temp = new Edge;
					temp->fromNode.position.x = i;
					temp->fromNode.position.y = j;
					temp->toNode.position.x = i - 1;
					temp->toNode.position.y = j;
					temp->fromNode.visited = false;
					temp->toNode.visited = false;
					graph.connections.push_back(temp);
				}
				if (terrain[i][j + 1] != 0) {
					temp = new Edge;
					temp->fromNode.position.x = i;
					temp->fromNode.position.y = j;
					temp->toNode.position.x = i;
					temp->toNode.position.y = j + 1;
					temp->fromNode.visited = false;
					temp->toNode.visited = false;
					graph.connections.push_back(temp);
				}
				if (terrain[i][j - 1] != 0) {
					temp = new Edge;
					temp->fromNode.position.x = i;
					temp->fromNode.position.y = j;
					temp->toNode.position.x = i;
					temp->toNode.position.y = j - 1;
					temp->fromNode.visited = false;
					temp->toNode.visited = false;
					graph.connections.push_back(temp);
				}
			}
		}
	}

	temp = new Edge;
	temp->fromNode.position.x = 0;
	temp->fromNode.position.y = 10;
	temp->toNode.position.x = 1;
	temp->toNode.position.y = 10;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 0;
	temp->fromNode.position.y = 11;
	temp->toNode.position.x = 1;
	temp->toNode.position.y = 11;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 0;
	temp->fromNode.position.y = 12;
	temp->toNode.position.x = 1;
	temp->toNode.position.y = 12;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 0;
	temp->fromNode.position.y = 10;
	temp->toNode.position.x = 39;
	temp->toNode.position.y = 10;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 0;
	temp->fromNode.position.y = 11;
	temp->toNode.position.x = 39;
	temp->toNode.position.y = 11;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 0;
	temp->fromNode.position.y = 12;
	temp->toNode.position.x = 39;
	temp->toNode.position.y = 12;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 0;
	temp->fromNode.position.y = 10;
	temp->toNode.position.x = 0;
	temp->toNode.position.y = 11;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 0;
	temp->fromNode.position.y = 11;
	temp->toNode.position.x = 0;
	temp->toNode.position.y = 10;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 0;
	temp->fromNode.position.y = 11;
	temp->toNode.position.x = 0;
	temp->toNode.position.y = 12;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 0;
	temp->fromNode.position.y = 12;
	temp->toNode.position.x = 0;
	temp->toNode.position.y = 11;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	//Right tunnels
	temp = new Edge;
	temp->fromNode.position.x = 39;
	temp->fromNode.position.y = 10;
	temp->toNode.position.x = 38;
	temp->toNode.position.y = 10;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 39;
	temp->fromNode.position.y = 11;
	temp->toNode.position.x = 38;
	temp->toNode.position.y = 11;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 39;
	temp->fromNode.position.y = 12;
	temp->toNode.position.x = 38;
	temp->toNode.position.y = 12;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 39;
	temp->fromNode.position.y = 10;
	temp->toNode.position.x = 0;
	temp->toNode.position.y = 10;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 39;
	temp->fromNode.position.y = 11;
	temp->toNode.position.x = 0;
	temp->toNode.position.y = 11;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 39;
	temp->fromNode.position.y = 12;
	temp->toNode.position.x = 0;
	temp->toNode.position.y = 12;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 39;
	temp->fromNode.position.y = 10;
	temp->toNode.position.x = 39;
	temp->toNode.position.y = 11;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 39;
	temp->fromNode.position.y = 11;
	temp->toNode.position.x = 39;
	temp->toNode.position.y = 10;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 39;
	temp->fromNode.position.y = 11;
	temp->toNode.position.x = 39;
	temp->toNode.position.y = 12;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);

	temp = new Edge;
	temp->fromNode.position.x = 39;
	temp->fromNode.position.y = 12;
	temp->toNode.position.x = 39;
	temp->toNode.position.y = 11;
	temp->fromNode.visited = false;
	temp->toNode.visited = false;
	graph.connections.push_back(temp);
}

bool ScenePathFindingGreedyBFS::loadTextures(char* filename_bg, char* filename_coin)
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

Vector2D ScenePathFindingGreedyBFS::cell2pix(Vector2D cell)
{
	int offset = CELL_SIZE / 2;
	return Vector2D(cell.x*CELL_SIZE + offset, cell.y*CELL_SIZE + offset);
}

Vector2D ScenePathFindingGreedyBFS::pix2cell(Vector2D pix)
{
	return Vector2D((float)((int)pix.x / CELL_SIZE), (float)((int)pix.y / CELL_SIZE));
}

bool ScenePathFindingGreedyBFS::isValidCell(Vector2D cell)
{
	if ((cell.x < 0) || (cell.y < 0) || (cell.x >= terrain.size()) || (cell.y >= terrain[0].size()))
		return false;
	return !(terrain[(unsigned int)cell.x][(unsigned int)cell.y] == 0);
}

void ScenePathFindingGreedyBFS::GBFS(Node objectiu, Node start) {

	contador = 0; //Per fer max, mins i mitja...

	priority_queue<Node, vector<Node>, LessThanByDistance> frontera;
	
	From.clear();
	From.resize(num_cell_x, std::vector<Node>(num_cell_y));
	frontera.push(start);
	std::vector<std::vector<bool>> visited(terrain.size(), vector<bool>(terrain[0].size()));
	for (int i = 0; i < terrain.size(); i++) {
		for (int j = 0; j < terrain[0].size(); j++) {
			visited[i][j] = false;
		}
	}

	//Per quan arrivi a l'objectiu netegi el path
	path.points.clear();

	while (!frontera.empty()) {
		Node nodeActual = frontera.top();
		currentX = nodeActual.position.x;
		currentY = nodeActual.position.y;
		nodeActual.fromNode = Vector2D(-1, -1);
		frontera.pop();
		std::vector<Node> neighbours = graph.getConnections(nodeActual);
		From[currentX][currentY].visited = true; //Anem marcant en true els nodes visitats

		if (!visited[currentX][currentY]) {
			visited[currentX][currentY] = true;
			for (int i = 0; i < neighbours.size(); i++) {
				if (!From[neighbours[i].position.x][neighbours[i].position.y].visited) {
					From[neighbours[i].position.x][neighbours[i].position.y].visited = true;
					From[neighbours[i].position.x][neighbours[i].position.y].fromNode = Vector2D(nodeActual.position.x, nodeActual.position.y); //Passem d'on venim
					//

					neighbours[i].heuristic_distance = pow((objectiu.position.x - neighbours[i].position.x), 2) + pow((objectiu.position.y - neighbours[i].position.y), 2);
					From[neighbours[i].position.x][neighbours[i].position.y].heuristic_distance = neighbours[i].heuristic_distance;
					
					frontera.push(neighbours[i]);

					contador++;

					//std::cout << "From node -> x: " << cameFrom2[neighbors[i].x][neighbors[i].y].fromNode.x << "  y: " << cameFrom2[neighbors[i].x][neighbors[i].y].fromNode.y << endl;
					//std::cout << "To node -> x: " << neighbors[i].x << " y: " << neighbors[i].y << endl;
					if (Vector2D(neighbours[i].position.x, neighbours[i].position.y) == Vector2D(objectiu.position.x, objectiu.position.y)) {
						nodeActual = objectiu;

						path.points.push_back(cell2pix(Vector2D(nodeActual.position.x, nodeActual.position.y)));
						while (Vector2D(nodeActual.position.x, nodeActual.position.y) != Vector2D(start.position.x, start.position.y)) {
							currentX = nodeActual.position.x;
							currentY = nodeActual.position.y;
							nodeActual.position.x = From[currentX][currentY].fromNode.x;
							//std::cout << nodeActual.position.x << endl;
							nodeActual.position.y = From[currentX][currentY].fromNode.y;
							//std::cout << nodeActual.position.y << endl;
							path.points.insert(path.points.begin(), cell2pix(Vector2D(nodeActual.position.x, nodeActual.position.y)));
						}
						path.points.insert(path.points.begin(), cell2pix(Vector2D(start.position.x, start.position.y)));
						std::cout << "Moneda trobada!" << endl;
						path.points.push_back(cell2pix(Vector2D(objectiu.position.x, objectiu.position.y)));
						times++; //Per fer la mitjana
						caculNodes();
						return;
					}
				}
			}
		}
	}
}

void ScenePathFindingGreedyBFS::caculNodes()
{
	if (contador < minimNodes) {
		minimNodes = contador;
	}
	if (contador > maximNodes) {
		int temp = maximNodes;
		maximNodes = contador;
		if (temp < minimNodes) {
			minimNodes = temp;
		}
	}
	if (minimNodes == 0) {
		minimNodes = 1000;
	}
	suma += contador;
	mitjaNodes = suma / times;
	std::cout << "Greedy Best First Search statistics: " << endl;
	if (minimNodes == 1000) {
		cout << "min: " << 0 << endl;
	}
	else cout << "min: " << minimNodes << endl;
	cout << "max: " << maximNodes << endl;
	cout << "mitja: " << mitjaNodes << endl;
}

