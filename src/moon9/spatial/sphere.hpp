#pragma once

#include <iostream>
#include <string>

#include "point.hpp"

namespace moon9
{
	struct sphere
	{
		moon9::point center;
		float radius;

		std::string id;

		sphere &init( float x, float y, float z, float _radius, const std::string &_id = std::string() )
		{
			this->id = _id;
			this->center.x = x;
			this->center.y = y;
			this->center.z = z;
			this->radius = _radius;

			return *this;
		}

		const bool operator==( const sphere &sph ) const
		{
			return radius == sph.radius && center.x == sph.center.x && center.y == sph.center.y && center.z == sph.center.z;
		}

#ifdef  MSGPACK_DEFINE
		MSGPACK_DEFINE( center, radius, id );
#endif
	};
}

std::ostream &operator<<( std::ostream &os, const moon9::sphere &s );

moon9::sphere merge( const moon9::sphere &s0, const moon9::sphere &s1 );
bool intersects( const moon9::sphere &a, const moon9::sphere &b );
