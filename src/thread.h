#pragma once

#include <stddef.h>

void thread__runner(size_t instruction_size, size_t num_of_threads,
                    void (*thread_fx)(size_t));
