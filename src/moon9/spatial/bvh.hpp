// simple bounding volume hierarchy class
// - rlyeh

// @todo: tree structure instead of vector (flat)
// @todo: test find() in depth
// @todo: find() could return number of iterations (~depth) where 0 is no collision (so it mimics boolean behavior)
// ie, if intersects then return end - ( search = it ); else return 0;

#pragma once

#include <vector>

template<typename T>
class bvh
{
	std::vector<T> family;
	typename std::vector<T>::reverse_iterator search;

	public:

	bvh()
	{}

	void add( const T &t )
	{
		extern T merge( const T &t1, const T &t2 );

		family.push_back( family.size() ? merge( family.back(), t ) : t );
	}

	bool find( const T &t )
	{
		extern bool intersects( const T &t1, const T &t2 );

		std::vector<T>::reverse_iterator it = family.rbegin(), end = family.rend();

		for( ; it != end ; ++it )
			if( intersects(*it, t) )
				return search = it, true;

		return search = end, false;
	}

	const T &found() const
	{
		return *search;
	}

	T &found()
	{
		return *search;
	}
};

