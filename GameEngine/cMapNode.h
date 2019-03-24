#pragma once

#include "cGameObject.h"
#include "cGameObject.h"

class cMapNode
{

public:
	cMapNode();
	~cMapNode();

	char nodeColour;
	cGameObject * thisNodeObj;

	cMapNode * neighbourNW;
	cMapNode * neighbourNE;
	cMapNode * neighbourSW;
	cMapNode * neighbourSE;
	cMapNode * neighbourNN;
	cMapNode * neighbourSS;
	cMapNode * neighbourWW;
	cMapNode * neighbourEE;

	std::string key;
	std::vector<cMapNode*> vec_neighbours;
	bool hasResource = false;
	bool visited = false;
	float distance = INT_MAX;
};
