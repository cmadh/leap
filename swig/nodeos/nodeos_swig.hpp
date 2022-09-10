#ifndef LEAP_NODEOS_SWIG_H
#define LEAP_NODEOS_SWIG_H

#include "deep_mind_swig_handler.hpp"

class NodeosSwig {
public:
    int Start(int argc, char** argv, swig_logger_base swig_logger);
};

#endif //LEAP_NODEOS_SWIG_H
