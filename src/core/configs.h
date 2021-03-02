#ifndef CONFIGS_H
#define CONFIGS_H

#include <iostream>
#include <fstream>

#include "../types.h"

namespace core {

    static config GLOBAL_CONFIG;

    void load_config(config& cfg, char* config_path) {
        std::ifstream ifs(config_path);
        ifs >> cfg;
        ifs.close();
    }

    // TODO: Move hardcoded file path to a constants file.
    void init_global_config(char* config_path = "../../data/configs/config.json") {
        load_config(GLOBAL_CONFIG, config_path);
    }

    // Update a parameter using the specified config.
    // By default, will update the value using GLOBAL_CONFIG values.
    template<typename T>
    bool load_param(T& var, char* var_name, config* cfg = nullptr) {
        cfg = cfg ? cfg : &GLOBAL_CONFIG;
        if ( cfg->find(var_name) != cfg->end() ) {
            var = (*cfg)[var_name].get<T>();
            return true;
        }
        return false;
    }

}  // namespace


#endif