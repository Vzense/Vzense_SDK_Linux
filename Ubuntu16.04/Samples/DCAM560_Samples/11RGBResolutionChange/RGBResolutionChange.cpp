#include <thread>
#include <iostream>
#include "DCAM560/Vzense_api_560.h"
#define frameSpace 10
using namespace std;

int main() {
	cout << "---RGBResolutionChange---\n";

	//about dev
	uint32_t deviceCount;
	PsDeviceInfo* pDeviceListInfo = NULL;
	PsDeviceHandle deviceHandle = 0;
	PsReturnStatus status = PsRetOthers;

	//about frame
	uint32_t sessionIndex = 0;
	PsFrameReady FrameReady = { 0 };
	PsFrame RGBFrame = { 0 };

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

	//switch RGBResolution
	PsResolution resolution = PsRGB_Resolution_640_480;
	//1.640_480
	resolution = PsRGB_Resolution_640_480;
	status = Ps2_SetRGBResolution(deviceHandle, sessionIndex, resolution);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_SetRGBResolution failed status:" <<status<< endl;
		return -1;
	}
	else
	{
		cout << "set to 640_480" << endl;
	}

	for (int i = 0; i < frameSpace; i++)
	{
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &FrameReady);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_ReadNextFrame failed status:" <<status<< endl;
			continue;
		}

		//cout resolution
		if (1 == FrameReady.rgb)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsRGBFrame, &RGBFrame);
			if (status == PsReturnStatus::PsRetOK && RGBFrame.pFrameData != NULL
			&& RGBFrame.width==640 )
			{
				cout << "get Frame successful,status:" << status << "  "
					 << "resolution： "<< RGBFrame.width  <<"x"<<RGBFrame.height<< endl;	
			}
		}

	}

	resolution = PsRGB_Resolution_1600_1200;
	//2.1600_1200
	status = Ps2_SetRGBResolution(deviceHandle, sessionIndex, resolution);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_SetRGBResolution failed status:" <<status<< endl;
		return -1;
	}
	else
	{
		cout << "set to 1600_1200" << endl;
	}

	for (int i = 0; i < frameSpace; i++)
	{
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &FrameReady);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "Ps2_ReadNextFrame failed status:" <<status<< endl;
			continue;
		}

		//cout resolution
		if (1 == FrameReady.rgb)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsRGBFrame, &RGBFrame);
			if (status == PsReturnStatus::PsRetOK && RGBFrame.pFrameData != NULL
						&& RGBFrame.width==1600 )

			{
				cout << "get Frame successful,status:" << status << "  "
					<< "resolution： "<< RGBFrame.width  <<"x"<<RGBFrame.height<< endl;	
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
