#ifndef RANDOM
#define RANDOM


#include <random>

namespace rng {

    static std::random_device random_device;
    extern std::default_random_engine generator(random_device());

}


#endif
