/* nodeos_swig.i file */
%module(directors="1") NodeosSwigLoggerModule

%{
  #include "swig_logger_base.hpp"
%}

//#%replace("%(camelcase)s", %$isclass) "";
//%replace("%(camelcase)s", %$isfunction) "";

%feature("director") swig_logger_base;

%include swig_logger_base.hpp
