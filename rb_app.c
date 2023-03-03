
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_time.h"
#include "sokol_glue.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "sokol_imgui.h"

extern void rbFrame(const float width, const float height);
extern const char* rbGetWindowTitle();
extern ImVec2 rbGetWindowSize();
extern ImVec4 rbGetBackgroundColor();

static struct {
    uint64_t laptime;
    sg_pass_action pass_action;
} state;

static void init(void) {
    sg_setup(&(sg_desc){
        .context = sapp_sgcontext()
    });
    stm_setup();
    simgui_setup(&(simgui_desc_t){ 0 });

    ImVec4 color = rbGetBackgroundColor();

    // initial clear color
    state.pass_action = (sg_pass_action) {
        .colors[0] = { .action = SG_ACTION_CLEAR,  .val = {color.x, color.y, color.z, color.w} }
    };
}

static void frame() {
    const int width = sapp_width();
    const int height = sapp_height();
    const double delta_time = stm_sec(stm_round_to_common_refresh_rate(stm_laptime(&state.laptime)));
    simgui_new_frame(width, height, delta_time);

    // Call app for rendering
    rbFrame(width, height);
        
    sg_begin_default_pass(&state.pass_action, width, height);
    simgui_render();
    sg_end_pass();
    sg_commit();
}

static void cleanup(void) {
    simgui_shutdown();
    sg_shutdown();
}

static void event(const sapp_event* ev) {
    simgui_handle_event(ev);
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    ImVec2 size = rbGetWindowSize();

    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .window_title = rbGetWindowTitle(),
        .width = size.x,
        .height = size.y,
    };
}
