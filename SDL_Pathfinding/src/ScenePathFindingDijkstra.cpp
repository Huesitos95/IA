#include "ScenePathFindingDijkstra.h"

ScenePathFindingDijkstra::ScenePathFindingDijkstra()
{
	draw_grid = false;
	principi = false;

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

ScenePathFindingDijkstra::~ScenePathFindingDijkstra()
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

void ScenePathFindingDijkstra::update(float dtime, SDL_Event *event)
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
	

	// Iniciar Dijkstra
	if (principi) {
		inici.position.x = pix2cell(agents[0]->getPosition()).x;
		inici.position.y = pix2cell(agents[0]->getPosition()).y;
		objectiu.position.x = coinPosition.x;
		objectiu.position.y = coinPosition.y;
		std::cout << "start" << endl;
		Dijkstra(inici, objectiu);
		principi = false;
	}
	if (start) {
		inici.position.x = pix2cell(agents[0]->getPosition()).x;
		inici.position.y = pix2cell(agents[0]->getPosition()).y;
		objectiu.position.x = coinPosition.x;
		objectiu.position.y = coinPosition.y;
		std::cout << "finished" << endl;
		Dijkstra(inici, objectiu);
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

void ScenePathFindingDijkstra::draw()
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

const char* ScenePathFindingDijkstra::getTitle()
{
	return "SDL Steering Behaviors :: Dijkstra Demo";
}

void ScenePathFindingDijkstra::drawMaze()
{
	
	if (draw_grid)
	{
		SDL_Rect rect = { 0 * CELL_SIZE, 0 * CELL_SIZE, 40 * CELL_SIZE, CELL_SIZE * 24 };

		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 102, 204, 0, 255);
		SDL_RenderFillRect(TheApp::Instance()->getRenderer(), &rect);
		
		rect = { 15*CELL_SIZE, 9*CELL_SIZE, 10*CELL_SIZE, CELL_SIZE *4 };
		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 102, 51, 0, 255);
		SDL_RenderFillRect(TheApp::Instance()->getRenderer(), &rect);

		SDL_SetRenderDrawColor(TheApp::Instance()->getRenderer(), 0, 51, 25, 255);
		for (unsigned int i = 0; i < maze_rects.size(); i++)
			SDL_RenderFillRect(TheApp::Instance()->getRenderer(), &maze_rects[i]);
	}
	else
	{
		SDL_RenderCopy(TheApp::Instance()->getRenderer(), background_texture, NULL, NULL);
	}
}

void ScenePathFindingDijkstra::drawCoin()
{
	Vector2D coin_coords = cell2pix(coinPosition);
	int offset = CELL_SIZE / 2;
	SDL_Rect dstrect = { (int)coin_coords.x - offset, (int)coin_coords.y - offset, CELL_SIZE, CELL_SIZE };
	SDL_RenderCopy(TheApp::Instance()->getRenderer(), coin_texture, NULL, &dstrect);
}

void ScenePathFindingDijkstra::initMaze()
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

void ScenePathFindingDijkstra::initGraph() {

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

bool ScenePathFindingDijkstra::loadTextures(char* filename_bg, char* filename_coin)
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

Vector2D ScenePathFindingDijkstra::cell2pix(Vector2D cell)
{
	int offset = CELL_SIZE / 2;
	return Vector2D(cell.x*CELL_SIZE + offset, cell.y*CELL_SIZE + offset);
}

Vector2D ScenePathFindingDijkstra::pix2cell(Vector2D pix)
{
	return Vector2D((float)((int)pix.x / CELL_SIZE), (float)((int)pix.y / CELL_SIZE));
}

bool ScenePathFindingDijkstra::isValidCell(Vector2D cell)
{
	if ((cell.x < 0) || (cell.y < 0) || (cell.x >= terrain.size()) || (cell.y >= terrain[0].size()))
		return false;
	return !(terrain[(unsigned int)cell.x][(unsigned int)cell.y] == 0);
}

void ScenePathFindingDijkstra::Dijkstra(Node start, Node objectiu)
{
	priority_queue<Node, vector<Node>,LessThanByCost> frontera;
	contador = 0; // Per fer els minims i maxims d'estadistiques
	int CostActual;
	From.clear();
	From.resize(num_cell_x,vector<Node>(num_cell_y));
	SetCosts();
	frontera.push(start);
	vector<vector<bool>> visited(terrain.size(), vector<bool>(terrain[0].size()));
	for (int i = 0; i < terrain.size(); i++)
	{
		for (int j = 0 ; j < terrain[0].size(); j++)
		{
			visited[i][j] = false;
		}
	}
	for (int i = 0; i < num_cell_x; i++)
	{
		for (int j = 0;  j < num_cell_y; j++)
		{
			From[i][j].accumulat = -1;
		}
	}
	path.points.clear();

	while (!frontera.empty())
	{
		Node NodeActual = frontera.top();
		currentX = NodeActual.position.x;
		currentY = NodeActual.position.y;
		NodeActual.fromNode = Vector2D(-1, -1);
		frontera.pop();
		vector<Node> neighbors = graph.getConnections(NodeActual);
		From[currentX][currentY].visited = true; //Anem marcant en true els nodes visitats
	
		for (int i = 0; i < neighbors.size(); i++)
		{
			CostActual = From[currentX][currentY].accumulat + From[neighbors[i].position.x][neighbors[i].position.y].cost;
			if (From[neighbors[i].position.x][neighbors[i].position.y].accumulat == -1 || CostActual < From[neighbors[i].position.x][neighbors[i].position.y].accumulat)
			{
				From[neighbors[i].position.x][neighbors[i].position.y].accumulat = CostActual;
				neighbors[i].accumulat = CostActual;
				From[neighbors[i].position.x][neighbors[i].position.y].fromNode = Vector2D(NodeActual.position.x, NodeActual.position.y);
				frontera.push(neighbors[i]);
				contador++;
				if (Vector2D(neighbors[i].position.x, neighbors[i].position.y) == Vector2D(objectiu.position.x,objectiu.position.y))
				{
					
					NodeActual = objectiu;
					path.points.push_back(cell2pix(Vector2D(NodeActual.position.x, NodeActual.position.y)));
					while (Vector2D(NodeActual.position.x, NodeActual.position.y) != Vector2D(start.position.x, start.position.y))
					{
						currentX = NodeActual.position.x;
						currentY = NodeActual.position.y;
						NodeActual.position.x = From[currentX][currentY].fromNode.x;
						NodeActual.position.y = From[currentX][currentY].fromNode.y;

						path.points.insert(path.points.begin(), cell2pix(Vector2D(NodeActual.position.x, NodeActual.position.y)));

					}
					path.points.insert(path.points.begin(), cell2pix(Vector2D(start.position.x, start.position.y)));
					cout << "got it!" << endl;
					times++;
					caculNodes();
					return;
				}
			}
		}
	}
}

void ScenePathFindingDijkstra::SetCosts()
{
	//10 per a montanya
	for (int i = 15; i < 25; i++) {
		for (int j = 9; j < 13; j++) {
			From[i][j].cost = 10;
		}
	}

//	//4 per a aigua
//	for (int i = ; i < ; i++) {
//
//		for (int j = ; j < ; j++) {
//			From[i][j].cost = 4;
//		}
//	}
}

void ScenePathFindingDijkstra::caculNodes()
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
	std::cout << "Dijkstra statistics: " << endl;
	if (minimNodes == 1000) {
		cout << "min: " << 0 << endl;
	}
	else cout << "min: " << minimNodes << endl;
	cout << "max: " << maximNodes << endl;
	cout << "mitja: " << mitjaNodes << endl;
}


