//#include "PathFinding.h"
//
//using namespace std;
//
//PathFinding::PathFinding()
//{
//}
//
//PathFinding::~PathFinding()
//{
//}
//
//vector<Vector2D> PathFinding::BFS(Agent * agent, float dtime, Graph graph, Vector2D coinPosition)
//{
//	//Node initial = agent->position;
//	//Node current(0, 0);
//	//Node from(0, 0), to(0, 0);
//	//Edge next(1, from, to);
//	//vector<Edge> connections;
//
//
//	//queue<Node> frontera;
//	//frontera.push(initial);
//	//initial.visited = true;
//
//	//unordered_map<Vector2D, Vector2D> came_from;
//	//came_from[initial.position] = NULL;
//
//	//vector<Vector2D> path;
//	//visited.clear();
//
//	//current = initial;
//
//	//while (!frontera.empty()) {
//
//	//	current = frontera.front();
//	//	frontera.pop();
//	//	
//	//	// Si estem a la moneda, reconstruim el camí que hem seguit
//	//	if (current.position == coinPosition) 
//	//	{
//	//		path.push_back(current.position);
//	//		while (current.position != initial.position)
//	//		{
//	//			current.position = came_from[current.position];
//	//			path.push_back(current.position);
//	//		}
//	//		path.push_back(initial.position);
//
//	//		reverse(path.begin(), path.end());
//
//	//		return path;
//	//	}
//
//
//	//	connections = graph.getConnections(current);
//	//	for (unsigned int i = 0; i < connections.size(); i++) {
//	//		next = connections[i];
//
//	//		for (unsigned int j = 0; j < came_from.size(); j++) {
//	//			if (came_from.find(next.toNode.position) != came_from.end()) {
//	//				next.toNode.visited = true;
//
//	//			}
//	//		}
//
//	//		if (!next.toNode.visited) {
//	//			frontera.push(next.toNode);
//	//			came_from[next.toNode.position] = current.position;
//	//			visited.push_back(next.toNode);
//	//		}
//
//	//	}
//	//}
//	//return path;
//}

