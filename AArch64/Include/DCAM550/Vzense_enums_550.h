#ifndef VZENSE_ENUMS_550_H
#define VZENSE_ENUMS_550_H

/**
 * @brief Depth range setting.\n 
 *        These set estimated ranges. Detection distances may be greater than what is listed for the given setting. \n 
 *        Precision and minimum distance for depth detection varies with longer ranges.
 */
typedef enum {
	PsUnknown = -1,
	PsNearRange = 0,               
	PsMidRange = 1,                
	PsFarRange = 2,                
	PsXNearRange = 3,              
	PsXMidRange = 4,               
	PsXFarRange = 5,               
	PsXXNearRange = 6,             
	PsXXMidRange = 7,              
	PsXXFarRange = 8               
}PsDepthRange;

/** 
 * @brief The data modes that determine the frame output from the device and the frame rate (fps). 
 */
typedef enum{
	PsDepth_30 = 0,					//!< Output both depth frames at 30 fps. The resolution of a depth frame is 640*480. 
	PsIR_30 = 1,					//!< Outputs both IR and RGB frames at 30 fps. The resolution of an IR frame is 640*480.
	PsDepthAndIR_30 = 2,			//!< Outputs both depth and IR frames at 30 fps. The resolution for both depth and IR frames is 640*480.
	PsNoCCD_30 = 4,					//!< Reserved for internal use.
	PsDepthAndIR_15 = 10,			//!< Outputs depth and IR frames at 15 fps, alternating between the two. The resolution of both depth and IR frames is 640*480.
	PsWDR_Depth = 11,               //!< WDR (Wide Dynamic Range) depth mode. Supports alternating multi-range depth frame output (e.g. Near/Far/Near/Far/Near).
	PsWDR_IR = 12,                  //!< WDR (Wide Dynamic Range) IR mode. Not currently implemented.
	PsWDR_DepthAndIR = 13,          //!< WDR (Wide Dynamic Range) Depth and IR mode. Not currently implemented.
}PsDataMode;

/**
 * @brief Camera device properties to get or set on a device.
 */
typedef enum{
	PsPropertyDataModeList = 9,		//!< Gets the data mode lists that the device support
	PsPropertyDepthRangeList = 10,	//!< Gets the depth range lists that the device support
}PsPropertyType;

/**
 * @brief Specifies the type of image frame.
 */
typedef enum{
	PsDepthFrame = 0,              //!< Depth frame with 16 bits per pixel in millimeters.
	PsIRFrame = 1,                 //!< IR frame with 16 bits per pixel.
	PsWDRDepthFrame = 9            //!< WDR depth frame with 16 bits per pixel in millimeters. This only takes effect when the data mode set to ::PsWDR_Depth.
}PsFrameType;

/**
 * @brief Specifies the type of camera sensor.
 */
typedef enum{
	PsDepthSensor = 0x01,          //!< Depth camera.
}PsSensorType;

/**
 * @brief Specifies the image pixel format.
 */
typedef enum{
	PsPixelFormatDepthMM16,        //!< Depth image pixel format, 16 bits per pixel in mm.
	PsPixelFormatGray16,           //!< IR image pixel format, 16 bits per pixel.
	PsPixelFormatGray8,            //!< Gray image pixel format, 8 bits per pixel.

}PsPixelFormat;

/**
 * @brief Return status codes for all APIs.\n 
 * 		  <code>PsRetOK = 0</code> means the API successfully completed its operation.\n 
 * 		  All other codes indicate a device, parameter, or API usage error.
 */
typedef enum
{
    PsRetOK                         =  0,   //!< The function completed successfully.
    PsRetNoDeviceConnected          = -1,   //!< There is no depth camera connected or the camera has not been connected correctly. Check the hardware connection or try unplugging and re-plugging the USB cable.
    PsRetInvalidDeviceIndex         = -2,   //!< The input device index is invalid.
    PsRetDevicePointerIsNull        = -3,   //!< The device structure pointer is null.
    PsRetInvalidFrameType           = -4,   //!< The input frame type is invalid.
    PsRetFramePointerIsNull         = -5,   //!< The output frame buffer is null.
    PsRetNoPropertyValueGet         = -6,   //!< Cannot get the value for the specified property.
    PsRetNoPropertyValueSet         = -7,   //!< Cannot set the value for the specified property.
    PsRetPropertyPointerIsNull      = -8,   //!< The input property value buffer pointer is null.
    PsRetPropertySizeNotEnough      = -9,   //!< The input property value buffer size is too small to store the specified property value.
    PsRetInvalidDepthRange          = -10,  //!< The input depth range mode is invalid.
    PsRetReadNextFrameTimeOut       = -11,  //!< Capture the next image frame time out.
    PsRetInputPointerIsNull         = -12,  //!< An input pointer parameter is null.
    PsRetCameraNotOpened            = -13,  //!< The camera has not been opened.
    PsRetInvalidCameraType          = -14,  //!< The specified type of camera is invalid.
    PsRetInvalidParams              = -15,  //!< One or more of the parameter values provided are invalid.
    PsRetCurrentVersionNotSupport   = -16,  //!< This feature is not supported in the current version.
    PsRetUpgradeImgError            = -17,  //!< There is an error in the upgrade file.
    PsRetUpgradeImgPathTooLong      = -18,  //!< Upgrade file path length greater than 260.
	PsRetUpgradeCallbackNotSet		= -19,  //!< Ps2_SetUpgradeStatusCallback is not called.
	PsRetNoAdapterConnected			= -100,	//!< There is no adapter connected
	PsRetReInitialized				= -101,	//!< The SDK has been Initialized
	PsRetNoInitialized				= -102,	//!< The SDK has bot been Initialized
	PsRetCameraOpened				= -103,	//!< The camera has been opened.
	PsRetCmdError					= -104,	//!< Set/Get cmd control error
	PsRetCmdSyncTimeOut				= -105,	//!< Set cmd ok.but time out for the sync return 
	PsRetIPNotMatch					= -106,	//!< IP is not in the same network segment

	PsRetOthers = -255,	             //!< An unknown error occurred.
}PsReturnStatus;

/**
 * @brief Specifies the number of depth ranges defined for WDR. Currently only two or three ranges are supported (e.g. Near/Far or Near/Mid/Far).
 */
typedef enum {
	PsWDRTotalRange_Two = 2,	//!< Two depth ranges.
	PsWDRTotalRange_Three = 3	//!< Three depth ranges.
}PsWDRTotalRange;

/** @brief The WDR style setting used for ::PsSetWDRStyle(). This determines if the WDR image output is a fusion from multiple ranges (e.g. Near/Far fusion)\n 
 * or an alternative output (e.g. Near/Far/Near/Far ... ).
 */ 
typedef enum {
	PsWDR_FUSION = 0,              //!< WDR image output is fused from multiple ranges.
	PsWDR_ALTERNATION = 1          //!< WDR image output alternates between depths (e.g. Near/Far/Near/Far ... ).
}PsWDRStyle;

/** @brief Stream type 
 */
typedef enum {
	PsStreamDepth = 0,             //!< Depth Stream
	PsStreamIR = 1                 //!< IR Stream
}PsStreamType;

typedef enum
{
	LinkeUNKNOWN = 0,
	LinkUSB = 1,
	LinkSocket = 2,
	LinkMIPI = 3,
}PsLinkType;

typedef enum {
	ConnectUNKNOWN,
	Unconnected,
	Connected,
	Opened,
}PsConnectStatus;

typedef enum
{
	NONE,
	DCAM305 = 305,
	DCAM500 = 500,
	CSI100	= 501,
	DCAM510 = 510,
	DCAM550U = 550,
	DCAM550P = 551,
	DCAM550E = 552,
	DCAM560 = 560,
	DCAM560CPRO = 561,
	DCAM560CLITE = 562,
	DCAM710 = 710,
	DCAM800 = 800,
	DCAM_MIPI = 801,
	DCAM800LITE = 802,
	DCAM800LITEUSB = 803,
	DCAM101 = 804,

	MAX,
}PsDeviceType;
#endif /* VZENSE_ENUMS_550_H */
