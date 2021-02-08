#ifndef TYPES_H
#define TYPES_H


#include <tuple>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

typedef unsigned int uint;

enum class Model {
    Voter,
    Sznajd
};

namespace core {
    // placeholder in case we change the type of ids at some point
    typedef uint id_t;
    typedef json config;
}

namespace graph {
    typedef struct graph Graph;
    typedef struct node Node;
    typedef std::pair<core::id_t, core::id_t> edge_t;
}


#endif