#ifndef __INSIGNIA__INSIGNIA_H__
#define __INSIGNIA__INSIGNIA_H__

#include "common.h"
#include "InsigniaTest.h"

#include <vector>


namespace Insignia {


class TestManager;

class Options {
public:
     bool           m_log_stderr;
     bool           m_master_nofork;
     bool           m_master_dfb;
     bool           m_exit_on_fail;
     bool           m_keep_success_logs;
     unsigned int   m_parallel;
     bool           m_user_stop;
     unsigned int   m_seed;
     unsigned int   m_duration;
     unsigned int   m_delay;
     bool           m_kill;
     unsigned int   m_loops;
     unsigned int   m_configs;
     Direct::String m_sysroot;
     unsigned int   m_processes;
     unsigned int   m_testrun_start;

     Options()
          :
          m_log_stderr( false ),
          m_master_nofork( false ),
          m_master_dfb( false ),
          m_exit_on_fail( false ),
          m_keep_success_logs( false ),
          m_parallel( false ),
          m_user_stop( false ),
          m_seed( 0 ),
          m_duration( 0 ),
          m_delay( 0 ),
          m_kill( false ),
          m_loops( 1 ),
          m_configs( 0 ),
          m_processes( 0 ),
          m_testrun_start( 1 )
     {
     }
};


class App {
public:
     static char           *m_argv0;
     static int             m_argv0_size;

     App();
     ~App();

     void Init( int argc, char *argv[] );

     void RunTests();

     Direct::String Data( const Direct::String &file )
     {
          return Direct::String::F( "%s%s%s", *m_options.m_sysroot ? *m_options.m_sysroot : "", DATADIR, *file );
     }

     static IDirectFB       m_dfb;

     DirectResult log_create     ( DirectLogType     type,
                                   const char       *param,
                                   DirectLog       **ret_log );

private:
     vector<string>         m_tests;
     vector<Test::Config*>  m_configs;
public:
     Options                m_options;
};


}

#endif
