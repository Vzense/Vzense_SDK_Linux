#ifndef VZENSE_ENUMS_H
#define VZENSE_ENUMS_H

#include "VZense_decmtype.h"

#ifdef DCAM_710
#include "DCAM710/VZense_enums_710.h"
#elif defined DCAM_305
#include "DCAM305/VZense_enums_305.h"
#elif defined DCAM_800
#include "DCAM800/VZense_enums_800.h"
#endif

#endif /* VZENSE_ENUMS_H */
