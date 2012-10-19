// Simple events (signal/slot)
// - rlyeh

// @todo: check-for-non-registered-events
// @todo: ordered_dispatch()

#pragma once

#include <string>
#include <deque>

namespace event
{
    // setup: enable event/disable event/disable set/disable all
    void  on( const std::string &verb, void (*event_callback)( const std::deque< std::string > &objects ) );
    void off( const std::string &verb, void (*event_callback)( const std::deque< std::string > &objects ) );
    void off( const std::string &verb );
    void off();

    // usage: immediate mode
    void trigger( const std::string &subject, const std::string &verb );
    void trigger( const std::string &subject, const std::string &verb, const std::string &object1 );
    void trigger( const std::string &subject, const std::string &verb, const std::string &object1, const std::string &object2 );

    // usage: batch mode; queue/dispatch all/dispatch set
    void queue( const std::string &subject, const std::string &verb );
    void queue( const std::string &subject, const std::string &verb, const std::string &object1 );
    void queue( const std::string &subject, const std::string &verb, const std::string &object1, const std::string &object2 );

    void dispatch();
    void dispatch( const std::string &verb );
}
