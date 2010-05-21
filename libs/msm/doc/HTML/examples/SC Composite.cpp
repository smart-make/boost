#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include "boost/mpl/list.hpp"

#include <vector>

#include <iostream>
#ifdef WIN32
#include "windows.h"
#else
#include <sys/time.h>
#endif

namespace sc = boost::statechart;
namespace mpl = boost::mpl;

namespace test_sc
{

    //events
    struct play : sc::event< play > {};
    struct end_pause : sc::event< end_pause > {};
    struct stop : sc::event< stop > {};
    struct pause : sc::event< pause > {};
    struct open_close : sc::event< open_close > {};
    struct cd_detected : sc::event< cd_detected > {};
    struct NextSong: sc::event< NextSong > {};
    struct PreviousSong : sc::event< PreviousSong >{};

    struct Empty;
    struct Open;
    struct Stopped;
    struct Playing;
    struct Paused;
    // SM
    struct player : sc::state_machine< player, Empty > 
    {
        void open_drawer(open_close const&)         { /*std::cout << "player::open_drawer\n";*/ }
        void store_cd_info(cd_detected const& cd)   {/*std::cout << "player::store_cd_info\n";*/ }
        void close_drawer(open_close const&)        { /*std::cout << "player::close_drawer\n";*/ }
        void start_playback(play const&)            { /*std::cout << "player::start_playback\n";*/ }
        void stopped_again(stop const&)	            {/*std::cout << "player::stopped_again\n";*/}
        void stop_playback(stop const&)             { /*std::cout << "player::stop_playback\n";*/ }
        void pause_playback(pause const&)           { /*std::cout << "player::pause_playback\n"; */}
        void stop_and_open(open_close const&)       { /*std::cout << "player::stop_and_open\n";*/ }
        void resume_playback(end_pause const&)      { /*std::cout << "player::resume_playback\n";*/ }
    };

    struct Empty : sc::simple_state< Empty, player >
    {
        Empty() { /*std::cout << "entering Empty" << std::endl;*/ } // entry
        ~Empty() { /*std::cout << "leaving Empty" << std::endl;*/ } // exit
        typedef mpl::list<
            sc::transition< open_close, Open,
            player, &player::open_drawer >,
            sc::transition< cd_detected, Stopped,
            player, &player::store_cd_info > > reactions;

    };
    struct Open : sc::simple_state< Open, player >
    {
        Open() { /*std::cout << "entering Open" << std::endl;*/ } // entry
        ~Open() { /*std::cout << "leaving Open" << std::endl;*/ } // exit
        typedef sc::transition< open_close, Empty,
            player, &player::close_drawer > reactions;

    };
    struct Stopped : sc::simple_state< Stopped, player >
    {
        Stopped() { /*std::cout << "entering Stopped" << std::endl;*/ } // entry
        ~Stopped() { /*std::cout << "leaving Stopped" << std::endl;*/ } // exit
        typedef mpl::list<
            sc::transition< play, Playing,
            player, &player::start_playback >,
            sc::transition< open_close, Open,
            player, &player::open_drawer >, 
            sc::transition< stop, Stopped,
            player, &player::stopped_again > > reactions;

    };
    struct Song1;
    struct Playing : sc::simple_state< Playing, player,Song1 >
    {
        Playing() { /*std::cout << "entering Playing" << std::endl;*/ } // entry
        ~Playing() { /*std::cout << "leaving Playing" << std::endl;*/ } // exit
        typedef mpl::list<
            sc::transition< stop, Stopped,
            player, &player::stop_playback >,
            sc::transition< pause, Paused,
            player, &player::pause_playback >, 
            sc::transition< open_close, Open,
            player, &player::stop_and_open > > reactions;
        void start_next_song(NextSong const&)       { /*std::cout << "Playing::start_next_song\n";*/ }
        void start_prev_song(PreviousSong const&)       { /*std::cout << "Playing::start_prev_song\n";*/ }
    };
    struct Song2;
    struct Song1  : sc::simple_state< Song1, Playing >
    {
        Song1() { /*std::cout << "entering Song1" << std::endl;*/ } // entry
        ~Song1() { /*std::cout << "leaving Song1" << std::endl;*/ } // exit
        typedef sc::transition< NextSong, Song2,
            Playing, &Playing::start_next_song > reactions;
    };
    struct Song3;
    struct Song2  : sc::simple_state< Song2, Playing >
    {
        Song2() { /*std::cout << "entering Song2" << std::endl;*/ } // entry
        ~Song2() { /*std::cout << "leaving Song2" << std::endl;*/ } // exit
        typedef mpl::list<
            sc::transition< NextSong, Song3,
            Playing, &Playing::start_next_song >,
            sc::transition< PreviousSong, Song1,
            Playing, &Playing::start_prev_song > > reactions;
    };
    struct Song3  : sc::simple_state< Song3, Playing >
    {
        Song3() { /*std::cout << "entering Song3" << std::endl;*/ } // entry
        ~Song3() { /*std::cout << "leaving Song3" << std::endl;*/ } // exit
        typedef sc::transition< PreviousSong, Song2,
            Playing, &Playing::start_prev_song > reactions;
    };
    struct Paused : sc::simple_state< Paused, player >
    {
        Paused() { /*std::cout << "entering Paused" << std::endl;*/ } // entry
        ~Paused() { /*std::cout << "leaving Paused" << std::endl;*/ } // exit
        typedef mpl::list<
            sc::transition< end_pause, Playing,
            player, &player::resume_playback >,
            sc::transition< stop, Stopped,
            player, &player::stop_playback >, 
            sc::transition< open_close, Open,
            player, &player::stop_and_open > > reactions;
    };
}


#ifndef WIN32
long mtime(struct timeval& tv1,struct timeval& tv2)
{
    return (tv2.tv_sec-tv1.tv_sec) *1000000 + ((tv2.tv_usec-tv1.tv_usec));
}
#endif

int main()
{
    test_sc::player p;
    p.initiate();
    // for timing
#ifdef WIN32
    LARGE_INTEGER res;
    ::QueryPerformanceFrequency(&res);
    LARGE_INTEGER li,li2;
    ::QueryPerformanceCounter(&li);
#else
    struct timeval tv1,tv2;
    gettimeofday(&tv1,NULL);
#endif

    for (int i=0;i<100;++i)
    {
        p.process_event(test_sc::open_close());
        p.process_event(test_sc::open_close()); 
        p.process_event(test_sc::cd_detected());
        p.process_event(test_sc::play()); 
        for (int j=0;j<100;++j)
        {
            p.process_event(test_sc::NextSong());
            p.process_event(test_sc::NextSong());
            p.process_event(test_sc::PreviousSong());
            p.process_event(test_sc::PreviousSong());
        }

        p.process_event(test_sc::pause()); 
        // go back to Playing
        p.process_event(test_sc::end_pause()); 
        p.process_event(test_sc::pause()); 
        p.process_event(test_sc::stop());  
        // event leading to the same state
        p.process_event(test_sc::stop());
        p.process_event(test_sc::open_close());
        p.process_event(test_sc::open_close());
    }
#ifdef WIN32
    ::QueryPerformanceCounter(&li2);
#else
    gettimeofday(&tv2,NULL);
#endif
#ifdef WIN32
    std::cout << "sc took in s:" << (double)(li2.QuadPart-li.QuadPart)/res.QuadPart <<"\n" <<std::endl;
#else
    std::cout << "sc took in us:" << mtime(tv1,tv2) <<"\n" <<std::endl;
#endif
    return 0;
}

