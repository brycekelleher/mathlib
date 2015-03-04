/*=============================================================================
	vector.h
============================================================================*/

#ifndef __VECTOR_H__
#define __VECTOR_H__

#define EPSILON_E4	(1e-4)
#define EPSILON_E8	(1e-8)

#include <math.h>

/*-----------------------------------------------------------------------------
	vec2
-----------------------------------------------------------------------------*/

class vec2
{
public:
	float	x;
	float	y;
			
	// constructors
	vec2();
	vec2(const float x, const float y);

	operator float*();
	float operator[](int i) const;
	float& operator[](int i);

	vec2 operator-() const;

	// functions
	void Set(const float _x, const float _y);
	void MakeZero();
	bool IsZero();
	bool IsNearlyZero();
	void Normalize();
	float Length();
	float LengthSquared();
	float Dot(const vec2& v) const;
	vec2 Perp() const;
	float* Ptr();
	const float* Ptr() const;
};

inline vec2::vec2()
{}

inline vec2::vec2(const float x, const float y)
	: x(x), y(y)
{}

inline vec2::operator float*()
{
	return &x;
}

// read from an indexed element
inline float vec2::operator[](int i) const
{
	return (&this->x)[i];
}

// write to an indexed element
inline float& vec2::operator[](int i)
{
	return (&this->x)[i];
}
// unary operators
inline vec2 vec2::operator-() const
{
	//Negate();
	return vec2(-x, -y);
}

// functions
inline void vec2::Set(const float _x, const float _y)
{
	x = _x;
	y = _y;
}

inline void vec2::MakeZero()
{
	x = 0; 
	y = 0;
}

inline bool vec2::IsZero()
{
	return (x == 0) && (y == 0);
}

inline bool vec2::IsNearlyZero()
{
	return((x < EPSILON_E4) && (y < EPSILON_E4));
}

inline void vec2::Normalize()
{
	float invl = 1.0f / sqrtf((x * x) + (y * y));
	
	x *= invl;
	y *= invl;
}

inline float vec2::Length()
{
	return sqrtf((x * x) + (y * y));
}

inline float vec2::LengthSquared()
{
	return (x * x) + (y * y);
}

inline float* vec2::Ptr()
{
	return &x;
}

inline const float* vec2::Ptr() const
{
	return &x;
}

// new operators
inline vec2 operator+(const vec2 a, const vec2 b)
{
	vec2 r;

	r.x = a.x + b.x;
	r.y = a.y + b.y;

	return r;
}

inline vec2 operator-(const vec2 a, const vec2 b)
{
	vec2 r;

	r.x = a.x - b.x;
	r.y = a.y - b.y;

	return r;
}

inline vec2 operator*(const vec2 a, const vec2 b)
{
	vec2 r;

	r.x = a.x * b.x;
	r.y = a.y * b.y;

	return r;
}

inline vec2 operator/(const vec2 a, const vec2 b)
{
	return vec2(a.x / b.x, a.y / b.y);
}

inline vec2 operator*(const float s, const vec2 v)
{
	vec2 r;

	r.x = s * v.x;
	r.y = s * v.y;

	return r;
}

inline vec2 operator*(const vec2 v, const float s)
{
	return operator*(s, v);
}

inline vec2 operator/(const vec2 v, const float s)
{
	float rs = 1.0f / s;
	return rs * v;
}

inline bool operator==(const vec2 a, const vec2 b)
{
	return (a.x == b.x) && (a.y == b.y);
}

inline bool operator!=(const vec2 a, const vec2 b)
{
	return (a.x != b.x) || (a.y != b.y);
}

inline float Dot(const vec2 a, const vec2 b)
{
	return (a.x * b.x) + (a.y * b.y);
}

inline vec2 Normalize(vec2 v)
{
	v. Normalize();
	return v;
}

inline vec2 Skew(vec2 a)
{
	return vec2(-a.y, a.x);
}

static vec2 vec2_zero = vec2(0.0f, 0.0f);

/*-----------------------------------------------------------------------------
	vec3
-----------------------------------------------------------------------------*/

class vec3
{
public:
	float	x;
	float	y;
	float	z;
			
	// constructors
	vec3();
	vec3(const float x, const float y, const float z);

	operator float*();
	float operator[](int i) const;
	float& operator[](int i);

	vec3 operator-() const;

	// functions
	void Set(const float _x, const float _y, const float _z);
	void MakeZero();
	bool IsZero();
	bool IsNearlyZero();
	void Normalize();
	float Length();
	float LengthSquared();
	float* Ptr();
	const float* Ptr() const;
};

inline vec3::vec3()
{}

inline vec3::vec3(const float x, const float y, const float z)
	: x(x), y(y), z(z)
{}

inline vec3::operator float*()
{
	return &x;
}

// read from an indexed element
inline float vec3::operator[](int i) const
{
	return (&this->x)[i];
}

// write to an indexed element
inline float& vec3::operator[](int i)
{
	return (&this->x)[i];
}
// unary operators
inline vec3 vec3::operator-() const
{
	return vec3(-x, -y, -z);
}

// functions
inline void vec3::Set(const float _x, const float _y, const float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

inline void vec3::MakeZero()
{
	x = 0; 
	y = 0;
	z = 0;
}

inline bool vec3::IsZero()
{
	return (x == 0) && (y == 0) && (z == 0);
}

inline bool vec3::IsNearlyZero()
{
	return (x < EPSILON_E4) && (y < EPSILON_E4) && (z < EPSILON_E4);
}

inline void vec3::Normalize()
{
	float invl = 1.0f / sqrtf((x * x) + (y * y) + (z * z));
	
	x *= invl;
	y *= invl;
	z *= invl;
}

inline float vec3::Length()
{
	return sqrtf((x * x) + (y * y) + (z * z));
}

inline float vec3::LengthSquared()
{
	return (x * x) + (y * y) + (z * z);
}

inline float* vec3::Ptr()
{
	return &x;
}

inline const float* vec3::Ptr() const
{
	return &x;
}

// new operators
inline vec3 operator+(const vec3 a, const vec3 b)
{
	vec3 r;

	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;

	return r;
}

inline vec3 operator-(const vec3 a, const vec3 b)
{
	vec3 r;

	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;

	return r;
}

inline vec3 operator*(const vec3 a, const vec3 b)
{
	vec3 r;

	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;

	return r;
}

inline vec3 operator/(const vec3 a, const vec3 b)
{
	return vec3(a.x / b.x, a.y / b.y, a.z / b.z);
}

inline vec3 operator*(const float s, const vec3 v)
{
	vec3 r;

	r.x = s * v.x;
	r.y = s * v.y;
	r.z = s * v.z;

	return r;
}

inline vec3 operator*(const vec3 v, const float s)
{
	return operator*(s, v);
}

inline vec3 operator/(const vec3 v, const float s)
{
	float rs = 1.0f / s;
	return rs * v;
}

inline bool operator==(const vec3 a, const vec3 b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

inline bool operator!=(const vec3 a, const vec3 b)
{
	return (a.x != b.x) || (a.y != b.y) || (a.z != b.z);
}

inline float Dot(const vec3 a, const vec3 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline vec3 Cross(const vec3& a, const vec3& b)
{
	return vec3((a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x));
}

inline vec3 Normalize(vec3 v)
{
	v.Normalize();
	return v;
}

static vec3 vec3_zero = vec3(0.0f, 0.0f, 0.0f);

/*-----------------------------------------------------------------------------
	vec4
-----------------------------------------------------------------------------*/

class vec4
{
public:
	float	x;
	float	y;
	float	z;
	float	w;
			
	// constructors
	vec4();
	vec4(const float x, const float y, const float z, const float w);

	operator float*();
	float operator[](int i) const;
	float& operator[](int i);

	vec4 operator-() const;

	// functions
	void Set(const float _x, const float _y, const float _z, const float _w);
	void MakeZero();
	bool IsZero();
	bool IsNearlyZero();
	void Normalize();
	float Length();
	float LengthSquared();
	float* Ptr();
	const float* Ptr() const;
};

inline vec4::vec4()
{}

inline vec4::vec4(const float x, const float y, const float z, const float w)
	: x(x), y(y), z(z), w(w)
{}

inline vec4::operator float*()
{
	return &x;
}

// read from an indexed element
inline float vec4::operator[](int i) const
{
	return (&this->x)[i];
}

// write to an indexed element
inline float& vec4::operator[](int i)
{
	return (&this->x)[i];
}
// unary operators
inline vec4 vec4::operator-() const
{
	return vec4(-x, -y, -z, -w);
}

// functions
inline void vec4::Set(const float _x, const float _y, const float _z, const float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

inline void vec4::MakeZero()
{
	x = 0; 
	y = 0;
	z = 0;
	w = 0;
}

inline bool vec4::IsZero()
{
	return (x == 0) && (y == 0) && (z == 0) && (w == 0);
}

inline bool vec4::IsNearlyZero()
{
	return (x < EPSILON_E4) && (y < EPSILON_E4) && (z < EPSILON_E4);
}

inline void vec4::Normalize()
{
	float invl = 1.0f / sqrtf((x * x) + (y * y) + (z * z) + (w * w));
	
	x *= invl;
	y *= invl;
	z *= invl;
	w *= invl;
}

inline float vec4::Length()
{
	return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
}

inline float vec4::LengthSquared()
{
	return (x * x) + (y * y) + (z * z) + (w * w);
}

inline float* vec4::Ptr()
{
	return &x;
}

inline const float* vec4::Ptr() const
{
	return &x;
}

inline vec4 operator+(const vec4 a, const vec4 b)
{
	vec4 r;

	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	r.w = a.w + b.w;

	return r;
}

inline vec4 operator-(const vec4 a, const vec4 b)
{
	vec4 r;

	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
	r.w = a.w - b.w;

	return r;
}

inline vec4 operator*(const vec4 a, const vec4 b)
{
	vec4 r;

	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;
	r.w = a.w * b.w;

	return r;
}

inline vec4 operator/(const vec4 a, const vec4 b)
{
	return vec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

inline vec4 operator*(const float s, const vec4 v)
{
	vec4 r;

	r.x = s * v.x;
	r.y = s * v.y;
	r.z = s * v.z;
	r.w = s * v.w;

	return r;
}

inline vec4 operator*(const vec4 v, const float s)
{
	return operator*(s, v);
}

inline vec4 operator/(const vec4 v, const float s)
{
	float rs = 1.0f / s;
	return rs * v;
}

inline bool operator==(const vec4 a, const vec4 b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

inline bool operator!=(const vec4 a, const vec4 b)
{
	return (a.x != b.x) || (a.y != b.y) || (a.z != b.z) || (a.w != b.w);
}

inline float Dot(const vec4 a, const vec4 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

inline vec4 Normalize(vec4 v)
{
	v.Normalize();
	return v;
}

static vec4 vec4_zero(0.0f, 0.0f, 0.0f);

#endif

