#include <thread>
#include <iostream>
#include "DCAM550/Vzense_api_550.h"
#define frameSpace 10
using namespace std;

int main() {
	cout << "---MultiConnection---\n";

	//about dev
	uint32_t deviceCount = 0;
	PsReturnStatus status = PsRetOthers;
	PsDeviceHandle *deviceHandle = nullptr;
	PsDeviceInfo* pDeviceListInfo = nullptr;

	
	//about Frame
	uint32_t sessionIndex = 0;
	PsFrameReady FrameReady = { 0 };
	PsFrame Depth = { 0 };

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

	deviceHandle = new PsDeviceHandle[deviceCount];
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
	for (int i = 0; i < deviceCount; i++)
	{
		cout << "CameraIndex: " << i << endl;
		cout << "uri:" << pDeviceListInfo[i].uri << endl
			<< "alias:" << pDeviceListInfo[i].alias << endl
			<< "connectStatus:" << pDeviceListInfo[i].status << endl;
	}
	for(int i = 0;i < deviceCount; i++)
	{
		status = Ps2_OpenDevice(pDeviceListInfo[i].uri, &deviceHandle[i]);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "OpenDevice failed status:" <<status << endl;
			return -1;
		}	
		//Starts capturing the image stream
		status = Ps2_StartStream(deviceHandle[i], sessionIndex);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_StartStream failed status:" <<status<< endl;
			return -1;
		}
	}
	
	//1.ReadNextFrame.
	//2.Get depth Frame acoording to Ready flag.
	for (int j = 0; j < frameSpace; j++)
	{
		for (int i = 0; i < deviceCount; i++)
		{
			if (deviceHandle[i])
			{
				PsFrame depthFrame = { 0 };
				PsFrameReady frameReady = { 0 };
				status = Ps2_ReadNextFrame(deviceHandle[i], sessionIndex, &frameReady);

				if (status != PsRetOK)
				{
					cout << pDeviceListInfo[i].alias <<"  Ps2_ReadNextFrame failed status:" << status << endl;
					continue;
				}

				//Get depth frame, depth frame only output in following data mode
				if (1 == frameReady.depth)
				{
					status = Ps2_GetFrame(deviceHandle[i], sessionIndex, PsDepthFrame, &depthFrame);

					if (status == PsRetOK && depthFrame.pFrameData != NULL)
					{
						cout << pDeviceListInfo[i].alias << " frameIndex :" << depthFrame.frameIndex << endl;
					}
					else
					{
						cout << pDeviceListInfo[i].alias << "Ps2_GetFrame PsDepthFrame status:" << status  << endl;
					}
				}
			}

		}
	}
 
	//1.close device
	//2.SDK shutdown
	for (int i = 0; i < deviceCount; i++)
	{
		status = Ps2_StopStream(deviceHandle[i], sessionIndex);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_StopStream failed status:" <<status<< endl;
		}
		status = Ps2_CloseDevice(&deviceHandle[i]);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_CloseDevice failed status:" <<status<< endl;
		}
	}
	status = Ps2_Shutdown();
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_Shutdown failed status:" <<status<< endl;
		return -1;
	}

	cout << "--end--";

	return 0;
}
