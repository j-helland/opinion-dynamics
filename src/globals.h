#ifndef GLOBALS_H
#define GLOBALS_H


#include "types.h"

// Rendering
extern uint g_fps_cap;
extern float g_frame_delta_correction;
namespace graphics {
    extern GLsizei g_scr_width;
    extern GLsizei g_scr_height;
}

// Dynamics simulation
extern Model g_model;
extern uint g_dynamics_updates_per_second;
extern uint g_graph_test_size;
extern float g_graph_connectivity;


#endif