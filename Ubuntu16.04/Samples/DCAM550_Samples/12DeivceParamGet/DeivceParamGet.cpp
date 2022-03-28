#include <thread>
#include <iostream>
#include "DCAM550/Vzense_api_550.h"

using namespace std;

int main() {
	cout << "---DeivceParamGet---\n";

	//about dev
	uint32_t deviceCount;
	PsDeviceInfo* pDeviceListInfo = NULL;
	PsDeviceHandle deviceHandle = 0;
	PsReturnStatus status = PsRetOthers;
	
	uint32_t sessionIndex = 0;
 
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

	//cameraParameters
	PsCameraParameters cameraParameters;
	status = Ps2_GetCameraParameters(deviceHandle, sessionIndex, PsDepthSensor, &cameraParameters);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_GetCameraParameters failed status:" << status << endl;
		return -1;
	}
	cout << endl;
	cout << "Get PsGetCameraParameters status: " << status << endl;
	cout << "Depth Camera Intinsic:" << endl;
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
	cout << endl;

	//Threshold
	uint16_t Threshold = 0;
	status = Ps2_GetThreshold(deviceHandle, sessionIndex, &Threshold);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_GetThreshold failed status:" << status << endl;
		return -1;
	}
	cout << "Threshold: " << Threshold << endl;
	cout << endl;

	//PulseCount
	uint16_t PulseCount = 0;
	status = Ps2_GetPulseCount(deviceHandle, sessionIndex, &PulseCount);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_GetPulseCount failed status:" << status << endl;
		return -1;
	}
	cout << "PulseCount: " << PulseCount << endl;
	cout << endl;

	//gmmgain
	uint16_t gmmgain = 0;
	status = Ps2_GetGMMGain(deviceHandle, sessionIndex, &gmmgain);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_GetGMMGain failed status:" << status << endl;
		return -1;
	}
	cout << "gmmgain: " << gmmgain << endl;
	cout << endl;

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

	cout << "--end--";
	return 0;
}
