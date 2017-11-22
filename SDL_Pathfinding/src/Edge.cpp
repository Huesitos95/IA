#include "Edge.h"

Edge::Edge()
{
}

Edge::~Edge()
{
}

float Edge::getCost()
{
	return cost;
}

Node Edge::getFromNode()
{
	return fromNode;
}

Node Edge::getToNode()
{
	return toNode;
}
