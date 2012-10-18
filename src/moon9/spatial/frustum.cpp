#include "frustum.hpp"

// point
bool collide( const moon9::frustum &fr, const moon9::vec3 &pt )
{
	const bool outside = false, inside = true;

	for( int i = 0; i < 6; i++ )
		if( distance( fr.pl[i], pt ) < 0 )
			return outside;

	return inside;
}
// sphere
bool collide( const moon9::frustum &fr, const moon9::vec3 &pt, float radius )
{
	const bool outside = false, inside = true, intersection = true;
	bool result = inside;

	for( int i = 0; i < 6; i++ )
	{
		float _distance = distance( fr.pl[i], pt );

		if( _distance < -radius )
			return outside;
		else if( _distance < radius )
			result = intersection;
	}

	return result;
}
// aabox
bool collide( const moon9::frustum &fr, const moon9::aabox &b )
{
	const bool outside = false, inside = true, intersection = true;
	bool result = inside;

	for( int i = 0; i < 6; i++ )
	{
		if( distance( fr.pl[i], b.getVertexP( fr.pl[i].normal ) ) < 0 )
			return outside;
		else if( distance( fr.pl[i], b.getVertexN( fr.pl[i].normal ) ) < 0 )
			result = intersection;
	}

	return result;
}
