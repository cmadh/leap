/* nodeos_swig.i file */
%module(directors="1") NodeosSwigLoggerModule

%{
  #include "swig_logger_base.hpp"
%}

%feature("director") swig_logger_base;

%include "swig_logger_base.hpp"
