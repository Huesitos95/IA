#pragma once
#include "Vector2D.h"
#include "Edge.h"
#include "Node.h"
#include <vector>
#include <iostream>

using namespace std;

//class Graph
//{
//
//public:
//	Graph();
//	~Graph();
//	//vector<Edge> Edges;
//	vector<Edge> connections;
//	vector<Edge> getConnections(Node fromNode);
//
//	//void AddEdge(Edge e);
//	
//};

class Graph
{
public:
	Graph();
	~Graph();
	std::vector<Edge*> connections;
	std::vector<Node> getConnections(Node fromNode);
};

inline bool operator== (const Node &c1, const Node &c2)
{
	return (c1.position.x == c2.position.x &&
		c1.position.y == c2.position.y);
}

inline bool operator!= (const Node &c1, const Node &c2)
{
	return !(c1 == c2);
}
