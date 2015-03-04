#ifndef __INC_PLANE_H
#define __INC_PLANE_H

#define PLANE_SIDE_ON		0
#define PLANE_SIDE_BACK		1
#define PLANE_SIDE_FRONT	2

#define PLANE_DEFAULT_EPSILON	(0.1f)

class vec3;

class plane_t
{
public:
	float	a;
	float	b;
	float	c;
	float	d;

	plane_t();
	plane_t(float a, float b, float c, float d);
	plane_t(vec3 n, float d);

	void Reverse();
	void PositionThroughPoint(vec3 p);
	//void SetNormal(vec3 n);
	//void SetDistance(float d);
	vec3 Normal();
	float Distance();
	void Zero();
	void FromPoints(vec3 p0, vec3 p1, vec3 p2);
	void FromVecs(vec3 v0, vec3 v1, vec3 p);

	float Distance(vec3& p);
	int Side(vec3& p, const float epsilon = 0.1f);
	bool PlaneLineIntersection(vec3 start, vec3 end, vec3 *hitpoint);
	bool RayIntersection(vec3 start, vec3 dir, float* fraction);

	plane_t operator-();
};

#endif
