#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H


#include <unordered_map>  // TODO: better implementation e.g. robin-hood-hashing?
#include <queue>

#include "../types.h"
#include "../data_structures/graph.h"

namespace core {

    // placeholder in case we change the type of ids at some point
    typedef uint id_t;

    // Is this just OOP ideology under a facade?
    namespace {
        static id_t NEXT_ID = 0;
        static std::unordered_map<id_t, void*> g_entity_id_map;

        // TODO: This can overflow if we do a shit-ton of deletions and subsequent insertions.
        id_t generate_new_id(void) { 
            assert( NEXT_ID < UINT_MAX - 1 );  // UINT_MAX is reserved for forcing a nullptr return from the entity manager
            return NEXT_ID++; 
        }
    }

    const size_t num_entities(void) { 
        return g_entity_id_map.size(); 
    }

    template<typename P>
    P* get_entity(const id_t id) {
        if (g_entity_id_map.find(id) != g_entity_id_map.end()) {
            return (P*) g_entity_id_map[id];  // Should this be a dynamic_cast or something?
        }
        return nullptr;
    }

    // Generate an id for the entity, add it to the hashtable, and return the new id.
    template<typename P>
    id_t register_entity(const P* entity, const id_t* _id = nullptr) {
        id_t id = ( _id == nullptr ) ? generate_new_id() : *_id;
        g_entity_id_map[id] = (void*) entity;
        return id;
    }

    void remove_entity(const id_t id) {
        g_entity_id_map.erase(id);
    }

    void clear_all_entities(void) {
        g_entity_id_map.clear();
        NEXT_ID = 0;
    }

}  // end namespace


#endif