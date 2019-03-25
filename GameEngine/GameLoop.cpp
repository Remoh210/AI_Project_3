#include "BMPImage.h"
#include "ResourceManager.h"
#include "globalStuff.h"
#include <vector>
#include <utility>

#include "BMPImage.h"
#include "ResourceManager.h"


#include "cGameObject.h"

struct sNode
{

public:
	sNode() {};
	~sNode() {};

	char nodeColour;
	cGameObject * thisNodeObj;

	std::vector<sNode*> vec_neighbours;
	bool Res = false;
	bool Gatherer = false;
	bool visited = false;
	float Dist = std::numeric_limits<float>::max();

	int Prev;
	bool Removed = false;
};


ResourceManager gResourceManager;

std::vector<std::vector<char>> bmpPixelValues;

std::vector<std::vector<cGameObject*>> nodeMeshObjs;

std::vector<std::vector<sNode*>> mapNodes;

std::vector<sNode*> vec_graph;

std::map<std::string, cGameObject*> resourceMap;
void PlaceObjects();
void CreateNodeNeighbours();
void dijkstra(std::vector<sNode*> &Graph, int Source);

sNode* homeNode = NULL;



char GetColourCharacter(unsigned char r, unsigned char g, unsigned char b)
{

	if (r == 255 && g == 0 && b == 0) {
		return 'r';
	}
	if (r == 0 && g == 255 && b == 0)
	{
		return 'g';
	}
	if (r == 0 && g == 0 && b == 255)
	{
		return 'b';
	}
	if (r == 255 && g == 255 && b == 255)
	{
		return 'w';
	}
	if (r == 0 && g == 0 && b == 0)
	{
		return '_';
	}
	return 'x';
}

void loadBmp(std::string bmpFileName)
{

	BMPImage* bmp = new BMPImage(bmpFileName);

	char* data = bmp->GetData();
	unsigned long imageWidth = bmp->GetImageWidth();
	unsigned long imageHeight = bmp->GetImageHeight();

	int index = 0;
	int r, g, b;
	r = 0;
	g = 0;
	b = 0;

	for (unsigned long y = 0; y < imageHeight; y++) 
	{
		std::vector<char> newVector;
		bmpPixelValues.push_back(newVector);
		for (unsigned long x = 0; x < imageWidth; x++) 
		{
			char pixelValue;

			if (index == 0) {
				b = index;

			}
			else {
				index++;
				b = index;

			}
			index++;
			g = index;
			index++;
			r = index;

			pixelValue = GetColourCharacter(data[r], data[g], data[b]);

			bmpPixelValues[y].push_back(pixelValue);

			printf("%c", bmpPixelValues[y][x]);

		}
	}

	std::vector<std::vector<char>> tempPixelValues;

	for (int i = 0; i < imageHeight; i++)
	{
		std::vector<char> tmp;

		for (int j = imageWidth - 1; j > -1; j--)
		{
			tmp.push_back(bmpPixelValues[i][j]);
		}

		tempPixelValues.push_back(tmp);
	}

	bmpPixelValues = tempPixelValues;

	
}

void GenerateMap()
{
	for (unsigned int a = 0; a < bmpPixelValues.size(); a++)
	{
		std::vector<cGameObject*> objVector;
		nodeMeshObjs.push_back(objVector);
		std::vector<sNode*> nodeVector;
		mapNodes.push_back(nodeVector);
		
		for (unsigned int b = 0; b < bmpPixelValues[a].size(); b++)
		{
			bool vis = true;
			glm::vec3 objColour;
			switch (bmpPixelValues[a][b])
			{
			case 'r':
			{
				objColour = glm::vec3(1.0f, 0.0f, 0.0f);
			}
				
				break;
			case 'g':
			{
				objColour = glm::vec3(0.0f, 1.0f, 0.0f);
			}

				break;
			case 'b':
			{
				objColour = glm::vec3(0.0f, 0.0f, 1.0f);
			}

				break;
			case 'w':
			{		
					objColour = glm::vec3(1.0f, 1.0f, 1.0f);
					vis = false;
			}
			
				break;
			case '_':
				objColour = glm::vec3(0.0f, 0.0f, 0.0f);
				break;
			default:
				return;
				break;
			}


			cGameObject* pCube = new cGameObject();
			objColour * 255.0f;
			pCube->setDiffuseColour(objColour);
			pCube->bDontLight = true;
			pCube->friendlyName = "cube" + std::to_string(a) + std::to_string(b);
			pCube->meshName = "cube_flat_shaded_xyz_n_uv.ply";	
			float scale = 15.0f;
			if (bmpPixelValues[a][b] == '_')
			{
				pCube->nonUniformScale = glm::vec3(scale, scale, scale);
			}
			else
			{
				pCube->setUniformScale(scale);
			}
			pCube->position = glm::vec3(b * 2.0f, 0.0f, a * 2.0f);
			pCube->position *= scale;
			pCube->bIsWireFrame = false;
			pCube->bIsVisible = vis;
			int scaleInt = (int)scale;
			vec_pObjectsToDraw.push_back(pCube);
			nodeMeshObjs[a].push_back(pCube);
			sNode* mapNode = new sNode();
			mapNode->thisNodeObj = nodeMeshObjs[a][b];
			mapNode->nodeColour = bmpPixelValues[a][b];
			mapNodes[a].push_back(mapNode);

			
			
		}
	}
	CreateNodeNeighbours();
	PlaceObjects();
}

void CreateNodeNeighbours()
{
	for (int i = 0; i < mapNodes.size(); i++)
	{
		for (int j = 0; j < mapNodes[i].size(); j++)
		{
			sNode* node = mapNodes[i][j];
			
			if (i != 0)
			{
				node->vec_neighbours.push_back(mapNodes[i - 1][j]);
				if (j != 0)
				{
					node->vec_neighbours.push_back(mapNodes[i - 1][j - 1]);
				}
				if (j != mapNodes[i].size() - 1)
				{
					node->vec_neighbours.push_back(mapNodes[i - 1][j + 1]);
				}
			}
			if (i != mapNodes.size() - 1)
			{
				node->vec_neighbours.push_back(mapNodes[i + 1][j]);
				if (j != 0)
				{
					node->vec_neighbours.push_back(mapNodes[i + 1][j - 1]);
				}
				if (j != mapNodes[i].size() - 1)
				{
					node->vec_neighbours.push_back(mapNodes[i + 1][j + 1]);
				}
			}
			if (j != 0)
			{
				node->vec_neighbours.push_back(mapNodes[i][j - 1]);
			}
			if (j != mapNodes[i].size() - 1)
			{
				node->vec_neighbours.push_back(mapNodes[i][j + 1]);
			}
		}
	}
}



void PlaceObjects()
{

	int Count = 0;

	for (int i = 0; i < mapNodes.size(); i++)
	{
		for (int j = 0; j < mapNodes[i].size(); j++)
		{
			if (mapNodes[i][j]->nodeColour == 'g')
			{
				cGameObject* gatherer = findObjectByFriendlyName("dalek");
				gatherer->position = mapNodes[i][j]->thisNodeObj->position;
				gatherer->position.y += ((mapNodes[i][j]->thisNodeObj->nonUniformScale.y)* 1.5f);
				mapNodes[i][j]->Gatherer = true;
			}

			if (mapNodes[i][j]->nodeColour == 'r')
			{

				cGameObject* pSphere = new cGameObject();
				pSphere->setDiffuseColour(glm::vec3(0.0f, 1.0f, 0.0f));
				pSphere->bIsUpdatedByPhysics = true;
				pSphere->bDontLight = true;
				pSphere->bIsWireFrame = true;
				pSphere->friendlyName = "res" + Count;
				pSphere->meshName = "Sphere_320.ply";		
				float scale = 5.0f;
				pSphere->setUniformScale(scale);
				pSphere->position = mapNodes[i][j]->thisNodeObj->position;
				pSphere->position.y += ((mapNodes[i][j]->thisNodeObj->nonUniformScale.y)* 1.5f);
				vec_pObjectsToDraw.push_back(pSphere);
				Count++;
				mapNodes[i][j]->Res = true;
			}
			if(mapNodes[i][j]->nodeColour == 'b')
			{
				homeNode = mapNodes[i][j];
			}
		}
	}

	//copieng to vec
	for (int i = 0; i < mapNodes.size(); i++)
	{
		for (int j = 0; j < mapNodes[i].size(); j++)
		{
			vec_graph.push_back(mapNodes[i][j]);
		}
	}

	int target = 0;
	int source = 0;
	//finding target
	for (int i = 0; i < vec_graph.size(); i++)
	{
		if(vec_graph[i]->Res)
		{
			target = i;
		}

		if (vec_graph[i]->Gatherer)
		{
			source = i;
		}
	}

	//void dijkstra(std::vector<sNode*> &Graph, int Source, int target);
}


//bool isEmpty(const std::vector<sNode*>&Graph) {
//	for (auto &Vert : Graph) {
//		if (!Vert->Removed)
//			return false;
//	}
//	return true;
//}
//
//float length(const sNode* A, const sNode* B) {
//
//
//	
//	return glm::distance(A->thisNodeObj->position, B->thisNodeObj->position);
//}
//
//
//int getMinDistVert(std::vector<sNode*> &Graph) {
//	float Dist = std::numeric_limits<float>::max();
//	int Index = -1;
//	for (int x = 0; x < Graph.size(); x++) {
//		if (Graph[x]->Removed)
//			continue;
//
//		if (Graph[x]->Dist < Dist) {
//			Dist = Graph[x]->Dist;
//			Index = x;
//		}
//	}
//	return Index == -1 ? 0 : Index;
//}
//
//void dijkstra(std::vector<sNode*> &Graph, int Source, int target) {
//
//	for (int x = 0; x < Graph.size(); x++) {
//		Graph[x]->Dist = std::numeric_limits<float>::max();
//		Graph[x]->Prev = -1;
//		Graph[x]->Removed = false;
//	}
//
//	Graph[Source]->Dist = .0f;
//
//		while (!isEmpty(Graph)) {
//			int u = getMinDistVert(Graph);
//			sNode* &Vert = Graph[u];
//			Vert->Removed = true;
//
//			if (Vert->Prev != -1 || Vert == Graph[target])
//			{
//				break;
//			}
//
//			for (int y = 0; y < Vert->vec_neighbours.size(); y++) {
//				sNode* &Neighbour = Vert->vec_neighbours[y];
//				float Alt = Vert->Dist + length(Vert, Neighbour);
//				if (Alt < Neighbour->Dist) {
//					Neighbour->Dist = Alt;
//					Neighbour->Prev = u;
//				}
//			}
//		}
//}

