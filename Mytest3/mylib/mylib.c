#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "./mylib.h"
#include <stdbool.h>

void freeMap(Map* currentMap){
    int i;
    for(i = 0; i < currentMap->height; i++);{
        free(currentMap->grid[i]);
    }
    free(currentMap->grid);
    free(currentMap);
}

Map* mallocMap(int width, int height){
    Map* currentMap = (Map*) malloc(sizeof(Map));
    currentMap->width = width;
	currentMap->height = height;
	currentMap->grid = (Block**) malloc(currentMap->width * sizeof(Block*));
	int j;
	for(j = 0; j < currentMap->width; j++){
        currentMap->grid[j] = (Block*) malloc(currentMap->height * sizeof(Block));
    }
    return currentMap;
}

Map* GenerateMap(unsigned char *map, unsigned long width, unsigned long height){
    Map* currentMap = mallocMap(width, height);

	int x = 0;
	int y = 0;
	
	int max = width * height;
	
	int current = 1;
	int i;
	while (current <= max) {
		current = (y * width) + x;
		i = map[current];
		currentMap->grid[x][y].walkable = (i & 1) ? 1 : 0;
		currentMap->grid[x][y].nodeInfo.whichlist = NONE;
        currentMap->grid[x][y].nodeInfo.openListIndex = NONE;
		if (x == currentMap->width - 1) {
			y++;
			x = 0;
		}
		else {
			x++;
		}
	}
	return currentMap;
}

int heuristic_cost_estimate(Node* currentNode, Node* goalNode)
{
	int xDistance = abs(currentNode->x - goalNode->x);
	int yDistance = abs(currentNode->y - goalNode->y);
	int hScore;
	if (xDistance > yDistance) {
		hScore = DIAGONAL * yDistance + ORTOGONAL * (xDistance - yDistance);
	}
	else {
		hScore = DIAGONAL * xDistance + ORTOGONAL * (yDistance - xDistance);
	}
	return hScore;
}

void organizeNeighborsStruct(Neighbors* currentNeighbors, Node* currentNode, Map* currentMap)
{
    int count = 0;
    int i;
	for (i = -1; i <= 1; i++)
	{
	    int j;
		for (j = -1; j <= 1; j++)
		{
			if (i == 0 && j == 0){ continue; }
			int x = currentNode->x + i;
			int y = currentNode->y + j;
			if (x > currentMap->width - 1 || y > currentMap->height - 1){ continue; }
			if (x < 0 || y < 0){ continue; }
			if (currentMap->grid[x][y].walkable == 0){ continue; }
			if (i != 0 && j != 0) {
                if (currentMap->grid[x][currentNode->y].walkable == 0 || currentMap->grid[currentNode->x][y].walkable == 0){ continue; }
                currentNeighbors->neighborNodes[count].distanceFromCurrent = DIAGONAL;
			} else {
                currentNeighbors->neighborNodes[count].distanceFromCurrent = ORTOGONAL;
			}
				currentNeighbors->neighborNodes[count].x = x;
				currentNeighbors->neighborNodes[count].y = y;
				count++;
		}
	}
	currentNeighbors->count = count;
}

//Openlist is a binary heap of min-heap type

void openListAdd (TypeList* openList, Node* infoAdress, int openListSize, Map* currentMap)
{
    openList[openListSize].x = infoAdress->x;
    openList[openListSize].y = infoAdress->y;
    openList[openListSize].f = infoAdress->f;
    currentMap->grid[openList[openListSize].x][openList[openListSize].y].nodeInfo.openListIndex = openListSize;
    int currentIndex = openListSize;
    TypeList Temporary;
    while (PARENT(currentIndex) >= 0) {
        if (openList[PARENT(currentIndex)].f > openList[currentIndex].f) {
            Temporary = openList[currentIndex];
            openList[currentIndex] = openList[PARENT(currentIndex)];
            currentMap->grid[openList[currentIndex].x][openList[currentIndex].y].nodeInfo.openListIndex = currentIndex;
            openList[PARENT(currentIndex)] = Temporary;
            currentMap->grid[openList[PARENT(currentIndex)].x][openList[PARENT(currentIndex)].y].nodeInfo.openListIndex = PARENT(currentIndex);
            currentIndex = PARENT(currentIndex);
        } else { break; }
    }
}

void reajustOpenListItem (TypeList* openList, Node* infoAdress, int openListSize, Map* currentMap)
{
    int currentIndex = infoAdress->openListIndex;
    openList[currentIndex].f = infoAdress->f;
    TypeList Temporary;
    while (PARENT(currentIndex) >= 0) {
        if (openList[PARENT(currentIndex)].f > openList[currentIndex].f) {
            Temporary = openList[currentIndex];
            openList[currentIndex] = openList[PARENT(currentIndex)];
            currentMap->grid[openList[currentIndex].x][openList[currentIndex].y].nodeInfo.openListIndex = currentIndex;
            openList[PARENT(currentIndex)] = Temporary;
            currentMap->grid[openList[PARENT(currentIndex)].x][openList[PARENT(currentIndex)].y].nodeInfo.openListIndex = PARENT(currentIndex);
            currentIndex = PARENT(currentIndex);
        } else { break; }
    }
}

Node* openListGetLowest (TypeList* openList, Map* currentMap, int openListSize)
{
    Node* lowestNode = &currentMap->grid[openList[0].x][openList[0].y].nodeInfo;
    openList[0] = openList[openListSize-1];
    currentMap->grid[openList[0].x][openList[0].y].nodeInfo.openListIndex = 0;
    int lowestChildIndex = 0;
    int currentIndex = 0;
    TypeList Temporary;
    while (LCHILD(currentIndex) < openListSize - 2) {
        //There are 2 children
        if (RCHILD(currentIndex) <= openListSize - 2) {
            if (openList[RCHILD(currentIndex)].f <= openList[LCHILD(currentIndex)].f) {
                lowestChildIndex = RCHILD(currentIndex);
            } else {
                lowestChildIndex = LCHILD(currentIndex);
            }
        } else {
            //There is 1 children
            if (LCHILD(currentIndex) <= openListSize - 2) {
                lowestChildIndex = LCHILD(currentIndex);
            } else {
                break;
            }
        }
        if (openList[currentIndex].f > openList[lowestChildIndex].f) {
            Temporary = openList[currentIndex];
            openList[currentIndex] = openList[lowestChildIndex];
            currentMap->grid[openList[currentIndex].x][openList[currentIndex].y].nodeInfo.openListIndex = currentIndex;
            openList[lowestChildIndex] = Temporary;
            currentMap->grid[openList[lowestChildIndex].x][openList[lowestChildIndex].y].nodeInfo.openListIndex = lowestChildIndex;
            currentIndex = lowestChildIndex;
        } else { break; }
    }
    return lowestNode;
}

void reconstruct_path(Map* currentMap, Node* startNode, Node* currentNode)
{
	int i = 0;
	while (currentNode->x != startNode->x || currentNode->y != startNode->y)
    {
        currentMap->grid[currentNode->parentX][currentNode->parentY].nodeInfo.whichlist = PATH;
        currentNode = &currentMap->grid[currentNode->parentX][currentNode->parentY].nodeInfo;
        i++;
    }
}

void Pathfind (Node* startNode, Node* goalNode, Map* currentMap)
{
    int size = currentMap->height * currentMap->width;
    int openListSize = 1;
    int Gscore = 0;
    int indexNeighbor = 0;
    int nodeList;
    TypeList* openList = (TypeList*) malloc(size * sizeof(TypeList));
    Node* currentNode;
    openList[0].x = startNode->x;
    openList[0].y = startNode->y;
    currentMap->grid[openList[0].x][openList[0].y].nodeInfo.x = startNode->x;
    currentMap->grid[openList[0].x][openList[0].y].nodeInfo.y = startNode->y;
    Neighbors* currentNeighbors = (Neighbors*) malloc(sizeof(Neighbors));
    Node* infoAdress;
    while (openListSize > 0) {
        //get lowest F score member of openlist and delete it from it
        currentNode = openListGetLowest (openList, currentMap, openListSize);
        openListSize--;

        //add currentNode to closedList
        currentNode->whichlist = CLOSED;

		//if current is the goal, return the path.
		if (currentNode->x == goalNode->x && currentNode->y == goalNode->y) {
            //return path
            reconstruct_path(currentMap, startNode, currentNode);
			break;
		}

		//add current to closedset.



		organizeNeighborsStruct(currentNeighbors, currentNode, currentMap);
		for (indexNeighbor = 0; indexNeighbor < currentNeighbors->count; indexNeighbor++) {
            infoAdress = &currentMap->grid[currentNeighbors->neighborNodes[indexNeighbor].x][currentNeighbors->neighborNodes[indexNeighbor].y].nodeInfo;
			nodeList = infoAdress->whichlist;
			if (nodeList == CLOSED) { continue; }

			Gscore = currentNode->g + currentNeighbors->neighborNodes[indexNeighbor].distanceFromCurrent;

			if (nodeList != OPEN) {
                infoAdress->x = currentNeighbors->neighborNodes[indexNeighbor].x;
                infoAdress->y = currentNeighbors->neighborNodes[indexNeighbor].y;
                infoAdress->parentX = currentNode->x;
                infoAdress->parentY = currentNode->y;
                infoAdress->whichlist = OPEN;
                infoAdress->g = Gscore;
                infoAdress->h = heuristic_cost_estimate(infoAdress, goalNode);
                infoAdress->f = Gscore + infoAdress->h;
				openListAdd (openList, infoAdress, openListSize, currentMap);
				openListSize++;
			} else {
                if (Gscore < infoAdress->g) {
                    infoAdress->parentX = currentNode->x;
                    infoAdress->parentY = currentNode->y;
                    infoAdress->g = Gscore;
                    infoAdress->f = Gscore + infoAdress->h;
                    reajustOpenListItem (openList, infoAdress, openListSize, currentMap);
                }
			}
		}
	}
    free(openList);
}

void PrintMap(Map* currentMap){
	FILE *block = fopen("printedMap.txt", "w");
	int tx = 0;
	int ty = currentMap->height - 1;
	int size = currentMap->width*currentMap->height;
	char walkable = ' ';
	char unwalkable = '#';
	char openList = 'c';
	char closedList = '.';
	char path = '0';
	int j;
	for (j = 0; j < size; j++)
	{
		fprintf(block, "%c", currentMap->grid[tx][ty].walkable ? currentMap->grid[tx][ty].nodeInfo.whichlist == 3? path : currentMap->grid[tx][ty].nodeInfo.whichlist == 2? closedList : currentMap->grid[tx][ty].nodeInfo.whichlist == 1? openList : walkable : unwalkable);
		if (tx == currentMap->width - 1) {
			ty--;
			tx = 0;
			fprintf(block, "\n");
		}
		else {
			tx++;
		}
	}
	fclose(block);
}

int main(Map* currentMap, int startX, int startY, int endX, int endY){
	currentMap->grid[startX][startY].nodeInfo.x = startX;
	currentMap->grid[startX][startY].nodeInfo.y = startY;
	currentMap->grid[startX][startY].nodeInfo.g = 0;
	currentMap->grid[endX][endY].nodeInfo.x = endX;
	currentMap->grid[endX][endY].nodeInfo.y = endY;
	Pathfind(&currentMap->grid[startX][startY].nodeInfo, &currentMap->grid[endX][endY].nodeInfo, currentMap);
	PrintMap(currentMap);
	freeMap(currentMap);
	return 0;
}

unsigned char * printinfo(unsigned char *data, unsigned long width, unsigned long height){
	FILE *block = fopen("printmessages.txt", "w");
	fprintf(block, "This is makedist\n\n");
	
	fprintf(block, "width %d\n", width);
	fprintf(block, "height %d\n", height);
	fprintf(block, "\n");
	
	int max = width * height;

	int i;
	int info;
	int walkable;
	
	int x = 0;
	int y = 0;
	
	for (i = 0; i < max; i++) {
		// first bit is 'walkable' info
		info = data[i];
		walkable = (info & 1) ? 1 : 0;
		if (walkable == 1) {
			data[i] = 255;
		} else {
			data[i] = 0;
		}
		
		fprintf(block, "block is %d %d\n", x, y);
		fprintf(block, "info is %d\n", info);
		fprintf(block, "walkable is %d\n", walkable);
		fprintf(block, "data is %d\n", data[i]);
		fprintf(block, "\n");
		if (x == width - 1) {
			y++;
			x = 0;
		}
		else {
			x++;
		}
	}
	
	int dist;
	int val;
	bool done;
	done = false;
	while (!done) {
		done = true;

		// 'push' wall distance right and up
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				i = y * width + x; // i: cell to examine
				dist = data[i]; // dist: initial dist of i from walkable/nonwalkable check above
				if (x != width - 1) { // ignore the width-1 because this is marked walkable in .gat but its not
					int ir = y * width + x + 1; // ir: cell to the right
					int distr = (int) data[ir]; // distr: initial dist of ir from walkable/nonwalkable check above
					int comp = dist - distr; // comp: 
					if (comp > 1) { // dist > distr: real dist is distr + 1
						val = distr + 1;
						if (val > 255)
							val = 255;
						data[i] = val;
						done = false;
					} else if (comp < -1) { // dist < distr: real distr is dist + 1
						val = dist + 1;
						if (val > 255)
							val = 255;
						data[ir] = val;
						done = false;
					}
				}

				if (y != height - 1) {
					int iu = (y + 1) * width + x;
					int distu = (int) data[iu];
					int comp = dist - distu;
					if (comp > 1) {
						int val = distu + 1;
						if (val > 255)
							val = 255;
						data[i] = (char) val;
						done = false;
					} else if (comp < -1) {
						int val = dist + 1;
						if (val > 255)
							val = 255;
						data[iu] = (char) val;
						done = true;
					}
				}
			}
		}

		// 'push' wall distance left and down
		for (y = height - 1; y >= 0; y--) {
			for (x = width - 1; x >= 0 ; x--) {
				i = y * width + x;
				dist = data[i];
				if (x != 0) {
					int il = y * width + x - 1;
					int distl = data[il];
					int comp = dist - distl;
					if (comp > 1) {
						val = distl + 1;
						if (val > 255)
							val = 255;
						data[i] = val;
						done = false;
					} else if (comp < -1) {
						val = dist + 1;
						if (val > 255)
							val = 255;
						data[il] = val;
						done = false;
					}
				}
				if (y != 0) {
					int id = (y - 1) * width + x;
					int distd = data[id];
					int comp = dist - distd;
					if (comp > 1) {
						val = distd + 1;
						if (val > 255)
							val = 255;
						data[i] = val;
						done = false;
					} else if (comp < -1) {
						val = dist + 1;
						if (val > 255)
							val = 255;
						data[id] = val;
						done = false;
					}
				}
			}
		}
	}
	
	x = 0;
	y = 0;
	
	fprintf(block, "\n");
	
	for (i = 0; i < max; i++) {
		fprintf(block, "block is %d %d\n", x, y);
		fprintf(block, "dist is %d\n", data[i]);
		fprintf(block, "\n");
		if (x == width - 1) {
			y++;
			x = 0;
		}
		else {
			x++;
		}
	}
	
	fclose(block);
	return data;
}
