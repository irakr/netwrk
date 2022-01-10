#pragma once

#include "netwrk/error.h"

#define NK_MAX(a, b)                \
    (                               \
        {                           \
            __typeof__(a) _a = (a); \
            __typeof__(b) _b = (b); \
            _a > _b ? _a : _b;      \
        })

#define NK_MIN(a, b)                \
    (                               \
        {                           \
            __typeof__(a) _a = (a); \
            __typeof__(b) _b = (b); \
            _a <= _b ? _a : _b;      \
        })

