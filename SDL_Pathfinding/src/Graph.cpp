#include "Graph.h"

Graph::Graph()
{
	
}

Graph::~Graph()
{
}

vector<Node> Graph::getConnections(Node fromNode)
{
	vector<Node>connected;
	//int counterToNodes = 0;
	for (unsigned int i = 0; i < connections.size(); i++)
	{
		if (connections[i]->fromNode.position == fromNode.position) {

			connected.push_back(connections[i]->toNode);
		}
	
	}
	return connected;
}

//void Graph::AddEdge(Edge e)
//{
//	Edges.push_back(e);
//}
//
