#ifndef LEAP_NODEOS_SWIG_H
#define LEAP_NODEOS_SWIG_H

#include "deep_mind_swig_handler.hpp"

class NodeosSwig {
public:
    void StartNodeos(int argc, std::vector<std::string> args, swig_logger_base *swig_logger);
    int Start(int argc, std::vector<std::string> argv, swig_logger_base *swig_logger);
};

#endif //LEAP_NODEOS_SWIG_H
