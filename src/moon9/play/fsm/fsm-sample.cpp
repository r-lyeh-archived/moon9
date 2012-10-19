#include <iostream>
#include <string>

#include <chrono>
#include <thread>

#include "fsm.hpp"

class cd_player : public moon9::fsm
{
    // implementation conditions / guards
    bool good_disk_format() { return true; }

    // implementation actions
    void open_tray()      { std::cout <<                       "opening tray" << std::endl; sleep4(1); }
    void close_tray()     { std::cout <<                       "closing tray" << std::endl; sleep4(2); }
    void get_cd_info()    { std::cout <<                 "retrieving CD info" << std::endl; sleep4(4); }
    void start_playback() { std::cout <<                      "playing track" << std::endl; sleep4(1); }

    // implementation variables
    bool got_cd;

    public:

    // available states
    moon9::fsm::state open, close, waiting, playing;

    // transition triggers
    moon9::fsm::trigger play, open_lid, close_lid, stop, insert_cd, remove_cd;

    // constructor

    cd_player() : got_cd(false)
    {
         // register states into fsm
         (*this) << open.as("open") << close.as("close") << waiting.as("waiting") << playing.as("playing");

         // register triggers into fsm
         (*this) << play.as("play") << open_lid.as("open_lid") << close_lid.as("close_lid") << stop.as("stop") << insert_cd.as("insert_cd") << remove_cd.as("remove_cd");
    }

    // transitions (logic)

    virtual moon9::fsm::state *first()
    {
        return &close;
    }

    virtual moon9::fsm::state *next()
    {
        return
        open && close_lid                     ? (close_tray(), got_cd ? get_cd_info(), &waiting : &close) :
        open && insert_cd                     ? (got_cd = true, &open) :
        open && remove_cd                     ? (got_cd = false, &open) :

        close && open_lid                     ? (open_tray(), &open) :

        waiting && play && good_disk_format() ? (start_playback(), &playing) :
        waiting && open_lid                   ? (open_tray(), &open) :

        playing && open_lid                   ? (open_tray(), &open) :
        playing && stop                       ? (&waiting) :
        0;
    }

    // config

    virtual bool config_be_verbose() const
    {
        return true;
    }

    /*
    virtual std::ostream &config_use_stream() const
    {
        return std::cout;
    }

    virtual bool config_throw_exceptions() const
    {
        return true;
    }
    */

    private:

    void sleep4( double seconds )
    {
        std::chrono::microseconds duration( (int)(seconds * 1000000) );
        std::this_thread::sleep_for( duration );
    }
};


int main( int argc, char **argv )
{
    cd_player cd;

    while( 1 )
    {
        char cmd;
        std::cout << "(o)pen lid, (c)lose lid, (i)nsert cd, (r)emove cd, (p)lay, (s)top cd, (q)uit? ";
        std::cout << "[current status: " << cd.get_current_state().str() << "] ";
        std::cin >> cmd;

        switch( cmd )
        {
            case 'p':      cd.play(); break;
            case 'o':  cd.open_lid(); break;
            case 'c': cd.close_lid(); break;
            case 's':      cd.stop(); break;
            case 'i': cd.insert_cd(); break;
            case 'r': cd.remove_cd(); break;

            case 'q': return 0;
            default : std::cout << "what?" << std::endl;
        }
    }

    return 0;
}


