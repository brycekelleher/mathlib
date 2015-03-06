#ifndef __POLYGON_H__
#define __POLYGON_H__

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
polygon_t* Polygon_Copy(polygon_t* p);
polygon_t *Polygon_Reverse(polygon_t* p);
void Polygon_BoundingBox(polygon_t* p, vec3* bmin, vec3* bmax);
vec3 Polygon_Centroid(polygon_t* p);
float Polygon_Area(polygon_t* p);
vec3 Polygon_ProjectedArea(polygon_t *p);
vec3 Polygon_Normal(polygon_t* p);
void Polygon_SplitWithPlane(polygon_t *in, vec3 normal, float dist, float epsilon, polygon_t **front, polygon_t **back);
int Polygon_OnPlaneSide(polygon_t *p, vec3 normal, float dist, float epsilon);

#endif

