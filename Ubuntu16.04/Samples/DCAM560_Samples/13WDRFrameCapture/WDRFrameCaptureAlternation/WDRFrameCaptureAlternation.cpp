#include <iostream>
#include "DCAM560/Vzense_api_560.h"
#include <thread>
#define frameSpace 20

using namespace std;

int main()
{
	cout << "---DeviceWDRFrameCapture---\n";

	//about dev
	uint32_t deviceCount;
	PsDeviceInfo* pDeviceListInfo = NULL;
	PsDeviceHandle deviceHandle = 0;
	PsReturnStatus status = PsRetOthers;

	//about Frame
	uint32_t sessionIndex = 0;
	PsFrameReady FrameReady = { 0 };
	PsFrame Depth = { 0 };

	//WDR
	PsWDROutputMode wdrMode = { PsWDRTotalRange_Two, PsNearRange, 1, PsFarRange, 1, PsUnknown, 1 };

	//SDK Initialize
	status = Ps2_Initialize();
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "PsInitialize failed status:" << status << endl;
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
		cout << "OpenDevice failed status:" << status << endl;
		return -1;
	}

	//Starts capturing the image stream
	status = Ps2_StartStream(deviceHandle, sessionIndex);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_StartStream failed status:" << status << endl;
		return -1;
	}

	//1.SetWDROutputMode(2 or 3 ranges generally)
	//2.set DataMode to WDR
	//3.set WDRStyle to alternations(fusion or alternations of ranges)
	//4.ReadNextFrame.
	//5.Get WDR Frame acoording to Ready flag.

	status = Ps2_SetWDROutputMode(deviceHandle, sessionIndex, &wdrMode);
	if (status != PsRetOK)
	{
		cout << "Ps2_SetWDROutputMode  status" << status << endl;
	}
	
	status = Ps2_SetDataMode(deviceHandle, sessionIndex, PsWDR_Depth);
	if (status != PsRetOK)
	{
		cout << "Ps2_SetDataMode  status" << status << endl;
	}
	else
	{
		cout << "set dateMode WDR" << endl;
	}

	status = Ps2_SetWDRStyle(deviceHandle, sessionIndex, PsWDR_ALTERNATION);
	if (PsRetOK == status)
	{
		cout << "WDR image output alternatively in multi range." << endl;
	}
	else
	{
		cout << "Ps2_SetWDRStyle status : " << status << endl;
	}

	for (int i = 0; i < frameSpace; i++)
	{
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &FrameReady);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_ReadNextFrame failed status:" << status << endl;
			continue;
		}

		//Depth
		if (1 == FrameReady.depth) 
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsDepthFrame, &Depth);
			if (Depth.pFrameData != NULL)
			{
				cout << "get Frame successful,status:" << status << "  "
					<< "frameTpye:" << Depth.frameType << "  "
					<< "frameIndex:" << Depth.frameIndex << "  "
					<< "depthRange:" << Depth.depthRange << endl;
			}
			else
			{
				cout << "no frame";
			}
		}

	}


	//Stop capturing the image stream
	status = Ps2_StopStream(deviceHandle, sessionIndex);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_StopStream failed status:" << status << endl;
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
