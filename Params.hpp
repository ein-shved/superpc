#ifndef _PARAMS_HPP_
#define _PARAMS_HPP_

#include "Hole.hpp"
#include "Manifest.hpp"
#include <string>

class Params {
public:
    virtual const Hole *hole() const = 0;
    virtual const EdgeCondition *edge() const = 0;

    static const Params *get(const std::string &name);

};


#endif /* Params.hpp */
