/* nodeos_interop.i file */
%module(directors="1") NodeosInteropModule

//%include interop_logger_base.i

%{
#include "nodeos_interop.hpp"
%}

/* make generated classes partial c# classes */
%typemap(csclassmodifiers) SWIGTYPE "public partial class"

/* string and vector to be able to work with string and vectors*/
%include std_vector.i
%include std_string.i

/* generate template around vector<string> - needs to placed before rename-directives */
%template(StringVector) std::vector<std::string>;

%rename("%(camelcase)s", %$isfunction, %$ispublic) "";
%rename("%(camelcase)s", %$ismember, %$ispublic) "";
%rename("%(pascalcase)s", %$isclass) "";
%rename("%(pascalcase)s", %$isenumitem) "";

/* needed to replace all integer-typedefs like uint8_t, uint16_t, .. */
%include "stdint.i"

%apply unsigned int { uint32_t }
%apply unsigned long long { uint64_t }
%apply int { int32_t }
%apply long long { int64_t }

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

/* enabling the director for interop_logger_base to allow nodeos to call overriden virtual methods
 * in C# and other languages supporting the directors-feature */
%feature("director") interop_logger_base;
%include interop_logger_base.hpp

%include nodeos_interop.hpp
