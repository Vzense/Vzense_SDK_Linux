#ifndef VZENSE_TYPES_H
#define VZENSE_TYPES_H

#include <stdint.h>
#include "VZense_enums.h"
#ifdef DCAM_710
#include "DCAM710/VZense_types_710.h"
#elif defined DCAM_305
#include "DCAM305/VZense_types_305.h"
#elif defined DCAM_800
#include "DCAM800/VZense_types_800.h"
#endif

#endif /* VZENSE_TYPES_H */
