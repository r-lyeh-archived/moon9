/*
 * (C) Copyright Mario "rlyeh" Rodriguez 2010
 * (C) Copyright Christopher Diggins 2005
 * (C) Copyright Pablo Aguilar 2005
 * (C) Copyright Kevlin Henney 2001
 *
 * Changelist: (27/Oct/2010)
 * - Changed all void * type to uintptr_t (rlyeh)
 * - Added support for pointer to method class (rlyeh)
 * - Removed stdio.h dependancy by changing NULL to 0 (rlyeh)
 * - Fixed the missing return value in the swap() function (rlyeh)
 * - Removed operator = that was causing some leaks (to checkkkkkkkkkkkkkkk ---------------)
 * - Replaced include guards with #pragma once (rlyeh)
 * - Rename cdiggins:: namespace to moon9:: (my own library :o) (rlyeh)
 * - Renamed cast<T>() method to as<T>() to conform my boo library convenion (rlyeh)
 * - Reformatted (rlyeh)
 * - Removed implicit (and dangerous) casting support (rlyeh)
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <stdexcept>
#include <typeinfo>
#include <algorithm>

#if 0
#include <moon9/os/exception.hpp>
#include <moon9/os/types.hpp>
#endif

#define any_voidptr uintptr_t *      //upcoming c++0x: std::uintptr_t

namespace moon9
{
    /*
    struct bad_any_cast : std::bad_cast {
        bad_any_cast(const std::type_info& src, const std::type_info& dest)
            : from(src.name()), to(dest.name())
        {
            moon9::callstack().print();
        }
        virtual const char* what() {
            static std::string ugh;
            ugh = std::string("bad cast from '")+from+"' to '"+to+"'";
            return ugh.c_str();
        }
        const char* from;
        const char* to;
    };
    */

    namespace any_detail {
        // function pointer table

        struct fxn_ptr_table {
            const std::type_info& (*get_type)();
            void (*static_delete)(any_voidptr*);
            void (*clone)(any_voidptr const*, any_voidptr*);
            void (*move)(any_voidptr const*,any_voidptr*);
        };

        // static functions for small value-types

        template<bool is_small>
        struct fxns
        {
            template<typename T>
            struct type {
                static const std::type_info& get_type() {
                    return typeid(T);
                }
                static void static_delete(any_voidptr* x) {
                    reinterpret_cast<T*>(x)->~T();
                }
                static void clone(any_voidptr const* src, any_voidptr* dest) {
                    new(dest) T(*reinterpret_cast<T const*>(src));
                }
                static void move(any_voidptr const* src, any_voidptr* dest) {
                    if(dest)reinterpret_cast<T*>(dest)->~T();
                    *reinterpret_cast<T*>(dest) = *reinterpret_cast<T const*>(src);
                }
            };
        };

        // static functions for big value-types (bigger than a any_voidptr)

        template<>
        struct fxns<false>
        {
            template<typename T>
            struct type {
                static const std::type_info& get_type() {
                    return typeid(T);
                }
                static void static_delete(any_voidptr* x) {
                    delete(*reinterpret_cast<T**>(x));
                }
                static void clone(any_voidptr const* src, any_voidptr* dest) {
                    *dest = (any_voidptr)new T(**reinterpret_cast<T* const*>(src));
                }
                static void move(any_voidptr const* src, any_voidptr* dest) {
                    if(dest) (*reinterpret_cast<T**>(dest))->~T();
                    **reinterpret_cast<T**>(dest) = **reinterpret_cast<T* const*>(src);
                }
            };
        };

        template<typename T>
        struct get_table
        {
            static const bool is_small = sizeof(T) <= sizeof(any_voidptr);

            static fxn_ptr_table* get()
            {
                static fxn_ptr_table static_table = {
                    fxns<is_small>::template type<T>::get_type,
                    fxns<is_small>::template type<T>::static_delete,
                    fxns<is_small>::template type<T>::clone,
                    fxns<is_small>::template type<T>::move
                };
                return &static_table;
            }
        };

        struct empty {
        };
    } // namespace any_detail


    struct any
    {
        // structors

        any() :
            table( any_detail::get_table<any_detail::empty>::get() ),
            object( 0 ),
            refs( 0 )
        {}

        ~any() {
            if( refs )
                table->static_delete(&object);
            object = 0;
        }

        template <typename T>
        any(const T& x) :
            table( any_detail::get_table<T>::get() ),
            refs( 1 ) {
                if (sizeof(T) <= sizeof(any_voidptr)) {
                    new(&object) T(x);
                }
                else {
                    object = (any_voidptr)new T(x);
                }
            }

        any(const any& x) :
            table( any_detail::get_table<any_detail::empty>::get() ), 
            object( 0 ),
            refs( 0 ) {
              assign(x);
            }

#if 1

        // assignment operator
        template<typename T>
        any& operator=( const T& x) { //T const& x) {
            moon9::any a( x );
            return swap( a );
        }

        any& operator=( const any& x ) {
            return assign( x );
        }

#endif

        // assignment
        #if 1

        any& assign(const any& x) {

            if( x.empty() )
            {
                reset();
                return *this;
            }

            // are we copying between the same type?
            if (table == x.table) {
                // if so, we can avoid reallocation
                x.table->move(&x.object, &object);
                table = x.table;
            }
            else {
                reset();
                x.table->clone(&x.object, &object);
                table = x.table;
            }

            refs = 1;

            return *this;
        }

        #endif

        #if 0

        template <typename T>
        any& assign(const T& x)
        {
            // are we copying between the same type?

            any_detail::fxn_ptr_table* x_table = any_detail::get_table<T>::get();
            if (table == x_table) {
                // if so, we can avoid deallocating and resuse memory

                if (sizeof(T) <= sizeof(any_voidptr)) {
                    // create copy on-top of object pointer itself

                    new(&object) T(x);
                }
                else {
                    // create copy on-top of old version

                    new(object) T(x);
                }
            }
            else {
                reset();
                if (sizeof(T) <= sizeof(any_voidptr)) {
                    // create copy on-top of object pointer itself

                    new(&object) T(x);
                    // update table pointer

                    table = x_table;
                }
                else {
                    object = (any_voidptr)new T(x);
                    table = x_table;
                }
            }

            refs = 0;

            return *this;
        }

#endif

        // utility functions

        any& swap(any& x) {
            std::swap(table, x.table);
            std::swap(object, x.object);
            std::swap(refs, x.refs);
            return *this;
        }

        const std::type_info& get_type() const {
            return table->get_type();
        }

        template<typename T>
        const T& as() const {
            if (get_type() != typeid(T)) {
#if 0                
                throw moon9::exception("moon9::exception::any", //bad_any_cast(get_type(), typeid(T));
                std::string("bad cast from '")+get_type().name()+"' to '"+typeid(T).name()+"'");
#else
                //assertion() << "bad cast from '"+get_type().name()+"' to '"+typeid(T).name()+"'" << std::endl;
                //assert4( get_type(), !=, typeid(T), "bad cast from '\1' to '\2'" );
                assert( !"bad cast" );
#endif
            }
            if (sizeof(T) <= sizeof(any_voidptr)) {
                return *reinterpret_cast<T const*>(&object);
            }
            else {
                return *reinterpret_cast<T const*>(object);
            }
        }

  #if 0
  
    // automatic casting operator

    template<typename T>
    operator T() const {
      return as<T>();
    }

    std::string str() const
    {
        std::stringstream ss;

        if( get_type() == typeid(int) ) { ss << as<int>(); return ss.str(); }
        if( get_type() == typeid(char) ) { ss << as<char>(); return ss.str(); }
        if( get_type() == typeid(size_t) ) { ss << as<size_t>(); return ss.str(); }
        if( get_type() == typeid(float) ) { ss << as<float>(); return ss.str(); }
        if( get_type() == typeid(double) ) { ss << as<double>(); return ss.str(); }
        if( get_type() == typeid(const char *) ) { ss << as<const char *>(); return ss.str(); }
        if( get_type() == typeid(std::string) ) { ss << as<std::string>(); return ss.str(); }
        if( get_type() == typeid(moon9::string) ) { ss << as<moon9::string>(); return ss.str(); }

        if( get_type() == typeid(int*) ) { ss << *as<int*>(); return ss.str(); }
        if( get_type() == typeid(float*) ) { ss << *as<float*>(); return ss.str(); }
        if( get_type() == typeid(moon9::string*) ) { ss << *as<moon9::string*>(); return ss.str(); }

        throw moon9::exception("moon9::exception::any", std::string("printing for type '") + get_type().name() + "' is unsupported (add it!)");

        return std::string();
    }

  #endif // implicit casting        


        bool empty() const {
            return table == any_detail::get_table<any_detail::empty>::get();
        }

        void reset()
        {
            if (!empty())
            {
                table->static_delete(&object);
                table = any_detail::get_table<any_detail::empty>::get();
            }
            object = 0;
            refs = 0;
        }

        // variadic structors & methods

        #define any_variadic_stuff( fn_args, ... ) \
        template < __VA_ARGS__ > \
        any( RETURN_TYPE (CLASS_TYPE::*ptr) fn_args ) \
        { \
            typedef RETURN_TYPE (CLASS_TYPE::*METHOD_TYPE) fn_args; \
            table = any_detail::get_table<METHOD_TYPE>::get(); \
            if (sizeof(METHOD_TYPE) <= sizeof(any_voidptr)) { \
                new(&object) METHOD_TYPE(ptr); \
            } \
            else { \
                object = (any_voidptr)new METHOD_TYPE(ptr); \
            } \
        } 
        #if 0 
        template < __VA_ARGS__ > \
        any& assign( RETURN_TYPE (CLASS_TYPE::*ptr)fn_args ) \
        { \
            typedef RETURN_TYPE (CLASS_TYPE::*METHOD_TYPE) fn_args; \
            /* are we copying between the same type? */ \
            any_detail::fxn_ptr_table* x_table = any_detail::get_table<METHOD_TYPE>::get(); \
            if (table == x_table) { \
                /* if so, we can avoid deallocating and resuse memory */ \
                if (sizeof(METHOD_TYPE) <= sizeof(any_voidptr)) { \
                    /* create copy on-top of object pointer itself */ \
                    new(&object) METHOD_TYPE(ptr); \
                } \
                else { \
                    /* create copy on-top of old version */ \
                    new(object) METHOD_TYPE(ptr); \
                } \
            } \
            else { \
                reset(); \
                if (sizeof(METHOD_TYPE) <= sizeof(any_voidptr)) { \
                    /* create copy on-top of object pointer itself */ \
                    new(&object) METHOD_TYPE(ptr); \
                    /* update table pointer */ \
                    table = x_table; \
                } \
                else { \
                    object = new METHOD_TYPE(ptr); \
                    table = x_table; \
                } \
            } \
            return *this; \
        } \
        template < __VA_ARGS__ > \
        any& operator=( RETURN_TYPE (CLASS_TYPE::*ptr)fn_args ) { \
            typedef RETURN_TYPE (CLASS_TYPE::*METHOD_TYPE) fn_args; \
            assign(ptr); \
            return *this; \
        }
        #endif

        #ifdef C
        #undef C // i hate people
        #endif

        any_variadic_stuff( (void), typename CLASS_TYPE, typename RETURN_TYPE )
        any_variadic_stuff( (A a), typename CLASS_TYPE, typename RETURN_TYPE, typename A )
        any_variadic_stuff( (A a, B b), typename CLASS_TYPE, typename RETURN_TYPE, typename A, typename B )
        any_variadic_stuff( (A a, B b, C c), typename CLASS_TYPE, typename RETURN_TYPE, typename A, typename B, typename C )
        any_variadic_stuff( (A a, B b, C c, D d), typename CLASS_TYPE, typename RETURN_TYPE, typename A, typename B, typename C, typename D )
        any_variadic_stuff( (A a, B b, C c, D d, E e), typename CLASS_TYPE, typename RETURN_TYPE, typename A, typename B, typename C, typename D, typename E )
        any_variadic_stuff( (A a, B b, C c, D d, E e, F f), typename CLASS_TYPE, typename RETURN_TYPE, typename A, typename B, typename C, typename D, typename E, typename F )
        any_variadic_stuff( (A a, B b, C c, D d, E e, F f, G g), typename CLASS_TYPE, typename RETURN_TYPE, typename A, typename B, typename C, typename D, typename E, typename F, typename G )
        any_variadic_stuff( (A a, B b, C c, D d, E e, F f, G g, H h), typename CLASS_TYPE, typename RETURN_TYPE, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H )
        any_variadic_stuff( (A a, B b, C c, D d, E e, F f, G g, H h, I i), typename CLASS_TYPE, typename RETURN_TYPE, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I )
        any_variadic_stuff( (A a, B b, C c, D d, E e, F f, G g, H h, I i, J j), typename CLASS_TYPE, typename RETURN_TYPE, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J )

        #undef any_variadic_stuff

        // fields

        any_detail::fxn_ptr_table* table;
        any_voidptr object;
        size_t refs;
    };

    // boost::any-like casting

    template<typename T>
    T* any_cast(any* this_) {
        if (this_->get_type() != typeid(T)) {
            throw moon9::exception("moon9::exception::any", //bad_any_cast(this_->get_type(), typeid(T));
            std::string("bad cast from '")+this_->get_type().name()+"' to '"+typeid(T).name()+"'");
        }
        if (sizeof(T) <= sizeof(any_voidptr)) {
            return reinterpret_cast<T*>(&this_->object);
        }
        else {
            return reinterpret_cast<T*>(this_->object);
        }
    }

    template<typename T>
    T const* any_cast(any const* this_) {
        return any_cast<T>(const_cast<any*>(this_));
    }

    template<typename T>
    T const& any_cast(any const& this_){
        return *any_cast<T>(const_cast<any*>(&this_));
    }

    #undef any_voidptr
}

