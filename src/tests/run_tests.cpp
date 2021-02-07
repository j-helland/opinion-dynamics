#ifndef RUN_TESTS
#define RUN_TESTS


#include <stdlib.h>

#include "entity_manager_test.cpp"
#include "graph_test.cpp"
#include "graph_serialization_test.cpp"
#include "graph_deserialization_test.cpp"
#include "voter_model_test.cpp"
#include "sznajd_model_test.cpp "

void print_status(const char* name, int return_code) {
    printf("%s.cpp\t", name);
    if (return_code == 0) {
        printf("[OK]");
    } else {
        printf("[FAILED] %i", return_code);
    }
    printf("\n");
}

int main() {
    print_status("entity_manager_test", entity_manager_test());
    print_status("graph_test", graph_test());
    print_status("graph_serialization_test", graph_serialization_test());
    print_status("graph_deserialization_test", graph_deserialization_test());  // NOTE: must be run after serialization test
    print_status("voter_model_test", voter_model_test());
    print_status("sznajd_model_test", sznajd_model_test());

    return 0;
}


#endif