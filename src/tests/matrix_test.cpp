#ifndef MATRIX_TEST
#define MATRIX_TEST

#include <iostream>

#include "../data_structures/matrix.h"

int matrix_test(void) {
    Matrix<float> mat { {1, 2, 3}, {4, 5, 6} };
    assert( mat.shape.first == 2 && mat.shape.second == 3 );
    assert( mat[0][0] == 1.f );

    return 0;
}


#endif