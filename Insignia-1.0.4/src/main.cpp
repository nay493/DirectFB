#include <++dfb.h>

extern "C" {
#include <dirent.h>
#include <dlfcn.h>

#include <direct/messages.h>
}

#include <direct/String.h>

#include <Insignia.h>
#include <InsigniaException.h>
#include <InsigniaTestManager.h>

int
main( int argc, char *argv[] )
{
     Insignia::App  insignia;

     DIR             *dir;
     struct dirent   *entry;
     Direct::String   mod_path = MODULEDIR;
     Direct::Strings  ld_lib_paths = Direct::String( getenv("LD_LIBRARY_PATH") ?: "" ).GetTokens( ":" );

     dir = opendir( *mod_path );
     if (!dir) {
          D_PERROR( "Could not open module directory '%s'!\n", *mod_path );

          for (Direct::Strings::const_iterator it=ld_lib_paths.begin(); it!=ld_lib_paths.end(); it++) {
               mod_path = Direct::String::F( "%s/Insignia", **it );

               D_INFO( "Trying %s\n", *mod_path );

               dir = opendir( *mod_path );
               if (dir)
                    break;

               D_PERROR( "Could not open module directory '%s'!\n", *mod_path );
          }

          if (!dir)
               return -1;
     }

     while ((entry = readdir( dir )) != NULL) {
          char buf[500];
          int  len = strlen(entry->d_name);

          if (len < 4 || entry->d_name[len-1] != 'o' || entry->d_name[len-2] != 'I' || entry->d_name[len-3] != '.')
               continue;

          snprintf( buf, sizeof(buf), "%s/%s", *mod_path, entry->d_name );

          if (!dlopen( buf, RTLD_NOW ))
               D_DLERROR( "Could not load module '%s'!\n", buf );
     }

     closedir( dir );

     try {
          insignia.Init( argc, argv );
          insignia.RunTests();
     }
     catch (DFBException *ex) {
          cerr << endl;
          cerr << "Caught DirectFB exception!" << endl;
          cerr << "  -- " << ex << endl;
     }
     catch (Insignia::Exception *ex) {
          cerr << endl;
          cerr << "Caught Insignia exception!" << endl;
          cerr << "  -- " << ex << endl;
     }

     return -1;
}

