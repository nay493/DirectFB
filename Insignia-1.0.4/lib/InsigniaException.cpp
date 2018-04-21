#include "InsigniaException.h"


namespace Insignia {

Exception::Exception( const Direct::String &message )
{
     m_message = message;
}

Exception::Exception( const char *format, ... )
{
     va_list  args;

     D_ASSERT( format != NULL );

     va_start( args, format );

     m_message.PrintF( format, args );

     va_end( args );
}

string
Exception::GetMessage() const
{
     return m_message;
}

std::ostream &operator << (std::ostream &stream, Exception *ex)
{
     stream << ex->GetMessage();

     return stream;
}

}

