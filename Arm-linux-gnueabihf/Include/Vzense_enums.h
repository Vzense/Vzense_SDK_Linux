#ifndef VZENSE_ENUMS_H
#define VZENSE_ENUMS_H

#include "Vzense_decmtype.h"

#ifdef DCAM_710
#include "DCAM710/Vzense_enums_710.h"
#elif defined DCAM_305
#include "DCAM305/Vzense_enums_305.h"
#elif defined DCAM_550
#include "DCAM550/Vzense_enums_550.h"
#elif defined DCAM_560
#include "DCAM560/Vzense_enums_560.h"
#endif

#endif /* VZENSE_ENUMS_H */
