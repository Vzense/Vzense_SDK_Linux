#include <thread>
#include <iostream>
#include "DCAM560/Vzense_api_560.h"
#define frameSpace 20

using namespace std;

int main() {
	cout << "---DeviceHWTriggerMode---\n";

	//about dev
	uint32_t deviceCount;
	PsDeviceInfo* pDeviceListInfo = NULL;
	PsDeviceHandle deviceHandle = 0;
	PsReturnStatus status = PsRetOthers;

	//about frame
	uint32_t sessionIndex = 0;
	PsFrameReady FrameReady = { 0 };
	PsFrame depthFrame = { 0 };
	bool bSlaveEnabled = true;

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
		return false;
	}
	
	//Starts capturing the image stream
	status = Ps2_StartStream(deviceHandle, sessionIndex);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_StartStream failed status:" <<status<< endl;
		return -1;
	}

	//set slave true
	status = Ps2_SetSlaveModeEnabled(deviceHandle,sessionIndex,bSlaveEnabled);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_SetSlaveModeEnabled failed status:" <<status<< endl;
		return -1;
	}
	//1.hardware trigger.
	//2.ReadNextFrame.
	//3.GetFrame acoording to Ready flag and Frametype.
	for(int i = 0;i < frameSpace;i++)
	{
		//wait for the external trigger signal
		//if the trigger is none, the below call will return timeout
		//please check the external connection
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &FrameReady);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_ReadNextFrame failed status:" <<status<<endl;
			this_thread::sleep_for(chrono::seconds(1));
			continue;
		}

		//depthFrame for example.
		if (1 == FrameReady.depth) {
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsDepthFrame, &depthFrame);
			if (depthFrame.pFrameData != NULL)
			{
				cout << "get Frame successful,status:" << status << "  "
				<< "frameTpye:" << depthFrame.frameType << "  "
				<< "frameIndex:" << depthFrame.frameIndex << endl;
			}
		}

	}
	
	//set slave false
	status = Ps2_SetSlaveModeEnabled(deviceHandle,sessionIndex,!bSlaveEnabled);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_SetSlaveModeEnabled failed status:" <<status<< endl;
		return -1;
	}
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
