#include <++dfb.h>

#include <sys/wait.h>

#include <iostream>
#include <fstream>
#include <list>

#include "Insignia.h"
#include "InsigniaException.h"
#include "InsigniaTest.h"
#include "InsigniaTestManager.h"
#include "InsigniaUtil.h"


extern "C" {

#include <core/core.h>

extern IDirectFB_C *idirectfb_singleton;
}


namespace Insignia {

D_LOG_DOMAIN( Insignia_App, "Insignia/App", "Insignia App" );


IDirectFB App::m_dfb;

/***********************************************************************************************************************
** Constructor/Destructor
*/

App::App()
{
}

App::~App()
{
}

/***********************************************************************************************************************
** Initialization
*/

void
App::Init( int argc, char *argv[] )
{
     DirectFB::Init( &argc, &argv );

     App::m_argv0      = argv[0];
     App::m_argv0_size = strlen( argv[0] ) + 1;

     for (int i=1; i<argc; i++) {
          if (!strcmp( argv[i], "-h" ) || !strcmp( argv[i], "--help" )) {
               cerr << endl;
               cerr << "Insignia [options]" << endl;
               cerr << endl;
               cerr << "  Options:" << endl;
               cerr << "    -t <Test Name>             Run specified test (otherwise all)" << endl;
               cerr << "    -q                         Suppress messages" << endl;
               cerr << "    -l                         Write messages to stderr instead of log files" << endl;
               cerr << "    -m                         Don't fork master process, for debugging" << endl;
               cerr << "    -M                         Like -m, but also keeps a single DirectFB instance for all test configs" << endl;
               cerr << "    -C <name>[=<value>][,...]  Add configuration to use for running tests" << endl;
               cerr << "    -c <filename>              Load configurations from file (one per line)" << endl;
               cerr << "    -e                         Exit when a test failed" << endl;
               cerr << "    -k                         Keep success logs" << endl;
               cerr << "    -K                         Kill processes randomly" << endl;
               cerr << "    -p[n]                      Run tests parallel (up to n)" << endl;
               cerr << "    -u                         User stops tests (may not work with all modules)" << endl;
               cerr << "    -s <uint>                  Set seed for random number generator (default 0)" << endl;
               cerr << "    -d <us>                    Set duration for all tests (default 0 => defined by test)" << endl;
               cerr << "    -D <us>                    Set delay after master process finalized for each tests (default 0)" << endl;
               cerr << "    -L <loops>                 Run all tests this many times (default 1)" << endl;
               cerr << "    -o <num>                   Limit number of configs per test (default 0 = unlimited)" << endl;
               cerr << "    -S <path>                  Set sysroot (prefix to all data files)" << endl;
               cerr << "    -T <num>                   Start with test run number (default 1)" << endl;
               cerr << "" << endl;
               cerr << "  Tests:" << endl;

               const TestManager::EntryMap&          entries = TestManager::GetEntries();
               TestManager::EntryMap::const_iterator itr     = entries.begin();

               for (; itr != entries.end(); itr++) {
                    TestManager::Entry *entry = (*itr).second;

                    cerr << "    " << entry->info.name << endl;
               }

               exit( 0 );
          } else
          if (!strcmp( argv[i], "-t" )) {
               if (++i == argc)
                    throw new Exception( "Missing argument to option '-t'!" );

               m_tests.push_back( argv[i] );
          } else
          if (!strcmp( argv[i], "-q" )) {
               DirectLog *log;

               direct_log_create( DLT_FILE, "/dev/null", &log );

               direct_log_set_default( log );
          } else
          if (!strcmp( argv[i], "-l" )) {
               m_options.m_log_stderr = true;
          } else
          if (!strcmp( argv[i], "-m" )) {
               m_options.m_master_nofork = true;
          } else
          if (!strcmp( argv[i], "-M" )) {
               m_options.m_master_dfb = true;
          } else
          if (!strcmp( argv[i], "-C" )) {
               if (++i == argc)
                    throw new Exception( "Missing argument to option '-C'!" );

               Test::Config *config = new Test::Config( argv[i] );

               m_configs.push_back( config );
          } else
          if (!strcmp( argv[i], "-c" )) {
               if (++i == argc)
                    throw new Exception( "Missing argument to option '-c'!" );

               char  buf[1000];
               FILE *f = fopen( argv[i], "r" );

               if (!f)
                    throw new Exception( Direct::String::F( "Could not open '%s'!", argv[i] ) );

               while (fgets( buf, sizeof(buf), f )) {
                    if (buf[strlen(buf)-1] == '\n')
                         buf[strlen(buf)-1] = 0;

                    Test::Config *config = new Test::Config( buf );

                    m_configs.push_back( config );
               }

               fclose( f );
          } else
          if (!strcmp( argv[i], "-e" )) {
               m_options.m_exit_on_fail = true;
          } else
          if (!strcmp( argv[i], "-k" )) {
               m_options.m_keep_success_logs = true;
          } else
          if (!strcmp( argv[i], "-K" )) {
               m_options.m_kill = true;
          } else
          if (!strncmp( argv[i], "-p", 2 )) {
               if (argv[i][2]) {
                    if (sscanf( &argv[i][2], "%u", &m_options.m_parallel ) != 1)
                         throw new Exception( "Invalid argument '%s' to option '-p'!", &argv[i][2] );
               }
               else
                    m_options.m_parallel = 8;
          } else
          if (!strcmp( argv[i], "-u" )) {
               m_options.m_user_stop = true;
          } else
          if (!strcmp( argv[i], "-s" )) {
               if (++i == argc)
                    throw new Exception( "Missing argument to option '-s'!" );

               if (sscanf( argv[i], "%u", &m_options.m_seed ) != 1)
                    throw new Exception( "Invalid argument '%s' to option '-s'!", argv[i] );
          } else
          if (!strcmp( argv[i], "-d" )) {
               if (++i == argc)
                    throw new Exception( "Missing argument to option '-d'!" );

               if (sscanf( argv[i], "%u", &m_options.m_duration ) != 1)
                    throw new Exception( "Invalid argument '%s' to option '-d'!", argv[i] );
          } else
          if (!strcmp( argv[i], "-D" )) {
               if (++i == argc)
                    throw new Exception( "Missing argument to option '-D'!" );

               if (sscanf( argv[i], "%u", &m_options.m_delay ) != 1)
                    throw new Exception( "Invalid argument '%s' to option '-D'!", argv[i] );
          }
          else
          if (!strcmp( argv[i], "-L" )) {
               if (++i == argc)
                    throw new Exception( "Missing argument to option '-L'!" );

               if (sscanf( argv[i], "%u", &m_options.m_loops ) != 1)
                    throw new Exception( "Invalid argument '%s' to option '-L'!", argv[i] );
          } else
          if (!strcmp( argv[i], "-o" )) {
               if (++i == argc)
                    throw new Exception( "Missing argument to option '-o'!" );

               if (sscanf( argv[i], "%u", &m_options.m_configs ) != 1)
                    throw new Exception( "Invalid argument '%s' to option '-o'!", argv[i] );
          } else
          if (!strcmp( argv[i], "-S" )) {
               if (++i == argc)
                    throw new Exception( "Missing argument to option '-S'!" );

               m_options.m_sysroot = argv[i];
          } else
          if (!strcmp( argv[i], "-T" )) {
               if (++i == argc)
                    throw new Exception( "Missing argument to option '-T'!" );

               if (sscanf( argv[i], "%u", &m_options.m_testrun_start ) != 1 || m_options.m_testrun_start < 1)
                    throw new Exception( "Invalid argument '%s' to option '-T'!", argv[i] );
          } else
               throw new Exception( "Invalid option '%s'!", argv[i] );
     }

     srand( m_options.m_seed );
}

/***********************************************************************************************************************
** Testing
*/

class TestRun {
     static Test::Config           nullConfig;
     static vector<Test::Config *> nullConfigs;

public:
     App                    *app;
     Test                   *test;
     Test::Info              info;
     Options                 options;
     Test::Config           &main_config;
     vector<Test::Config *> &main_configs;
     size_t                  main_config_count;
     size_t                  main_config_index;
     Test::Config           &test_config;
     vector<Test::Config *> &test_configs;
     size_t                  test_config_count;
     size_t                  test_config_index;

     long long               testrun;
     Direct::String          dirname;
     Test::Result            result;
     Direct::String          status;
     Direct::String          resultstring;
     bool                    is_error;
     bool                    do_unlink;
     Direct::String          mainlogname;

     TestRun()
          :
          app( NULL ),
          test( NULL ),
          main_config( nullConfig ),
          main_configs( nullConfigs ),
          main_config_count( 0 ),
          main_config_index( 0 ),
          test_config( nullConfig ),
          test_configs( nullConfigs ),
          test_config_count( 0 ),
          test_config_index( 0 ),

          testrun( 0 ),
          result( Test::RESULT_SUCCESS ),
          is_error( false ),
          do_unlink( false )
     {
     }

     TestRun( App                    *app,
              Test                   *test,
              const Options          &options,
              vector<Test::Config *> &main_configs,
              size_t                  main_config_index,
              vector<Test::Config *> &test_configs,
              size_t                  test_config_index )
          :
          app( app ),
          test( test ),
          options( options ),
          main_config( *main_configs[main_config_index] ),
          main_configs( main_configs ),
          main_config_count( main_configs.size() ),
          main_config_index( main_config_index ),
          test_config( *test_configs[test_config_index] ),
          test_configs( test_configs ),
          test_config_count( test_configs.size() ),
          test_config_index( test_config_index ),

          testrun( 0 ),
          result( Test::RESULT_SUCCESS ),
          is_error( false ),
          do_unlink( false )
     {
          test->GetInfo( info );

          if (options.m_duration)
               test_config.duration = options.m_duration;
     }

     int Run( size_t index, size_t count, size_t errors, int session = 0 )
     {
          testrun = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC ) * 1000 + session;

          fprintf( stdout, "#\n#\n#\n#\n#\n#\n# [test run %zu/%zu - %zu errors] | "
                   "(main config %zu/%zu) - Running '%s'...  (testrun %012lld)... (config %zu/%zu)\n#\n",
                   index+1, count, errors, main_config_index+1, main_config_count, info.name.c_str(),
                   testrun, test_config_index+1, test_config_count );
          fprintf( stdout, "# Main:    %s\n", main_config.PrintOptions().c_str() );
          fprintf( stdout, "# Master:  %s\n", main_config.PrintOptionsMaster().c_str() );
          fprintf( stdout, "# Slaves:  %s\n#\n", main_config.PrintOptionsSlaves().c_str() );
          fprintf( stdout, "# Options: %s\n", test_config.PrintOptions().c_str() );
          fprintf( stdout, "# Config:  %s\n", *test_config.GetDescription() );

          dirname = Direct::String::F( "%012lld", testrun );

          int ret;

          ret = mkdir( *dirname, 0775 );
          if (ret && errno != EEXIST)
               throw new Exception( "Error while creating directory '%s'!", *dirname );


          if (!app->m_options.m_log_stderr) {
               mainlogname = Direct::String::F( "%s/%s_main_%012lld.txt", *dirname, info.name.c_str(), testrun );

               int logfd = open( *mainlogname, O_RDWR | O_APPEND | O_CREAT, 0644 );

               if (logfd < 0)
                    throw new Exception( Direct::String::F( "Creating log file '%s' failed (%s)!", *mainlogname, strerror(errno) ) );


               int errfd = fileno( stderr );

               dup2( logfd, errfd );
               close( logfd );
          }


          direct_initialize();

          setenv( "DIRECTFB_SESSION", *Direct::String::F("%d",session), 1 );

          ret = runTest( index, count, session );

          direct_shutdown();

          return ret;
     }

     inline TestRun& operator =(const TestRun &other) {
          this->app = other.app;
          this->test = other.test;
          this->options = other.options;
          this->main_config = other.main_config;
          this->main_configs = other.main_configs;
          this->main_config_count = other.main_config_count;
          this->main_config_index = other.main_config_index;
          this->test_config = other.test_config;
          this->test_configs = other.test_configs;
          this->test_config_count = other.test_config_count;
          this->test_config_index = other.test_config_index;
          this->testrun = other.testrun;
          this->dirname = other.dirname;
          this->result = other.result;
          this->status = other.status;
          this->resultstring = other.resultstring;
          this->is_error = other.is_error;
          this->do_unlink = other.do_unlink;
          return *this;
     }

private:
     int runTest( size_t index, size_t count, int session );

     void runTestInstance( Test               &test,
                           Test::Setup        &setup,
                           const Test::Config &main_config );

     void readStatus( Test         &test,
                      Test::Status *statussi );
};

Test::Config           TestRun::nullConfig;
vector<Test::Config *> TestRun::nullConfigs;

/**********************************************************************************************************************/

void
App::RunTests()
{
     /* Run all tests? */
     if (m_tests.size() == 0) {
          const TestManager::EntryMap           &entries = TestManager::GetEntries();
          TestManager::EntryMap::const_iterator  itr     = entries.begin();

          for (; itr != entries.end(); itr++) {
               TestManager::Entry *entry = (*itr).second;

               m_tests.push_back( entry->info.name );
          }
     }

     if (m_configs.empty())
          m_configs.push_back( new Test::Config() );


     vector<TestRun> test_runs;


     for (vector<string>::iterator itr = m_tests.begin(); itr != m_tests.end(); itr++) {
          Test                  *test;
          Direct::String         name = *itr;
          vector<Test::Config*> &configs = * new vector<Test::Config*>();

          if ((*itr).find( ':' ) != (*itr).npos) {
               Direct::Strings tokens = name.GetTokens( ":" );

               if (tokens.size() != 2)
                    throw new Exception( "invalid number (%zu) of tokens from '%s'", tokens.size(), *name );

               test = TestManager::LookupTest( tokens[0] );
               if (test == NULL)
                    throw new Exception( "invalid test '%s' requested", *tokens[0] );

               test->GetConfigs( configs );


               size_t index = atoi(*tokens[1]) - 1;

               D_ASSERT( index < configs.size() );

               for (size_t m=0; m<m_configs.size(); m++)
                    test_runs.push_back( TestRun( this, test, m_options, m_configs, m, configs, index ) );
          }
          else {
               test = TestManager::LookupTest( name );
               if (test == NULL)
                    throw new Exception( "invalid test '%s' requested", *name );

               test->GetConfigs( configs );

               for (size_t m=0; m<m_configs.size(); m++) {
                    if (m_options.m_configs && m_options.m_configs < configs.size()) {
                         if (m_options.m_configs == 1)
                              test_runs.push_back( TestRun( this, test, m_options, m_configs, m, configs, 0 ) );
                         else
                              for (unsigned int n=0; n<m_options.m_configs; n++)
                                   test_runs.push_back( TestRun( this, test, m_options, m_configs, m, configs,
                                                                 n * (configs.size() - 1) / (m_options.m_configs - 1) ) );
                    }
                    else
                         for (size_t n=0; n<configs.size(); n++)
                              test_runs.push_back( TestRun( this, test, m_options, m_configs, m, configs, n ) );
               }
          }
     }

     fprintf( stdout, "#\n#\n# .......................Going for %zu test runs......................\n\n", test_runs.size() );

     if (test_runs.size() == 0)
          return;



     DirectResult    ret;
     Direct::String  alllogname = Direct::String::F( "TEST_summary.txt" );
     Direct::String  errlogname = Direct::String::F( "TEST_errors.txt" );
     DirectLog      *alllog;
     DirectLog      *errlog;

     ret = log_create( DLT_FILE, *alllogname, &alllog );
     if (ret)
          throw new Exception( Direct::String::F( "Creating log file '%s' failed (%s)!",
                                                  *alllogname, DirectResultString((DirectResult)ret) ) );

     ret = log_create( DLT_FILE, *errlogname, &errlog );
     if (ret)
          throw new Exception( Direct::String::F( "Creating log file '%s' failed (%s)!",
                                                  *errlogname, DirectResultString((DirectResult)ret) ) );

     size_t errors = 0;


     if (m_options.m_master_dfb)
          App::m_dfb = DirectFB::Create();

     for (unsigned int l=0; m_options.m_loops == 0 || l < m_options.m_loops; l++) {
          fprintf( stdout, "#\n#\n# .......................... LOOP %u/%u ............................\n\n", l + 1, m_options.m_loops );

          pid_t   children[m_options.m_parallel];
          int     completed = 0;

          memset( children, 0, m_options.m_parallel * sizeof(pid_t) );

          for (size_t r=m_options.m_testrun_start-1; r<test_runs.size(); r++) {
               memset( m_argv0, 0, m_argv0_size );
               direct_snputs( m_argv0, *Direct::String::F( "Insignia {%d/%zu}", completed, m_tests.size() * m_configs.size() ), m_argv0_size );

               if (m_options.m_parallel) {
                    int session = -1;

restart:
                    for (unsigned int i=0; i<m_options.m_parallel; i++) {
                         if (!children[i]) {
                              session = i;
                              break;
                         }
                    }

                    if (session == -1) {
                         int   status;
                         pid_t pid = wait( &status );

                         if (pid < 0)
                              throw new Exception( "wait() failed (%s)!", strerror(errno) );

                         if (pid > 0) {
                              if (WIFSIGNALED( status )) {
                                   direct_log_printf( NULL, "#  -> main pid %d exited with signal %d\n", pid, WTERMSIG(status) );

                                   throw new Exception( Direct::String::F( "unexpected signal %d in main pid %d",
                                                                           WTERMSIG(status), pid ) );
                              }
                              else {
                                   direct_log_printf( NULL, "#  -> main pid %d exited with result %d\n", pid, WEXITSTATUS(status) );

                                   switch (WEXITSTATUS( status )) {
                                        case 0:
                                             break;
                                        case 1:
                                             errors++;
                                             break;
                                        default:
                                             throw new Exception( Direct::String::F( "invalid exit status %d from main pid %d",
                                                                                     WEXITSTATUS(status), pid ) );
                                   }
                              }

                              for (unsigned int i=0; i<m_options.m_parallel; i++) {
                                   if (children[i] == pid) {
                                        children[i] = 0;
                                        completed++;
                                        goto restart;
                                   }
                              }

                              D_BREAK( "child %d not found", pid );
                         }

                         goto restart;
                    }



                    int   ret;
                    pid_t pid = fork();

                    switch (pid) {
                         case -1:
                              D_PERROR( "Insignia/RunTest: fork() failed!\n" );
                              return;

                         case 0:
                              D_ASSERT( session != -1 );
                              ret = test_runs[r].Run( r, test_runs.size(), errors, session );

                              if (test_runs[r].is_error)
                                   direct_log_printf( errlog, "# %s", *test_runs[r].resultstring );

                              direct_log_printf( alllog, "# %s", *test_runs[r].resultstring );

                              _exit( ret );
                              break;

                         default:
                              children[session] = pid;
                    }
               }
               else {
                    test_runs[r].Run( r, test_runs.size(), errors );

                    if (test_runs[r].is_error) {
                         errors++;

                         direct_log_printf( errlog, "# %s", *test_runs[r].resultstring );
                    }

                    direct_log_printf( alllog, "# %s", *test_runs[r].resultstring );
               }
          }


          pid_t ret;

          do {
               int status;

               ret = wait( &status );

               if (ret > 0) {
                    if (WIFSIGNALED( status )) {
                         direct_log_printf( NULL, "#  -> main pid %d exited with signal %d\n", ret, WTERMSIG(status) );

                         throw new Exception( Direct::String::F( "unexpected signal %d in main pid %d",
                                                                 WTERMSIG(status), ret ) );
                    }
                    else {
                         direct_log_printf( NULL, "#  -> main pid %d exited with result %d\n", ret, WEXITSTATUS(status) );

                         switch (WEXITSTATUS( status )) {
                              case 0:
                                   break;
                              case 1:
                                   errors++;
                                   break;
                              default:
                                   throw new Exception( Direct::String::F( "invalid exit status %d from main pid %d",
                                                                           WEXITSTATUS(status), ret ) );
                         }
                    }


                    completed++;

                    memset( m_argv0, 0, m_argv0_size );
                    direct_snputs( m_argv0, *Direct::String::F( "Insignia {%d/%zu}", completed,
                                                                m_tests.size() * m_configs.size() ), m_argv0_size );
               }
          } while (ret > 0);
     }


     direct_log_printf( alllog, "#\n#\n# .......................Finished all runs with %zu/%zu errors......................\n\n", errors, test_runs.size() );
     direct_log_printf( errlog, "#\n#\n# .......................Finished all runs with %zu/%zu errors......................\n\n", errors, test_runs.size() );

     direct_log_flush( alllog, false );
     direct_log_destroy( alllog );

     direct_log_flush( errlog, false );
     direct_log_destroy( errlog );
}

DirectResult
App::log_create( DirectLogType     type,
                 const char       *param,
                 DirectLog       **ret_log )
{
     if (m_options.m_log_stderr)
          return direct_log_create( DLT_STDERR, NULL, ret_log );

     return direct_log_create( type, param, ret_log );
}

/***********************************************************************************************************************
** Internal
*/

int
TestRun::runTest( size_t index, size_t count, int session )
{
     DFBResult      res = DFB_OK;
     Direct::String testlogname = Direct::String::F( "%s_log.txt", info.name.c_str() );

     int testlog_fd = open( *testlogname, O_RDWR | O_CREAT | O_APPEND, 0644 );
     if (testlog_fd < 0)
          throw new Exception( Direct::String::F( "Could not open '%s'!", *testlogname ) );


     int ret;

     {
          {
               memset( App::m_argv0, 0, App::m_argv0_size );
               direct_snputs( App::m_argv0, *Direct::String::F( "%d.%012lld.%s [%zu/%zu] %zu/%zu",
                                                                session, testrun, info.name.c_str(),
                                                                main_config_index+1, main_config_count,
                                                                test_config_index+1, test_config_count ),
                              App::m_argv0_size );


               Direct::String str_head;
               Direct::String str_tail;

               str_head.PrintF( "%012lld  [%4zu/%zu]  %4zu/%zu", testrun, main_config_index+1, main_config_count, test_config_index+1, test_config_count );

               str_tail.PrintF( "  %-30s  %-20s", *test_config.GetDescription(), main_config.PrintOptions().c_str() );

               if (test_config.PrintOptions().size() > 0)
                    str_tail.PrintF( "  %-20s", test_config.PrintOptions().c_str() );


               if (!options.m_parallel) {
//                    fprintf( stdout, "# %-20s  %s  %s", info.name.c_str(), *str_head, *str_tail );
                    fprintf( stdout, "#   ===>   " );
                    fflush( stdout );
               }


               direct_log_printf( NULL, "\n#\n#\n# [test run %zu/%zu] | (main config %zu/%zu) - Running '%s' (testrun %012lld)... (config %zu/%zu)\n#\n",
                                  index+1, count, main_config_index+1, main_config_count, info.name.c_str(), testrun,
                                  test_config_index+1, test_config_count );

               direct_log_printf( NULL, "# Main:    %s\n", main_config.PrintOptions().c_str() );
               direct_log_printf( NULL, "# Master:  %s\n", main_config.PrintOptionsMaster().c_str() );
               direct_log_printf( NULL, "# Slaves:  %s\n#\n", main_config.PrintOptionsSlaves().c_str() );
               direct_log_printf( NULL, "# Options: %s\n", test_config.PrintOptions().c_str() );

               direct_log_printf( NULL, "\n#\n#\n# Spawning %u '%s' processes... \n#\n%s\n\n",
                                  test_config.num_processes, info.name.c_str(), *test_config.GetDescription() );


               Direct::String  lognames[test_config.num_processes];

               for (unsigned int np=0; np<test_config.num_processes; np++)
                    lognames[np].PrintF( "%s/%s__%d_%012lld.dfblog", *dirname, info.name.c_str(), np, testrun );


               int fds[test_config.num_processes*2+2];

               for (unsigned int np=0; np<test_config.num_processes+1; np++) {
                    //ret = pipe( &fds[np*2] );
                    ret = direct_socketpair( PF_LOCAL, SOCK_STREAM, 0, &fds[np*2] );
                    if (ret) {
                         D_DERROR( ret, "Insignia/%s(): direct_socketpair( PF_LOCAL, SOCK_STREAM, 0, fds ) failed!\n", __FUNCTION__ );
                         res = (DFBResult) ret;
                         goto out;
                    }
               }



               Test::Setup setups[test_config.num_processes];
               pid_t       pid[test_config.num_processes];

               for (unsigned int np=!!options.m_master_nofork; np<test_config.num_processes; np++) {
                    setups[np] = Test::Setup( app, test_configs, test_config_index, np, testrun, fds );

                    pid[np] = fork();

                    switch (pid[np]) {
                         case -1:
                              D_PERROR( "Insignia/runTest: fork() failed!\n" );
                              res = DFB_IO;
                              goto out;

                         case 0: {
                              if (options.m_master_dfb)
                                   App::m_dfb = NULL;

                              memset( App::m_argv0, 0, App::m_argv0_size );
                              direct_snputs( App::m_argv0, *Direct::String::F( "(%u/%u) %s",
                                                                               np+1, test_config.num_processes,
                                                                               info.name.c_str() ),
                                             App::m_argv0_size );


                              DirectLog *plog;

                              ret = app->log_create( DLT_FILE, *lognames[np], &plog );
                              if (ret)
                                   throw new Exception( Direct::String::F( "Creating log file '%s' failed (%s)!",
                                                                           *lognames[np], DirectResultString((DirectResult)ret) ) );

                              direct_log_set_default( plog );


                              test->DoSetup( setups[np] );

                              main_config.ApplyOptions( np == 0 );

                              runTestInstance( *test, setups[np], main_config );


                              if (np != 0) {
                                   Test::PacketFinalise packet;

                                   test->Post( 0, packet );
                              }

                              direct_log_printf( plog, "#  -> done, calling direct_shutdown and _exit(0)...\n" );

                              direct_shutdown();  // shutdown ref from RunTest()

                              direct_log_destroy( plog );

                              _exit( 0 );
                              D_BUG( "exit() returned" );
                              res = DFB_BUG;
                              goto out;
                         }

                         default:
                              direct_log_printf( NULL, "#  -> [%d] pid %d\n", np, pid[np] );
                              break;
                    }
               }


               if (options.m_master_nofork) {
                    setups[0] = Test::Setup( app, test_configs, test_config_index, 0, testrun, fds );


                    DirectLog *plog;

                    ret = app->log_create( DLT_FILE, *lognames[0], &plog );
                    if (ret)
                         throw new Exception( Direct::String::F( "Creating log file '%s' failed (%s)!",
                                                                 *lognames[0], DirectResultString((DirectResult)ret) ) );

                    direct_log_set_default( plog );


                    test->DoSetup( setups[0] );

                    main_config.ApplyOptions( true );

                    runTestInstance( *test, setups[0], main_config );

                    direct_log_destroy( plog );

                    direct_log_set_default( NULL );
               }


               Test::Setup setup_main( app, test_configs, test_config_index, -1, testrun, fds );

               test->DoSetup( setup_main );


               if (options.m_user_stop) {
                    char c;

                    ssize_t ret = read( fileno(stdin), &c, 1 );
                    D_ASSERT( ret == 1 );
                    (void) ret;
                    //sleep(2);

                    Test::PacketStop packet;

                    test->PostAll( packet );
               }



               Test::Status statussi[test_config.num_processes];


               direct_log_printf( NULL, "\n# Waiting for '%s' processes...\n#\n", info.name.c_str() );

               long long start           = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );
               long long timeout         = test_config.num_processes * (options.m_duration + 60000000);
               long long timeout_us      = start + timeout;
               long long timeout_us_kill = timeout_us + 360000000LL;

               for (int np=test_config.num_processes-1; np>=!!options.m_master_nofork; np--) {
                    int       status;
                    long long now = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );

                    readStatus( *test, statussi );

                    ret = waitpid( pid[np], &status, WNOHANG );
                    switch (ret) {
                         case 0:
                              if (options.m_kill) {
                                   unsigned int p = ((unsigned)rand()) % test_config.num_processes;

                                   direct_thread_sleep( ((unsigned)rand())%(100000+test_config.duration/4) );

                                   switch (rand()&3) {
                                        case 0:
                                             fprintf( stdout, "  =====> SENDING SIGNAL %d TO PROCESS %u (pid %d)\n", SIGTERM, p, pid[p] );
                                             kill( pid[p], SIGTERM );
                                             break;

                                        case 1:
                                             fprintf( stdout, "  =====> SENDING SIGNAL %d TO PROCESS %u (pid %d)\n", SIGKILL, p, pid[p] );
                                             kill( pid[p], SIGKILL );
                                             break;

                                        case 2:
                                             fprintf( stdout, "  =====> SENDING SIGNAL %d TO PROCESS %u (pid %d)\n", SIGSTOP, p, pid[p] );
                                             kill( pid[p], SIGSTOP );
                                             break;

                                        case 3:
                                             fprintf( stdout, "  =====> SENDING SIGNAL %d TO PROCESS %u (pid %d)\n", SIGCONT, p, pid[p] );
                                             kill( pid[p], SIGCONT );
                                             break;
                                   }
                              }
                              else if (now >= timeout_us_kill) {
                                   fprintf( stdout, "  =====> TIMED OUT (%lld.%03lldms) !!! SENDING SIGNAL %d TO PROCESS %u (pid %d)\n",
                                            (now - start) / 1000, (now - start) % 1000, SIGKILL, np, pid[np] );
                                   fflush( stdout );

                                   kill( pid[np], SIGKILL );

                                   timeout_us_kill = now + 1000000;
                              }
                              else if (now >= timeout_us && timeout_us >= 0) {
                                   fprintf( stdout, "  =====> TIMED OUT (%lld.%03lldms) !!! SENDING SIGNAL %d TO PROCESS %u (pid %d)\n",
                                            (now - start) / 1000, (now - start) % 1000, 30, 0, pid[0] );
                                   fflush( stdout );

                                   kill( pid[0], 30 );

                                   timeout_us += 1000000;
                              }
                              else
                                   direct_thread_sleep( 100000 );

                              /* retry */
                              ++np;
                              break;

                         case -1:
                              D_PERROR( "Insignia/runTest: waitpid() failed!\n" );
                              res = DFB_IO;
                              goto out;

                         default:
                              D_ASSUME( ret == pid[np] );
                              if (WIFSIGNALED( status )) {
                                   direct_log_printf( NULL, "#  -> [%d] pid %d exited with signal %d\n", np, pid[np], WTERMSIG(status) );

                                   if (np != 0) {
                                        Test::PacketFinalise packet;

                                        test->Post( 0, packet );
                                   }

                                   Test::Status s( Test::RESULT_SIGNAL, WTERMSIG( status ) );

                                   switch (WTERMSIG( status )) {
                                        case SIGSEGV:
                                             s.result = Test::RESULT_SEGFAULT;
                                             break;

                                        default:
                                             break;
                                   }

                                   test->PostStatusAs( s, np );
                              }
                              else {
                                   direct_log_printf( NULL, "#  -> [%d] pid %d exited with result %d\n", np, pid[np], WEXITSTATUS( status ) );

                                   if (WEXITSTATUS( status ) != 0)
                                        D_WARN( "invalid exit status %d", WEXITSTATUS( status ) );
                              }
                              break;
                    }
               }

               readStatus( *test, statussi );


               for (unsigned int np=0; np<test_config.num_processes; np++) {
                    status.PrintF( " %u:%s", np, *ToString<Test::Status>(statussi[np]) );

                    if (result < statussi[np].result) {
                         if (np > 0) {
                              if (statussi[0].result == Test::RESULT_EXCEPTION)
                                   continue;
                         }

                         result = statussi[np].result;
                    }
               }


               ToString<Test::Result> resultname( result );


               //if (result > Test::RESULT_FALLBACK)
               //     fail++;


               /*
                * Generate single line test result
                */
               Direct::String str_n = Direct::String::F( "%s  %-10s  %s%s\n", *str_head, *resultname, *str_tail, *status );


               /*
                *    => main log
                */
               direct_log_printf( NULL, "\n\n#\n# %s", *str_n );
               direct_log_flush( NULL, false );


               /*
                *    => test log
                */
               ret = write( testlog_fd, *str_n, str_n.length() );
               D_ASSERT( ret == (int) str_n.length() );


               resultstring.PrintF( "%-20s  %s", info.name.c_str(), *str_n );

               /*
                *    => stdout
                */
               if (!options.m_parallel)
                    fprintf( stdout, "%s\n", *resultname );
               else
//                    fprintf( stdout, "# %s", *resultstring );
                    fprintf( stdout, "#   ===>   %s", *resultname );

               fflush( stdout );



               is_error  = (result > Test::RESULT_FALLBACK);
               do_unlink = (!is_error && !options.m_keep_success_logs);

               if (!do_unlink) {
                    Direct::String olddirname = dirname;
                    dirname.PrintF( "_%s_%s_%zu-%zu", *resultname, info.name.c_str(), main_config_index+1, test_config_index+1 );

                    //direct_log_printf( NULL, "# rename '%s' -> '%s'\n", *olddirname, *dirname );

                    rename( *olddirname, *dirname );
               }

               if (is_error) {
                    /*
                     * Gather as much information as we can, e.g. process info, stack dumps, fusion /proc entries
                     */


                    if (options.m_exit_on_fail) {
                         direct_log_printf( NULL, "# -> pid %d exit on failure with result %d\n", direct_getpid(), result );
                         exit( result );
                    }
               }

               if (options.m_master_dfb) {
                    direct_print_interface_leaks();

                    dfb_core_dump_all( NULL, &Insignia_App, DIRECT_LOG_VERBOSE );
               }


               if (do_unlink) {
                    //direct_log_printf( NULL, "# unlink '%s'\n", *mainlogname );

                    if (mainlogname.length() > 0) {
                         ret = unlink( *mainlogname );
                         if (ret < 0)
                              D_ERROR( "Insignia: unlink( %s ) failed (%s)!\n", *mainlogname, strerror(errno) );
                    }

                    for (unsigned int np=0; np<test_config.num_processes; np++) {
                         Direct::String plog = Direct::String::F( "%s/%s__%d_%012lld.dfblog",
                                                                  *dirname, info.name.c_str(), np, testrun );

                         //direct_log_printf( NULL, "# unlink '%s'\n", *plog );

                         ret = unlink( *plog );
                         if (ret < 0)
                              D_ERROR( "Insignia: unlink( %s ) failed (%s)!\n", *plog, strerror(errno) );
                    }

                    test->unlinkExtraFiles();

                    //sync();

                    //direct_log_printf( NULL, "# rmdir '%s'\n", *dirname );

//                    do {

                    int errfd = fileno(stderr);
                    dup2( fileno(stdout), errfd );
                         ret = rmdir( *dirname );
                         if (ret < 0) {
                              D_ERROR( "Insignia: rmdir( %s ) failed (%s)!\n", *dirname, strerror(errno) );
                              //sleep(5);

//                              if (errno == ENOTEMPTY)
//                                   break;
                         }
//                    } while (ret < 0);
               }


               for (unsigned int i=0; i<test_config.num_processes*2+2; i++)
                    close( fds[i] );
          }
     }

     /*
      *    => test log
      */
     {
          Direct::String str;

//          str.PrintF( "#\n#\n# Done '%s'... (%d failures)\n#\n", info.name.c_str(), fail );

          ret = write( testlog_fd, *str, str.length() );
          D_ASSERT( ret == (int) str.length() );
     }

out:
     close( testlog_fd );

     return res ? res : is_error;
}

void
TestRun::runTestInstance( Test               &test,
                          Test::Setup        &setup,
                          const Test::Config &main_config )
{
     Test::Info info;

     test.GetInfo( info );


     DirectLog *log = direct_log_default();

     log << Direct::String::F( "\n#\n#\n# Initializing '%s' (testrun %012lld, process %u/%u)... (config %d/%zu)\n#\n",
                               info.name.c_str(), setup.testrun, setup.process_index+1, setup.config->num_processes,
                               setup.config_index+1, setup.configs->size() );

     log << Direct::String::F( "# Config:  %s\n", *setup.config->GetDescription() );
     log << Direct::String::F( "# Options: %s\n", main_config.PrintOptions().c_str() );
     log << Direct::String::F( "# Options: %s\n", setup.config->PrintOptions().c_str() );
     log << Direct::String::F( "# Master:  %s\n", setup.config->PrintOptionsMaster().c_str() );
     log << Direct::String::F( "# Slaves:  %s\n#\n", setup.config->PrintOptionsSlaves().c_str() );

     if (setup.process_index)
          DirectFBSetOption( "force-slave", NULL );
     else
          DirectFBSetOption( "no-force-slave", NULL );

     if (getenv("DIRECTFB_SESSION"))
          DirectFBSetOption( "session", getenv("DIRECTFB_SESSION") );


     Test::Status status( Test::RESULT_INITIALISE );

     try {
          setup.config->ApplyOptions( setup.process_index == 0 );


          test.PostStatus( status );

          test.Initialize( setup.process_index );



          log << Direct::String::F( "\n# Preparing '%s' (%u)...\n", info.name.c_str(), setup.process_index );

          status.result = Test::RESULT_PREPARE;
          test.PostStatus( status );

          test.Prepare( *setup.config );

          if (idirectfb_singleton)
               idirectfb_singleton->WaitIdle( idirectfb_singleton );


          log << Direct::String::F( "\n# Running '%s' (%u)...\n", info.name.c_str(), setup.process_index );

          status.result = Test::RESULT_EXCEPTION;
          test.PostStatus( status );

          Test::Result test_result = test.Run( *setup.config );


          log << Direct::String::F( "\n# Cleaning '%s' (%u)...\n", info.name.c_str(), setup.process_index );

          status.result = Test::RESULT_SHUTDOWN;
          test.PostStatus( status );

          if (setup.process_index == 0) {
               Test::PacketFinalise packet;

               test.ReceiveAllSlaves( packet );
          }

          test.Cleanup( *setup.config );


          log << Direct::String::F( "\n# Finalizing '%s' (%u)...\n", info.name.c_str(), setup.process_index );

          test.Finalize();

          log << Direct::String::F( "\n# Finalized '%s' (%u)!\n", info.name.c_str(), setup.process_index );

          direct_print_interface_leaks();

          if (setup.process_index == 0 && options.m_delay > 0)
               direct_thread_sleep( options.m_delay );

          status.result = test_result;
          test.PostStatus( status );
     }
     catch (DFBException *ex) {
          log << Direct::String::F( "\n###\n### %s: Caught DirectFB exception!\n###  === '%s' -> '%s'\n###\n",
                                    __FUNCTION__, ex->GetAction(), ex->GetResult() );

          status = Direct::String::F( "DirectFB exception: '%s' -> '%s'", ex->GetAction(), ex->GetResult() );

          test.PostStatus( status );
     }
     catch (Insignia::Exception *ex) {
          log << Direct::String::F( "\n###\n### %s: Caught Insignia exception!\n###  === '%s'\n###\n",
                                    __FUNCTION__, ex->GetMessage().c_str() );

          status = Direct::String::F( "Insignia exception: '%s'", ex->GetMessage().c_str() );

          test.PostStatus( status );
     }
}

void
TestRun::readStatus( Test         &test,
                     Test::Status *statussi )
{
     while (test.GotPacket( Test::Packet::TYPE_STATUS )) {
          Test::PacketStatus packet;

          test.Receive( packet );

          direct_log_printf( NULL, "# STATUS from %d: result %d, signal %d, message '%s'\n",
                             packet.sender, packet.status.result, packet.status.signal, packet.status.message );

          //D_INDEX_ASSERT( packet.sender, statussi );

          Test::Status &status = statussi[packet.sender];
          Test::Result  result = status.result;

          Direct::String msg( status.message );

          status = packet.status;

          if (!status.message[0])
               direct_snputs( status.message, *msg, sizeof(status.message) );

          if (status.signal) {
               switch (result) {
                    case Test::RESULT_EXCEPTION:
                         break;

                    default:
                         status.result = result;
               }
          }
     }
}

/**********************************************************************************************************************/

char *App::m_argv0;
int   App::m_argv0_size;

}

