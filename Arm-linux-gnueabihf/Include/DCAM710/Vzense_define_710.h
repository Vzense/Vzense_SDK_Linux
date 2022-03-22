#ifndef VZENSE_DEFINE_710_H
#define VZENSE_DEFINE_710_H

#include "Vzense_enums_710.h"
#include "Vzense_types_710.h"

#ifdef PS_EXPORT_ON
    #ifdef _WIN32
        #define VZENSE_API_EXPORT __declspec(dllexport)
    #else
        #define VZENSE_API_EXPORT __attribute__((visibility("default")))
    #endif
#else
    #ifdef _WIN32
        #define VZENSE_API_EXPORT __declspec(dllimport)
    #else
        #define VZENSE_API_EXPORT __attribute__((visibility("default")))
    #endif
#endif

#ifdef __cplusplus
#define VZENSE_C_API_EXPORT extern "C" VZENSE_API_EXPORT
#else
#define VZENSE_C_API_EXPORT VZENSE_API_EXPORT
#define bool uint8_t
#endif

#endif /* VZENSE_DEFINE_710_H */
