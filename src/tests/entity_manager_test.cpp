#ifndef ENTITY_MANAGER_TEST
#define ENTITY_MANAGER_TEST


#include <stdio.h>

#include "../types.h"
#include "../data_structures/graph.h"
#include "../core/entity_manager.h"

int entity_manager_test(void) {
    // Create some dummy entities and add them to the id manager.
    core::id_t id1 = graph::create_node();  // NOTE: create_node() automatically registers to manager.
    core::id_t id2 = graph::create_node();
    int* e1 = new int[10];
    core::id_t id3 = core::register_entity(e1);
    // Make sure that distinct entities are not assigned the same id.
    assert( id1 != id2 );
    assert( id1 != id3 );
    assert( id2 != id3 );

    // Make sure the same memory block we put in gets returned.
    int* ent3 = core::get_entity<int>(id3);
    assert( ent3 == e1 );

    // Make sure that requesting a non-existent entity returns nullptr.
    auto ent4 = core::get_entity<graph::Node>( core::num_entities() );
    assert( ent4 == nullptr );

    // Make sure that removing an entity causes nullptr return.
    graph::destroy_node(id1);  // NOTE: remove_entity does not free the pointer's memory block.
    core::remove_entity(id1);
    auto ent1 = core::get_entity<graph::Node>(id1);
    assert( ent1 == nullptr );

    // Clear everything.
    graph::destroy_node(id2);
    delete[] e1;
    core::clear_all_entities();

    return 0;
}


#endif