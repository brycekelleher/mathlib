#include "vector.h"
#include "plane.h"

plane_t::plane_t()
{}

plane_t::plane_t(float a, float b, float c, float d)
	: a(a), b(b), c(c), d(d)
{}

plane_t::plane_t(vec3 n, float d)
{
	a = n.x;
	b = n.y;
	c = n.z;
	d = -d;
}

void plane_t::Reverse()
{
	plane_t(-a, -b, -c, -d);
}

//void plane_t::PositionThroughPoint(vec3 p)
//{
//}
//
//void plane_t::SetNormal(vec3 n)
//{
//	normal = n;
//}
//
//void plane_t::SetDistance(float d)
//{
//	dist = d;
//}

vec3 plane_t::Normal()
{
	return vec3(a, b, c);
}

float plane_t::Distance()
{
	return d;
}

void plane_t::FromVecs(vec3 s, vec3 t, vec3 p)
{
	vec3 n	= Vec3_Cross(s, t);

	a	= n.x;
	b	= n.y;
	c	= n.z;
	d	= -Vec3_Dot(n, p);
}

void plane_t::FromPoints(vec3 p0, vec3 p1, vec3 p2)
{
	vec3 s = p1 - p0;
	vec3 t = p2 - p1;

	FromVecs(s, t, p0);
}

float plane_t::Distance(vec3& p)
{
	return (a * p.x) + (b * p.y) + (c * p.z) + d;
}

int plane_t::Side(vec3& p, const float epsilon)
{
	float d = Distance(p);

	if(d > epsilon)
	{
		return PLANE_SIDE_FRONT;
	}

	if(d < -epsilon)
	{
		return PLANE_SIDE_BACK;
	}

	return PLANE_SIDE_ON;
}

bool plane_t::PlaneLineIntersection(vec3 start, vec3 end, vec3 *hitpoint)
{
	float d1, d2;

	d1 = Distance(start);
	d2 = Distance(end);

	if (d1 == d2)
	{
		return false;
	}

	if (d1 > 0.0f && d2 > 0.0f)
	{
		return false;
	}

	if (d1 < 0.0f && d2 < 0.0f)
	{
		return false;
	}

	// calculate the intersection
	{
		float fraction;
		fraction = (d1 / (d1 - d2));

		*hitpoint = start + fraction * (start - end);
		return true;
	}

#if 0
	// calculate a more precise intersection
	{
		// this will only work if the normal is normalized
		vec3 n = Normal();
		fraction = (d1 / (d1 - d2));
		for(i = 0; i < 3; i++)
		{
			if (n[i] == 1)
			{
				hitpoint[i] = d;
			}
			else if (n[i] == -1)
			{
				hitpoint[i] = -d;
			}
			else
			{
				hitpoint[i] = start[i] + fraction * (start[i] - end[i]);
			}
		}
	}
#endif
}

bool plane_t::RayIntersection(vec3 start, vec3 dir, float* fraction)
{
#if 0	
	float d1, d2;

	d1 = normal.Dot(start) + dist;
	d2 = normal.Dot(dir);
	if(d2 == 0.0f)
		return f alse;

	*fraction = -(d1 / d2);
	return 1;
#endif	
	return false;
}

plane_t plane_t::operator-()
{
	plane_t r = plane_t(a, b, c, d);
	r.Reverse();

	return r;
}

