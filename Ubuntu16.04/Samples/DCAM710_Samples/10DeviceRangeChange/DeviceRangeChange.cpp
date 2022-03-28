#include <thread>
#include <iostream>
#include "DCAM710/Vzense_api_710.h"
#define frameSpace 10

using namespace std;

int main() {
	cout << "---DeviceRangeChange---\n";

	//about dev
	uint32_t deviceCount;
	PsDeviceInfo* pDeviceListInfo = NULL;
	PsDeviceHandle deviceHandle = 0;
	PsReturnStatus status = PsRetOthers;

	//about frame
	uint32_t sessionIndex = 0;
	PsDepthRange depthRange = PsNearRange;
	PsFrameReady FrameReady = { 0 };
	PsFrame depthFrame = { 0 };

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
	cout << "open device successful,status :" << status << endl;

	//depth range 0
	depthRange = PsNearRange;
	status = Ps2_SetDepthRange(deviceHandle, sessionIndex, depthRange);
	if (status != PsRetOK)
	{
		cout << "Set depth range failed! " << endl;
	}
	else
	{
		cout << "Set depth range to Near," << " status: " << status << endl;
	}
	//cout Range
	for (int i = 0; i < frameSpace; 	i++)
	{
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &FrameReady);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_ReadNextFrame failed status:" <<status<< endl;
			continue;
		}

		if (1 == FrameReady.depth)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsDepthFrame, &depthFrame);
			if (depthFrame.pFrameData != NULL && PsNearRange==depthFrame.depthRange)
			{
				cout 
					<< "get Frame successful,status:" << status <<"  "
					<< "DepthRange:" << depthFrame.depthRange <<std::endl;
			
			}
		}
	}

	//depth range 2
	depthRange = PsFarRange;
	status = Ps2_SetDepthRange(deviceHandle, sessionIndex, depthRange);
	if (status != PsRetOK)
	{
		cout << "Set depth range failed! " << endl;
	}
	else
	{
		cout << "Set depth range to Far," << " status: " << status << endl;
	}
	//cout Range
	for (int j = 0; j < frameSpace;j++)
	{
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &FrameReady);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_ReadNextFrame failed status:" <<status<< endl;
			continue;
		}

		if (1 == FrameReady.depth)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsDepthFrame, &depthFrame);
				if (depthFrame.pFrameData != NULL && PsFarRange==depthFrame.depthRange)
			{
				cout 
					<< "get Frame successful,status:" << status <<"  "
					<< "DepthRange:" << depthFrame.depthRange << std::endl;
				
			}
		}
	}

	//Stop capturing the image stream
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
		cout << "Ps2_CloseDevice failed status:" << status << endl;
		return -1;
	}
	status = Ps2_Shutdown();
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_Shutdown failed status:" << status << endl;
		return -1;
	}
	cout << endl << "---end---";

	return 0;
}
