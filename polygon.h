#include "vector.h"

typedef struct polygon_s
{
	int	maxvertices;
	int	numvertices;
	vec3	*vertices;

} polygon_t;

#define POLYGON_SIDE_ON		0
#define POLYGON_SIDE_FRONT	1
#define POLYGON_SIDE_BACK	2
#define POLYGON_SIDE_CROSS	3

void Polygon_SetMemCallbacks(void *(*alloccallback)(int numbytes), void (*freecallback)(void *p));
polygon_t *Polygon_Alloc(int maxvertices);
void Polygon_Free(polygon_t* p);

float Polygon_Area(polygon_t* p);
vec3 Polygon_Normal2(polygon_t *p);

