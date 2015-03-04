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

vec3 Polygon_Normal(polygon_t* p)
{
	vec3 n;
	vec3 e0, e1;

	assert(p->numvertices >= 3);
	e0 = p->vertices[1] - p->vertices[0];
	e1 = p->vertices[2] - p->vertices[1];
	
	assert(e0.Length() > 0.0f && e1.Length() > 0.0f);
	n = Cross(e0, e1);
	n.Normalize();

	return n;
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

float Polygon_TriangleArea2D(float v[3][2])
{
	return 0.5f * ((v[1][0] - v[0][0]) * (v[2][1] - v[0][1])) - ((v[2][0] - v[0][0]) * (v[1][1] - v[0][1]));
}

float Polygon_TriangleArea2D(float v0[2], float v1[2], float v2[2])
{
	return 0.5f * ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]));
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

float Polygon_TriangleArea(float v0[3], float v1[3], float v2[3])
{
	// projected areas on the x, y and z axis
	float x = 0.5f * ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]));
	float y = 0.5f * ((v1[2] - v0[2]) * (v2[0] - v0[0]) - (v2[2] - v0[2]) * (v1[0] - v0[0]));
	float z = 0.5f * ((v1[1] - v0[1]) * (v2[2] - v0[2]) - (v2[1] - v0[1]) * (v1[2] - v0[2]));

	float a = sqrtf((x * x) + (y * y) + (z * z));

	return a;
}

vec3 Polygon_TriangleProjectedAreas(float v0[3], float v1[3], float v2[3])
{
	// projected areas on the x, y and z axis
	float x = 0.5f * ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]));
	float y = 0.5f * ((v1[2] - v0[2]) * (v2[0] - v0[0]) - (v2[2] - v0[2]) * (v1[0] - v0[0]));
	float z = 0.5f * ((v1[1] - v0[1]) * (v2[2] - v0[2]) - (v2[1] - v0[1]) * (v1[2] - v0[2]));

	return vec3(x, y, z);
}

static float Polygon_ProjectedArea(polygon_t *p, int axis)
{
	float a = 0;

	for(int i = 0; i < p->numvertices - 2; i++)
	{
		a += Polygon_TriangleProjectedArea(axis, p->vertices[0], p->vertices[i + 1], p->vertices[i + 2]);
	}

	return a;
}

#include <stdio.h>

float Polygon_Area(polygon_t* p)
{
	// projected areas on the x, y and z axis
	float x = Polygon_ProjectedArea(p, 0);
	float y = Polygon_ProjectedArea(p, 1);
	float z = Polygon_ProjectedArea(p, 2);

	printf("x: %f, y: %f, z: %f\n", x, y, z);

	float a = sqrtf((x * x) + (y * y) + (z * z));

	return a;
}

vec3 Polygon_Normal2(polygon_t *p)
{
	// projected areas on the x, y and z axis
	float x = Polygon_ProjectedArea(p, 0);
	float y = Polygon_ProjectedArea(p, 1);
	float z = Polygon_ProjectedArea(p, 2);

	vec3 n = vec3(x, y, z);
	n = Normalize(n);

	return n;
}

#if 0
bool Polygon_IsConvex()
{
	int i = -1;
	int j = 0;
	int k = 1;

	i = j;
	j = k;
	k++;

	int signs[2] = { 0 };

	for(int i = 0; i < p->numvertices; i++)
	{
		float area = Polygon_TriangleArea(i, j, k);
		
		signs[area < 0.0f ? 1 : 0];
	}

	return (signs[0] && !signs[1]) || (!signs[0] && signs[1]);
}
#endif

float Polygon_ProjectedArea(polygon_t *p, int axis0, int axis1)
{
	int axis[2];

	int numtriangles = p->numvertices - 2;
	for(int i = 0; i < numtriangles - 2; i++)
	{
		float v[3][2];

		v[0][0] = p->vertices[0][axis[0]];
		v[0][1] = p->vertices[0][axis[1]];
		v[1][0] = p->vertices[i + 0][axis[0]];
		v[1][1] = p->vertices[i + 0][axis[1]];
		v[2][0] = p->vertices[i + 1][axis[0]];
		v[2][1] = p->vertices[i + 1][axis[1]];

		// Compute the 3x3 determinant

		//Polygon(
	
	}

	return 0;
}

#if 0
// Compute the plane the polygon is on
// fixme: d should be negative
void Polygon_PlaneDistance(polygon_t* p, vec3 n, float d)
{
	n = Polygon_Normal(p);
	d = Dot(n, p->vertices[0]);
}
#endif

polygon_t *Polygon_Reverse(polygon_t* p)
{
	polygon_t	*r;

	r = (polygon_t*)Polygon_Alloc(p->maxvertices);

	for(int i = 0; i < p->numvertices; i++)
		r->vertices[(i + i) % p->numvertices] = p->vertices[p->numvertices - 1 - i];

	return r;
}

#if 0
static float Polygon_VecDot(vec3 a, vec3 b)
{
	return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

static float Polygon_PlaneDist(plane_t plane, vec3 point)
{
	return (plane.a  * point.x) + (plane.b * point.y) + (plane.c * point.z) + plane.d;
}
#endif


void Polygon_ClipWithPlane (polygon_t *in, vec3 normal, float dist, float epsilon, polygon_t **front, polygon_t **back)
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
			// Add the point to the front winding
			f->vertices[f->numvertices] = p1;
			f->numvertices++;

			// Add the point to the back winding
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
		//Error ("ClipWinding: points exceeded estimate");
	}

	if (f->numvertices > 32 || b->numvertices > 32)
	{
		assert(0);
		//Error ("ClipWinding: MAX_POINTS_ON_WINDING");
	}
}

#if 0
// Clip the polygon inplace
void PolygonClipWithPlane (polygon_t** inout, vec3 normal, float dist, float epsilon)
{
	float	dists[MAX_POLYGONVERTICES+4];
	int		sides[MAX_POLYGONVERTICES+4];
	int		counts[3];		// FRONT, BACK, ON
	static	float dot;		// VC 4.2 optimizer bug if not static
	int		i, j;
	vec3	p1, p2;
	vec3	mid;
	polygon_t	*f;
	int		maxpts;
	
	polygon_t* in = *inout;
	counts[0] = counts[1] = counts[2] = 0;

	// Classify each point
	for(i = 0; i < in->numvertices; i++)
	{
		dot = in->vertices[i].Dot(normal);
		dot -= dist;
		dists[i] = dot;
		
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

	// This case doesn't exist in the quake compiler
	if(!counts[0] && !counts[1])
	{
		// All points are on the split plane, treat as in front
		return;
	}

	if (!counts[0])
	{
		// All points are on the back side
		Polygon_Free(in);
		*inout = 0;
		return;
	}

	if (!counts[1])
	{
		// All points are on the front side
		return;
	}

	maxpts = in->numvertices+4;	// cant use counts[0]+2 because
								// of fp grouping errors

	f = Polygon_Alloc();
		
	for(i = 0; i < in->numvertices; i++)
	{
		p1 = in->vertices[i];
		
		if(sides[i] == POLYGON_SIDE_ON)
		{
			// Add the point to the front winding
			f->vertices[f->numvertices] = p1;
			f->numvertices++;

			continue;
		}
	
		if(sides[i] == POLYGON_SIDE_FRONT)
		{
			// Add the point to the front polygon
			f->vertices[f->numvertices] = p1;
			f->numvertices++;
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
	}
	
	if(f->numvertices > maxpts)
	{
		assert(0);
		//Error ("ClipWinding: points exceeded estimate");
	}

	if (f->numvertices > MAX_POLYGONVERTICES)
	{
		assert(0);
		//Error ("ClipWinding: MAX_POINTS_ON_WINDING");
	}

	Polygon_Free(in);
	*inout = f;
}
#endif

void Polygon_ClipWithPlane(polygon_t** inout, vec3 normal, float dist, float epsilon)
{
	polygon_t* f;
	polygon_t* b;

	Polygon_ClipWithPlane(*inout, normal, dist, epsilon, &f, &b);

	if(b)
		Polygon_Free(b);

	// fixme: copying of vertices will break if the polygon allocation
	// is changed to support variable amounts of verts
	*inout = f;
}

// Classify where a polygon is with respect to a plane
int	PolygonOnPlaneSide(polygon_t *p, vec3 normal, float dist, float epsilon)
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
// Generate a polygon from a plane
polygon_t *PolygonFromPlane(vec3& normal, float dist, float size)
{
	int		i, x;
	float	max, v;
	vec3	org, vright, vup;
	polygon_t	*p;
	
// find the major axis

	max = -1.0f;
	x = -1;
	for (i=0 ; i<3; i++)
	{
		v = fabs(normal[i]);
		if (v > max)
		{
			x = i;
			max = v;
		}
	}

	// Asserts if no axis was found
	assert(x != -1);
		
	vup = vec3(0, 0, 0);
	switch (x)
	{
	case 0:
		vup[1] = 1;
		break;		
	case 1:
		vup[2] = 1;
		break;		
	case 2:
		vup[0] = 1;
		break;		
	}

	v = vup.Dot(normal);
	vup = vup + -v * normal;
	vup.Normalize();

	org = normal * dist;
	
	vright = vup.Cross(normal);

	vup = size * vup;
	vright = size * vright;

// project a really big	axis aligned box onto the plane
	p = Polygon_Alloc();
	
	p->vertices[0] = org - vright + vup;
	p->vertices[1] = org - vright - vup;
	p->vertices[2] = org + vright - vup;
	p->vertices[3] = org + vright + vup;
	p->numvertices = 4;
	
	return p;
}

void FindCoIncidentVertices(polygon_t* p)
{
	int i, j, k;

	for(i = 0; i < p->numvertices; i++)
	{
		j = (i + 1) % p->numvertices;
		k = (i + 2) % p->numvertices;

		vec3 e0 = p->vertices[k] - p->vertices[i];
		vec3 e1 = p->vertices[j] - p->vertices[i];
		e0.Normalize();
		e1.Normalize();

		float f = e0.Dot(e1);
		if(f > 0.99f)
		{
			// Got a coincident vertex, so remove j
			//memcpy(p->vertices + j, p->vertices + k, (p->numvertices - i - 2) * sizeof(vec3));
			//p->numvertices--;
		}
	}
}

#endif

