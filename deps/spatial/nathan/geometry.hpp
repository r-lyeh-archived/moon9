// GEOMETRY.HPP
// Copyright (C) 2004-2007 by Nathan Reed.  All rights and priveleges reserved.
// Lightweight 3D math library

#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>

namespace nathan {

// Some useful constants
const float pi = 3.141592654f;
extern const float epsilon;
extern const float infty;

//////////////////////////////////////////////////////////////////////////////
// Templated n-dimensional vector of floats

template <int n>
struct floatvec
{
	float elem[n];

	// Constructors
	floatvec ();
	floatvec (const floatvec<n> &rhs);
	floatvec (const float *elements);

	// Access operators
	operator float * ();
	operator const float * () const;

	// Relational operators
	bool operator == (const floatvec<n> &rhs) const;
	bool operator != (const floatvec<n> &rhs) const;

	// Arithmetic operators
	floatvec<n> operator + (const floatvec<n> &rhs) const;
	floatvec<n> operator - (const floatvec<n> &rhs) const;
	floatvec<n> operator - () const;
	floatvec<n> operator * (float f) const;
	floatvec<n> operator / (float f) const;

	// Compound arithmetic operators
	floatvec<n> &operator += (const floatvec<n> &rhs);
	floatvec<n> &operator -= (const floatvec<n> &rhs);
	floatvec<n> &operator *= (float f);
	floatvec<n> &operator /= (float f);

	// Other operations
	float magnitude () const;
	floatvec<n> normalize () const;
};

// External operator to allow expressions of the form float * vector
template <int n>
floatvec<n> operator * (float f, const floatvec<n> &rhs);

// Dot product, implemented as a global function
template <int n>
float dot (const floatvec<n> &lhs, const floatvec<n> &rhs);

// Output operator
template <int n>
std::ostream &operator << (std::ostream &out, const floatvec<n> &rhs);

//////////////////////////////////////////////////////////////////////////////
// Templated n x m matrix of floats

template <int n, int m>
struct floatmat
{
	float elem[n][m];

	// Constructors
	floatmat ();
	floatmat (const floatmat<n,m> &rhs);
	explicit floatmat (const float *elements);

	// Access operators
	floatvec<m> &operator [] (size_t i);					// Returns a row
	const floatvec<m> &operator [] (size_t i) const;		// Returns a row

	const float *data () const { return &elem[0][0]; }
	float *data () { return &elem[0][0]; }

	// Relational operators
	bool operator == (const floatmat<n,m> &rhs) const;
	bool operator != (const floatmat<n,m> &rhs) const;

	// Arithmetic operators
	floatmat<n,m> operator + (const floatmat<n,m> &rhs) const;
	floatmat<n,m> operator - (const floatmat<n,m> &rhs) const;
	floatmat<n,m> operator - () const;
	floatmat<n,m> operator * (float f) const;
	floatmat<n,m> operator / (float f) const;

	// Compound arithmetic operators
	floatmat<n,m> &operator += (const floatmat<n,m> &rhs);
	floatmat<n,m> &operator -= (const floatmat<n,m> &rhs);
	floatmat<n,m> &operator *= (float f);
	floatmat<n,m> &operator /= (float f);

	// Multiplication operators - these use the template syntax to enforce constraints on
	// the sizes of matrices and vectors that can be multiplied together
	floatvec<n> operator * (const floatvec<m> &rhs) const;
	template <int r> floatmat<n,r> operator * (const floatmat<m,r> &rhs) const;

	// Miscellaneous other operations
	floatmat<m,n> transpose () const;
};

// External operator to allow expressions of the form float * matrix and rowvector * matrix
template <int n, int m>
floatmat<n,m> operator * (float f, const floatmat<n,m> &rhs);

template <int n, int m>
floatvec<m> operator * (const floatvec<n> &lhs, const floatmat<n,m> &rhs);

// Output operator
template <int n, int m>
std::ostream &operator << (std::ostream &out, const floatmat<n,m> &rhs);

//////////////////////////////////////////////////////////////////////////////
// Specialization of floatmat for square matrices, adding inverse function

// Exception class, thrown when try to invert a noninvertable matrix
struct singular_matrix_exception {};

template <int n>
struct square_floatmat: public floatmat<n,n>
{
	// Constructors - these are needed to allow proper implicit conversions
	square_floatmat ();
	square_floatmat (const floatmat<n,n> &rhs);
	square_floatmat (const square_floatmat<n> &rhs);
	explicit square_floatmat (const float *elements);

	// Identity matrix
	static square_floatmat<n> identity ();

	// Trace
	float trace () const;

	// Throws singular_matrix_exception when the inverse does not exist
	square_floatmat<n> inverse () const;
};

//////////////////////////////////////////////////////////////////////////////
// Specializations of floatvec for 2, 3, and 4 dimensions

struct vec2: public floatvec<2>
{
	// Constructors
	vec2 (float x = 0, float y = 0)
	{
		elem[0] = x;
		elem[1] = y;
	}
	vec2 (const floatvec<2> &rhs): floatvec<2>(rhs) {}
	vec2 (const vec2 &rhs): floatvec<2>(rhs) {}
	explicit vec2 (const float *elements): floatvec<2>(elements) {}
};

struct vec3: public floatvec<3>
{
	// Constructors
	vec3 (float x = 0, float y = 0, float z = 0)
	{
		elem[0] = x;
		elem[1] = y;
		elem[2] = z;
	}
	vec3 (const vec2 &xy, float z)
	{
		elem[0] = xy[0];
		elem[1] = xy[1];
		elem[2] = z;
	}
	vec3 (const floatvec<3> &rhs): floatvec<3>(rhs) {}
	vec3 (const vec3 &rhs): floatvec<3>(rhs) {}
	explicit vec3 (const float *elements): floatvec<3>(elements) {}

	// Conversion to vec2
	const vec2 &xy () const
		{ return *reinterpret_cast<const vec2 *>(elem); }
	vec2 &xy ()
		{ return *reinterpret_cast<vec2 *>(elem); }
};

// Cross product, implemented as a global function
inline vec3 cross (const vec3 &lhs, const vec3 &rhs)
{
	return vec3(lhs[1]*rhs[2] - lhs[2]*rhs[1],
	            lhs[2]*rhs[0] - lhs[0]*rhs[2],
	            lhs[0]*rhs[1] - lhs[1]*rhs[0]);
}

struct vec4: public floatvec<4>
{
	// Constructors
	vec4 (float x = 0, float y = 0, float z = 0, float w = 0)
	{
		elem[0] = x;
		elem[1] = y;
		elem[2] = z;
		elem[3] = w;
	}
	vec4 (const vec2 &xy, float z, float w)
	{
		elem[0] = xy[0];
		elem[1] = xy[1];
		elem[2] = z;
		elem[3] = w;
	}
	vec4 (const vec3 &xyz, float w)
	{
		elem[0] = xyz[0];
		elem[1] = xyz[1];
		elem[2] = xyz[2];
		elem[3] = w;
	}
	vec4 (const floatvec<4> &rhs): floatvec<4>(rhs) {}
	vec4 (const vec4 &rhs): floatvec<4>(rhs) {}
	explicit vec4 (const float *elements): floatvec<4>(elements) {}

	// Conversion to vec2
	const vec2 &xy () const
		{ return *reinterpret_cast<const vec2 *>(elem); }
	vec2 &xy ()
		{ return *reinterpret_cast<vec2 *>(elem); }

	// Conversion to vec3
	const vec3 &xyz () const
		{ return *reinterpret_cast<const vec3 *>(elem); }
	vec3 &xyz ()
		{ return *reinterpret_cast<vec3 *>(elem); }
};

//////////////////////////////////////////////////////////////////////////////
// Specializations of floatmat for 2x2, 3x3, and 4x4 matrices

struct mat2: public square_floatmat<2>
{
	// Constructors
	mat2 () {}
	mat2 (float f00, float f01, float f10, float f11);
	mat2 (const vec2 &a, const vec2 &b);						// By columns
	mat2 (const floatmat<2,2> &rhs): square_floatmat<2>(rhs) {}
	mat2 (const square_floatmat<2> &rhs): square_floatmat<2>(rhs) {}
	mat2 (const mat2 &rhs): square_floatmat<2>(rhs) {}
	explicit mat2 (const float *elements): square_floatmat<2>(elements) {}
	
	// Functions to generate various common transformations
	static mat2 rotation (float radians);
	static mat2 scale (const vec2 &s);
};

struct mat3: public square_floatmat<3>
{
	// Constructors
	mat3 () {}
	mat3 (float f00, float f01, float f02, float f10, float f11, float f12, float f20, float f21, float f22);
	mat3 (const vec3 &a, const vec3 &b, const vec3 &c);			// By columns
	mat3 (const floatmat<3,3> &rhs): square_floatmat<3>(rhs) {}
	mat3 (const square_floatmat<3> &rhs): square_floatmat<3>(rhs) {}
	mat3 (const mat3 &rhs): square_floatmat<3>(rhs) {}
	explicit mat3 (const float *elements): square_floatmat<3>(elements) {}

	// Functions to generate various common transformations
	static mat3 rotation (const vec3 &axis, float radians);
	static mat3 scale (const vec3 &s);
};

struct affine;
struct mat4: public square_floatmat<4>
{
	// Constructors
	mat4 () {}
	mat4 (const vec4 &a, const vec4 &b, const vec4 &c, const vec4 &d);	// By columns
	explicit mat4 (const affine &a);												// Homogeneous version of affine xform
	explicit mat4 (const mat3 &m);												// Homogeneous version of 3x3 mat
	mat4 (const floatmat<4,4> &rhs): square_floatmat<4>(rhs) {}
	mat4 (const square_floatmat<4> &rhs): square_floatmat<4>(rhs) {}
	mat4 (const mat4 &rhs): square_floatmat<4>(rhs) {}
	explicit mat4 (const float *elements): square_floatmat<4>(elements) {}
};

//////////////////////////////////////////////////////////////////////////////
// Affine transformation on R^3

// Forward-declare classes needed in the affine definition
struct line;
struct plane;

struct affine
{
	mat3 L;				// Linear part
	vec3 t;				// Translation part

	// Constructors
	affine () {}
	affine (const vec3 &a, const vec3 &b, const vec3 &c, const vec3 &t_):	// By columns
		L(a, b, c), t(t_) {}
	affine (const mat3 &linear_part, const vec3 &translation_part):
		L(linear_part), t(translation_part) {}

	// Operators - we use multiplication to compose transformations
	affine operator * (const affine &rhs) const
		{ return affine(L * rhs.L, L * rhs.t + t); }
	affine operator * (const mat3 &rhs) const
		{ return affine(L * rhs, t); }
	affine &operator *= (const affine &rhs) { *this = *this * rhs; return *this; }
	affine &operator *= (const mat3 &rhs) { *this = *this * rhs; return *this; }

	// Affine transformations can be applied to points and to vectors; the former
	// are affected by the translation part, but not the latter.  Therefore, we have
	// two application functions.
	vec3 transform_pt (const vec3 &rhs) const
		{ return L * rhs + t; }
	vec3 transform_vec (const vec3 &rhs) const
		{ return L * rhs; }	
	
	// Two varieties of inverse - these throw std::domain error if no inverse exists
	affine inverse () const;		// General inverse for an affine transformation
	affine rigid_inverse () const;	// For rigid body transforms (rotation and translation): quicker inverse

	mat3 invtrans () const { return L.inverse().transpose(); }

	// Functions to generate various common transformations
	static affine translation (const vec3 &t);
	static affine translation (float x, float y, float z);
	static affine rotation (const vec3 &axis, float angle);
	static affine rotation (float x, float y, float z, float angle);
	static affine scale (float s);
	static affine scale (const vec3 &s);
	static affine scale (float x, float y, float z);
	static affine projection (const line &l);					// Orthogonal projection onto a line
	static affine projection (const plane &p);					// Orthogonal projection onto a plane
	static affine projection (const plane &p, const vec3 &v);	// Projection along a vector v onto a plane p
	static affine reflection (const plane &p);					// Reflection though a plane

	static affine identity ()
		{ return affine(mat3::identity(), vec3(0, 0, 0)); }
};

// A further operator to allow mat3 * affine expressions
inline affine operator * (const mat3 &lhs, const affine &rhs)
{
	return affine(lhs * rhs.L, lhs * rhs.t);
}

//////////////////////////////////////////////////////////////////////////////
// Structs for other simple geometric objects

struct line
{
	vec3 origin, direction;

	// Constructors
	line ();
	line (const vec3 &origin_, const vec3 &direction_);

	// Evaluate parametric line function
	vec3 operator () (float t) const;

	// Transform by a matrix
	line transform (const affine &trans) const;
};

struct ray
{
	vec3 origin, direction;
	float max_t;			// initialized to infinity by default

	ray (): max_t(infty){}
	ray (const vec3 &origin_, const vec3 &direction_, float max_t_ = infty):
		origin(origin_), direction(direction_), max_t(max_t_) {}
	
	// Evaluate parametric line function
	vec3 operator () (float t) const { return origin + t * direction; }

	// Transform by a matrix
	ray transform (const affine &xform) const
	{
		return ray(xform.transform_pt(origin),
		           xform.transform_vec(direction),
		           max_t);
	}
};

struct plane
{
	vec4 coeff;								// Coefficients of the plane equation Ax + By + Cz + D = 0

	// Constructors
	plane (float a = 0, float b = 0, float c = 0, float d = 0);
	plane (const vec3 &normal, const vec3 &point);				// Normal and a point on the plane
	plane (const vec3 &u, const vec3 &v, const vec3 &q);		// Three points on the plane

	const vec3 &normal () const;
	vec3 &normal ();
	plane normalize () const;

	// Transform by a matrix
	plane transform (const affine &xform) const;
	plane transform (const affine &xform, const mat3 &invtrans) const;

	// The "outside" of a plane is the side to which its normal points
	bool outside (const vec3 &u) const;
	bool outside (float x, float y, float z) const;
};

// Bivector - a rank-2 contravariant tensor;
// represents an oriented area element
struct bivector
{
	mat3 coeff;

	// Constructors
	bivector ();
	explicit bivector (const mat3 &mat);						// Specify coefficients directly
	bivector (const vec3 &u, const vec3 &v);		// Edge vectors of the area element
	explicit bivector (const vec3 &normal);					// Normal to the area element

	// Extract the normal or its magnitude from a bivector
	vec3 normal () const;
	float magnitude () const;

	// Multiply/divide by scalar
	bivector operator * (float f) const;
	bivector operator / (float f) const;
	bivector& operator *= (float f);
	bivector& operator /= (float f);

	// Transform by a matrix
	bivector transform (const mat3 &xform) const;

};
// External operator for float * bivector multiplications
bivector operator * (float f, const bivector &b);

// Axis-aligned bounding box (AABB)
struct aabb
{
	vec3 mins, maxs;

	aabb ();
	aabb (const vec3 &a, const vec3 &b);			// Two opposite corners of the box

	bool isnull () const;
	
	bool outside (const vec3 &u) const;
	bool outside (float x, float y, float z) const;

	bool intersects (const ray &r) const;

	static const aabb null;

	static aabb bound (const aabb &box, const vec3 &p);			// Compute a box bounding another box and a point
	static aabb bound (const aabb &a, const aabb &b);			// Compute a box bounding two other boxes
	static aabb bound (const aabb &box, const affine &trans);	// Transform a box and compute new box bounding it
};

struct triangle
{
	vec3 a, b, c;
	plane p;

	triangle ();
	triangle (const vec3 &a_, const vec3 &b_, const vec3 &c_);

	vec3 from_barycentric (float a_, float b_, float c_) const;
	vec3 from_barycentric (const vec3 &coords) const;
	vec3 to_barycentric (const vec3 &v) const;
};

//////////////////////////////////////////////////////////////////////////////
// Basis of R^3 with arbitrary origin

struct basis
{
	vec3 v1, v2, v3;
	vec3 origin;

	basis ();
	basis (const vec3 &v1_, const vec3 &v2_, const vec3 &v3_, const vec3 &origin_ = vec3(0, 0, 0));
	explicit basis (const vec3 &origin_);

	affine to_standard () const;
	affine from_standard () const;
	static affine change (const basis &b1, const basis &b2);

	static const basis standard;

	basis orthonormalize () const;					// Performs Gram-Schmidt process
	float determinant () const;						// Determinant of to_standard()
};

//////////////////////////////////////////////////////////////////////////////
// Rectangle class - axis-aligned rectangle with integer coordinates.
// Used to represent an image region

struct rectangle
{
	int left, bottom;
	int width, height;

	int right () const { return left + width; }
	int top () const { return bottom + height; }

	rectangle (int left_ = 0, int bottom_ = 0, int width_ = 0, int height_ = 0);
};

//////////////////////////////////////////////////////////////////////////////
// !!!UNDONE: quaternion?

//////////////////////////////////////////////////////////////////////////////
// Interpolation functions
// Each of these takes two arguments and a float value in [0, 1]; it blends
// between the first two arguments based on the value of the third.  The
// arguments must be members of a vector space, i.e. they must support
// addition and scalar multiplication.

// Linear interpolation
template <typename T>
T lerp (const T &a, const T &b, float u);

// C1 (cubic) interpolation
template <typename T>
T C1spline (const T &a, const T &b, float u);

// C2 (quintic) interpolation
template <typename T>
T C2spline (const T &a, const T &b, float u);

// Include implementation of the template classes
#include "geometry_impl.hpp"

} // namespace nathan

#endif
