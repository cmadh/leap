/* nodeos_swig.i file */
%module(directors="1") NodeosSwigModule

//%include swig_logger_base.i

%{
#include "nodeos_swig.hpp"
%}

//%rename("%(camelcase)s", %$isclass) "";
//%rename("%(camelcase)s", %$isfunction, %$ispublic) "";
//%rename("%(lowercamelcase)s", %$isfunction, %$isprivate) "";
//%rename("%(lowercamelcase)s", %$isvariable) "";

/* string and vector to be able to work with string and vectors*/
%include std_vector.i
%include std_string.i

/* needed to replace all integer-typedefs like uint8_t, uint16_t, .. */
%include "stdint.i"

/* Create some functions for working with pointers, replaces pointers with values where possible*/
%include "cpointer.i"
%pointer_functions(int, p_int);
%pointer_functions(long long, p_long_long);
%pointer_functions(short, p_short);
%pointer_functions(signed char, p_signed_char);
%pointer_functions(unsigned int, p_uint);
%pointer_functions(unsigned char, p_unsigned_char);
%pointer_functions(unsigned int, p_unsigned_int);
%pointer_functions(unsigned long long, p_unsigned_long_long);
%pointer_functions(unsigned short, p_unsigned_short);

/* enabling the director for swig_logger_base to allow nodeos to call overriden virtual methods in C# */
%feature("director") swig_logger_base;
%include swig_logger_base.hpp

/* make used classes partial c# classes */
%typemap(csclassmodifiers) SWIGTYPE "public partial class"

/* generate template around vector<string> */
%template(StringVector) std::vector<std::string>;

%include nodeos_swig.hpp
