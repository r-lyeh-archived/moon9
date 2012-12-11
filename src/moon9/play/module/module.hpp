// simple game module class
// - rlyeh

#pragma once

// definition

namespace moon9
{
	class none;
	class module
	{
		public:	module() {}
		public: virtual ~module() {}
		public: virtual void update( double t, float dt ) = 0;
	};
}

// implementation

#include <iostream>
#include <memory>

namespace moon9
{
	class manager
	{
		double t;
		std::auto_ptr<module> last;

		public:

		manager() : t(0)
		{}

		template<class T>
		void next()
		{
			last.reset( new T() );
		}

		// extra arguments {

		template<class T, class A0 >
		void next( const A0 &arg0 )
		{
			last.reset( new T( arg0 ) );
		}

		template<class T, class A0, class A1 >
		void next( const A0 &arg0, const A1 &arg1 )
		{
			last.reset( new T( arg0, arg1 ) );
		}

		// (...) }

		template<>
		void next<moon9::none>()
		{
			last.reset();
		}

		void run( float dt = 1/60.f ) // replace 'dt' with your custom timer functions
		{
			t += dt;

			if( last.get() )
				last->update( t, dt );
			else
				std::cout << "<moon9/play/manager.hpp> says: idle" << std::endl;
		}
	};
}
