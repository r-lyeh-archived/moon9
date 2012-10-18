// GEOMETRY_IMPL.HPP
// Copyright (C) 2004-2007 by Nathan Reed.  All rights and priveleges reserved.
// Implements the floatvec and floatmat template classes.

//////////////////////////////////////////////////////////////////////////////
// floatvec implementation

// Constructors

template <int n>
floatvec<n>::floatvec ()
{
	for (size_t i = 0; i < n; ++i)
		elem[i] = 0;
}

template <int n>
floatvec<n>::floatvec (const floatvec<n> &rhs)
{
	for (size_t i = 0; i < n; ++i)
		elem[i] = rhs[i];
}

template <int n>
floatvec<n>::floatvec (const float *elements)
{
	for (size_t i = 0; i < n; ++i)
		elem[i] = elements[i];
}

// Access operators

template <int n>
floatvec<n>::operator float * ()
{
	return elem;
}

template <int n>
floatvec<n>::operator const float * () const
{
	return elem;
}

// Relational operators

template <int n>
bool floatvec<n>::operator == (const floatvec<n> &rhs) const
{
	for (size_t i = 0; i < n; ++i)
		if (elem[i] != rhs[i]) return false;
	return true;
}

template <int n>
bool floatvec<n>::operator != (const floatvec<n> &rhs) const
{
	return !(*this == rhs);
}

// Arithmetic operators

template <int n>
floatvec<n> floatvec<n>::operator + (const floatvec<n> &rhs) const
{
	floatvec<n> result;
	for (size_t i = 0; i < n; ++i)
		result[i] = elem[i] + rhs[i];
	return result;
}

template <int n>
floatvec<n> floatvec<n>::operator - (const floatvec<n> &rhs) const
{
	floatvec<n> result;
	for (size_t i = 0; i < n; ++i)
		result[i] = elem[i] - rhs[i];
	return result;
}

template <int n>
floatvec<n> floatvec<n>::operator - () const
{
	floatvec<n> result;
	for (size_t i = 0; i < n; ++i)
		result[i] = -elem[i];
	return result;
}

template <int n>
floatvec<n> floatvec<n>::operator * (float f) const
{
	floatvec<n> result;
	for (size_t i = 0; i < n; ++i)
		result[i] = elem[i] * f;
	return result;
}

template <int n>
floatvec<n> floatvec<n>::operator / (float f) const
{
	floatvec<n> result;
	for (size_t i = 0; i < n; ++i)
		result[i] = elem[i] / f;
	return result;
}

template <int n>
floatvec<n> operator * (float f, const floatvec<n> &rhs)
{
	return rhs * f;
}

// Compound arithmetic operators

template <int n>
floatvec<n> &floatvec<n>::operator += (const floatvec<n> &rhs)
{
	*this = *this + rhs;
	return *this;
}

template <int n>
floatvec<n> &floatvec<n>::operator -= (const floatvec<n> &rhs)
{
	*this = *this - rhs;
	return *this;
}

template <int n>
floatvec<n> &floatvec<n>::operator *= (float f)
{
	*this = *this * f;
	return *this;
}

template <int n>
floatvec<n> &floatvec<n>::operator /= (float f)
{
	*this = *this / f;
	return *this;
}

// Other operations

// Dot product
template <int n>
float dot (const floatvec<n> &lhs, const floatvec<n> &rhs)
{
	float result = 0;
	for (size_t i = 0; i < n; ++i)
		result += lhs[i] * rhs[i];
	return result;
}

template <int n>
float floatvec<n>::magnitude () const
{
	return sqrtf(dot(*this, *this));
}

template <int n>
floatvec<n> floatvec<n>::normalize () const
{
	return *this / magnitude();
}

// Output operator
template <int n>
std::ostream &operator << (std::ostream &out, const floatvec<n> &rhs)
{
	out << '(';
	for (size_t i = 0; i < n-1; ++i)
		out << rhs[i] << ", ";
	out << rhs[n-1] << ')';
	return out;
}

//////////////////////////////////////////////////////////////////////////////
// floatmat implementation

// Constructors

template <int n, int m>
floatmat<n,m>::floatmat ()
{
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			elem[i][j] = 0;
}

template <int n, int m>
floatmat<n,m>::floatmat (const floatmat<n,m> &rhs)
{
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			elem[i][j] = rhs[i][j];
}

template <int n, int m>
floatmat<n,m>::floatmat (const float *elements)
{
	for (size_t i = 0, k = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j, ++k)
			elem[i][j] = elements[k];
}

// Access operators

template <int n, int m>
floatvec<m> &floatmat<n,m>::operator [] (size_t i)
{
	assert(i < m);
	return *reinterpret_cast<floatvec<m> *>(&elem[i][0]);
}

template <int n, int m>
const floatvec<m> &floatmat<n,m>::operator [] (size_t i) const
{
	assert(i < m);
	return *reinterpret_cast<const floatvec<m> *>(&elem[i][0]);
}

// Relational operators

template <int n, int m>
bool floatmat<n,m>::operator == (const floatmat<n,m> &rhs) const
{
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			if (elem[i][j] != rhs[i][j]) return false;
	return true;
}

template <int n, int m>
bool floatmat<n,m>::operator != (const floatmat<n,m> &rhs) const
{
	return !(*this == rhs);
}

// Arithmetic operators

template <int n, int m>
floatmat<n,m> floatmat<n,m>::operator + (const floatmat<n,m> &rhs) const
{
	floatmat<n,m> result;
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			result[i][j] = elem[i][j] + rhs[i][j];
	return result;
}

template <int n, int m>
floatmat<n,m> floatmat<n,m>::operator - (const floatmat<n,m> &rhs) const
{
	floatmat<n,m> result;
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			result[i][j] = elem[i][j] - rhs[i][j];
	return result;
}

template <int n, int m>
floatmat<n,m> floatmat<n,m>::operator - () const
{
	floatmat<n,m> result;
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			result[i][j] = -elem[i][j];
	return result;
}

template <int n, int m>
floatmat<n,m> floatmat<n,m>::operator * (float f) const
{
	floatmat<n,m> result;
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			result[i][j] = elem[i][j] * f;
	return result;
}

template <int n, int m>
floatmat<n,m> floatmat<n,m>::operator / (float f) const
{
	floatmat<n,m> result;
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			result[i][j] = elem[i][j] / f;
	return result;
}

template <int n, int m>
floatmat<n,m> operator * (float f, const floatmat<n,m> &rhs)
{
	return rhs * f;
}

// Compound arithmetic operators

template <int n, int m>
floatmat<n,m> &floatmat<n,m>::operator += (const floatmat<n,m> &rhs)
{
	*this = *this + rhs;
	return *this;
}

template <int n, int m>
floatmat<n,m> &floatmat<n,m>::operator -= (const floatmat<n,m> &rhs)
{
	*this = *this - rhs;
	return *this;
}

template <int n, int m>
floatmat<n,m> &floatmat<n,m>::operator *= (float f)
{
	*this = *this * f;
	return *this;
}

template <int n, int m>
floatmat<n,m> &floatmat<n,m>::operator /= (float f)
{
	*this = *this / f;
	return *this;
}

// Multiplication operators

template <int n, int m>
floatvec<n> floatmat<n,m>::operator * (const floatvec<m> &rhs) const
{
	floatvec<n> result;
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			result[i] += elem[i][j] * rhs[j];
	return result;
}

template <int n, int m>
floatvec<m> operator * (const floatvec<n> &lhs, const floatmat<n,m> &rhs)
{
	floatvec<m> result;
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			result[j] += lhs[i] * rhs[i][j];
	return result;
}

template <int n, int m>
template <int r>
floatmat<n,r> floatmat<n,m>::operator * (const floatmat<m,r> &rhs) const
{
	floatmat<n,r> result;
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < r; ++j)
			for (size_t k = 0; k < m; ++k)
				result[i][j] += elem[i][k] * rhs[k][j];
	return result;
}

// Transpose
template <int n, int m>
floatmat<m,n> floatmat<n,m>::transpose () const
{
	floatmat<m,n> result;
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			result[j][i] = elem[i][j];
	return result;
}

// Output operator
template <int n, int m>
std::ostream &operator << (std::ostream &out, const floatmat<n,m> &rhs)
{
	out << std::endl;
	for (size_t i = 0; i < n; ++i)
	{
		out << (i == 0 ? '[' : ' ');
		for (size_t j = 0; j < m; ++j)
			out << std::setw(8) << rhs[i][j];
		if (i == n - 1)
			out << ']';
		else
			out << std::endl;
		out << std::endl;
	}
	return out;
}

//////////////////////////////////////////////////////////////////////////////
// square_floatmat implementation

// Constructors

template <int n>
square_floatmat<n>::square_floatmat (): floatmat<n,n>()
{
	// Nothing to do
}

template <int n>
square_floatmat<n>::square_floatmat (const floatmat<n,n> &rhs): floatmat<n,n>(rhs)
{
	// Nothing to do
}

template <int n>
square_floatmat<n>::square_floatmat (const square_floatmat<n> &rhs): floatmat<n,n>(rhs)
{
	// Nothing to do
}

template <int n>
square_floatmat<n>::square_floatmat (const float *elements): floatmat<n,n>(elements)
{
	// Nothing to do
}

// Identity
template <int n>
square_floatmat<n> square_floatmat<n>::identity ()
{
	square_floatmat<n> result;
	for (size_t i = 0; i < n; ++i)
		result[i][i] = 1.0f;
	return result;
}

// Trace
template <int n>
float square_floatmat<n>::trace () const
{
	float result = 0;
	for (size_t i = 0; i < n; ++i)
		result += this->elem[i][i];
	return result;
}

// Inverse
// In order to get around the fact that C++ doesn't allow specialization of just
// one member function, we use a 'proxy function' which has the specializations.

// General inverse by Gauss-Jordan elimination
template <int n>
square_floatmat<n> inverse_proxy (const square_floatmat<n> &mat)
{
	square_floatmat<n> a(mat);
	square_floatmat<n> b(square_floatmat<n>::identity);

	// Loop through columns
	for (size_t j = 0; j < n; ++j)
	{
		// Select pivot element: maximum magnitude in this column
		size_t pivot = 0;
		for (size_t i = 1; i < n; ++i)
			if (fabs(a[i][j]) > fabs(a[pivot][j]))
				pivot = i;
		if (fabs(a[pivot][j]) < epsilon)
			throw std::domain_error("Matrix inverse does not exist!");

		// Interchange rows to put pivot element on the diagonal,
		// if it is not already there
		if (pivot != j)
		{
			floatvec<n> temp = a[j];
			a[j] = a[pivot];
			a[pivot] = temp;
			temp = b[j];
			b[j] = b[pivot];
			b[pivot] = temp;
		}

		// Divide the whole row by the pivot element
		if (a[j][j] != 1.0f)								// Skip if already equal to 1
		{
			float temp = a[j][j];
			a[j] /= temp;
			b[j] /= temp;
			// Now the pivot element has become 1
		}

		// Subtract this row from others to make the rest of column j zero
		for (size_t i = 0; i < n; ++i)
		{
			if ((i != j) && (fabs(a[i][j]) > epsilon))		// skip rows already zero
			{
				float scale = -a[i][j];
				a[i] += a[j] * scale;
				b[i] += b[j] * scale;
			}
		}
	}
	
	// When these operations have been completed, a should have been transformed to the identity matrix
	// and b should have been transformed into the inverse of the original a.
	return b;
}

// The specialized inverses for 2x2 and 3x3 matrices are in geometry.cpp, since they
// constitute actual code and not a template for code (and thus cause duplicate definition
// errors in the linker if they are placed in an implementation include).  So, just
// the declarations are here, to prevent unspecialized code based on the general inverse
// function from being generated.
template <>
square_floatmat<2> inverse_proxy (const square_floatmat<2> &mat);
template <>
square_floatmat<3> inverse_proxy (const square_floatmat<3> &mat);

// Now the actual inverse function, which simply calls the above
template <int n>
square_floatmat<n> square_floatmat<n>::inverse () const
{
	return inverse_proxy<n>(*this);
}

//////////////////////////////////////////////////////////////////////////////
// Interpolation implementation

// Linear interpolation
template <typename T>
T lerp (const T &a, const T &b, float u)
{
	return (1 - u) * a + u * b;
}

// C1 (cubic) interpolation - evaluates the spline curve 3x^2 - 2x^3
template <typename T>
T C1spline (const T &a, const T &b, float u)
{
	float v = u * u * (3 - 2 * u);
	return (1 - v) * a + v * b;
}

// C2 (quintic) interpolation - evaluates the spline curve 6t^5 - 15t^4 + 10t^3
template <typename T>
T C2spline (const T &a, const T &b, float u)
{
	float v = u * u * u * (u * (u * 6 - 15) + 10);
	return (1 - v) * a + v * b;
}
