#include <thread>
#include <iostream>
#include "DCAM550/Vzense_api_550.h"
#include <fstream>
#define frameSpace 20

using namespace std;

int main() {
	cout << "---PointCloudCaptureAndSave---\n";

	//about dev
	uint32_t deviceCount;
	PsDeviceInfo* pDeviceListInfo = NULL;
	PsDeviceHandle deviceHandle = 0;
	PsReturnStatus status = PsRetOthers;
	
	//about frame
	uint32_t sessionIndex = 0;
	PsFrameReady FrameReady = { 0 };
	PsFrame depthFrame = { 0 };
	ofstream PointCloudWriter;

	//SDK Initialize
	status = Ps2_Initialize();
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "PsInitialize failed status:" <<status << endl;
		system("pause");
		return -1;
	}

	//1.Search and notice the count of devices.
	//2.get infomation of the devices. 
	//3.open devices accroding to the info.
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

	pDeviceListInfo = new PsDeviceInfo[deviceCount];
	status = Ps2_GetDeviceListInfo(pDeviceListInfo, deviceCount);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "GetDeviceListInfo failed status:" << status << endl;
		return -1;
	}
	else
	{
		if (Connected != pDeviceListInfo[0].status)
		{
			cout << "connect statu" << pDeviceListInfo[0].status << endl;
			cout << "Call Ps2_OpenDevice with connect status :" << Connected << endl;
			return -1;
		}
	}

	cout << "uri:" << pDeviceListInfo[0].uri << endl
		<< "alias:" << pDeviceListInfo[0].alias << endl
		<< "connectStatus:" << pDeviceListInfo[0].status << endl;

	status = Ps2_OpenDevice(pDeviceListInfo[0].uri, &deviceHandle);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "OpenDevice failed status:" <<status << endl;
		return -1;
	}

	//Starts capturing the image stream
	status = Ps2_StartStream(deviceHandle, sessionIndex);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_StartStream failed status:" <<status<< endl;
		return -1;
	}

	//1.ReadNextFrame.
	//2.GetFrame acoording to Ready flag and Frametype.
	//3.save points.
	for (int i = 0; i < frameSpace; i++)
	{
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &FrameReady);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_ReadNextFrame failed status:" <<status<< endl;
			continue;
		}

		//depthFrame only.
		if (1 == FrameReady.depth) {
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsDepthFrame, &depthFrame);
			if (depthFrame.pFrameData != NULL)
			{
				// once save
				
				PointCloudWriter.open("PointCloud.txt");
				PsFrame &srcFrame = depthFrame;
				const int len = srcFrame.width * srcFrame.height;
				PsVector3f* worldV = new PsVector3f[len];

				Ps2_ConvertDepthFrameToWorldVector(deviceHandle, sessionIndex, srcFrame, worldV); //Convert Depth frame to World vectors.

				for (int i = 0; i < len; i++)
				{
					PointCloudWriter << worldV[i].x << "\t" << worldV[i].y << "\t" << worldV[i].z << std::endl;
				}
				delete[] worldV;
				worldV = NULL;
				std::cout << "Save point cloud successful in PointCloud.txt" << std::endl;
				PointCloudWriter.close();
				break;
			}
		}

	}


	//Stops capturing the image stream
	status = Ps2_StopStream(deviceHandle, sessionIndex);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_StopStream failed status:" <<status<< endl;
		return -1;
	}

	//1.close device
	//2.SDK shutdown
	status = Ps2_CloseDevice(&deviceHandle);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_CloseDevice failed status:" <<status<< endl;
		return -1;
	}
	status = Ps2_Shutdown();
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_Shutdown failed status:" <<status<< endl;
		return -1;
	}
	cout << endl << "---end---";

	return 0;
}
