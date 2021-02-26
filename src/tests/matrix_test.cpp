#ifndef MATRIX_TEST
#define MATRIX_TEST

#include <iostream>

#include "../data_structures/matrix.h"

// template<typename T>
// void print_matrix(const Matrix<T>& mat) {
//     for (size_t r = 0; r < mat.rows; ++r) {
//         for (size_t c = 0; c < mat.cols; ++c) {
//             std::cout << mat[r][c] << " ";
//         }
//         std::cout << std::endl;
//     }
// }

template<typename T>
bool is_equal(const Matrix<T>& m1, const Matrix<T>& m2) {
    bool eq = (m1.shape == m2.shape);
    if (eq) {
        for (size_t r = 0; r < m1.rows; ++r) {
            for (size_t c = 0; c < m1.cols; ++c) {
                eq &= (m1[r][c] == m2[r][c]);
            }
        }
    }
    return eq;
}

int matrix_test(void) {
    Matrix<int> mat1 { {1, 2, 3}, {4, 5, 6} };
    assert( mat1.rows == 2 && mat1.cols == 3 );
    assert( mat1.shape.first == 2 && mat1.shape.second == 3 );

    // Test copy constructor.
    Matrix<int> mat2(mat1);
    assert( is_equal(mat1, mat2) );

    // Test copy assignment operator and subtraction
    Matrix<int> mat3 { {6, 5, 4}, {3, 2, 1} };
    Matrix<int> mat4 = mat3 - mat2;
    Matrix<int> check { {5, 3, 1}, {-1, -3, -5} };
    assert( is_equal(mat4, check) );

    // Test addition.
    Matrix<int> mat5 = mat4 + mat4;
    check = Matrix<int> { {10, 6, 2}, {-2, -6, -10} };
    assert( is_equal(mat5, check) );

    return 0;
}


#endif