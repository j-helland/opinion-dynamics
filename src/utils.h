#ifndef UTILS_H
#define UTILS_H


#include <fstream>

namespace utils {

    bool is_file(char* file_path) {
        std::ifstream f(file_path);
        return f.good();
    }

} // end namespace


#endif