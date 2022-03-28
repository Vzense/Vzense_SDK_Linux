#include <thread>
#include <iostream>
#include "DCAM560/Vzense_api_560.h"
#define frameSpace 10

using namespace std;

int main() {
	cout << "---DeviceModeChange---\n";

	//about dev
	uint32_t deviceCount;
	PsDeviceInfo* pDeviceListInfo = NULL;
	PsDeviceHandle deviceHandle = 0;
	PsReturnStatus status = PsRetOthers;

	//about frame
	uint32_t sessionIndex = 0;
	PsDataMode dataMode = PsDepthAndRGB_30;
	PsFrameReady FrameReady = { 0 };
	PsFrame depthFrame = { 0 };
	PsFrame RGBFrame = { 0 };
	PsFrame IRFrame = { 0 };

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
	cout << "open device successful,status :" << status <<endl;

	PsDataMode t_datamode;

	//PsDepthAndRGB_30

	t_datamode = PsDepthAndRGB_30;
	status = Ps2_SetDataMode(deviceHandle, sessionIndex, (PsDataMode)t_datamode);
	if (status != PsRetOK)
	{
		cout << "Ps2_SetDataMode  status" << status << endl;
	}
	else
	{
		dataMode = t_datamode;
		cout << "set PsDepthAndRGB_30,datemode index:" << t_datamode << endl;
	}

	//1.ReadNextFrame.
	//2.Get Depth and Get RGB Frame.
	for (int i = 0; i < frameSpace; i++)
	{
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &FrameReady);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_ReadNextFrame failed status:" <<status<< endl;
			continue;
		}

		if (1 == FrameReady.depth && 1 == FrameReady.rgb)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsDepthFrame, &depthFrame);
			if (depthFrame.pFrameData != NULL)
			{
				cout 
					<< "get Frame successful,status:" << status << "  "
					<< "frameTpye:" << depthFrame.frameType << "  "
					<< "frameIndex:" << depthFrame.frameIndex << endl;
			}
		
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsRGBFrame, &RGBFrame);
			if (RGBFrame.pFrameData != NULL)
			{
				cout 
					<< "get Frame successful,status:" << status << "  "
					<< "frameTpye:" << RGBFrame.frameType << "  "
					<< "frameIndex:" << RGBFrame.frameIndex << endl;
			}
		}
	}




	//PsDepthAndIRAndRGB_30

	t_datamode = PsDepthAndIRAndRGB_30;
	status = Ps2_SetDataMode(deviceHandle, sessionIndex, (PsDataMode)t_datamode);
	if (status != PsRetOK)
	{
		cout << "PsDepthAndIRAndRGB_30  status" << status << endl;
	}
	else
	{
		dataMode = t_datamode;
		cout << "set PsDepthAndIRAndRGB_30,datemode index:" << t_datamode << endl;
	}

	//1.ReadNextFrame.
	//2.Get Depth and IR and RGB Frame.
	for (int i = 0; i < frameSpace; i++)
	{
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &FrameReady);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_ReadNextFrame failed status:" <<status<< endl;
			continue;
		}

		if (1 == FrameReady.depth && 1 == FrameReady.ir && 1 == FrameReady.rgb)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsDepthFrame, &depthFrame);
			if (depthFrame.pFrameData != NULL)
			{
				cout
					<< "get Frame successful,status:" << status << "  "
					<< "frameTpye:" << depthFrame.frameType << "  "
					<< "frameIndex:" << depthFrame.frameIndex << endl;
			}
		
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsIRFrame, &IRFrame);
			if (IRFrame.pFrameData != NULL)
			{
				cout
					<< "get Frame successful,status:" << status << "  "
					<< "frameTpye:" << IRFrame.frameType << "  "
					<< "frameIndex:" << IRFrame.frameIndex << endl;
			}
		
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsRGBFrame, &RGBFrame);
			if (RGBFrame.pFrameData != NULL)
			{
				cout
					<< "get Frame successful,status:" << status << "  "
					<< "frameTpye:" << RGBFrame.frameType << "  "
					<< "frameIndex:" << RGBFrame.frameIndex << endl;
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
