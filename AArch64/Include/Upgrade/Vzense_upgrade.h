#ifndef VZENSE_UPGRADE_H
#define VZENSE_UPGRADE_H

/**
* @file Vzense_update.h
* @brief Vzense API header file.
* Copyright (c) 2018-2019 V Interactive, Inc.
*/

/*! \mainpage Vzense API Documentation
*
* \section intro_sec Introduction
*
* Welcome to the Vzense API documentation. This documentation enables you to quickly get started in your development efforts to programmatically interact with the Vzense TOF RGBD Camera (not DCAM710 and not DCAM305).
*/

#include "Vzense_define.h"

#pragma pack (push, 1)
typedef enum
{
 	PsPropertyEnterJsonConfigMode_Uint16 = -4, //!< Sets JsonConfig Mode
	PsPropertyJsonPath_Uint16 = -5			   //!< Sets Json Path
}PsPropertyTypeForJson;

typedef enum
{
	DEVICE_NORMAL = 0x00,
	DEVICE_UPGRADE_BEGIN = 0x01,
	DEVICE_PRE_UPGRADE_IMG_COPY = 0x02,
	DEVICE_UPGRADE_IMG_CHECK_DOING = 0x03,
	DEVICE_UPGRADE_IMG_CHECK_DONE = 0x04,
	DEVICE_UPGRADE_UPGRAD_DOING = 0x05,
	DEVICE_UPGRADE_RECHECK_DOING = 0x06,
	DEVICE_UPGRADE_RECHECK_DONE = 0x07,
	DEVICE_UPGRADE_UPGRAD_DONE = 0x08,
}PsDeviceStatus;

typedef enum
{
	DEVICE_JSON_NORMAL = 0x00,
	DEVICE_UPGRADE_JSON_BEGIN = 0x01,
	DEVICE_PRE_UPGRADE_JSON_COPY = 0x02,
	DEVICE_UPGRADE_JSON_CHECK_DOING = 0x03,
	DEVICE_UPGRADE_JSON_CHECK_DONE = 0x04,
	DEVICE_UPGRADE_JSON_UPGRAD_DOING = 0x05,
	DEVICE_UPGRADE_JSON_UPGRAD_DONE = 0x06,
}PsUpgradeJsonStatus;

#pragma pack (pop)

typedef void(*PtrUpgradeStatusCallback)(int status, int params);

typedef void(*PtrUpgradeJsonStatusCallback)(const int status, const int params);


/**
* @brief 		Sets upgrade status callback function
* @param[in] 	device			The handle of the device on which to set the pulse count.
* @param[in] 	sessionIndex	The index of the session. See ::Ps2_StartStream() & ::Ps2_StopStream() api for more information.
* @param[in]	pCallback		Pointer to the callback function. See ::PtrUpgradeStatusCallback
* @return 		::PsRetOK		if the function succeeded, or one of the error values defined by ::PsReturnStatus.
*/
VZENSE_C_API_EXPORT PsReturnStatus Ps2_SetUpgradeStatusCallback(PsDeviceHandle device, uint32_t sessionIndex, PtrUpgradeStatusCallback pCallback);

/**
* @brief 		Sets upgrade status callback function
* @param[in] 	device			The handle of the device on which to set the pulse count.
* @param[in] 	sessionIndex	The index of the session. See ::Ps2_StartStream() & ::Ps2_StopStream() api for more information.
* @param[in]	pImgPath		Pointer to the path of firmware file. For DCAM560, do not use Chinese path
* @return 		::PsRetOK		if the function succeeded, or one of the error values defined by ::PsReturnStatus.
*/
VZENSE_C_API_EXPORT PsReturnStatus Ps2_StartUpgradeFirmWare(PsDeviceHandle device, uint32_t sessionIndex, char* pImgPath);

#endif /* VZENSE_UPGRADE_H */
