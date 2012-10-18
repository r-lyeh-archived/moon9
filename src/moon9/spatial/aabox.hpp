// [ref] based on code by Lighthouse3D

#pragma once

#include "vec.hpp"

namespace moon9
{
	class aabox
	{
		public:

		moon9::vec3 corner;
		float x,y,z;

		aabox()
		{
			corner = moon9::vec3(0,0,0);

			x = 1.0f;
			y = 1.0f;
			z = 1.0f;
		}

		aabox( const moon9::vec3 &_corner, float x, float y, float z )
		{
			corner = _corner;

			if (x < 0.0) {
				x = -x;
				corner.x -= x;
			}
			if (y < 0.0) {
				y = -y;
				corner.y -= y;
			}
			if (z < 0.0) {
				z = -z;
				corner.z -= z;
			}

			this->x = x;
			this->y = y;
			this->z = z;
		}

		// for use in frustum computations

		moon9::vec3 getVertexP( const moon9::vec3 &normal ) const
		{
			moon9::vec3 res = corner;

			if (normal.x > 0)
				res.x += x;

			if (normal.y > 0)
				res.y += y;

			if (normal.z > 0)
				res.z += z;

			return res;
		}

		moon9::vec3 getVertexN( const moon9::vec3 &normal ) const
		{
			moon9::vec3 res = corner;

			if (normal.x < 0)
				res.x += x;

			if (normal.y < 0)
				res.y += y;

			if (normal.z < 0)
				res.z += z;

			return res;
		}
	};
}
