#include "DFBTestSimple.h"


class DFBTestInitConfigTemplate : public Insignia::ConfigTemplate<Insignia::ConfigSimple<DFBTestInitConfigTemplate> > {
public:
     int delay_created;
     int delay_released;

     DFBTestInitConfigTemplate()
     {
          addOption( new Insignia::ConfigOptionMinMax<int>( "delay_created", delay_created, 0, 1 ) );
          addOption( new Insignia::ConfigOptionMinMax<int>( "delay_released", delay_released, 0, 1 ) );
     }
};


class DFBTestInit : public DFBTestSimple<DFBTestInitConfigTemplate>
{
     INSIGNIA_TEST_CLASS( DFBTestInit );

public:
     DFBTestInit() : DFBTestSimple( "DFBTestInit" ) {}

     virtual Result Run( const Insignia::ConfigSimple<DFBTestInitConfigTemplate>& config ) {
          while (!stopped()) {
               IDirectFB dfb = DirectFB::Create();

               if (config.delay_created)
                    direct_thread_sleep( config.delay_created );

               dfb = NULL;

               if (config.delay_released)
                    direct_thread_sleep( config.delay_released );
          }

          return RESULT_SUCCESS;
     }

     virtual void Initialize( unsigned int process_index ) {
     }

     virtual void Finalize() {
     }
};


INSIGNIA_TEST_STATICS( DFBTestInit );

