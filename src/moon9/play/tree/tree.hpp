#pragma once

#include <algorithm>
#include <vector>
#include <sstream>

#include <kptree/tree.hh>
#include <kptree/tree_util.hh>

namespace moon9
{
	template< class T >
	class tree : public kptree::tree<T>
	{
		kptree::tree<T>::iterator root, cursor;
		std::vector< kptree::tree<T>::iterator > depth;

		public:

		tree( const T &root_item = T() )
		{
			root = this->insert( this->begin(), root_item );
			depth.push_back( root );

			cursor = root;
		}

		bool find( const T &name )
		{
			return ( cursor = std::find( this->begin(), this->end(), name ) ) != this->end();
		}

		typename tree<T>::iterator found() const
		{
			return cursor;
		}

		typename tree<T>::iterator at( const T &name )
		{
			return std::find( this->begin(), this->end(), name );
		}

		tree<T>& push( const T &name )
		{
			depth.push_back( this->append_child( depth.back(), name ) );
			return *this;
		}
		tree<T>& node( const T &name )
		{
			this->append_child( depth.back(), name );
			return *this;
		}
		tree<T>& back()
		{
			if( depth.size() > 1 ) depth.pop_back();
			return *this;
		}

		void move( const T &from_id, const T &to_id )
		{
			iterator from = std::find( this->begin(), this->end(), from_id );
			iterator to   = std::find( this->begin(), this->end(), to_id );

			if( parent(from) == to )
				return; // already there

			iterator temp = this->append_child( to );

			this->move_ontop( temp, from );
		}

		std::string str() const
		{
			std::ostringstream oss;
			kptree::print_tree_bracketed( *this, oss );
			return oss.str();
		}

		// apply function to all nodes, including children
		// example:
		// tree<node> scene;
		// scene.apply( [](node &n){ render(n); } );

		template< typename FUNC >
		void apply( const FUNC &f )
		{
			if( !this->empty() )
				for( typename sibling_iterator it = this->begin(), end = this->end(); it != end; ++it )
					apply_subtree( it, f );
		}

		protected:

		template< typename FUNC >
		void apply_subtree( const typename tree::sibling_iterator &iRoot, const FUNC &f )
		{
			f( (*iRoot) );

			if( this->number_of_children(iRoot) != 0 )
				for( typename sibling_iterator it = this->begin(iRoot), end = this->end(iRoot); it != end; ++it )
					apply_subtree( it, f );
		}
	};
}

namespace
{
	template <typename T>
	std::ostream &operator<<( std::ostream &oss, const moon9::tree<T> &scene )
	{
		oss << scene.str();
		return oss;
	}
}
