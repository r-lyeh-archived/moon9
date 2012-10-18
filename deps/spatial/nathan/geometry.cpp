// GEOMETRY.CPP
// Copyright (C) 2004-2005 by Nathan Reed.  All rights and priveleges reserved.
// Implements the non-inline functions declared in geometry.h.

#include <limits>
#include "geometry.hpp"

using std::min;
using std::max;

namespace nathan {

// Initialize constants
const float epsilon = std::numeric_limits<float>::epsilon();
const float infty = std::numeric_limits<float>::infinity();


//////////////////////////////////////////////////////////////////////////////
// Matrix implementations

// mat2
mat2::mat2 (float f00, float f01, float f10, float f11)
{
	elem[0][0] = f00; elem[0][1] = f01;
	elem[1][0] = f10; elem[1][1] = f11;
}
mat2::mat2 (const vec2 &a, const vec2 &b)						// By columns
{
	elem[0][0] = a[0]; elem[0][1] = b[0];
	elem[1][0] = a[1]; elem[1][1] = b[1];
}
mat2 mat2::rotation (float radians)
{
	float sin = sinf(radians), cos = cosf(radians);
	return mat2(cos, -sin, sin, cos);
}
mat2 mat2::scale (const vec2 &s)
{
	return mat2(s[0], 0, 0, s[1]);
}

// mat3
mat3::mat3 (float f00, float f01, float f02, float f10, float f11, float f12, float f20, float f21, float f22)
{
	elem[0][0] = f00; elem[0][1] = f01; elem[0][2] = f02;
	elem[1][0] = f10; elem[1][1] = f11; elem[1][2] = f12;
	elem[2][0] = f20; elem[2][1] = f21; elem[2][2] = f22;
}
mat3::mat3 (const vec3 &a, const vec3 &b, const vec3 &c)		// By columns
{
	elem[0][0] = a[0]; elem[0][1] = b[0]; elem[0][2] = c[0];
	elem[1][0] = a[1]; elem[1][1] = b[1]; elem[1][2] = c[1];
	elem[2][0] = a[2]; elem[2][1] = b[2]; elem[2][2] = c[2];
}
mat3 mat3::rotation (const vec3 &axis, float angle)
{
	float sin = sinf(angle);
	float cos = cosf(angle);
	vec3 v = axis.normalize();
	return mat3(cos + (1-cos)*v[0]*v[0],     (1-cos)*v[0]*v[1] - sin*v[2], (1-cos)*v[0]*v[2] + sin*v[1],
	           (1-cos)*v[0]*v[1] + sin*v[2], cos + (1-cos)*v[1]*v[1],      (1-cos)*v[1]*v[2] - sin*v[0],
	           (1-cos)*v[0]*v[2] - sin*v[1], (1-cos)*v[1]*v[2] + sin*v[0], cos + (1-cos)*v[2]*v[2]);
}
mat3 mat3::scale (const vec3 &s)
{
	return mat3(s[0], 0,   0,
	            0,   s[1], 0,
				0,   0,   s[2]);
}

// mat4
mat4::mat4 (const vec4 &a, const vec4 &b, const vec4 &c, const vec4 &d)		// By columns
{
	elem[0][0] = a[0]; elem[0][1] = b[0]; elem[0][2] = c[0]; elem[0][3] = d[0];
	elem[1][0] = a[1]; elem[1][1] = b[1]; elem[1][2] = c[1]; elem[1][3] = d[1];
	elem[2][0] = a[2]; elem[2][1] = b[2]; elem[2][2] = c[2]; elem[2][3] = d[2];
	elem[3][0] = a[3]; elem[3][1] = b[3]; elem[3][2] = c[3]; elem[3][3] = d[3];
}
mat4::mat4 (const affine &a): square_floatmat<4>(square_floatmat<4>::identity())
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
			elem[i][j] = a.L[i][j];
		elem[i][3] = a.t[i];
	}
}
mat4::mat4 (const mat3 &m): square_floatmat<4>(square_floatmat<4>::identity())
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
			elem[i][j] = m[i][j];
	}
}


//////////////////////////////////////////////////////////////////////////////
// Affine transformation implementation

// Inverse for affine transformations (arbitrary linear transform on R^3 plus a translation):
// does a full inverse on the 3x3 matrix and reverses the translation vector
affine affine::inverse () const
{
	mat3 Linv = L.inverse();
	return affine(Linv, Linv * -t);
}

// Inverse for rigid-body transformations (rotation/reflection on R^3 plus a translation):
// transposes the 3x3 matrix and reverses the translation vector
affine affine::rigid_inverse () const
{
	mat3 Linv = L.transpose();
	return affine(Linv, Linv * -t);
}

// Functions to generate matrices for various common transformations
affine affine::translation (const vec3 &t)
{
	return affine(mat3::identity(), t);
}
affine affine::translation (float x, float y, float z)
{
	return affine(mat3::identity(), vec3(x, y, z));
}
affine affine::rotation (const vec3 &axis, float angle)
{
	return affine(mat3::rotation(axis, angle), vec3(0, 0, 0));
}
affine affine::rotation (float x, float y, float z, float angle)
{
	return affine(mat3::rotation(vec3(x, y, z), angle), vec3(0, 0, 0));
}
affine affine::scale (float s)
{
	return affine(mat3::scale(vec3(s, s, s)), vec3(0, 0, 0));
}
affine affine::scale (const vec3 &s)
{
	return affine(mat3::scale(s), vec3(0, 0, 0));
}
affine affine::scale (float x, float y, float z)
{
	return affine(mat3::scale(vec3(x, y, z)), vec3(0, 0, 0));
}
affine affine::projection (const line &l)
{
	// Calculate projection as if the line went through (0, 0, 0)
	vec3 u = l.direction.normalize();
	mat3 m(u[0]*u[0], u[1]*u[0], u[2]*u[0],
	       u[0]*u[1], u[1]*u[1], u[2]*u[1],
	       u[0]*u[2], u[1]*u[2], u[2]*u[2]);
	return translation(l.origin) * m * translation(-l.origin);
}
affine affine::projection (const plane &p)
{
	// Calculate projection as if the plane went through (0, 0, 0)
	vec3 u = p.normal().normalize();
	mat3 m(1 - u[0]*u[0],    -u[1]*u[0],    -u[2]*u[0],
	          -u[0]*u[1], 1 - u[1]*u[1],    -u[2]*u[1],
	          -u[0]*u[2],    -u[1]*u[2], 1 - u[2]*u[2]);
	vec3 point = p.normal() * -p.coeff[3];
	return translation(point) * m * translation(-point);
}
affine affine::projection (const plane &p, const vec3 &v)
{
	// Calculate projection as if the plane went through (0, 0, 0)
	// Find two vectors in the plane
	vec3 s = cross(p.normal(), vec3(1, 0, 0));
	if (dot(s, s) < epsilon)
		s = cross(p.normal(), vec3(0, 1, 0));
	vec3 t = cross(p.normal(), s);
	// Now s, t, and v form a basis of R^3 in which the projection
	// is orthogonal.  Transform into this basis, do the projection,
	// then get back into the original coordinates.
	mat3 change = mat3(s, t, v);
	mat3 proj(1, 0, 0,
	          0, 1, 0,
	          0, 0, 0);
	mat3 m = change * proj * change.inverse();
	vec3 point = p.normal() * -p.coeff[3];
	return translation(point) * m * translation(-point);
}
affine affine::reflection (const plane &p)
{
	// Calculate reflection as if the plane went through (0, 0, 0)
	vec3 u = p.normal().normalize();
	mat3 m(1 - 2*u[0]*u[0],    -2*u[1]*u[0],    -2*u[2]*u[0],
	          -2*u[0]*u[1], 1 - 2*u[1]*u[1],    -2*u[2]*u[1],
	          -2*u[0]*u[2],    -2*u[1]*u[2], 1 - 2*u[2]*u[2]);
	vec3 point = p.normal() * -p.coeff[3];
	return translation(point) * m * translation(-point);
}


//////////////////////////////////////////////////////////////////////////////
// Implementations of other geometry objects

line::line ()
{
	// Nothing to do
}
line::line (const vec3 &origin_, const vec3 &direction_): origin(origin_), direction(direction_)
{
	// Nothing to do
}
vec3 line::operator () (float t) const
{
	return origin + t * direction;
}
line line::transform (const affine &trans) const
{
	return line(trans.transform_pt(origin), trans.transform_vec(direction));
}

plane::plane (float a, float b, float c, float d): coeff(a, b, c, d)
{
	// Nothing to do
}
plane::plane (const vec3 &normal, const vec3 &point): coeff(normal, -dot(normal, point))
{
	// Nothing to do
}
plane::plane (const vec3 &u, const vec3 &v, const vec3 &q): coeff(cross(v - u, q - u), 0)
{
	// 3-point plane formulation - u, v, q in counterclockwise winding
	coeff[3] = -dot(coeff.xyz(), u);
}
const vec3 &plane::normal () const
{
	return coeff.xyz();
}
vec3 &plane::normal ()
{
	return coeff.xyz();
}
plane plane::normalize () const
{
	// Normalize plane's normal vector, and scale fourth component to match
	plane result(*this);
	result.coeff /= normal().magnitude();
	return result;
}
plane plane::transform (const affine &xform) const
{
	return transform(xform, xform.invtrans());
}
plane plane::transform (const affine &xform, const mat3 &invtrans) const
{
	// Transform plane normal by inverse transpose of the given matrix
	plane result(*this);
	result.normal() = invtrans * normal();
	result.coeff[3] = coeff[3] - dot(result.normal(), xform.t);
	return result;
}
bool plane::outside (const vec3 &u) const
{
	return (dot(coeff, vec4(u, 1)) > 0.0f);
}
bool plane::outside (float x, float y, float z) const
{
	return (dot(coeff, vec4(x, y, z, 1)) > 0.0f);
}

bivector::bivector ()
{
	// Nothing to do
}
bivector::bivector (const mat3 &mat): coeff(mat)
{
	// Nothing to do
}
bivector::bivector (const vec3 &u, const vec3 &v)
{
	// Evaulate u^i v^j - v^i u^j
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			coeff[i][j] = u[i]*v[j] - v[i]*u[j];
}
bivector::bivector (const vec3 &normal)
{
	coeff[1][2] =  normal[0];
	coeff[2][1] = -normal[0];
	coeff[2][0] =  normal[1];
	coeff[0][2] = -normal[1];
	coeff[0][1] =  normal[2];
	coeff[1][0] = -normal[2];
}
vec3 bivector::normal () const
{
	return vec3(coeff[1][2], coeff[2][0], coeff[0][1]).normalize();
}
float bivector::magnitude () const
{
	mat3 temp = coeff * coeff.transpose();
	return temp.trace();
}
bivector bivector::operator * (float f) const
{
	return bivector(coeff * f);
}
bivector operator * (float f, const bivector &b)
{
	return b * f;
}
bivector bivector::operator / (float f) const
{
	return bivector(coeff / f);
}
bivector& bivector::operator *= (float f)
{
	*this = *this * f;
	return *this;
}
bivector& bivector::operator /= (float f)
{
	*this = *this / f;
	return *this;
}
bivector bivector::transform (const mat3 &xform) const
{
	return bivector(xform * coeff * xform.transpose());
}

aabb::aabb ():
	mins(0, 0, 0),
	maxs(-1, -1, -1)
{
	// Nothing to do
}
aabb::aabb (const vec3 &a, const vec3 &b):
	mins(min(a[0], b[0]), min(a[1], b[1]), min(a[2], b[2])),
	maxs(max(a[0], b[0]), max(a[1], b[1]), max(a[2], b[2]))
{
}
bool aabb::isnull () const
{
	return ((mins[0] > maxs[0]) || (mins[1] > maxs[1]) || (mins[2] > maxs[2]));
}
bool aabb::outside (const vec3 &u) const
{
	return outside(u[0], u[1], u[2]);
}
bool aabb::outside (float x, float y, float z) const
{
	return ((x < mins[0]) || (y < mins[1]) || (z < mins[2]) ||
			(x > maxs[0]) || (y > maxs[1]) || (z > maxs[2]));
}
bool aabb::intersects (const ray &r) const
{
	// test for intersection with any of the six planes

	if (fabsf(r.direction[0]) > epsilon)
	{
		float t0 = (mins[0] - r.origin[0]) / r.direction[0];
		if (t0 > epsilon && t0 < r.max_t - epsilon)
		{
			vec3 posIsct = r(t0);
			if (posIsct[1] > 0 && posIsct[1] < 1 && posIsct[2] > 0 && posIsct[2] < 1)
				return true;
		}

		float t1 = (maxs[0] - r.origin[0]) / r.direction[0];
		if (t1 > epsilon && t1 < r.max_t - epsilon)
		{
			vec3 posIsct = r(t1);
			if (posIsct[1] > 0 && posIsct[1] < 1 && posIsct[2] > 0 && posIsct[2] < 1)
				return true;
		}
	}
	if (fabsf(r.direction[1]) > epsilon)
	{
		float t0 = (mins[1] - r.origin[1]) / r.direction[1];
		if (t0 > epsilon && t0 < r.max_t - epsilon)
		{
			vec3 posIsct = r(t0);
			if (posIsct[0] > 0 && posIsct[0] < 1 && posIsct[2] > 0 && posIsct[2] < 1)
				return true;
		}

		float t1 = (maxs[1] - r.origin[1]) / r.direction[1];
		if (t1 > epsilon && t1 < r.max_t - epsilon)
		{
			vec3 posIsct = r(t1);
			if (posIsct[0] > 0 && posIsct[0] < 1 && posIsct[2] > 0 && posIsct[2] < 1)
				return true;
		}
	}
	if (fabsf(r.direction[2]) > epsilon)
	{
		float t0 = (mins[2] - r.origin[2]) / r.direction[2];
		if (t0 > epsilon && t0 < r.max_t - epsilon)
		{
			vec3 posIsct = r(t0);
			if (posIsct[0] > 0 && posIsct[0] < 0 && posIsct[1] > 1 && posIsct[1] < 1)
				return true;
		}

		float t1 = (maxs[2] - r.origin[2]) / r.direction[2];
		if (t1 > epsilon && t1 < r.max_t - epsilon)
		{
			vec3 posIsct = r(t1);
			if (posIsct[0] > 0 && posIsct[0] < 1 && posIsct[1] > 0 && posIsct[1] < 1)
				return true;
		}
	}

	return false;
}
aabb aabb::bound (const aabb &box, const vec3 &p)
{
	if (box.isnull())
		return aabb(p, p);

	return aabb(vec3(min(box.mins[0], p[0]),
	                 min(box.mins[1], p[1]),
	                 min(box.mins[2], p[2])),
	            vec3(max(box.maxs[0], p[0]),
	                 max(box.maxs[1], p[1]),
	                 max(box.maxs[2], p[2])));
}
aabb aabb::bound (const aabb &a, const aabb &b)
{
	if (a.isnull())
		return b;
	if (b.isnull())
		return a;

	return aabb(vec3(min(a.mins[0], b.mins[0]),
	                 min(a.mins[1], b.mins[1]),
	                 min(a.mins[2], b.mins[2])),
	            vec3(max(a.maxs[0], b.maxs[0]),
	                 max(a.maxs[1], b.maxs[1]),
	                 max(a.maxs[2], b.maxs[2])));
}
aabb aabb::bound (const aabb &box, const affine &trans)
{
	if (box.isnull())
		return null;

	aabb result;
	result.mins = result.maxs = trans.transform_pt(box.mins);

	// Take mins as the origin and calculate three spanning vectors
	vec3 a(box.maxs[0] - box.mins[0], 0, 0),
	     b(0, box.maxs[1] - box.mins[1], 0),
	     c(0, 0, box.maxs[2] - box.mins[2]);
	
	// Transform them and take new mins and maxs
	vec3 a_ = trans.transform_vec(a),
	     b_ = trans.transform_vec(b),
	     c_ = trans.transform_vec(c);

	for (size_t i = 0; i < 3; ++i)
	{
		if (a[i] < 0) result.mins[i] += a[i]; else result.maxs[i] += a[i];
		if (b[i] < 0) result.mins[i] += b[i]; else result.maxs[i] += b[i];
		if (c[i] < 0) result.mins[i] += c[i]; else result.maxs[i] += c[i];
	}
	return result;
}
const aabb aabb::null = aabb(vec3(0, 0, 0), vec3(-1, -1, -1));

triangle::triangle ()
{
	// Nothing to do
}
triangle::triangle (const vec3 &a_, const vec3 &b_, const vec3 &c_): a(a_), b(b_), c(c_), p(a_, b_, c_)
{
	// Nothing to do
}
vec3 triangle::from_barycentric (float a_, float b_, float c_) const
{
	return a_*a + b_*b + c_*c;
}
vec3 triangle::from_barycentric (const vec3 &coords) const
{
	return coords[0]*a + coords[1]*b + coords[2]*c;
}
vec3 triangle::to_barycentric (const vec3 &v) const
{
	// We want to find coord.xyz so that A*coord[0] + B*coord[1] + C*coord[2] = V, and coord sums to 1
	// Notice coord[2] = 1 - coord[0] - coord[1], so we have (A-C)*coord[0] + (B-C)*coord[1] = (V-C)
	// Find least-squares solution to [ A-C B-C ] * coord.xy = V-C
	vec2 coord;
	vec3 CA = a - c, CB = b - c, CV = v - c;
	// Setup normal equation: calculate M = [ A-C B-C ]^T * [ A-C B-C ]
	float dot1 = dot(CA, CA), dot2 = dot(CA, CB), dot3 = dot(CB, CB);
	mat2 M(dot1, dot2, dot2, dot3);
	// Solve normal equation: calculate coord.xy = M^-1 * [ A-C B-C ]^T * V-C
	coord = M.inverse() * vec2(dot(CA, CV), dot(CB, CV));
	// Final coordinate is chosen to sum to 1
	return vec3(coord, 1 - coord[0] - coord[1]);
}

//////////////////////////////////////////////////////////////////////////////
// Basis implementation

basis::basis ()
{
	// Nothing to do
}
basis::basis (const vec3 &v1_, const vec3 &v2_, const vec3 &v3_, const vec3 &origin_):
	v1(v1_), v2(v2_), v3(v3_), origin(origin_)
{
	// Nothing to do
}
basis::basis (const vec3 &origin_): v1(1, 0, 0), v2(0, 1, 0), v3(0, 0, 1), origin(origin_)
{
	// Nothing to do
}
affine basis::to_standard () const
{
	// Compute transformation that takes the basis coordinates to standard coordinates
	return affine(v1, v2, v3, origin);
}
affine basis::from_standard () const
{
	// Compute matrix that takes standard coordinates to basis coordinates
	return to_standard().inverse();
}
affine basis::change (const basis &b1, const basis &b2)
{
	// Compute change of basis matrix from b1 to b2
	return b2.from_standard() * b1.to_standard();
}
basis basis::orthonormalize () const
{
	// Perform Gram-Schmidt process on basis
	basis result;

	// Normalize first vector
	result.v1 = v1.normalize();

	// Project and normalize second vector
	result.v2 = v2 - dot(v2, result.v1) * result.v1;
	result.v2 = result.v2.normalize();

	// Project and normalize third vector
	result.v3 = v3 - dot(v3, result.v1) * result.v1 - dot(v3, result.v2) * result.v2;
	result.v3 = result.v3.normalize();

	// Copy origin
	result.origin = origin;

	return result;
}
float basis::determinant () const
{
	// Volume of parallelepiped spanned by the three basis vectors -
	// this is the determinant of the transformation from this basis to the standard basis
	return dot(cross(v1, v2), v3);
}

const basis basis::standard = basis(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));

//////////////////////////////////////////////////////////////////////////////
// Specialized matrix inverses - these are in a .cpp file because they constitute actual code

// Specialized inverse for 2x2 matrices
template <>
square_floatmat<2> inverse_proxy (const square_floatmat<2> &mat)
{
	float determinant = mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];
	if (-epsilon < determinant && determinant < epsilon)
		throw singular_matrix_exception();
	square_floatmat<2> adjoint = mat2(mat[1][1], -mat[0][1], -mat[1][0], mat[0][0]);
	return (1.0f / determinant) * adjoint;
}

// Specialized inverse for 3x3 matrices
// Is this a savings over G-J?  Don't know.  Commenting this out will return to general G-J
template <>
square_floatmat<3> inverse_proxy (const square_floatmat<3> &mat)
{
	square_floatmat<3> adjoint;

	adjoint[0][0] = mat[1][1]*mat[2][2] - mat[2][1]*mat[1][2];
	adjoint[0][1] = mat[2][1]*mat[0][2] - mat[0][1]*mat[2][2];
	adjoint[0][2] = mat[0][1]*mat[1][2] - mat[1][1]*mat[0][2];

	adjoint[1][0] = mat[2][0]*mat[1][2] - mat[1][0]*mat[2][2];
	adjoint[1][1] = mat[0][0]*mat[2][2] - mat[2][0]*mat[0][2];
	adjoint[1][2] = mat[1][0]*mat[0][2] - mat[0][0]*mat[1][2];

	adjoint[2][0] = mat[1][0]*mat[2][1] - mat[2][0]*mat[1][1];
	adjoint[2][1] = mat[2][0]*mat[0][1] - mat[0][0]*mat[2][1];
	adjoint[2][2] = mat[0][0]*mat[1][1] - mat[1][0]*mat[0][1];

	float determinant = mat[0][0]*adjoint[0][0] + mat[1][0]*adjoint[0][1] + mat[2][0]*adjoint[0][2];
	
	if (-epsilon < determinant && determinant < epsilon)
		throw singular_matrix_exception();

	return (1.0f / determinant) * adjoint;
}

//////////////////////////////////////////////////////////////////////////////
// Rectangle implementation

rectangle::rectangle (int left_, int bottom_, int width_, int height_):
	left(left_),
	bottom(bottom_),
	width(width_),
	height(height_)
{
	// Nothing to do
}

}