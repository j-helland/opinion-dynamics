#ifndef MATRIX_H
#define MATRIX_H


#include <vector>
#include <tuple>

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
            values[i] = blocks + i * cols;
        }
    }

    // Sometimes more convenient to initialize via shape.
    Matrix(std::pair<size_t, size_t> shape)
        : rows{shape.first}, 
          cols{shape.second}, 
          shape{shape} 
    {
        values.resize(rows);
        blocks = (T*) malloc(sizeof(T) * rows * cols);  
        for (size_t i = 0; i < rows; ++i) {
            values[i] = blocks + i * cols;
        }
    }

    // Initializer list constructor.
    Matrix(std::initializer_list<std::initializer_list<T>> init_list) 
        : rows{ init_list.size() }, 
          cols{ init_list.begin()->size() }, 
          shape{ std::make_pair(rows, cols) } 
    {
        values.resize(rows);
        blocks = (T*) malloc(sizeof(T) * rows * cols);  

        for (size_t m = 0; m < rows; ++m) {
            values[m] = blocks + m * cols;
            for (size_t n = 0; n < cols; ++n) {
                values[m][n] = (init_list.begin() + m)->begin()[n];
            }
        }
    }

    // Copy constructor.
    Matrix(const Matrix<T>& mat) 
        : rows{mat.rows}, 
          cols{mat.cols}, 
          shape{mat.shape} \
    {
        assert( shape == mat.shape );
        values.resize(rows);
        blocks = (T*) malloc(sizeof(T) * rows * cols);  

        memcpy(blocks, mat.blocks, sizeof(T) * rows * cols);
        for (size_t m = 0; m < rows; ++m) {
            values[m] = blocks + m * cols;
        }
    }

    ~Matrix() { free(blocks); }

    T* operator[](const size_t i) const { return values[i]; }

    // Copy assignment.
    Matrix<T>& operator=(const Matrix<T>& mat) {
        assert( shape == mat.shape );
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                values[r][c] = mat[r][c];
            }
        }
        return *this;
    }

    bool operator==(const Matrix<T>& mat) const {
        assert( shape == mat.shape );
        Matrix<bool> idx(shape);
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                idx[r][c] = (values[r][c] == mat[r][c]);
            }
        }
        return idx;
    }

    bool operator>(const Matrix<T>& mat) const {
        assert( shape == mat.shape );
        Matrix<bool> idx(shape);
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                idx[r][c] = (values[r][c] > mat[r][c]);
            }
        }
        return idx;
    }

    bool operator>=(const Matrix<T>& mat) const {
        assert( shape == mat.shape );
        Matrix<bool> idx(shape);
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                idx[r][c] = (values[r][c] >= mat[r][c]);
            }
        }
        return idx;
    }

    Matrix<T> operator+(const Matrix<T>& mat) const {
        assert( shape == mat.shape );
        Matrix<T> result(*this);
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                result[r][c] += mat[r][c];
            }
        }
        return result;
    }

    Matrix<T> operator-(const Matrix<T>& mat) const {
        assert( shape == mat.shape );
        Matrix<T> result(*this);
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                result[r][c] -= mat[r][c];
            }
        }
        return result;
    }
};


#endif