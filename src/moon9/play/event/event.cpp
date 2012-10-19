#include "event.hpp"

#include <string>
#include <map>          // multimap?
#include <set>
#include <deque>

namespace event
{
    namespace
    {
        typedef std::deque< std::string > event_arguments;
        typedef void (*event_callback)( const event_arguments & );

        namespace detail
        {
            struct event
            {
                std::set< event_callback  > callbacks;
                std::set< event_arguments > arguments;
            };
        }

        std::map< std::string, detail::event > events;
    }

    // enable
    void on( const std::string &verb, event_callback cb )
    {
        events[ verb ] = events[ verb ];
        events[ verb ].callbacks.insert( cb );
    }

    // disable
    void off( const std::string &verb, event_callback cb )
    {
        events[ verb ] = events[ verb ];
        events[ verb ].callbacks.erase( cb );
    }

    void off( const std::string &verb )
    {
        events[ verb ] = events[ verb ];
        events[ verb ] = detail::event();
    }

    void off()
    {
        events = std::map< std::string, detail::event >();
    }

    // immediate mode
    void trigger( const std::string &subject, const std::string &verb )
    {
        event_arguments args;
        args.push_back( subject );

        events[ verb ] = events[ verb ];

        for( std::set< event_callback  >::iterator jt = events[ verb ].callbacks.begin(), jend = events[ verb ].callbacks.end(); jt != jend; ++jt )
            (**jt)( args );
    }
    void trigger( const std::string &subject, const std::string &verb, const std::string &object1 )
    {
        event_arguments args;
        args.push_back( subject );
        args.push_back( object1 );

        events[ verb ] = events[ verb ];

        for( std::set< event_callback  >::iterator jt = events[ verb ].callbacks.begin(), jend = events[ verb ].callbacks.end(); jt != jend; ++jt )
            (**jt)( args );
    }
    void trigger( const std::string &subject, const std::string &verb, const std::string &object1, const std::string &object2 )
    {
        event_arguments args;
        args.push_back( subject );
        args.push_back( object1 );
        args.push_back( object2 );

        events[ verb ] = events[ verb ];

        for( std::set< event_callback  >::iterator jt = events[ verb ].callbacks.begin(), jend = events[ verb ].callbacks.end(); jt != jend; ++jt )
            (**jt)( args );
    }

    void queue( const std::string &subject, const std::string &verb )
    {
        event_arguments args;
        args.push_back( subject );

        events[ verb ] = events[ verb ];
        events[ verb ].arguments.insert( args );
    }
    void queue( const std::string &subject, const std::string &verb, const std::string &object1 )
    {
        event_arguments args;
        args.push_back( subject );
        args.push_back( object1 );

        events[ verb ] = events[ verb ];
        events[ verb ].arguments.insert( args );
    }
    void queue( const std::string &subject, const std::string &verb, const std::string &object1, const std::string &object2 )
    {
        event_arguments args;
        args.push_back( subject );
        args.push_back( object1 );
        args.push_back( object2 );

        events[ verb ] = events[ verb ];
        events[ verb ].arguments.insert( args );
    }

    // dispatch set of queued events
    void dispatch( const std::string &verb )
    {
        events[ verb ] = events[ verb ];

        detail::event &e = events[ verb ];

        // for every callback
        for( std::set< event_callback  >::iterator jt = e.callbacks.begin(), jend = e.callbacks.end(); jt != jend; ++jt )
        for( std::set< event_arguments >::iterator kt = e.arguments.begin(), kend = e.arguments.end(); kt != kend; ++kt )
        {
            event_arguments copy = *kt;

            (**jt)( copy );
        }

        // clear
        e.arguments = std::set<event_arguments>();
    }
    // dispatch all queued events
    void dispatch()
    {
        // for every event
        for( std::map< std::string, detail::event >::iterator it = events.begin(), end = events.end(); it != end; ++it )
        {
            detail::event &e = it->second;

            // for every callback
            for( std::set< event_callback  >::iterator jt = e.callbacks.begin(), jend = e.callbacks.end(); jt != jend; ++jt )
            for( std::set< event_arguments >::iterator kt = e.arguments.begin(), kend = e.arguments.end(); kt != kend; ++kt )
            {
                event_arguments copy = *kt;

                (**jt)( copy );
            }

            // clear
            e.arguments = std::set<event_arguments>();
        }
    }
}
