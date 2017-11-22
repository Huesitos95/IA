#include "Node.h"



Node::Node()
{
	position.x = 0;
	position.y = 0;
	visited = false;
	veiDreta = nullptr;
	veiEsquerra = nullptr;
	veiAdalt = nullptr;
	veiAbaix = nullptr;
	//fromNode = nullptr;
}

Node::Node(float x, float y)
{
	position.x = x;
	position.y = y;
	visited = false;
	veiDreta = nullptr;
	veiEsquerra = nullptr;
	veiAdalt = nullptr;
	veiAbaix = nullptr;
	//fromNode = nullptr;
}

Node::Node(Vector2D Vec)
{
	position = Vec;
	visited = false;
	veiDreta = nullptr;
	veiEsquerra = nullptr;
	veiAdalt = nullptr;
	veiAbaix = nullptr;
	//fromNode = nullptr;
}

Node::~Node()
{

}
