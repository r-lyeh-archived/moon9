/*
 * Simple component library for modern game object composition pattern.
 * This manager holds a record of all live instances of any trackable class,
 * thru moon9::component<class>::begin()/end() iterators.

 * Copyright (c) 2011-2012 Mario 'rlyeh' Rodriguez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.

 * entity = instance of a given class
 * entity = components[]
 * component manager/system -> process every component grouped per buckets
 *
 *                     .---------------------------------------------------------.
 *                     |                   COMPONENT MANAGERS                    |
 *                     |                                                         |
 *  .--------------.   | position, movement, render, script, target, physics ... |
 *  | OBJECTS LIST |   '----|---------|--------|-------|-------|-------|---------'
 *  |              |        |         |        |       |       |       |
 *  |       alien -|--------#---------#--------#-------#-------#-------|--------->
 *  |      player -|--------#---------#--------#-------#-------|-------#--------->
 *  |      target -|--------#---------|--------|-------#-------#-------|--------->
 *  | script-only -|--------|---------|--------|-------#-------|-------|--------->
 *  |     grenade -|--------#---------#--------#-------|-------#-------#--------->
 *  |         ...  |        |         |        |       |       |       |
 *  '--------------'        |         |        |       |       |       |
 *                          V         V        V       V       V       V
 *

 * you can iterate like:
 *
 * for( auto &it : moon9::all<class>() )
 *      it->method();
 *
 * for( auto &it = moon9::begin<class>(), end = moon9::end<class>(); it != end; ++it )
 *      it->method();
 *
 * for( auto &it = moon9::component<class>::iterators::begin(), end = moon9::component<class>::iterators::end(); it != end; ++it )
 *      it->method();

 * Note: always derive from moon9::component<T> first.
 * ie,
 * OK:    class class_a : moon9::component<class_a>, public class_b, private class_c {};
 * WRONG: class class_a : public class_b, private class_c, moon9::component<class_a> {};

 * [ref]
 * - http://cowboyprogramming.com/2007/01/05/evolve-your-heirachy/
 * - "Game Engine Architecture", Jason Gregory

 * To do:
 * - Make it thread-safe (useful?)

 * - rlyeh ~~ listening to Sia / Breath me
 */

#pragma once

#include <cassert>

#include <iostream>
#include <algorithm>

//c++0x
//#include <unordered_set>
#include <set>

namespace moon9
{
//c++0x
//	template <class T, typename container_t = std::unordered_set<T *> > // std::vector<T *>
	template <class T, typename container_t = std::set<T *> > // std::vector<T *>
	class component
	{
		enum config
		{
			verbose = false,
			debug = true
		};

		typedef typename container_t::iterator container_it;

		enum mode_t { ADD, DEL, NOP };

		static container_t *operation( mode_t mode, void *self = 0 )
		{
#if 1
			static container_t container;
#else
            static container_t *container_p = new container_t();
            container_t &container   = *container_p;
#endif

			if( mode == ADD )
			{
				if( config::verbose )
					std::cout << "+ moon9::component<" << typeid(T).name() << "> | .self=" << self << std::endl;

				T* t = reinterpret_cast<T*>(self);

				if( config::debug )
				{
					container_it it = container.find( t );
//					assert1txt( it == container.end(), "error: registering instance twice" );
				}

				container.insert( t );
			}

			if( mode == DEL )
			{
				if( config::verbose )
					std::cout << "- moon9::component<" << typeid(T).name() << "> | .self=" << self << std::endl;

				T* t = reinterpret_cast<T*>(self);

				container_it it = container.find( t );

				if( it != container.end() )
				{
					container.erase( it );
				}
			}

			return &container;
		}

		public:

		component()
		{
			operation( ADD, this );
		}
		component( const component &t )
		{
			operation( ADD, this );
		}
		component &operator=( const component &t )
		{
			operation( ADD, this );
			return *this;
		}
		~component()
		{
			operation( DEL, this );
		}

		struct iterators
		{
			static typename container_t::iterator begin()
			{
				return operation( NOP )->begin();
			}
			static typename container_t::iterator end()
			{
				return operation( NOP )->end();
			}
	        static typename container_t::reverse_iterator rbegin()
	        {
	            return operation( NOP )->rbegin();
	        }
	        static typename container_t::reverse_iterator rend()
	        {
	            return operation( NOP )->rend();
	        }
			static size_t size()
			{
				return operation( NOP )->size();
			}

			typedef typename container_t::iterator iterator;
			typedef typename container_t::const_iterator const_iterator;
			typedef typename container_t::reverse_iterator reverse_iterator;
			typedef typename container_t::const_reverse_iterator const_reverse_iterator;
		};
	};

	template<typename T>
	typename moon9::component<T>::iterators all()
	{
		return moon9::component<T>::iterators();
	}

	template<typename T>
	typename moon9::component<T>::iterators::iterator begin()
	{
		return moon9::component<T>::iterators::begin();
	}

	template<typename T>
	typename moon9::component<T>::iterators::iterator end()
	{
		return moon9::component<T>::iterators::end();
	}

	template<typename T>
	typename moon9::component<T>::iterators::reverse_iterator rbegin()
	{
		return moon9::component<T>::iterators::rbegin();
	}

	template<typename T>
	typename moon9::component<T>::iterators::reverse_iterator rend()
	{
		return moon9::component<T>::iterators::rend();
	}

    template<class T>
    typename size_t size()
    {
        return moon9::component<T>::iterators::size();
    }

    template<class T, typename container_t>
    typename moon9::component<T, container_t>::iterators all()
    {
        return moon9::component<T, container_t>::iterators();
    }

    template<class T, typename container_t>
    typename moon9::component<T, container_t>::iterators::iterator begin()
    {
        return moon9::component<T, container_t>::iterators::begin();
    }

    template<class T, typename container_t>
    typename moon9::component<T, container_t>::iterators::iterator end()
    {
        return moon9::component<T, container_t>::iterators::end();
    }

    template<class T, typename container_t>
    typename moon9::component<T, container_t>::iterators::reverse_iterator rbegin()
    {
        return moon9::component<T, container_t>::iterators::rbegin();
    }

    template<class T, typename container_t>
    typename moon9::component<T, container_t>::iterators::reverse_iterator rend()
    {
        return moon9::component<T, container_t>::iterators::rend();
    }

    template<class T, typename container_t>
    typename size_t size()
    {
        return moon9::component<T, container_t>::iterators::size();
    }
}

