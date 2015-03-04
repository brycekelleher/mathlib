#include <stdio.h>
#include "polygon.h"

static void PrintPolygon(polygon_t *p)
{
	printf("polygon:\n");
	for(int i = 0; i < p->numvertices; i++)
	{
		printf("vertex %2i: %6.2f, %6.2f, %6.2f\n",
				i,
				p->vertices[i][0],
				p->vertices[i][1],
				p->vertices[i][2]);
	}

	printf("area: %f\n", Polygon_Area(p));

	vec3 n = Polygon_Normal2(p);
	printf("normal: %f, %f, %f\n", n[0], n[1], n[2]);
}

static void Polygon_Test1()
{
	polygon_t* p = Polygon_Alloc(4);

	p->numvertices = 4;
	p->vertices[0] = vec3(0, 0, 0);
	p->vertices[1] = vec3(4, 0, 0);
	p->vertices[2] = vec3(4, 4, 0);
	p->vertices[3] = vec3(0, 4, 0);

	PrintPolygon(p);
}

static void Polygon_Test2()
{
	polygon_t* p = Polygon_Alloc(3);

	p->numvertices = 3;
	p->vertices[0] = vec3(1, 0, 0);
	p->vertices[1] = vec3(0, 1, 0);
	p->vertices[2] = vec3(0, 0, 1);

	PrintPolygon(p);
}

int main(int argc, char *argv[])
{
	//Polygon_Test1();

	Polygon_Test2();

	return 0;
}
