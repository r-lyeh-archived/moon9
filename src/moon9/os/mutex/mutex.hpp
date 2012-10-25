/*
 * Simple mutex classes
 * Copyright (c) 2010 Mario 'rlyeh' Rodriguez

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

 * To do:
 * - never exclude/block when mutex.requester.this == mutex.owner.this
 * - synchronize( this ) { } / synchronized(this) { }
 * - _InterlockedCompareExchange() may be 150x faster than EnterCriticalSection():
 *   http://www.dreamincode.net/forums/topic/161166-microsoft-windows-thread-synchronization/
 *   http://wesnoth.repositoryhosting.com/trac/wesnoth_wesnoth/browser/trunk/Classes/Boost/regex_140/static_mutex.cpp?rev=8

 * - rlyeh
 */

#pragma once

#include <algorithm>
#include <map>
#include <vector>

#include <cassert>

#if 1 // kMoon9CompilerCxx11 // c++11
#   include <mutex>
#   include <thread>
#   define MOON9_THREAD_NS std
#else
#   include <tinythread.h>
#   define MOON9_THREAD_NS tthread
#endif

namespace moon9
{
    // Block/unblock local resource to grant current thread exclusive access
    // Block/unblock a shared/global resource to grant current thread exclusive access

    class mutex
    {
        public:

        mutex() : locked_by( nobody() )
        {}

        void lock()
        {
            if( locked_by == me() )
                return;

            self.lock();
            locked_by = me();
        }

        void unlock()
        {
            if( locked_by != me() )
                return;

            locked_by = nobody();
            self.unlock();
        }

        bool try_lock()
        {
            if( locked_by == me() )
                return true;

            if( !self.try_lock() )
                return false;

            locked_by = me();
            return true;
        }

        bool is_locked() const
        {
            return locked_by != nobody();
        }

        bool is_locked_by_me() const
        {
            return locked_by == me();
        }

        private:

#ifdef __linux__
        typedef MOON9_THREAD_NS::recursive_mutex mutex_type; // or tthread::fast_mutex, or std::mutex, ...
#else
        typedef MOON9_THREAD_NS::mutex mutex_type; // or tthread::fast_mutex, or std::mutex, ...
#endif
        typedef MOON9_THREAD_NS::thread::id thread_id;

        thread_id locked_by;
        mutex_type self;

        thread_id nobody() const // return empty/invalid thread
        {
            return thread_id();
        }

        thread_id me() const // return current thread id
        {
            return MOON9_THREAD_NS::this_thread::get_id();
        }

        // Private copy-constructor, copy-assignment

        mutex( const mutex & );
        mutex& operator=( const mutex & );
    };

    class blocker
    {
        public:

        template<typename T>
        explicit blocker( const T *shared_ptr ) : ptr( (const void *) shared_ptr )
        {
            if( ptr ) manager( ptr, LOCK );
        }

        ~blocker()
        {
            if( ptr ) manager( ptr, UNLOCK );
        }

        private:

        const void *ptr;

        enum lock_mode
        {
            LOCK, UNLOCK, TRY_LOCK
        };

        static bool manager( const void *ptr, lock_mode mode )
        {
#ifdef __linux__
            typedef MOON9_THREAD_NS::recursive_mutex mutex_type; // or tthread::fast_mutex, or std::mutex, ...
#else
            typedef MOON9_THREAD_NS::mutex mutex_type; // or tthread::fast_mutex, or std::mutex, ...
#endif
            static struct container : public std::map< const void *, mutex_type * >
            {
                 container() {}
                ~container() { for( iterator it = this->begin(); it != this->end(); ++it ) delete it->second; }
            }
            map;

            static moon9::mutex master;

            master.lock();

            if( map.find(ptr) == map.end() )
                { map[ ptr ] = map[ ptr ]; map[ ptr ] = new mutex_type; }

            mutex_type *found = map[ ptr ];

            master.unlock();

            if( mode == TRY_LOCK )
                return found->try_lock();

            if( mode == LOCK )
                found->lock();
            else
                found->unlock();

            return false;
        }

        // Private copy-constructor, copy-assignment

        blocker();
        blocker( const blocker & );
        blocker& operator=( const blocker & );
    };
}

#undef MOON9_THREAD_NS
