#ifndef __DFBTEST_RENDER___H__
#define __DFBTEST_RENDER___H__


#include <++dfb.h>

#include <InsigniaConfig.h>
#include <InsigniaTest.h>


template <typename _ConfigTemplate>
class DFBTestSimple : public Insignia::Test
{
protected:
     IDirectFB       dfb;
     Direct::String  name;
     _ConfigTemplate config_template;

     DFBTestSimple( const Direct::String &name )
          :
          name( name )
     {
     }

public:
     virtual void GetConfigs( vector<Config*> &configs ) {
          config_template.GetConfigs( configs );
     }

     virtual void GetInfo( Info &info ) {
          info.name = name.string();
     }

     virtual void Initialize( unsigned int process_index ) {
          dfb = DirectFB::Create();
     }

     virtual Result Run( const Config& config ) {
          return Run( (const Insignia::ConfigSimple<_ConfigTemplate>&) config );
     }

     virtual void Finalize() {
          dfb = NULL;
     }


protected:
     virtual Result Run( const Insignia::ConfigSimple<_ConfigTemplate>& config ) = 0;
};


#endif

