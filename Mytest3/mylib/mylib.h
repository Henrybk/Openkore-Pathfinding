#define DIAGONAL 14
#define ORTOGONAL 10
#define NONE 0
#define OPEN 1
#define CLOSED 2
#define PATH 3
#define LCHILD(currentIndex) 2 * currentIndex + 1
#define RCHILD(currentIndex) 2 * currentIndex + 2
#define PARENT(currentIndex) (int)floor((currentIndex - 1) / 2)

typedef struct Nodes{
    unsigned short x;
    unsigned short y;
    unsigned short parentX;
    unsigned short parentY;
	unsigned int whichlist : 2;
	unsigned int openListIndex;
	unsigned int g;
	unsigned short h;
	unsigned int f;
} Node;

typedef struct Blocks{
	unsigned int walkable : 1;
	unsigned int water : 1;
	unsigned int snipleable : 1;
	unsigned int cliff : 1;
	Node nodeInfo;
} Block;

typedef struct Maps{
	unsigned int height;
	unsigned int width;
	Block **grid;
} Map;

typedef struct {
    int x;
    int y;
    int distanceFromCurrent;
} eachNeigh;

typedef struct {
    eachNeigh neighborNodes[8];
    int count;
} Neighbors;

typedef struct {
    int x;
    int y;
    int f;
} TypeList;