#include <iostream>
#include <string>

#include "fsm.hpp"

namespace moon9
{
    fsm::state::state( const char *_debug_name ) : active( false ), debug_name(_debug_name)
    {}

    fsm::state &fsm::state::as( const std::string &debug_name )
    {
        this->debug_name.assign( debug_name );
        return *this;
    }

    std::string fsm::state::str() const
    {
        return debug_name;
    }

    bool fsm::state::is_active() const
    {
        return active;
    }

    // syntax sugar:

    fsm::state::operator const bool() const
    {
        return active;
    }

    fsm::trigger::trigger( const char *_debug_name ) : debug_name(_debug_name), active( false ), parent_fsm( 0 )
    {}

    fsm::trigger &fsm::trigger::as( const std::string &debug_name )
    {
        this->debug_name.assign( debug_name );
        return *this;
    }

    std::string fsm::trigger::str() const
    {
        return debug_name;
    }

    bool fsm::trigger::is_active() const
    {
        return active;
    }

    // syntax sugar:

    fsm::trigger::operator const bool() const
    {
        return active;
    }

    void fsm::trigger::operator ()()
    {
        if( parent_fsm )
            parent_fsm->notify( this );
    }



    fsm::fsm() : no_state("no_state"), current(&no_state), previous(&no_state), triggered(false)
    {
        current->active = true;
    }

    fsm::~fsm()
    {}

    fsm &fsm::add_trigger( fsm::trigger &trigger )
    {
        alltriggers.insert( &trigger );
        trigger.parent_fsm = this;
        get_ready();
        return *this;
    }

    fsm &fsm::add_state( fsm::state &state )
    {
        allstates.insert( &state );
        get_ready();
        return *this;
    }

    const fsm::state &fsm::get_current_state() const
    {
        return *current;
    }

    const fsm::state &fsm::get_previous_state() const
    {
        return *previous;
    }

    const fsm::triggers &fsm::get_registered_triggers() const
    {
        return alltriggers;
    }

    const fsm::states &fsm::get_registered_states() const
    {
        return allstates;
    }

    const fsm::roadmap &fsm::get_current_roadmap() const
    {
        return roads;
    }

    const fsm::history &fsm::get_state_history() const
    {
        return past;
    }

    // configuration to override

    bool fsm::config_be_verbose() const
    {
        return false;
    }

    std::ostream &fsm::config_use_stream() const
    {
        /*
        //defaults to null: try to override and return clog,cerr,cout,custom streams [...] instead
        static std::ostream cnull(0);
        return cnull.flush();
        */
        return std::cout;
    }

    bool fsm::config_throw_exceptions() const
    {
        return false;
    }

    // syntax sugar:

    fsm &fsm::operator<<( fsm::trigger &trigger )
    {
        add_trigger( trigger );
        return *this;
    }

    fsm &fsm::operator<<( fsm::state &state )
    {
        add_state( state );
        return *this;
    }

    // fsm engine core

    void fsm::notify( fsm::trigger *child_trigger )
    {
        if( !child_trigger )
            return;

    #if 0
        static bool throw_exceptions = false;
        static bool is_verbose = true;
        static std::ostream cnull(0);
        static std::ostream &output = std::cerr; //cnull;
    #else
        bool throw_exceptions = config_throw_exceptions();
        bool is_verbose = config_be_verbose();
        std::ostream &output = config_use_stream();
    #endif

        if( is_verbose )
            output << "fsm::raising trigger " << *child_trigger << "()" << std::endl;

        child_trigger->active = true;
        state *new_current = next();
        child_trigger->active = false;

        if( new_current )
        {
            if( is_verbose )
                output << "fsm::trigger " << *child_trigger << "() released" << std::endl;

            triggered = true;

            previous = current;
            current = new_current;

            previous->active = false;
            current->active = true;

            if( is_verbose )
                output << "fsm::transition from state '" << *previous << "' to state '" << *current << "' went ok" << std::endl;
        }
        else
        {
            if( is_verbose )
                output << "fsm::invalid trigger " << *child_trigger << "() raised during state '" << *current << "'" << std::endl;

            if( throw_exceptions )
                throw std::string("<moon9/play/fsm.hpp> says: invalid_fsm_trigger: ") + child_trigger->str();
        }

        // roadmap generation

        std::string from = previous ? previous->str() : std::string();
        std::string to = current ? current->str() : std::string();

        if( from == to )
            return; // dont track loops (~track state changes only)

        if( from.size() == 0 || to.size() == 0 )
            return; // dont track empty states

        roads[ from ].insert( to );

        // history generation

        std::string pretty = from + " -> " + to;

        if( past.size() > 0 )
            if( past.back() == pretty )
                return; // avoid dupes

        past.push_back( pretty );
        if( past.size() > 8 )
            past.pop_front();
    }

    void fsm::get_ready()
    {
        previous = current = first();
        current->active = true;

        // init history
        past = std::deque<std::string>();
        past.push_back( current->str() );
    }

}

