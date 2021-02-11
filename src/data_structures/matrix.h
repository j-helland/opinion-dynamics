#ifndef MATRIX_H
#define MATRIX_H


#include <vector>
#include <tuple>

#include "../types.h"


template<typename T>
struct Matrix {
    const size_t rows, cols;
    const std::pair<size_t, size_t> shape;
    std::vector<T*> values;
    T* blocks;

    Matrix(size_t m, size_t n) : rows{m}, cols{n}, shape{m, n} {
        values.resize(rows);
        blocks = (T*) malloc(sizeof(T) * rows * cols);  
        for (size_t i = 0; i < rows; ++i) {
            values[i] = blocks + i * rows;
        }
    }

    Matrix(std::initializer_list<std::initializer_list<T>> init_list) 
        : cols{ init_list.begin()->size() }, 
          rows{ init_list.size() }, 
          shape{ std::make_pair(rows, cols) } {

        values.resize(rows);
        blocks = (T*) malloc(sizeof(T) * rows * cols);  

        for (size_t m = 0; m < rows; ++m) {
            values[m] = blocks + m * rows;
            for (size_t n = 0; n < cols; ++n) {
                values[m][n] = (init_list.begin() + m)->begin()[n];
            }
        }
    }

    ~Matrix() {
        free(blocks);
    }

    T* operator[](const uint i) {
        return values[i];
    }
};


#endif