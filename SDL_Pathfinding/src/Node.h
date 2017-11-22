#pragma once
#include "Vector2D.h"

class Node
{

public:
	Vector2D position;
	bool visited; 
	int cost = 1;
	int accumulat;
	int heuristic_distance;
	float priority;
	Node();
	Node(float x, float y);
	Node(Vector2D Vec);
	~Node();

	Node * veiDreta;
	Node * veiEsquerra;
	Node * veiAdalt;
	Node * veiAbaix;

	Vector2D fromNode;
};

inline bool operator< (Node a, Node b) {
	return (a.position.x + a.position.y < b.position.x + b.position.y);
}

struct LessThanByCost {

	bool operator()(const Node a, const Node b) {
		return (a.accumulat > b.accumulat);
	}
};
struct LessThanByDistance {

	bool operator()(const Node a, const Node b) {
		return (a.heuristic_distance > b.heuristic_distance);
	}
};

struct LessThanByPriority {

	bool operator()(const Node a, const Node b) {
		return (a.priority > b.priority);
	}
};