/*
 * Simple FSM class
 * Copyright (c) 2011 Mario 'rlyeh' Rodriguez

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

 * - inspiration:   BOOST MSM (eUML FSM)
 *   format:        source + event [guard(s)] [/ action(s)] == target
 *   original sample:
 *
 *   BOOST_MSM_EUML_TRANSITION_TABLE((
 *   Stopped + play [some_guard] / (some_action , start_playback)  == Playing ,
 *   Stopped + open_close/ open_drawer                             == Open    ,
 *   Stopped + stop                                                == Stopped ,
 *   Open    + open_close / close_drawer                           == Empty   ,
 *   Empty   + open_close / open_drawer                            == Open    ,
 *   Empty   + cd_detected [good_disk_format] / store_cd_info      == Stopped
 *   ),transition_table)
 *
 * - proposal:      moon9 fsm
 *   format:        return
 *                  current_state && trigger [&& guard(s)] ? ( [action(s),] new_state) :
 *                  [...]
 *                  0
 *   sample becomes:
 *
 *   return
 *   Stopped && play && some_guard()            ? (some_action(), start_playback(), Playing) :
 *   Stopped && open_close                      ? (open_drawer(), Open) :
 *   Stopped && stop                            ? (Stopped) :
 *   Open && open_close                         ? (close_drawer(), Empty) :
 *   Empty && open_close                        ? (open_drawer(), Open) :
 *   Empty && cd_detected && good_disk_format() ? (store_cd_info(), Stopped) :
 *   0

 * todo:
 * - thread safe
 * - throw exceptions
 * - HFSM? GOAP? behavior trees?

 * - rlyeh
 */

#pragma once

#include <iostream>
#include <string>

#include <deque>
#include <map>
#include <set>
#include <vector>

namespace moon9
{
    class fsm
    {
        public:

        // child classes

        class state
        {
            public /*api*/:

                state( const char *_debug_name = "" );

                state &as( const std::string &debug_name );

                std::string str() const;

                bool is_active() const;

                // syntax sugar:

                operator const bool() const;

            protected: friend class fsm;

                bool active;
                std::string debug_name;
        };

        class trigger 
        {
            public /*api*/:

                trigger( const char *_debug_name = "" );

                trigger &as( const std::string &debug_name );

                std::string str() const;

                bool is_active() const;

                // syntax sugar:

                operator const bool() const;

                void operator ()();

            protected: friend class fsm;

                std::string debug_name;
                fsm *parent_fsm;
                bool active;
        };

        typedef std::deque< std::string > history;
        typedef std::set< state * > states;
        typedef std::set< trigger * > triggers;
        typedef std::map< std::string /*from*/, std::set< std::string /*to*/ > > roadmap;

        public /*api*/:

            fsm();

            virtual ~fsm();

            moon9::fsm     &add_trigger( moon9::fsm::trigger &trigger );
            moon9::fsm     &add_state( moon9::fsm::state &state );

            const state    &get_current_state() const;
            const state    &get_previous_state() const;

            const states   &get_registered_states() const;
            const triggers &get_registered_triggers() const;
            const roadmap  &get_current_roadmap() const;
            const history  &get_state_history() const;

            bool            has_triggered() const { bool ret = triggered; triggered = false; return ret; } // should clear trigger flag automatically?
            void            clear_trigger_flag() { triggered = false; }

        // transitions to override

            virtual state *first() = 0;
            virtual state *next() = 0;

        // configuration to override

            virtual bool          config_be_verbose() const;
            virtual std::ostream &config_use_stream() const;
            virtual bool          config_throw_exceptions() const;

        // syntax sugar:

            moon9::fsm &operator<<( moon9::fsm::trigger &trigger );
            moon9::fsm &operator<<( moon9::fsm::state &state );

        // fsm engine core

        protected: friend class moon9::fsm::trigger;

            void notify( moon9::fsm::trigger *child_trigger );

        // fsm details

        private:

            state no_state, *current, *previous;

            mutable bool     triggered;

            triggers alltriggers;
            states   allstates;
            roadmap  roads;
            history  past;

            void get_ready();
    };
}

