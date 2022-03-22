//Note:use DCAM_305 in Vzense_dcamtype.h

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "DCAM305/Vzense_api_305.h"
#include <thread>

using namespace std;
using namespace cv;

void HotPlugStateCallback(const PsDeviceInfo* pInfo, int status);

static void Opencv_Depth(uint32_t slope, int height, int width, uint8_t*pData, cv::Mat& dispImg)
{
	dispImg = cv::Mat(height, width, CV_16UC1, pData);
	Point2d pointxy(width / 2, height / 2);
	int val = dispImg.at<ushort>(pointxy);
	char text[20];
	snprintf(text, sizeof(text), "%d", val);
	dispImg.convertTo(dispImg, CV_8U, 255.0 / slope);
	applyColorMap(dispImg, dispImg, cv::COLORMAP_RAINBOW);
	int color;
	if (val > 2500)
		color = 0;
	else
		color = 4096;
	circle(dispImg, pointxy, 4, Scalar(color, color, color), -1, 8, 0);
	putText(dispImg, text, pointxy, FONT_HERSHEY_DUPLEX, 2, Scalar(color, color, color));
}

int main(int argc, char *argv[])
{
	PsReturnStatus status;
	uint32_t deviceIndex = 0;
	uint32_t deviceCount = 0;
	uint32_t slope = 1450;
	uint32_t wdrSlope = 4400;
	PsDepthRange depthRange = PsNearRange;
	PsDataMode dataMode = PsDepthAndIR15_RGB30;
 
	status = Ps2_Initialize();
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "PsInitialize failed!" << endl;
		system("pause");
		return -1;
	}
GET:
	status = Ps2_GetDeviceCount(&deviceCount);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "PsGetDeviceCount failed!" << endl; 
		system("pause");
		return -1;
	}
	cout << "Get device count: " << deviceCount << endl;
	if (0 == deviceCount)
	{
		this_thread::sleep_for(chrono::seconds(1));
		goto GET;
	}
    Ps2_SetHotPlugStatusCallback(HotPlugStateCallback);

	PsDeviceInfo* pDeviceListInfo = new PsDeviceInfo[deviceCount];
	status = Ps2_GetDeviceListInfo(pDeviceListInfo, deviceCount);
	PsDeviceHandle deviceHandle = 0;
	status = Ps2_OpenDevice(pDeviceListInfo->uri, &deviceHandle);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "OpenDevice failed!" << endl;
		system("pause");
		return -1;
	}
	uint32_t sessionIndex = 0;

	status = Ps2_StartStream(deviceHandle, sessionIndex);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "StartStream failed!" << endl;
		system("pause");
		return -1;
	}

	status = Ps2_SetDataMode(deviceHandle, sessionIndex, dataMode);
	if (status != PsReturnStatus::PsRetOK)
		cout << "Ps2_SetDataMode failed!" << endl;
	else
		cout << "Set Data Mode to PsDepthAndIR_30" << endl;

	//Enable the Depth and RGB synchronize feature
	Ps2_SetSynchronizeEnabled(deviceHandle, sessionIndex, true);
 
	PsCameraParameters cameraParameters;
	status = Ps2_GetCameraParameters(deviceHandle, sessionIndex, PsDepthSensor, &cameraParameters);

	cout << "Get PsGetCameraParameters status: " << status << endl;
	cout << "Depth Camera Intinsic: " << endl;
	cout << "Fx: " << cameraParameters.fx << endl;
	cout << "Cx: " << cameraParameters.cx << endl;
	cout << "Fy: " << cameraParameters.fy << endl;
	cout << "Cy: " << cameraParameters.cy << endl;
	cout << "Depth Distortion Coefficient: " << endl;
	cout << "K1: " << cameraParameters.k1 << endl;
	cout << "K2: " << cameraParameters.k2 << endl;
	cout << "P1: " << cameraParameters.p1 << endl;
	cout << "P2: " << cameraParameters.p2 << endl;
	cout << "K3: " << cameraParameters.k3 << endl;
	cout << "K4: " << cameraParameters.k4 << endl;
	cout << "K5: " << cameraParameters.k5 << endl;
	cout << "K6: " << cameraParameters.k6 << endl;


	PsCameraExtrinsicParameters CameraExtrinsicParameters;
	status = Ps2_GetCameraExtrinsicParameters(deviceHandle, sessionIndex, &CameraExtrinsicParameters);

	cout << "Get PsGetCameraExtrinsicParameters status: " << status << endl;
	cout << "Camera rotation: " << endl;
	cout << CameraExtrinsicParameters.rotation[0] << " "
		<< CameraExtrinsicParameters.rotation[1] << " "
		<< CameraExtrinsicParameters.rotation[2] << " "
		<< CameraExtrinsicParameters.rotation[3] << " "
		<< CameraExtrinsicParameters.rotation[4] << " "
		<< CameraExtrinsicParameters.rotation[5] << " "
		<< CameraExtrinsicParameters.rotation[6] << " "
		<< CameraExtrinsicParameters.rotation[7] << " "
		<< CameraExtrinsicParameters.rotation[8] << " "
		<< endl;

	cout << "Camera transfer: " << endl;
	cout << CameraExtrinsicParameters.translation[0] << " "
		<< CameraExtrinsicParameters.translation[1] << " "
		<< CameraExtrinsicParameters.translation[2] << " " << endl;

	//Get MeasuringRange
	PsMeasuringRange measuringrange = { 0 };

	status = Ps2_GetMeasuringRange(deviceHandle, sessionIndex, depthRange, &measuringrange);
	if (status != PsReturnStatus::PsRetOK)
		cout << "Ps2_GetMeasuringRange failed!" << endl;
	else
	{
		switch (depthRange)
		{
		case PsNearRange:
		case PsXNearRange:
		case PsXXNearRange:
			slope = measuringrange.effectDepthMaxNear;
			break;

		case PsMidRange:
		case PsXMidRange:
		case PsXXMidRange:
			slope = measuringrange.effectDepthMaxMid;
			break;

		case PsFarRange:
		case PsXFarRange:
		case PsXXFarRange:

			slope = measuringrange.effectDepthMaxFar;
			break;
		default:
			break;
		}
		cout << "slope  ==  " << slope << endl;
	}
	cv::Mat imageMat;
	const string irImageWindow = "IR Image";
	const string depthImageWindow = "Depth Image";
	const string wdrDepthImageWindow = "WDR Depth Image";
 
	ofstream PointCloudWriter;
	PsDepthVector3 DepthVector = { 0, 0, 0 };
	PsVector3f WorldVector = { 0.0f };

	bool f_bSync = false;

	status = Ps2_GetDataMode(deviceHandle, sessionIndex, &dataMode);
	if (status != PsReturnStatus::PsRetOK)
		cout << "Ps2_GetDataMode failed!" << endl;
	else
		cout << "Get Ps2_GetDataMode : " << dataMode << endl;


	//Set PixelFormat as PsPixelFormatBGR888 for opencv display
	Ps2_SetColorPixelFormat(deviceHandle, sessionIndex, PsPixelFormatBGR888);

	status = Ps2_GetCameraParameters(deviceHandle, sessionIndex, PsRgbSensor, &cameraParameters);

	cout << "Get PsGetCameraParameters status: " << status << endl;
	cout << "RGB Camera Intinsic: " << endl;
	cout << "Fx: " << cameraParameters.fx << endl;
	cout << "Cx: " << cameraParameters.cx << endl;
	cout << "Fy: " << cameraParameters.fy << endl;
	cout << "Cy: " << cameraParameters.cy << endl;
	cout << "RGB Distortion Coefficient: " << endl;
	cout << "K1: " << cameraParameters.k1 << endl;
	cout << "K2: " << cameraParameters.k2 << endl;
	cout << "K3: " << cameraParameters.k3 << endl;
	cout << "P1: " << cameraParameters.p1 << endl;
	cout << "P2: " << cameraParameters.p2 << endl;

	const string rgbImageWindow = "RGB Image";
	const string mappedDepthImageWindow = "MappedDepth Image";
	const string mappedIRImageWindow = "MappedIR Image";
	const string mappedRgbImageWindow = "MappedRGB Image";
	bool f_bMappedRGB = true;
	bool f_bMappedDepth = false;
	bool f_bMappedIR = false;
	bool f_bPointClound = false;

	cout << "\n--------------------------------------------------------------------" << endl;
	cout << "--------------------------------------------------------------------" << endl;
	cout << "Press following key to set corresponding feature:" << endl;
	cout << "S/s: Enable or disable the Depth and RGB synchronize feature " << endl;
	cout << "P/p: Save point cloud data into PointCloud.txt in current directory" << endl;
	cout << "T/t: Change background filter threshold value" << endl;
	cout << "M/m: Change data mode: input corresponding index in terminal:" << endl;
	cout << "                    0: Output Depth and IR in 15 fps, and RGB in 30fps" << endl;
	cout << "                    1: Only Output RGB in 30 fps" << endl;
	cout << "                    2: StandBy" << endl;
	cout << "R/r: Change the RGB resolution: input corresponding index in terminal:" << endl;
	cout << "                             0: 1920*1080" << endl;
	cout << "                             1: 1280*720" << endl;
	cout << "                             2: 640*480" << endl;
	cout << "                             3: 640*360" << endl;
	cout << "Q/q: Enable or disable the mapped RGB in Depth space" << endl;
	cout << "L/l: Enable or disable the mapped Depth in RGB space" << endl; 
	cout << "J/j: Enable or disable the mapped IR in RGB space" << endl;
	cout << "Esc: Program quit " << endl;
	cout << "--------------------------------------------------------------------" << endl;
	cout << "--------------------------------------------------------------------\n" << endl;

	int destroycount = 0;

	for (;;)
	{
		if (destroycount > 0)
		{
			destroycount--;
			cv::destroyAllWindows();
		}

		PsFrame depthFrame = { 0 };
		PsFrame irFrame = { 0 };
		PsFrame rgbFrame = { 0 };
		PsFrame mappedDepthFrame = { 0 };
		PsFrame mappedIRFrame = { 0 };
		PsFrame mappedRGBFrame = { 0 };

		// Read one frame before call PsGetFrame
		PsFrameReady frameReady = { 0 };
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &frameReady);

		if (status != PsRetOK)
		{
			goto KEY;
		}

		//Get depth frame, depth frame only output in following data mode
		if (1 == frameReady.depth)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsDepthFrame, &depthFrame);

			if (depthFrame.pFrameData != NULL)
			{
				if (f_bPointClound)
				{
					PointCloudWriter.open("PointCloud.txt");
					PsFrame &srcFrame = depthFrame;
					const int len = srcFrame.width * srcFrame.height;
					PsVector3f* worldV = new PsVector3f[len];

					Ps2_ConvertDepthFrameToWorldVector(deviceHandle, sessionIndex, srcFrame, worldV); //Convert Depth frame to World vectors.

					for (int i = 0; i < len; i++)
					{
						if (worldV[i].z == 0 || worldV[i].z == 0xFFFF)
							continue; //discard zero points
						PointCloudWriter << worldV[i].x << "\t" << worldV[i].y << "\t" << worldV[i].z << std::endl;
					}
					delete[] worldV;
					worldV = NULL;
					std::cout << "Save point cloud successful in PointCloud.txt" << std::endl;
					PointCloudWriter.close();
					f_bPointClound = false;
				}
				//Display the Depth Image
				Opencv_Depth(slope, depthFrame.height, depthFrame.width, depthFrame.pFrameData, imageMat);
				cv::imshow(depthImageWindow, imageMat);
			}
			else
			{
				cout << "Ps2_GetFrame PsDepthFrame status:" << status << " pFrameData is NULL " << endl;
			}
		}

		//Get IR frame, IR frame only output in following data mode
		if (1 == frameReady.ir)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsIRFrame, &irFrame);

			if (irFrame.pFrameData != NULL)
			{
				//Display the IR Image
				imageMat = cv::Mat(irFrame.height, irFrame.width, CV_16UC1, irFrame.pFrameData);

				// Convert 16bit IR pixel (max pixel value is 3840) to 8bit for display
				imageMat.convertTo(imageMat, CV_8U, 255.0 / 3840);
				cv::imshow(irImageWindow, imageMat);
			}
			else
			{
				cout << "Ps2_GetFrame PsIRFrame status:" << status << " pFrameData is NULL " << endl;
			}
		}

		//Get RGB frame, RGB frame only output in following data mode
		if (1 == frameReady.rgb)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsRGBFrame, &rgbFrame);

			if (rgbFrame.pFrameData != NULL)
			{
				//Display the RGB Image
				imageMat = cv::Mat(rgbFrame.height, rgbFrame.width, CV_8UC3, rgbFrame.pFrameData);
				cv::imshow(rgbImageWindow, imageMat);
			}
			else
			{
				cout << "Ps2_GetFrame PsRGBFrame status:" << status << " pFrameData is NULL " << endl;
			}
		}
	
		//Get mapped depth frame which is mapped to rgb camera space
		//Mapped depth frame only output in following data mode
		//And can only get when the feature is enabled through api Ps2_SetMapperEnabledRGBToDepth
		//When the key "L/l" pressed, this feature enable or disable
		if (1 == frameReady.mappedDepth)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsMappedDepthFrame, &mappedDepthFrame);

			if (mappedDepthFrame.pFrameData != NULL)
			{
				//Display the MappedDepth Image
				imageMat = cv::Mat(mappedDepthFrame.height, mappedDepthFrame.width, CV_16UC1, mappedDepthFrame.pFrameData);
				cv::Mat mappedDepthMat;
				imageMat.convertTo(mappedDepthMat, CV_8U, 255.0 / slope);
				cv::applyColorMap(mappedDepthMat, mappedDepthMat, cv::COLORMAP_RAINBOW);
				cv::imshow(mappedDepthImageWindow, mappedDepthMat);
			}
			else
			{
				cout << "Ps2_GetFrame PsMappedDepthFrame status:" << status << " pFrameData is NULL " << endl;
			}
		}
		//Get mapped ir frame which is mapped to rgb camera space
		//Mapped ir frame only output in following data mode
		//And can only get when the feature is enabled through api Ps2_SetMapperEnabledRGBToIR
		//When the key "J/j" pressed, this feature enable or disable
		if (1 == frameReady.mappedIR)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsMappedIRFrame, &mappedIRFrame);

			if (mappedIRFrame.pFrameData != NULL)
			{
				//Display the MappedDepth Image
				imageMat = cv::Mat(mappedIRFrame.height, mappedIRFrame.width, CV_16UC1, mappedIRFrame.pFrameData);
				imageMat.convertTo(imageMat, CV_8U, 255.0 / 3840);
				cv::imshow(mappedIRImageWindow, imageMat);
			}
			else
			{
				cout << "Ps2_GetFrame PsMappedIRFrame status:" << status << " pFrameData is NULL " << endl;
			}
		}
		//Get mapped rgb frame which is mapped to depth camera space
		//Mapped rgb frame only output in following data mode
		//And can only get when the feature is enabled through api Ps2_SetMapperEnabledDepthToRGB
		//When the key "Q/q" pressed, this feature enable or disable
		if (1 == frameReady.mappedRGB)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsMappedRGBFrame, &mappedRGBFrame);

			if (mappedRGBFrame.pFrameData != NULL)
			{
				//Display the MappedRGB Image
				imageMat = cv::Mat(mappedRGBFrame.height, mappedRGBFrame.width, CV_8UC3, mappedRGBFrame.pFrameData);
				cv::imshow(mappedRgbImageWindow, imageMat);
			}
			else
			{
				cout << "Ps2_GetFrame PsMappedRGBFrame status:" << status << " pFrameData is NULL " << endl;
			}
		}

	KEY:
		unsigned char key = waitKey(1);
		imageMat.release();

		if (key == 'M' || key == 'm')
		{
			cout << "Selection: 0:PsDepthAndIR15_RGB30; 1:Rgb_30; 2:StandBy" << endl;
	
			int index = -1;
			cin >> index;
			//clear buffer and cin flag. if not, cin will not get input anymore;
			if (cin.fail())
			{
				std::cout << "Unexpected input\n";
				cin.clear();
				cin.ignore(1024, '\n');
				continue;
			}
			else
			{
				cin.clear();
				cin.ignore(1024, '\n');
			}
 
			switch (index)
			{
			case 0:
				dataMode = PsDepthAndIR15_RGB30;
				break;
			case 1:
				dataMode = PsRGB30;
				break;
			case 2:
				dataMode = PsStandBy;
				break;
			default:
				cout << "Unsupported data mode!" << endl;
				continue;
			}

			status = Ps2_SetDataMode(deviceHandle, sessionIndex, (PsDataMode)dataMode);
			if (status != PsRetOK)
			{
				cout << "Ps2_SetDataMode  status" << status << endl;
			}
			destroycount = 3;
		}
	
		else if (key == 'P' || key == 'p')
		{
			if (dataMode == PsDepthAndIR15_RGB30)
			{
				f_bPointClound = true;
			}
			else
			{
				cout << "has no depth" << endl;
			}
		}
		else if (key == 'T' || key == 't')
		{
			//Set background filter threshold
			static uint16_t threshold = 0;
			threshold += 10;
			if (threshold > 100)
				threshold = 0;
			Ps2_SetThreshold(deviceHandle, sessionIndex, threshold);
			cout << "Set background threshold value: " << threshold << endl;
		}		
		else if (key == 'S' || key == 's')
		{
			status = Ps2_SetSynchronizeEnabled(deviceHandle, sessionIndex, f_bSync);
			if (status == PsRetOK)
			{
				cout << "Set Synchronize " << (f_bSync ? "Enabled." : "Disabled.") << endl;
				f_bSync = !f_bSync;
			}
		}
		else if (key == 27)	//ESC Pressed
		{
			break;
		}
 		else if (key == 'R' || key == 'r')
		{
			cout << "please select RGB resolution to set: 0:1080P; 1:720P; 2:480P; 3:360P" << endl;
			int index = 0;
			cin >> index;
			if (cin.fail())
			{
				std::cout << "Unexpected input\n";
				cin.clear();
				cin.ignore(1024, '\n');
				continue;
			}
			else
			{
				cin.clear();
				cin.ignore(1024, '\n');
			}
			PsResolution resolution = PsRGB_Resolution_1920_1080;
			switch (index)
			{
			case 1:
				resolution = PsRGB_Resolution_1280_720;
				break;
			case 2:
				resolution = PsRGB_Resolution_640_480;
				break;
			case 3:
				resolution = PsRGB_Resolution_640_360;
				break;
			default:
				break;
			}
			Ps2_SetRGBResolution(deviceHandle, sessionIndex, resolution);
		}
		else if (key == 'J' || key == 'j')
		{
			status = Ps2_SetMapperEnabledRGBToIR(deviceHandle, sessionIndex, f_bMappedIR);
			if (status == PsRetOK)
			{
				cout << "Set Mapper RGBToIR " << (f_bMappedIR ? "Enabled." : "Disabled.") << endl;
				f_bMappedIR = !f_bMappedIR;
			}
		}
		else if (key == 'L' || key == 'l')
		{
			status = Ps2_SetMapperEnabledRGBToDepth(deviceHandle, sessionIndex, f_bMappedDepth);
			if (status == PsRetOK)
			{
				cout << "Set Mapper RGBToDepth " << (f_bMappedDepth ? "Enabled." : "Disabled.") << endl;
				f_bMappedDepth = !f_bMappedDepth;
			}
		}
		else if (key == 'Q' || key == 'q')
		{
			status = Ps2_SetMapperEnabledDepthToRGB(deviceHandle, sessionIndex, f_bMappedRGB);
			if (status == PsRetOK)
			{
				cout << "Set Mapper DepthToRGB " << (f_bMappedRGB ? "Enabled." : "Disabled.") << endl;
				f_bMappedRGB = !f_bMappedRGB;
			}
		}
	}

	status = Ps2_CloseDevice(&deviceHandle);
	cout << "CloseDevice status: " << status << endl;

	status = Ps2_Shutdown();
	cout << "Shutdown status: " << status << endl;
	cv::destroyAllWindows();
	return 0;
}

void HotPlugStateCallback(const PsDeviceInfo* pInfo, int status)
{
	cout << "uri " << status << "  " << pInfo->uri << "    " << (status == 0 ? "add" : "remove") << endl;
	cout << "alia " << status << "  " << pInfo->alias << "    " << (status == 0 ? "add" : "remove") << endl;
}
