#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "polygon.h"

// memory allocation
static void *Polygon_MemAllocHandler(int numbytes)
{
	return malloc(numbytes);
}

static void Polygon_MemFreeHandler(void *ptr)
{
	free(ptr);
}

static void *(*Polygon_MemAlloc)(int numbytes)	= Polygon_MemAllocHandler;
static void (*Polygon_MemFree)(void *p)		= Polygon_MemFreeHandler;

void Polygon_SetMemCallbacks(void *(*alloccallback)(int numbytes), void (*freecallback)(void *p))
{
	Polygon_MemAlloc	= alloccallback;
	Polygon_MemFree		= freecallback;
}

static int Polygon_MemSize(int maxvertices)
{
	return sizeof(polygon_t) + (maxvertices * sizeof(vec3));
}

polygon_t *Polygon_Alloc(int maxvertices)
{
	polygon_t	*p;

	int numbytes = Polygon_MemSize(maxvertices);
	p = (polygon_t*)Polygon_MemAlloc(numbytes);

	p->maxvertices	= maxvertices;
	p->numvertices	= 0;
	p->vertices	= (vec3*)(p + 1);

	return p;
}

void Polygon_Free(polygon_t* p)
{
	Polygon_MemFree(p);
}

polygon_t* Polygon_Copy(polygon_t* p)
{
	polygon_t	*c;

	int numbytes = Polygon_MemSize(p->maxvertices);
	c = (polygon_t*)Polygon_MemAlloc(numbytes);

	memcpy(c, p, numbytes);

	return c;
}

polygon_t *Polygon_Reverse(polygon_t* p)
{
	polygon_t	*r;

	r = (polygon_t*)Polygon_Alloc(p->maxvertices);

	for(int i = 0; i < p->numvertices; i++)
		r->vertices[(i + 1) % p->numvertices] = p->vertices[p->numvertices - 1 - i];

	return r;
}

void Polygon_BoundingBox(polygon_t* p, vec3* bmin, vec3* bmax)
{
	*bmin = vec3( 1e20f,  1e20f,  1e20f);
	*bmax = vec3(-1e20f, -1e20f, -1e20f);

	for(int i = 0; i < p->numvertices; i++)
	{
		vec3 *v = p->vertices + i;

		for(int j = 0; j < 3; j++)
		{
			if(v[j] < bmin[j])
				bmin[j] = v[j];
			if(v[j] > bmax[j])
				bmax[j] = v[j];
		}
	}
}

vec3 Polygon_Centroid(polygon_t* p)
{
	vec3 v;
	
	v = vec3_zero;
	for(int i = 0; i < p->numvertices; i++)
		v = v + p->vertices[i];

	v = (1.0f / (float)p->numvertices) * v;

	return v;
}

static vec3 Polygon_TriCrossVector(float v0[3], float v1[3], float v2[3])
{
	float x = (v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]);
	float y = (v1[2] - v0[2]) * (v2[0] - v0[0]) - (v2[2] - v0[2]) * (v1[0] - v0[0]);
	float z = (v1[1] - v0[1]) * (v2[2] - v0[2]) - (v2[1] - v0[1]) * (v1[2] - v0[2]);

	return vec3(x, y, z);
}

static vec3 Polygon_TriProjectedArea(float v0[3], float v1[3], float v2[3])
{
	return 0.5f * Polygon_TriCrossVector(v0, v1, v2);
}

static float Polygon_TriArea(float v0[3], float v1[3], float v2[3])
{
	vec3 a = Polygon_TriCrossVector(v0, v1, v2);
	return 0.5f * sqrtf((a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2]));
}

vec3 Polygon_ProjectedArea(polygon_t *p)
{
	float x, y, z;

	x = y = z = 0;

	for(int i = 0; i < p->numvertices - 2; i++)
	{
		vec3 area = Polygon_TriProjectedArea(p->vertices[0], p->vertices[i + 1], p->vertices[i + 2]); 

		x += area[0];
		y += area[1];
		z += area[2];
	}

	return vec3(x, y, z);
}

float Polygon_Area(polygon_t* p)
{
	float area = 0;

	for(int i = 0; i < p->numvertices - 2; i++)
	{
		area += Polygon_TriArea(p->vertices[0], p->vertices[i + 1], p->vertices[i + 2]); 
	}

	return area;
}

static vec3 Polygon_NormalFromEdges(polygon_t* p)
{
	vec3 n;
	vec3 e0, e1;

	assert(p->numvertices >= 3);
	e0 = p->vertices[1] - p->vertices[0];
	e1 = p->vertices[2] - p->vertices[1];
	
	assert(Length(e0) > 0.0f && Length(e1) > 0.0f);
	n = Cross(e0, e1);
	n = Normalize(n);

	return n;
}

static vec3 Polygon_NormalFromArea(polygon_t *p)
{
	vec3 n = Polygon_ProjectedArea(p);

	n = Normalize(n);

	return n;
}

vec3 Polygon_Normal(polygon_t *p)
{
	return Polygon_NormalFromArea(p);
}

void Polygon_SplitWithPlane(polygon_t *in, vec3 normal, float dist, float epsilon, polygon_t **front, polygon_t **back)
{
	float	dists[32+4];
	int		sides[32+4];
	int		counts[3];		// FRONT, BACK, ON
	static	float dot;		// VC 4.2 optimizer bug if not static
	int		i, j;
	vec3	p1, p2;
	vec3	mid;
	polygon_t	*f, *b;
	int		maxpts;
	
	counts[0] = counts[1] = counts[2] = 0;

	// classify each point
	{
		for(i = 0; i < in->numvertices; i++)
		{
			dists[i] = Dot(in->vertices[i], normal) + dist;
			
			if(dot > epsilon)
			{
				sides[i] = POLYGON_SIDE_FRONT;
			}
			else if(dot < -epsilon)
			{
				sides[i] = POLYGON_SIDE_BACK;
			}
			else
			{
				sides[i] = POLYGON_SIDE_ON;
			}

			counts[sides[i]]++;
		}

		sides[i] = sides[0];
		dists[i] = dists[0];
	}
	
	// all points are on the plane
	// fixme: what happens if the polygon is degenerate?
	if(!counts[POLYGON_SIDE_FRONT] && !counts[POLYGON_SIDE_BACK])
	{
		*front = NULL;
		*back = NULL;
		return;
	}

	// all points are front side
	if(!counts[POLYGON_SIDE_BACK])
	{
		*front = Polygon_Copy(in);
		*back = NULL;
		return;
	}

	// all points are back side
	if(!counts[POLYGON_SIDE_FRONT])
	{
		*front = NULL;
		*back = Polygon_Copy(in);
		return;
	}

	// split the polygon
	maxpts = in->numvertices+4;	// cant use counts[0]+2 because
								// of fp grouping errors

	*front = f = Polygon_Alloc(maxpts);
	*back = b = Polygon_Alloc(maxpts);
		
	for(i = 0; i < in->numvertices; i++)
	{
		p1 = in->vertices[i];
		
		if(sides[i] == POLYGON_SIDE_ON)
		{
			// Add the point to the front polygon
			f->vertices[f->numvertices] = p1;
			f->numvertices++;

			// Add the point to the back polygon
			b->vertices[b->numvertices] = p1;
			b->numvertices++;
			
			continue;
		}
	
		if(sides[i] == POLYGON_SIDE_FRONT)
		{
			// Add the point to the front polygon
			f->vertices[f->numvertices] = p1;
			f->numvertices++;
		}

		if(sides[i] == POLYGON_SIDE_BACK)
		{
			b->vertices[b->numvertices] = p1;
			b->numvertices++;
		}

		// If the next point doesn't straddle the plane continue
		if (sides[i+1] == POLYGON_SIDE_ON || sides[i+1] == sides[i])
		{
			continue;
		}
		
		// The next point crosses the plane, so generate a split point
		p2 = in->vertices[(i + 1) % in->numvertices];
		
		dot = dists[i] / (dists[i] - dists[i+1]);
		for(j = 0; j < 3; j++)
		{	
			// avoid round off error when possible
			if(normal[j] == 1)
			{
				mid[j] = dist;
			}
			else if(normal[j] == -1)
			{
				mid[j] = -dist;
			}
			else
			{
				mid[j] = p1[j] + dot * (p2[j] - p1[j]);
			}
		}
			
		f->vertices[f->numvertices] = mid;
		f->numvertices++;
		b->vertices[b->numvertices] = mid;
		b->numvertices++;
	}
	
	if(f->numvertices > maxpts || b->numvertices > maxpts)
	{
		assert(0);
	}

	if (f->numvertices > 32 || b->numvertices > 32)
	{
		assert(0);
	}
}

// Classify where a polygon is with respect to a plane
int Polygon_OnPlaneSide(polygon_t *p, vec3 normal, float dist, float epsilon)
{
	bool	front, back;
	int			i;
	float		d;

	front = 0;
	back = 0;

	for(i = 0; i < p->numvertices; i++)
	{
		d = Dot(p->vertices[i], normal) - dist;
		
		if(d < -epsilon)
		{
			if(front)
				return POLYGON_SIDE_CROSS;
			back = 1;
			continue;
		}

		if(d > epsilon)
		{
			if(back)
				return POLYGON_SIDE_CROSS;
			front = 1;
			continue;
		}
	}

	if (back)
		return POLYGON_SIDE_BACK;
	if (front)
		return POLYGON_SIDE_FRONT;
	
	return POLYGON_SIDE_ON;
}

#if 0
// fixme: move these somewhere else
float Polygon_TriangleArea2D(float v[3][2])
{
	return ((v[1][0] - v[0][0]) * (v[2][1] - v[0][1]) - (v[2][0] - v[0][0]) * (v[1][1] - v[0][1]));
}

float Polygon_TriangleArea2D(float v0[2], float v1[2], float v2[2])
{
	return ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]));
}

static float Polygon_TriangleProjectedArea(int axis, float v0[3], float v1[3], float v2[3])
{
	float v[3][2];

	static int axistable[3][2] =
	{
		{ 1, 2 },
		{ 2, 0 },
		{ 0, 1 }
	};

	v[0][0] = v0[axistable[axis][0]];
	v[0][1] = v0[axistable[axis][1]];
	v[1][0] = v1[axistable[axis][0]];
	v[1][1] = v1[axistable[axis][1]];
	v[2][0] = v2[axistable[axis][0]];
	v[2][1] = v2[axistable[axis][1]];

	return Polygon_TriangleArea2D(v[0], v[1], v[2]);
}
#endif


