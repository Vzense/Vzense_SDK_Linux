#include <thread>
#include <iostream>
#include "DCAM560/Vzense_api_560.h"
using namespace std;

int main() {
	cout << "---DeviceSearchAndConnect---\n";

	uint32_t deviceCount;
	PsDeviceInfo* pDeviceInfo = NULL;
	PsDeviceHandle deviceHandle = 0;
	PsReturnStatus status = PsRetOthers;

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

	pDeviceInfo = new PsDeviceInfo;
	uint32_t deviceIndex = 0;

	//Ps2_GetDeviceInfo is similar to Ps2_GetDeviceListInfo,but hasn't list.
	status = Ps2_GetDeviceInfo(pDeviceInfo, deviceIndex);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "GetDeviceListInfo failed status:" <<status<< endl;
		return -1;
	}

	cout << "uri:" << pDeviceInfo->uri << endl
		<< "alias:" << pDeviceInfo->alias << endl
		<< "connectStatus:" << pDeviceInfo->status << endl;

	status = Ps2_OpenDevice(pDeviceInfo->uri, &deviceHandle);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "OpenDevice failed status:" <<status << endl;
		return -1;
	}
	cout << "open device successful,status :" << status << endl;

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
	cout << "--end--";

	return 0;
}
