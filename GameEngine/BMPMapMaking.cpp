#include "BMPImage.h"
#include "ResourceManager.h"
#include "globalStuff.h"
#include <vector>
#include <utility>

#include "BMPImage.h"
#include "ResourceManager.h"


#include "cMapNode.h"

ResourceManager gResourceManager;

std::vector<std::vector<char>> bmpPixelValues;

std::vector<std::vector<cGameObject*>> nodeMeshObjs;

std::vector<std::vector<cMapNode*>> mapNodes;

std::map<cMapNode*> Graph;

std::map<std::string, cGameObject*> resourceMap;
void PopulateMap();
void CreateNodeNeighbours();
void dijkstra(std::vector<cMapNode*> &Graph, int Source);

cMapNode* homeNode = NULL;



int findElementNumber(std::vector<cMapNode*>, cMapNode* node)
{


	std::vector<cMapNode*>::iterator itr = std::find(v.begin(), v.end(), key);

	if (itr != v.cend()) {
		std::cout << "Element present at index " << std::distance(v.begin(), itr);
	}
	else {
		std::cout << "Element not found";
	}

}

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

void GetBMPPixelValues(std::string bmpFileName)
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

void CreateMap()
{
	for (unsigned int a = 0; a < bmpPixelValues.size(); a++)
	{
		std::vector<cGameObject*> objVector;
		nodeMeshObjs.push_back(objVector);
		std::vector<cMapNode*> nodeVector;
		mapNodes.push_back(nodeVector);

		for (unsigned int b = 0; b < bmpPixelValues[a].size(); b++)
		{
			bool validColour = true;
			glm::vec3 objColour;
			switch (bmpPixelValues[a][b])
			{
			case 'r':
			{
				printf("r");
				objColour = glm::vec3(1.0f, 0.0f, 0.0f);
			}
				
				break;
			case 'g':
				objColour = glm::vec3(0.0f, 1.0f, 0.0f);
				break;
			case 'b':
				objColour = glm::vec3(0.0f, 0.0f, 1.0f);
				break;
			case 'w':
				objColour = glm::vec3(1.0f, 1.0f, 1.0f);
//				validColour = false;
				break;
			case '_':
				objColour = glm::vec3(0.0f, 0.0f, 0.0f);
				break;
			default:
				validColour = false;
				break;
			}


			if (validColour)
			{
				cGameObject* pCube = new cGameObject();
				objColour * 255.0f;
				pCube->setDiffuseColour(objColour);
				pCube->bDontLight = true;
				pCube->friendlyName = "cube" + std::to_string(a) + std::to_string(b);
				pCube->meshName = "cube_flat_shaded_xyz_n_uv.ply";		// "cube_flat_shaded_xyz.ply";
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


				int scaleInt = (int)scale;

				vec_pObjectsToDraw.push_back(pCube);
				
				//Add GO to the map
				nodeMeshObjs[a].push_back(pCube);

				cMapNode* mapNode = new cMapNode();
				mapNode->thisNodeObj = nodeMeshObjs[a][b];
				mapNode->nodeColour = bmpPixelValues[a][b];
				mapNodes[a].push_back(mapNode);

				//glm::vec3 roundedPos = pCube->position;
				////roundedPos.x = (int)roundedPos.x;
				////roundedPos.y = (int)roundedPos.y;
				////roundedPos.z = (int)roundedPos.z;

				//int xRemainder = (int)(pCube->position.x) % (10);
				//int zRemainder = (int)(pCube->position.z) % (10);

				////printf("%s %d", "x val", xRemainder);
				////printf("%s %d", "z val", zRemainder);

				//if (xRemainder < 5)
				//{
				//	roundedPos.x = roundedPos.x - xRemainder;
				//}
				//else
				//{
				//	roundedPos.x = roundedPos.x - xRemainder + 5;
				//}

				//if (zRemainder < 5)
				//{
				//	roundedPos.z = roundedPos.z - zRemainder;
				//}
				//else
				//{
				//	roundedPos.z = roundedPos.z - zRemainder + 5;

				//}

				////roundedPos.x = roundedPos.x + scaleInt / 2;
				////roundedPos.x -= (int)roundedPos.x % (int)scaleInt;

				////roundedPos.y = 0;

				////roundedPos.z = roundedPos.z + scaleInt / 2;
				////roundedPos.z -= (int)roundedPos.z % (int)scaleInt;

				//std::string key = std::to_string((int)roundedPos.z);
				//key.append(std::to_string((int)roundedPos.x));

				////printf("%s %s\n", "Key: ", key.c_str());

				//mapNode->key = key;

				//std::pair<std::string, cMapNode*> vecNodePair = std::make_pair(key, mapNode);

				//positionNodeMap.insert(vecNodePair);
			}
		}
	}
	CreateNodeNeighbours();
	PopulateMap();
}

void CreateNodeNeighbours()
{
	for (int i = 0; i < mapNodes.size(); i++)
	{
		for (int j = 0; j < mapNodes[i].size(); j++)
		{
			cMapNode* node = mapNodes[i][j];
			//If not on top row
			
			if (i != 0)
			{
				node->vec_neighbours.push_back(mapNodes[i - 1][j]);
				if (j != 0)
				{
					//node->neighbourSE = mapNodes[i - 1][j - 1]; //add se neighbour
					node->vec_neighbours.push_back(mapNodes[i - 1][j - 1]);
				}
				if (j != mapNodes[i].size() - 1)
				{
					//node->neighbourSW = mapNodes[i - 1][j + 1]; //add sw neighbour
					node->vec_neighbours.push_back(mapNodes[i - 1][j + 1]);
				}
			}
			if (i != mapNodes.size() - 1)
			{
				node->vec_neighbours.push_back(mapNodes[i + 1][j]);
				if (j != 0)
				{
					//node->neighbourNE = mapNodes[i + 1][j - 1]; //add se neighbour
					node->vec_neighbours.push_back(mapNodes[i + 1][j - 1]);
				}
				if (j != mapNodes[i].size() - 1)
				{
					//node->neighbourNW = mapNodes[i + 1][j + 1]; //add sw neighbour
					node->vec_neighbours.push_back(mapNodes[i + 1][j + 1]);
				}
			}
			if (j != 0)
			{
				//node->neighbourEE = mapNodes[i][j - 1]; //add east neighbour
				node->vec_neighbours.push_back(mapNodes[i][j - 1]);
			}
			if (j != mapNodes[i].size() - 1)
			{
				//node->neighbourWW = mapNodes[i][j + 1]; //add west neighbour
				node->vec_neighbours.push_back(mapNodes[i][j + 1]);
			}
		}
	}
}



void PopulateMap()
{

	int resourceCount = 0;

	for (int i = 0; i < mapNodes.size(); i++)
	{
		for (int j = 0; j < mapNodes[i].size(); j++)
		{
			//Player starting position
			if (mapNodes[i][j]->nodeColour == 'g')
			{
				cGameObject* player = findObjectByFriendlyName("dalek");
				player->position = mapNodes[i][j]->thisNodeObj->position;
				player->position.y += ((mapNodes[i][j]->thisNodeObj->nonUniformScale.y)* 1.5f);
			}

			if (mapNodes[i][j]->nodeColour == 'r')
			{

				cGameObject* pPlayer = new cGameObject();
				pPlayer->setDiffuseColour(glm::vec3(1.0f, 0.0f, 0.0f));
				pPlayer->setAlphaTransparency(1.0f);
				pPlayer->bIsUpdatedByPhysics = true;
				pPlayer->friendlyName = "Resource" + resourceCount;
				pPlayer->meshName = "Sphere_320.ply";		
				float scale = 3.0f;
				pPlayer->setUniformScale(scale);
				pPlayer->position = mapNodes[i][j]->thisNodeObj->position;
				pPlayer->position.y += ((mapNodes[i][j]->thisNodeObj->nonUniformScale.y)* 1.5f);
				pPlayer->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
				//pPlayer->objectClass = cGameObject::objClass::RESOURCE;

				pPlayer->pTheShape = new sSphere(scale);
				pPlayer->shapeType = cGameObject::SPHERE;

				vec_pObjectsToDraw.push_back(pPlayer);

				resourceCount++;
				mapNodes[i][j]->hasResource = true;
			}
			if(mapNodes[i][j]->nodeColour == 'b')
			{
				homeNode = mapNodes[i][j];
			}
		}
	}





	void dijkstra(std::vector<cMapNode*> &Graph, int Source, int target)
}


bool isEmpty(const std::vector<cMapNode*>&Graph) {
	for (auto &Vert : Graph) {
		if (!Vert->Removed)
			return false;
	}
	return true;
}

float length(const cMapNode* A, const cMapNode* B) {


	
	return glm::distance(A->thisNodeObj->position, B->thisNodeObj->position);
}


int getMinDistVert(std::vector<cMapNode*> &Graph) {
	float Dist = std::numeric_limits<float>::max();
	int Index = -1;
	for (int x = 0; x < Graph.size(); x++) {
		if (Graph[x]->Removed)
			continue;

		if (Graph[x]->Dist < Dist) {
			Dist = Graph[x]->Dist;
			Index = x;
		}
	}
	return Index == -1 ? 0 : Index;
}

void dijkstra(std::vector<cMapNode*> &Graph, int Source, int target) {

	for (int x = 0; x < Graph.size(); x++) {
		Graph[x]->Dist = std::numeric_limits<float>::max();
		Graph[x]->Prev = -1;
		Graph[x]->Removed = false;
	}

	Graph[Source]->Dist = .0f;

		while (!isEmpty(Graph)) {
			int u = getMinDistVert(Graph);
			cMapNode* &Vert = Graph[u];
			Vert->Removed = true;

			if (Vert->Prev != -1 || Vert == Graph[target])
			{
				break;
			}

			for (int y = 0; y < Vert->vec_neighbours.size(); y++) {
				cMapNode* &Neighbour = Vert->vec_neighbours[y];
				float Alt = Vert->Dist + length(Vert, Neighbour);
				if (Alt < Neighbour->Dist) {
					Neighbour->Dist = Alt;
					Neighbour->Prev = u;
				}
			}
		}
}

