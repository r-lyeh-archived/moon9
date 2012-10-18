// [ref] based on code by Lighthouse3D
// [ref] http://gamedev.stackexchange.com/questions/19455/clipping-frustum-to-portal-wheres-the-bug

#pragma once

#include "vec.hpp"
#include "plane.hpp"
#include "aabox.hpp"

namespace moon9
{
	class frustum
	{
		public:

		moon9::vec3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
		float nw, nh, fw, fh;

		public:

		float fov, aspect, neard, fard;

		enum
		{
			TOP = 0,
			BOTTOM,
			LEFT,
			RIGHT,
			NEARP,
			FARP
		};

		moon9::plane pl[6];

		size_t objects_processed, objects_rendered;

		frustum()
		{
			setup( 45, 1.f, 1.f, 100.f );
		}

		void setup( float _fov, float _aspect, float _near, float _far )
		{
			objects_processed = objects_rendered = 0;

			aspect = _aspect;
			fov = _fov;
			neard = _near;
			fard = _far;

			float tangent = (float)tan( deg2rad(fov * 0.5) ) ;
			nh = _near * tangent;
			nw = nh * _aspect;
			fh = _far * tangent;
			fw = fh * _aspect;
		}

		void update( const moon9::vec3 &pos, const moon9::vec3 &look, const moon9::vec3 &up )
		{
			/*

			//also:

		    mat4 viewMatrixInverse = glm::inverse( viewMatrix );
		    mat4 viewMatrixTranspose = glm::transpose( viewMatrix );

		    const vec3 position = vec3( viewMatrixInverse[3] ); // pos

		    const vec3 xAxis = glm::normalize( vec3(viewMatrixTranspose[0]) ); // X
		    const vec3 yAxis = glm::normalize( vec3(viewMatrixTranspose[1]) ); // Y
		    const vec3 zAxis = glm::normalize( vec3(viewMatrixTranspose[2]) ); // Z
		    */

			moon9::vec3 Z = norm( pos - look );
			moon9::vec3 X = norm(cross( up, Z ));
			moon9::vec3 Y = cross( Z, X );

			moon9::vec3 nc = pos - Z * neard; // near center
			moon9::vec3 fc = pos - Z * fard;  //  far center

			ntl = nc + ( Y * nh ) - ( X * nw );
			ntr = nc + ( Y * nh ) + ( X * nw );
			nbl = nc - ( Y * nh ) - ( X * nw );
			nbr = nc - ( Y * nh ) + ( X * nw );

			ftl = fc + ( Y * fh ) - ( X * fw );
			ftr = fc + ( Y * fh ) + ( X * fw );
			fbl = fc - ( Y * fh ) - ( X * fw );
			fbr = fc - ( Y * fh ) + ( X * fw );

			pl[    TOP ] = moon9::plane(ntr,ntl,ftl);
			pl[ BOTTOM ] = moon9::plane(nbl,nbr,fbr);
			pl[   LEFT ] = moon9::plane(ntl,nbl,fbl);
			pl[  RIGHT ] = moon9::plane(nbr,ntr,fbr);
			pl[  NEARP ] = moon9::plane(ntl,ntr,nbr);
			pl[   FARP ] = moon9::plane(ftr,ftl,fbl);
		}
	};
}

// point
bool collide( const moon9::frustum &fr, const moon9::vec3 &pt );
// sphere
bool collide( const moon9::frustum &fr, const moon9::vec3 &pt, float radius );
// aabox
bool collide( const moon9::frustum &fr, const moon9::aabox &b );

#if 0

namespace moon9
{
	typedef moon9::vec3 ray;
	typedef moon9::vec3 vector;
	typedef moon9::vec3 point;

    struct sphere
    {
        point center;
        float radius;
    };
}

// tests if a sphere is within the frustum
bool collide( const moon9::camera &cam, const moon9::sphere &sphere )
{
    // [ref] http://www.flipcode.com/archives/Frustum_Culling.shtml

    // calculate our distances to each of the planes
    for( int i = 0; i < 6; ++i )
    {
        // find the distance to this plane
        float distance = dot( m_plane[i].normal(), sphere.position ) + m_plane[i].Distance();

        // if this distance is < -sphere.radius, we are outside
        // else if the distance is between +- radius, then we intersect

        if( distance < -sphere.radius )
            return false;

        if( float( std::abs(distance) ) < sphere.radius )
            return true;
    }

    // otherwise we are fully in view
    return true;
}


#endif
