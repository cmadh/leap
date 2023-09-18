/* interop_logger_base.i file */
%module(directors="1") NodeosInteropLoggerModule

%{
  #include "interop_logger_base.hpp"
%}

//#%replace("%(camelcase)s", %$isclass) "";
//%replace("%(camelcase)s", %$isfunction) "";

//%feature("director") interop_logger_base;

%include interop_logger_base.hpp
