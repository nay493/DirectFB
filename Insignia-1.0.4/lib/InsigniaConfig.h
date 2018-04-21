#ifndef __INSIGNIA__CONFIG_H__
#define __INSIGNIA__CONFIG_H__

#include "common.h"

#include <list>
#include <map>
#include <vector>

#include "InsigniaTest.h"


namespace Insignia {


template <typename _Type>
class ConfigTypeAll : public std::vector<_Type>
{
public:
     ConfigTypeAll( const _Type &min,
                    const _Type &max )
     {
          for (_Type it = min; it <= max; it++)
               this->push_back( it );   // for some strange reason we need to use this-> here
     }

     ConfigTypeAll( const _Type &min,
                    const _Type &max,
                    const _Type &mod )
     {
          this->push_back( min );

          for (_Type it = min+1; it <= max-1; it++) {
               if (it % mod == 0)
                    this->push_back( it );   // for some strange reason we need to use this-> here
          }

          this->push_back( max );
     }
};



class ConfigOptionBase {
protected:
     ConfigOptionBase() {}

public:
     virtual ~ConfigOptionBase() {}

     virtual size_t count() const = 0;
     virtual void   choose( size_t index ) = 0;
     virtual void   describe( Direct::String &string ) = 0;
};


template <typename _Type>
class ConfigOption : public ConfigOptionBase {
protected:
     Direct::String  name;
     _Type          &var;

     ConfigOption( const Direct::String &name, _Type &var )
          :
          name( name ),
          var( var )
     {
     }

     virtual size_t count() const {
          return 1;
     }

     virtual void describe( Direct::String &string )
     {
          string.PrintF( " %s:%s", *name, *ToString<_Type>( var ) );
     }

     virtual void set( const _Type &val ) {
          var = val;
     }
};


template <typename _Type>
class ConfigOptionMinMax : public ConfigOption<_Type> {
protected:
     _Type min;
     _Type max;
     ConfigTypeAll<_Type> all;

public:
     ConfigOptionMinMax( const Direct::String &name, _Type &var, const _Type &min, const _Type &max )
          :
          ConfigOption<_Type>( name, var ),
          min( min ),
          max( max ),
          all( min, max )
     {
     }

     ConfigOptionMinMax( const Direct::String &name, _Type &var, const _Type &min, const _Type &max, const _Type &mod )
          :
          ConfigOption<_Type>( name, var ),
          min( min ),
          max( max ),
          all( min, max, mod )
     {
     }

     virtual size_t count() const {
          return all.size();
     }

     virtual void choose( size_t index ) {
          ConfigOption<_Type>::set( all[index] );
     }
};



class ConfigTemplateBase {
protected:
     std::vector<ConfigOptionBase*>    options;

     ConfigTemplateBase() {}

     virtual ~ConfigTemplateBase() {
          for (std::vector<ConfigOptionBase*>::const_iterator it=options.begin(); it!=options.end(); it++)
               delete *it;
     }


     void addOption( ConfigOptionBase *option ) {
          options.push_back( option );
     }

     virtual Test::Config *newConfig() = 0;


public:
     virtual void DescribeOptions(Direct::String &string) const
     {
          for (std::vector<ConfigOptionBase*>::const_iterator it=options.begin(); it!=options.end(); it++)
               (*it)->describe( string );
     }

     virtual void    GetConfigs( vector<Test::Config*> &configs );

private:
     vector<Test::Config*> getConfigs( unsigned int option_index = 0 );
};


template <typename _Config>
class ConfigTemplate : public ConfigTemplateBase {
protected:
     ConfigTemplate() {}

     virtual ~ConfigTemplate() {}

     virtual Test::Config *newConfig() {
          return new _Config();
     }
};


template <typename _ConfigTemplate>
class ConfigSimple : public Test::Config, public _ConfigTemplate {
public:
     /*
      * Description
      */
     virtual void Describe(Direct::String &string) const
     {
          Config::Describe( string );

          dynamic_cast<const ConfigTemplateBase*>(this)->DescribeOptions( string );
     }
};








template <>
class ConfigTypeAll<DFBDimension> : public std::vector<DFBDimension>
{
public:
     ConfigTypeAll( const DFBDimension &min,
                    const DFBDimension &max )
     {
          DFBDimension d = min;

          push_back( d );

          while (d.w != max.w || d.h != max.h) {
               if (d.w > 0)
                    d.w = d.w * 3 / 2 + 1;
               else
                    d.w++;

               if (d.w > max.w)
                    d.w = max.w;


               if (d.h > 0)
                    d.h = d.h * 3 / 2 + 1;
               else
                    d.h++;

               if (d.h > max.h)
                    d.h = max.h;


               push_back( d );
          }
     }
};


template <>
class ConfigTypeAll<int> : public std::vector<int>
{
public:
     ConfigTypeAll( const int &min,
                    const int &max )
     {
          for (int i=min; i<=max; i++)
               push_back( i );
     }

     ConfigTypeAll( const int &min,
                    const int &max,
                    const int &mod )
     {
          push_back( min );

          for (int i=min+1; i<=max-1; i+=mod)
               push_back( i );

          push_back( max );
     }
};



}


#endif

