#ifndef VZENSE_API2_H
#define VZENSE_API2_H
/**
* @file picozense_api2.h
* @brief VZense API header file.
* Copyright (c) 2018-2019 V Interactive, Inc.
*/

/*! \mainpage VZense API Documentation
*
* \section intro_sec Introduction
*
* Welcome to the VZense API documentation. This documentation enables you to quickly get started in your development efforts to programmatically interact with the VZense TOF RGBD Camera (DCAM710).
*/

#include "VZense_decmtype.h"
 
#ifdef DCAM_710
#include "DCAM710/VZense_api_710.h"
#elif defined DCAM_305
#include "DCAM305/VZense_api_305.h"
#elif defined DCAM_800
#include "DCAM800/VZense_api_800.h"
#endif

#endif /* VZENSE_API2_H */
