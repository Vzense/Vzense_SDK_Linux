//Note:DCAM550 has no RGB frame and no use DCAM_305 in Vzense_dcamtype.h

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "DCAM550/Vzense_api_550.h"
#include <thread>
#include <sys/timeb.h>
 
#define FPS
#define FPS_LEN 100
using namespace std;
using namespace cv;

#ifdef FPS

long delayT = 0;

int countof_loop_tof = 0;
long totaldelay_tof = 0;
int fps_tof = 0;

int countof_loop_rgb = 0;
long totaldelay_rgb = 0;
int fps_rgb = 0;

int countof_loop_ir = 0;
long totaldelay_ir = 0;
int fps_ir = 0;

int countof_loop_wdr1 = 0;
long totaldelay_wdr1 = 0;
int fps_wdr1 = 0;


int countof_loop_wdr2 = 0;
long totaldelay_wdr2 = 0;
int fps_wdr2 = 0;

int countof_loop_wdr3 = 0;
long totaldelay_wdr3 = 0;
int fps_wdr3 = 0;

#endif
void HotPlugStateCallback(const PsDeviceInfo* pInfo, int params);

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
	uint32_t wdrRange1Slope = 1450;
	uint32_t wdrRange2Slope = 4400;
	uint32_t wdrRange3Slope = 6000;

	PsDepthRange depthRange = PsNearRange;
	PsDataMode dataMode = PsDepthAndIR_30;
	PsWDROutputMode wdrMode = { PsWDRTotalRange_Two, PsNearRange, 1, PsFarRange, 1, PsUnknown, 1 };
	bool f_bWDRMode = false;
	bool bWDRStyle = true;
	bool f_bConfidence = true;

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
		cout << "PsGetDeviceCount failed! make sure the DCAM is connected" << endl;
		this_thread::sleep_for(chrono::seconds(1));
		goto GET;
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

	//Get MeasuringRange
	PsMeasuringRange measuringrange = { 0 };

	status = Ps2_GetDataMode(deviceHandle, sessionIndex, &dataMode);
	if (status != PsReturnStatus::PsRetOK)
		cout << "Ps2_GetDataMode failed!" << endl;
	else
		cout << "Get Ps2_GetDataMode : " << dataMode << endl;
	if (dataMode == PsWDR_Depth)
	{
		f_bWDRMode = true;

		status = Ps2_GetWDROutputMode(deviceHandle, sessionIndex, &wdrMode);

		if (status != PsReturnStatus::PsRetOK)
			cout << "Ps2_GetWDROutputMode failed!" << endl;
		else
		{
			if (wdrMode.totalRange == PsWDRTotalRange_Two)
			{
				depthRange = wdrMode.range2;
				status = Ps2_GetMeasuringRange(deviceHandle, sessionIndex, (PsDepthRange)wdrMode.range1, &measuringrange);
				if (status != PsReturnStatus::PsRetOK)
					cout << "Ps2_GetMeasuringRange failed!" << endl;
				else
				{
					switch ((PsDepthRange)wdrMode.range1)
					{
					case PsNearRange:
					case PsXNearRange:
					case PsXXNearRange:
						wdrRange1Slope = measuringrange.effectDepthMaxNear;
						break;

					case PsMidRange:
					case PsXMidRange:
					case PsXXMidRange:
						wdrRange1Slope = measuringrange.effectDepthMaxMid;
						break;

					case PsFarRange:
					case PsXFarRange:
					case PsXXFarRange:

						wdrRange1Slope = measuringrange.effectDepthMaxFar;
						break;
					default:
						break;
					}
					cout << "wdrRange1Slope   =  " << wdrRange1Slope << endl;
				}

				status = Ps2_GetMeasuringRange(deviceHandle, sessionIndex, (PsDepthRange)wdrMode.range2, &measuringrange);
				if (status != PsReturnStatus::PsRetOK)
					cout << "Ps2_GetMeasuringRange failed!" << endl;
				else
				{
					switch ((PsDepthRange)wdrMode.range2)
					{
					case PsNearRange:
					case PsXNearRange:
					case PsXXNearRange:
						wdrRange2Slope = wdrSlope == measuringrange.effectDepthMaxNear;
						break;

					case PsMidRange:
					case PsXMidRange:
					case PsXXMidRange:
						wdrRange2Slope = wdrSlope = measuringrange.effectDepthMaxMid;
						break;

					case PsFarRange:
					case PsXFarRange:
					case PsXXFarRange:

						wdrRange2Slope = wdrSlope = measuringrange.effectDepthMaxFar;
						break;
					default:
						break;
					}
					cout << "wdrSlope   =  wdrRange2Slope  " << wdrSlope << endl;
				}

			}
			else if (wdrMode.totalRange == PsWDRTotalRange_Three)
			{
				status = Ps2_GetMeasuringRange(deviceHandle, sessionIndex, (PsDepthRange)wdrMode.range1, &measuringrange);
				if (status != PsReturnStatus::PsRetOK)
					cout << "Ps2_GetMeasuringRange failed!" << endl;
				else
				{
					switch ((PsDepthRange)wdrMode.range1)
					{
					case PsNearRange:
					case PsXNearRange:
					case PsXXNearRange:
						wdrRange1Slope = measuringrange.effectDepthMaxNear;
						break;

					case PsMidRange:
					case PsXMidRange:
					case PsXXMidRange:
						wdrRange1Slope = measuringrange.effectDepthMaxMid;
						break;

					case PsFarRange:
					case PsXFarRange:
					case PsXXFarRange:

						wdrRange1Slope = measuringrange.effectDepthMaxFar;
						break;
					default:
						break;
					}
					cout << "wdrRange1Slope   =  " << wdrRange1Slope << endl;
				}


				status = Ps2_GetMeasuringRange(deviceHandle, sessionIndex, (PsDepthRange)wdrMode.range2, &measuringrange);
				if (status != PsReturnStatus::PsRetOK)
					cout << "Ps2_GetMeasuringRange failed!" << endl;
				else
				{
					switch ((PsDepthRange)wdrMode.range2)
					{
					case PsNearRange:
					case PsXNearRange:
					case PsXXNearRange:
						wdrRange2Slope = measuringrange.effectDepthMaxNear;
						break;

					case PsMidRange:
					case PsXMidRange:
					case PsXXMidRange:
						wdrRange2Slope = measuringrange.effectDepthMaxMid;
						break;

					case PsFarRange:
					case PsXFarRange:
					case PsXXFarRange:

						wdrRange2Slope = measuringrange.effectDepthMaxFar;
						break;
					default:
						break;
					}
					cout << "wdrRange2Slope   =  " << wdrSlope << endl;
				}
				status = Ps2_GetMeasuringRange(deviceHandle, sessionIndex, (PsDepthRange)wdrMode.range3, &measuringrange);
				if (status != PsReturnStatus::PsRetOK)
					cout << "Ps2_GetMeasuringRange failed!" << endl;
				else
				{
					switch ((PsDepthRange)wdrMode.range3)
					{
					case PsNearRange:
					case PsXNearRange:
					case PsXXNearRange:
						wdrRange3Slope = wdrSlope = measuringrange.effectDepthMaxNear;
						break;

					case PsMidRange:
					case PsXMidRange:
					case PsXXMidRange:
						wdrRange3Slope = wdrSlope = measuringrange.effectDepthMaxMid;
						break;

					case PsFarRange:
					case PsXFarRange:
					case PsXXFarRange:

						wdrRange3Slope = wdrSlope = measuringrange.effectDepthMaxFar;
						break;
					default:
						break;
					}
					cout << "wdrSlope   =  wdrRange3Slope  " << wdrSlope << endl;
				}
			}
		}
	}
	else
	{
		status = Ps2_GetDepthRange(deviceHandle, sessionIndex, &depthRange);
		if (status != PsReturnStatus::PsRetOK)
			cout << "Ps2_GetDepthRange failed!" << endl;
		else
			cout << "Get Depth Range " << depthRange << endl;

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
	}
	cv::Mat imageMat;
	const string irImageWindow = "IR Image";
	const string depthImageWindow = "Depth Image";
	const string wdrDepthImageWindow = "WDR Depth Image";
	const string wdrDepthRange1ImageWindow = "WDR Depth Range1 Image";
	const string wdrDepthRange2ImageWindow = "WDR Depth Range2 Image";
	const string wdrDepthRange3ImageWindow = "WDR Depth Range3 Image";

	ofstream PointCloudWriter;
	PsDepthVector3 DepthVector = { 0, 0, 0 };
	PsVector3f WorldVector = { 0.0f };

	bool f_bPointClound = false;

	PsDepthRangeList rangelist = { 0 };
	int len = sizeof(rangelist);
	status = Ps2_GetProperty(deviceHandle, sessionIndex, PsPropertyDepthRangeList, &rangelist, &len);

	if (status == PsReturnStatus::PsRetOK&&rangelist.count > 0)
	{
		cout << "Available Range List: ";
		for (int i = 0; i < rangelist.count-1; i++)
		{
			cout << (int)rangelist.depthrangelist[i] <<",";
		}
		cout << (int)rangelist.depthrangelist[rangelist.count - 1] << endl;
	}

	cout << "\n--------------------------------------------------------------------" << endl;
	cout << "--------------------------------------------------------------------" << endl;
	cout << "Press following key to set corresponding feature:" << endl;
	cout << "0/1/2...: Change depth range Near/Middle/Far..." << endl;
	cout << "P/p: Save point cloud data into PointCloud.txt in current directory" << endl;
	cout << "T/t: Change background filter threshold value" << endl;
	cout << "M/m: Change data mode: input corresponding index in terminal:" << endl;
	cout << "                    0: Output Depth in 30 fps" << endl;
	cout << "                    1: Output IR in 30 fps" << endl;
	cout << "                    2: Output Depth and IR in 30 fps" << endl;
	cout << "                    3: Output WDR_Depth in 30 fps" << endl;
	cout << "C/c: Enable or disable the ConfidenceFilter in DataMode(DepthAndIR_30) " << endl;
	cout << "F/f: set the ConfidenceFilter Threshold in DataMode(DepthAndIR_30)" << endl;
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
		PsFrame wdrDepthFrame = { 0 };

		// Read one frame before call PsGetFrame
		PsFrameReady frameReady = { 0 };
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &frameReady);
		
#ifdef FPS

		struct  timeb   stTimeb;
		ftime(&stTimeb);
		long dwEnd = stTimeb.millitm;
		long timedelay = dwEnd - delayT;
		delayT = dwEnd;
		if (timedelay < 0) {
			timedelay += 1000;
		}
		totaldelay_tof += timedelay;
		totaldelay_rgb += timedelay;
		totaldelay_ir += timedelay;
		totaldelay_wdr1 += timedelay;
		totaldelay_wdr2 += timedelay;
		totaldelay_wdr3 += timedelay;

#endif
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
				if (f_bWDRMode&&dataMode == PsWDR_Depth)
				{
					if (depthFrame.depthRange == wdrMode.range1&&wdrMode.range1Count!=0)
					{
#ifdef FPS
						countof_loop_wdr1++;
						if (countof_loop_wdr1 >= FPS_LEN)
						{
							fps_wdr1 = 1000 * FPS_LEN / totaldelay_wdr1;
							//cout << fps_tof<<endl;
							countof_loop_wdr1 = 0;
							totaldelay_wdr1 = 0;
						}
#endif
						Opencv_Depth(wdrRange1Slope, depthFrame.height, depthFrame.width, depthFrame.pFrameData, imageMat);
#ifdef FPS
						if (fps_wdr1 != 0)
						{
							char fps[20];
							snprintf(fps, sizeof(fps), "FPS: %d", fps_wdr1);
							putText(imageMat, fps, Point2d(10, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 0));
						}
#endif
						cv::imshow(wdrDepthRange1ImageWindow, imageMat);					
					}
					else if (depthFrame.depthRange == wdrMode.range2&&wdrMode.range2Count != 0)
					{
#ifdef FPS
						countof_loop_wdr2++;
						if (countof_loop_wdr2 >= FPS_LEN)
						{
							fps_wdr2 = 1000 * FPS_LEN / totaldelay_wdr2;
							//cout << fps_tof<<endl;
							countof_loop_wdr2 = 0;
							totaldelay_wdr2 = 0;
						}
#endif
						Opencv_Depth(wdrRange2Slope, depthFrame.height, depthFrame.width, depthFrame.pFrameData, imageMat);
#ifdef FPS
						if (fps_wdr2 != 0)
						{
							char fps[20];
							snprintf(fps, sizeof(fps), "FPS: %d", fps_wdr2);
							putText(imageMat, fps, Point2d(10, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 0));
						}
#endif
						cv::imshow(wdrDepthRange2ImageWindow, imageMat);
					}
					else if (depthFrame.depthRange == wdrMode.range3&&wdrMode.range3Count != 0)
					{
#ifdef FPS
						countof_loop_wdr3++;
						if (countof_loop_wdr3 >= FPS_LEN)
						{
							fps_wdr3 = 1000 * FPS_LEN / totaldelay_wdr3;
							//cout << fps_tof<<endl;
							countof_loop_wdr3 = 0;
							totaldelay_wdr3 = 0;
						}
#endif
						Opencv_Depth(wdrRange3Slope, depthFrame.height, depthFrame.width, depthFrame.pFrameData, imageMat);
#ifdef FPS
						if (fps_wdr3 != 0)
						{
							char fps[20];
							snprintf(fps, sizeof(fps), "FPS: %d", fps_wdr3);
							putText(imageMat, fps, Point2d(10, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 0));
						}
#endif
						cv::imshow(wdrDepthRange3ImageWindow, imageMat);
					}
				}
				else
				{
#ifdef FPS
					countof_loop_tof++;
					if (countof_loop_tof >= FPS_LEN)
					{
						fps_tof = 1000 * FPS_LEN / totaldelay_tof;
						//cout << fps_tof<<endl;
						countof_loop_tof = 0;
						totaldelay_tof = 0;
					}
#endif
					Opencv_Depth(slope, depthFrame.height, depthFrame.width, depthFrame.pFrameData, imageMat);
#ifdef FPS
					if (fps_tof != 0)
					{
						char fps[20];
						snprintf(fps, sizeof(fps), "FPS: %d", fps_tof);
						putText(imageMat, fps, Point2d(10, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 0));
					}
#endif
					cv::imshow(depthImageWindow, imageMat);
				}
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
#ifdef FPS	 
				countof_loop_ir++;
				if (countof_loop_ir >= FPS_LEN)
				{
					fps_ir = 1000 * FPS_LEN / totaldelay_ir;
					//cout << fps_tof << endl;
					countof_loop_ir = 0;
					totaldelay_ir = 0;
				}		 
#endif
				//Display the IR Image
				imageMat = cv::Mat(irFrame.height, irFrame.width, CV_16UC1, irFrame.pFrameData);

				// Convert 16bit IR pixel (max pixel value is 3840) to 8bit for display
				imageMat.convertTo(imageMat, CV_8U, 255.0 / 3840);
#ifdef FPS
				if (fps_ir != 0)
				{
					char fps[20];
					snprintf(fps, sizeof(fps), "FPS: %d", fps_ir);
					putText(imageMat, fps, Point2d(10, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 255, 255));
				}
#endif
				cv::imshow(irImageWindow, imageMat);
			}
			else
			{
				cout << "Ps2_GetFrame PsIRFrame status:" << status << " pFrameData is NULL " << endl;
			}
		}

		//Get WDR depth frame(fusion or alternatively, determined by Ps2_SetWDRStyle, default in fusion)
		//WDR depth frame only output in PsWDR_Depth data mode
		if (1 == frameReady.wdrDepth)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsWDRDepthFrame, &wdrDepthFrame);
			if (wdrDepthFrame.pFrameData != NULL)
			{
#ifdef FPS			 
				countof_loop_tof++;
				if (countof_loop_tof >= FPS_LEN)
				{
					fps_tof = 1000 * FPS_LEN / totaldelay_tof;
					//cout << fps_tof << endl;
					countof_loop_tof = 0;
					totaldelay_tof = 0;
				}				 
#endif
				if (f_bPointClound)
				{
					PointCloudWriter.open("PointCloud.txt");
					PsFrame &srcFrame = wdrDepthFrame ;
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
				//Display the WDR Depth Image
				Opencv_Depth(wdrSlope, wdrDepthFrame.height, wdrDepthFrame.width, wdrDepthFrame.pFrameData, imageMat);
#ifdef FPS
				if (fps_tof != 0)
				{
					char fps[20];
					snprintf(fps, sizeof(fps), "FPS: %d", fps_tof);
					putText(imageMat, fps, Point2d(10, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 0));
				}
#endif
				cv::imshow(wdrDepthImageWindow, imageMat);
			}
			else
			{
				cout << "Ps2_GetFrame PsWDRDepthFrame status:" << status << " pFrameData is NULL " << endl;
			}
		}

	KEY:
		unsigned char key = waitKey(1);
		imageMat.release();

		if (key == 'M' || key == 'm')
		{
			cout << "Selection: 0:Depth_30; 1:Ir_30; 2:DepthAndIR_30; 3:WDR_Depth" << endl;
		
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
			PsDataMode t_datamode = PsDepthAndIR_30;

			switch (index)
			{
			case 0:
				t_datamode = PsDepth_30;
				break;
			case 1:
				t_datamode = PsIR_30;
				break;
			case 2:
				t_datamode = PsDepthAndIR_30;
				break;
			case 3:
				t_datamode = PsWDR_Depth;
				break;
			default:
				cout << "Unsupported data mode!" << endl;
				continue;
			}
		
			if (t_datamode == PsWDR_Depth)
			{
				Ps2_SetWDROutputMode(deviceHandle, sessionIndex, &wdrMode);
				f_bWDRMode = true;
				
			}
			else
			{
				f_bWDRMode = false;
			}
			if (dataMode == PsWDR_Depth&&t_datamode != PsWDR_Depth)
			{
				status = Ps2_GetDepthRange(deviceHandle, sessionIndex, &depthRange);
				cout << "Get depth range," << " depthRange: " << depthRange << endl;
				if (status != PsRetOK)
				{
					cout << "Get depth range failed! " << endl;
				}
				else
				{
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
				}
			}
			 
			status = Ps2_SetDataMode(deviceHandle, sessionIndex, (PsDataMode)t_datamode);
			if (status != PsRetOK)
			{
				cout << "Ps2_SetDataMode  status" << status << endl;
			}		
			else
			{
				dataMode = t_datamode;
			}
			destroycount = 3;
#ifdef FPS
			delayT = 0;

			countof_loop_tof = 0;
			totaldelay_tof = 0;
			fps_tof = 0;

			countof_loop_rgb = 0;
			totaldelay_rgb = 0;
			fps_rgb = 0;

			countof_loop_ir = 0;
			totaldelay_ir = 0;
			fps_ir = 0;

			countof_loop_wdr1 = 0;
			totaldelay_wdr1 = 0;
			fps_wdr1 = 0;

			countof_loop_wdr2 = 0;
			totaldelay_wdr2 = 0;
			fps_wdr2 = 0;

			countof_loop_wdr3 = 0;
			totaldelay_wdr3 = 0;
			fps_wdr3 = 0;
#endif
		}
		else if ((key == '0') || (key == '1') || (key == '2') || (key == '3') || (key == '4') || (key == '5') || (key == '6') || (key == '7') || (key == '8'))
		{
			switch (key)
			{
			case '0':
				depthRange = PsNearRange;
				slope = 1450;
				break;
			case '1':
				depthRange = PsMidRange;
				slope = 3000;
				break;
			case '2':
				depthRange = PsFarRange;
				slope = 4400;
				break;
			case '3':
				depthRange = PsXNearRange;
				slope = 4800;
				break;
			case '4':
				depthRange = PsXMidRange;
				slope = 5600;
				break;
			case '5':
				depthRange = PsXFarRange;
				slope = 7500;
				break;
			case '6':
				depthRange = PsXXNearRange;
				slope = 9600;
				break;
			case '7':
				depthRange = PsXXMidRange;
				slope = 11200;
				break;
			case '8':
				depthRange = PsXXFarRange;
				slope = 15000;
				break;
			default:
				cout << "Unsupported Range!" << endl;
				continue;
			}
			status = Ps2_SetDepthRange(deviceHandle, sessionIndex, depthRange);
			if (depthRange == PsNearRange)
				cout << "Set depth range to Near," << " status: " << status << endl;
			else if (depthRange == PsMidRange)
				cout << "Set depth range to Mid," << " status: " << status << endl;
			else if (depthRange == PsFarRange)
				cout << "Set depth range to Far," << " status: " << status << endl;
			else if (depthRange == PsXNearRange)
				cout << "Set depth range to XNearRange," << " status: " << status << endl;
			else if (depthRange == PsXMidRange)
				cout << "Set depth range to XMidRange," << " status: " << status << endl;
			else if (depthRange == PsXFarRange)
				cout << "Set depth range to XFarRange," << " status: " << status << endl;
			else if (depthRange == PsXXNearRange)
				cout << "Set depth range to XXNearRange," << " status: " << status << endl;
			else if (depthRange == PsXXMidRange)
				cout << "Set depth range to XXMidRange," << " status: " << status << endl;
			else if (depthRange == PsXXFarRange)
				cout << "Set depth range to XXFarRange," << " status: " << status << endl;

			if (status != PsRetOK)
			{
				cout << "Set depth range failed! " << endl;
			}

			status = Ps2_GetDepthRange(deviceHandle, sessionIndex, &depthRange);
			cout << "Get depth range," << " depthRange: " << depthRange << endl;
			if (status != PsRetOK)
			{
				cout << "Get depth range failed! " << endl;
			}
			else
			{
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
			}
		}		
		else if (key == 'P' || key == 'p')
		{
			if (dataMode != PsIR_30)
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
			status = Ps2_SetThreshold(deviceHandle, sessionIndex, threshold);

			if (PsRetOK == status)
			{
				cout << "Set background threshold value: " << threshold << endl;
			}
			else
			{
				cout << "Set background threshold error,check if the datamode is WDR mode or the confidence filter is opened" << endl;
			}
		}		
		
		else if (key == 'V' || key == 'v')
		{
			static bool bWDRStyle = true;
			status = Ps2_SetWDRStyle(deviceHandle, sessionIndex, bWDRStyle ? PsWDR_ALTERNATION : PsWDR_FUSION);
			if (PsRetOK == status)
			{
				cout << "WDR image output " << (bWDRStyle ? "alternatively in multi range." : "Fusion.") << endl;
				bWDRStyle = !bWDRStyle;
			}
			else
			{
				cout << "Set WDR Style " << (bWDRStyle ? PsWDR_ALTERNATION : PsWDR_FUSION) << "  satus : " << status << endl;
			}
#ifdef FPS
			delayT = 0;

			countof_loop_tof = 0;
			totaldelay_tof = 0;
			fps_tof = 0;

			countof_loop_rgb = 0;
			totaldelay_rgb = 0;
			fps_rgb = 0;

			countof_loop_ir = 0;
			totaldelay_ir = 0;
			fps_ir = 0;

			countof_loop_wdr1 = 0;
			totaldelay_wdr1 = 0;
			fps_wdr1 = 0;

			countof_loop_wdr2 = 0;
			totaldelay_wdr2 = 0;
			fps_wdr2 = 0;

			countof_loop_wdr3 = 0;
			totaldelay_wdr3 = 0;
			fps_wdr3 = 0;
#endif
		}
		else if (key == 'C' || key == 'c')
		{
			status = Ps2_SetConfidenceFilterEnabled(deviceHandle, sessionIndex, f_bConfidence);
			if (PsRetOK == status)
			{
				cout << "Set Confidence Filter " <<(f_bConfidence  ? "true" : "false") << endl;
			}
			else
			{
				cout << "Set Confidence Filter failed, status:" << status << endl;
			}
			f_bConfidence = !f_bConfidence;
		}
		else if (key == 'F' || key == 'f')
		{
			static uint16_t conthreshold = 0;
			conthreshold += 10;
			if (conthreshold > 300)
				conthreshold = 0;

			status = Ps2_SetConfidenceFilterThreshold(deviceHandle, sessionIndex, conthreshold);

			if (PsRetOK == status)
			{
				cout << "Set Confidence Filter Threshold  value: " << conthreshold << endl;
			}
			else
			{
				cout << "Set Confidence Filter Threshold failed, status:" << status <<endl;
			}
		}
		else if (key == 27)	//ESC Pressed
		{
			break;
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
