#include <iostream>
#include <fstream>
#include "DCAM710/Vzense_api_710.h"
#include <thread>

using namespace std;
PsDeviceInfo* pDeviceListInfo = NULL;
PsDeviceHandle deviceHandle = 0;
uint32_t sessionIndex = 0;

bool InitDevice(const int deviceCount);
bool InitDevice(const int deviceCount)
{
	if (pDeviceListInfo)
	{
		delete[] pDeviceListInfo;
		pDeviceListInfo = NULL;

	}

	pDeviceListInfo = new PsDeviceInfo[deviceCount];
	PsReturnStatus status = Ps2_GetDeviceListInfo(pDeviceListInfo, deviceCount);
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

	deviceHandle = 0;

	status = Ps2_OpenDevice(pDeviceListInfo[0].uri, &deviceHandle);

	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "OpenDevice failed status:" <<status << endl;
		return false;
	}

	sessionIndex = 0;

	status = Ps2_StartStream(deviceHandle, sessionIndex);

	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "StartStream failed status:" <<status << endl;
		return false;
	}
	return true;
}

void HotPlugStateCallback(const PsDeviceInfo* pInfo, int params);
void HotPlugStateCallback(const PsDeviceInfo* pInfo, int status)
{
	cout << "uri " << status << "  " << pInfo->uri << "    " << (status == 0 ? "add" : "remove") << endl;
	cout << "alia " << status << "  " << pInfo->alias << "    " << (status == 0 ? "add" : "remove") << endl;

	if (status == 0)
	{
	
		cout << "Ps2_OpenDevice " << Ps2_OpenDevice(pInfo->uri, &deviceHandle) << endl;
		cout << "Ps2_StartStream " << Ps2_StartStream(deviceHandle, sessionIndex) << endl;
	}
	else
	{
		cout << "Ps2_StopStream " << Ps2_StopStream(deviceHandle, sessionIndex) << endl;
		cout << "Ps2_CloseDevice " << Ps2_CloseDevice(&deviceHandle) << endl;
	}
}
 
int main(int argc, char *argv[])
{
	uint32_t deviceCount = 0;

	PsReturnStatus status = Ps2_Initialize();
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "PsInitialize failed status:" <<status << endl;
		system("pause");
		return -1;
	}

GET:
	status = Ps2_GetDeviceCount(&deviceCount);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "PsGetDeviceCount failed status:" <<status << endl;
		system("pause");
		return -1;
	}
	cout << "Get device count: " << deviceCount << endl;
	if (0 == deviceCount)
	{
		this_thread::sleep_for(chrono::seconds(1));
		goto GET;
	}

	if (InitDevice(deviceCount))
	{
		status = Ps2_SetHotPlugStatusCallback(HotPlugStateCallback);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "SetHotPlugStatusCallback failed status:" <<status << endl;
 		}
		else
		{
			cout <<" wait for hotplug operation "<<endl;
			// wait for hotplug
			for (;;)
			{	
				this_thread::sleep_for(chrono::seconds(1));
			}
		}
		status = Ps2_CloseDevice(&deviceHandle);
		if (status != PsReturnStatus::PsRetOK)
		{
			cout << "CloseDevice failed status:" <<status << endl;
		}
	}
	status = Ps2_Shutdown();
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Shutdown failed status:" <<status << endl;
	} 
 
	delete[] pDeviceListInfo;
	pDeviceListInfo = NULL;

	return 0;
}

