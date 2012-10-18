// [ref] based on code by Lighthouse3D

#pragma once

#include <string>
#include <iostream>

#include "vec.hpp"

namespace moon9
{
	class plane
	{
		public:

		moon9::vec3 normal;
		float d;

		plane()
		{}

		plane( const moon9::vec3 &_normal, const moon9::vec3 &_point )
		{
			normal = norm(_normal);
			d = -dot( normal, _point );
		}

		plane( const moon9::vec3 &p1, const moon9::vec3 &p2, const moon9::vec3 &p3 ) // 3 points
		{
			normal = norm(cross( p3 - p2, p1 - p2 ));
			d = -dot( normal, p2 );
		}

		plane( float a, float b, float c, float d ) // 4 coefficients
		{
			// set the normal vector
			normal = moon9::vec3( a, b, c );
			//compute the lenght of the vector
			float l = len(normal);
			// normalize the vector
			normal = moon9::vec3( a/l, b/l, c/l );
			// and divide d by th length as well
			this->d = d/l;
		}
	};
}

float distance( const moon9::plane &plane, const moon9::vec3 &point );

std::ostream &operator<<( std::ostream &os, const moon9::plane &p );
