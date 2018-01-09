#ifndef _LOG_VIEWER__H_
#define _LOG_VIEWER__H_

#include <memory.hpp>

struct log_viewer {
    log_allocator *la;
};

void draw_log_viewer(log_allocator* la, const char* title);

#endif
