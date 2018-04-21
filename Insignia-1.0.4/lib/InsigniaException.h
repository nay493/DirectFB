#ifndef __INSIGNIA__INSIGNIAEXCEPTION_H__
#define __INSIGNIA__INSIGNIAEXCEPTION_H__

#include "common.h"

extern "C" {
#include <direct/compiler.h>
}

#include <direct/String.h>


namespace Insignia {

     class Exception {
     public:
          Exception( const Direct::String &message );
          Exception( const char *format, ... )    D_FORMAT_PRINTF(2);
     
          string GetMessage() const;
     
          friend std::ostream &operator << (std::ostream &stream, Exception *ex);
     
     private:
          Direct::String m_message;
     };

}

#endif
