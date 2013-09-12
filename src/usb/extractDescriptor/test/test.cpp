#include<stdio.h>

#include "uvcview.h"
#include "vndrlist.h"
#include "langidlist.h"

//Version information printed at lower left of UI Window and top of output text window
#define USBVIEW_MAJOR_VERSION   2
#define USBVIEW_MINOR_VERSION   0
#define UVC_SPEC_MAJOR_VERSION  1
#define UVC_SPEC_MINOR_VERSION  5

#define NUM_STRING_DESC_TO_GET 32

#pragma comment (lib, "Setupapi.lib")

LIST_ENTRY EnumeratedHCListHead =
{
    &EnumeratedHCListHead,
    &EnumeratedHCListHead
};

DEVICE_GUID_LIST gHubList;
DEVICE_GUID_LIST gDeviceList;

PCHAR ConnectionStatuses[] =
{
    "",                   // 0  - NoDeviceConnected
    "",                   // 1  - DeviceConnected
    "FailedEnumeration",  // 2  - DeviceFailedEnumeration
    "GeneralFailure",     // 3  - DeviceGeneralFailure
    "Overcurrent",        // 4  - DeviceCausedOvercurrent
    "NotEnoughPower",     // 5  - DeviceNotEnoughPower
    "NotEnoughBandwidth", // 6  - DeviceNotEnoughBandwidth
    "HubNestedTooDeeply", // 7  - DeviceHubNestedTooDeeply
    "InLegacyHub",        // 8  - DeviceInLegacyHub
    "Enumerating",        // 9  - DeviceEnumerating
    "Reset"               // 10 - DeviceReset
};

ULONG TotalDevicesConnected;

BOOL gDoAnnotation;

HWND            ghTreeWnd        = NULL;

int             giComputer       = 0;
int             giHub            = 0;
int             giNoDevice       = 0;
int             giGoodSsDevice   = 0;
int             giGoodDevice     = 0;
int             giNoSsDevice     = 0;
int             giBadDevice      = 0;

int gDeviceSpeed;

void
EnumerateAllDevices();

void
EnumerateAllDevicesWithGuid(
    PDEVICE_GUID_LIST DeviceList, 
    LPGUID Guid
    );

VOID
EnumerateHostController (
    HANDLE                   hHCDev,    _Inout_ PCHAR            leafName,
    _In_    HANDLE           deviceInfo,
    _In_    PSP_DEVINFO_DATA deviceInfoData
);

VOID
FreeDeviceInfoNode(
    _In_ PDEVICE_INFO_NODE *ppNode
    );

PCHAR GetHCDDriverKeyName (
    HANDLE  HCD
);

PUSB_DEVICE_PNP_STRINGS 
DriverNameToDeviceProperties( 
        _In_reads_bytes_(cbDriverName) PCHAR  DriverName, 
        _In_ size_t cbDriverName
        );

DWORD 
GetHostControllerPowerMap(
    HANDLE hHCDev, 
    PUSBHOSTCONTROLLERINFO hcInfo
	);

DWORD 
GetHostControllerInfo(
    HANDLE hHCDev, 
    PUSBHOSTCONTROLLERINFO hcInfo
	);

PCHAR GetRootHubName (
    HANDLE HostController
);

VOID
EnumerateHub (
    _In_reads_(cbHubName) PCHAR                     HubName,
    _In_ size_t                                     cbHubName,
    _In_opt_ PUSB_NODE_CONNECTION_INFORMATION_EX    ConnectionInfo,
    _In_opt_ PUSB_NODE_CONNECTION_INFORMATION_EX_V2 ConnectionInfoV2,
    _In_opt_ PUSB_PORT_CONNECTOR_PROPERTIES         PortConnectorProps,
    _In_opt_ PUSB_DESCRIPTOR_REQUEST                ConfigDesc,
    _In_opt_ PUSB_DESCRIPTOR_REQUEST                BosDesc,
    _In_opt_ PSTRING_DESCRIPTOR_NODE                StringDescs,
    _In_opt_ PUSB_DEVICE_PNP_STRINGS                DevProps
);

PCHAR WideStrToMultiStr ( 
                         _In_reads_bytes_(cbWideStr) PWCHAR WideStr, 
                         _In_ size_t                   cbWideStr
                         );

BOOL 
DriverNameToDeviceInst(
    _In_reads_bytes_(cbDriverName) PCHAR DriverName,
    _In_ size_t cbDriverName,
    _Out_ HDEVINFO *pDevInfo,
    _Out_writes_bytes_(sizeof(SP_DEVINFO_DATA)) PSP_DEVINFO_DATA pDevInfoData
    );

VOID FreeDeviceProperties(_In_ PUSB_DEVICE_PNP_STRINGS *ppDevProps);

VOID
EnumerateHubPorts (
    HANDLE      hHubDevice,
    ULONG       NumPorts
);

PCHAR GetDriverKeyName (
    HANDLE  Hub,
    ULONG   ConnectionIndex
);

PDEVICE_INFO_NODE
FindMatchingDeviceNodeForDriverName(
    _In_ PSTR   DriverKeyName,
    _In_ BOOLEAN IsHub
    );

PUSB_DESCRIPTOR_REQUEST
GetConfigDescriptor (
    HANDLE  hHubDevice,
    ULONG   ConnectionIndex,
    UCHAR   DescriptorIndex
);

PUSB_DESCRIPTOR_REQUEST
GetBOSDescriptor (
    HANDLE  hHubDevice,
    ULONG   ConnectionIndex
);

BOOL
AreThereStringDescriptors (
    PUSB_DEVICE_DESCRIPTOR          DeviceDesc,
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc
);

PSTRING_DESCRIPTOR_NODE
GetAllStringDescriptors (
    HANDLE                          hHubDevice,
    ULONG                           ConnectionIndex,
    PUSB_DEVICE_DESCRIPTOR          DeviceDesc,
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc
);

PCHAR GetExternalHubName (
    HANDLE  Hub,
    ULONG   ConnectionIndex
);

PSTRING_DESCRIPTOR_NODE
GetStringDescriptor (
    HANDLE  hHubDevice,
    ULONG   ConnectionIndex,
    UCHAR   DescriptorIndex,
    USHORT  LanguageID
);

HRESULT
GetStringDescriptors (
    _In_ HANDLE                         hHubDevice,
    _In_ ULONG                          ConnectionIndex,
    _In_ UCHAR                          DescriptorIndex,
    _In_ ULONG                          NumLanguageIDs,
    _In_reads_(NumLanguageIDs) USHORT  *LanguageIDs,
    _In_ PSTRING_DESCRIPTOR_NODE        StringDescNodeHead
);

// 출력관련 함수들

VOID
DisplayConnectionInfo (
    _In_     PUSB_NODE_CONNECTION_INFORMATION_EX    ConnectInfo,
    _In_     PUSBDEVICEINFO                         info,
    _In_     PSTRING_DESCRIPTOR_NODE                StringDescs,
    _In_opt_ PUSB_NODE_CONNECTION_INFORMATION_EX_V2 ConnectionInfoV2
);

void
InitializePerDeviceSettings (
    PUSBDEVICEINFO info
    );

VOID
DisplayUSEnglishStringDescriptor (
    UCHAR                       Index,
    PSTRING_DESCRIPTOR_NODE     USStringDescs,
    DEVICE_POWER_STATE          LatestDevicePowerState
    );

UINT
IsIADDevice (
    PUSBDEVICEINFO info
    );

UINT
IsIADDevice (
    PUSBDEVICEINFO info
    );

PCHAR
GetVendorString (
    USHORT     idVendor
    );

VOID
DisplayStringDescriptor (
    UCHAR                    Index,
    PSTRING_DESCRIPTOR_NODE  StringDescs,
    DEVICE_POWER_STATE       LatestDevicePowerState
    );

VOID
DisplayPipeInfo (
    ULONG           NumPipes,
    USB_PIPE_INFO  *PipeInfo
    );

UINT
IsUVCDevice (
    PUSBDEVICEINFO info
    );

PCHAR
GetLangIDString (
    USHORT     idLang
    );


VOID
DisplayEndpointDescriptor (
    _In_     PUSB_ENDPOINT_DESCRIPTOR
                        EndpointDesc,
    _In_opt_ PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR 
                        EpCompDesc,
    _In_     UCHAR      InterfaceClass,
    _In_     BOOLEAN    EpCompDescAvail
    );

VOID
DisplayEndointCompanionDescriptor (
    PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR EpCompDesc,
    UCHAR                                         DescType
    );
	

int main()
{
	CHAR  statusText[128];
	ULONG devicesConnected;
	int i = 0;

	gHubList.DeviceInfo = INVALID_HANDLE_VALUE;
    InitializeListHead(&gHubList.ListHead);
    gDeviceList.DeviceInfo = INVALID_HANDLE_VALUE;
    InitializeListHead(&gDeviceList.ListHead);

	for( i  = 0 ; i < 2 ; i++ )
	{
		// 모든 USB 디바이스 열거
		//
		EnumerateHostControllers(&devicesConnected);

		// 상태 업데이트 및 출력
		//
		memset(statusText, 0, sizeof(statusText));
		StringCchPrintf(statusText, sizeof(statusText),
	#ifdef H264_SUPPORT
		"UVC Spec Version: %d.%d Version: %d.%d Devices Connected: %d   Hubs Connected: %d",
		UVC_SPEC_MAJOR_VERSION, UVC_SPEC_MINOR_VERSION, USBVIEW_MAJOR_VERSION, USBVIEW_MINOR_VERSION,
		devicesConnected, TotalHubs);
	#else
		"Devices Connected: %d   Hubs Connected: %d",
		devicesConnected, TotalHubs);
	#endif

		printf("%s", statusText);

		getchar();
	}

	return 0;
}

//*****************************************************************************
//
// EnumerateHostControllers()
//
// hTreeParent - Handle of the TreeView item under which host controllers
// should be added.
//
//*****************************************************************************

VOID
EnumerateHostControllers (
    ULONG     *DevicesConnected
)
{
    HANDLE                           hHCDev = NULL;
    HDEVINFO                         deviceInfo = NULL;
    SP_DEVINFO_DATA                  deviceInfoData;
    SP_DEVICE_INTERFACE_DATA         deviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA deviceDetailData = NULL;
    ULONG                            index = 0;
    ULONG                            requiredLength = 0;
    BOOL                             success;

    TotalDevicesConnected = 0;
    TotalHubs = 0;

	// USB 허브 디바이스를 포함한 모든 USB 관련 Device 열거
	//
    EnumerateAllDevices();

    // GUID 기반 인터페이스를 사용하여 호스트 컨트롤러를 반복
    // Handle 반환
	//
    deviceInfo = SetupDiGetClassDevs((LPGUID)&GUID_CLASS_USB_HOST_CONTROLLER,
                                     NULL,
                                     NULL,
                                     (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (index=0;
         SetupDiEnumDeviceInfo(deviceInfo,
                               index,
                               &deviceInfoData); // 디바이스 정보 요소를 명시하는 구조체 반환
         index++)
    {
        deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        success = SetupDiEnumDeviceInterfaces(deviceInfo,
                                              0,
                                              (LPGUID)&GUID_CLASS_USB_HOST_CONTROLLER,
                                              index,
                                              &deviceInterfaceData); // 디바이스 interfaces 를 열거
        if (!success)
        {
            OOPS();
            break;
        }

        success = SetupDiGetDeviceInterfaceDetail(deviceInfo,
                                                  &deviceInterfaceData,
                                                  NULL,
                                                  0,
                                                  &requiredLength,
                                                  NULL); // 디바이스 interface의 정보를 읽어온다. 크기와 함께

        if (!success && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            OOPS();
            break;
        }

        deviceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)ALLOC(requiredLength);
        if (deviceDetailData == NULL)
        {
            OOPS();
            break;
        }

        deviceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        success = SetupDiGetDeviceInterfaceDetail(deviceInfo,
                                                  &deviceInterfaceData,
                                                  deviceDetailData,
                                                  requiredLength,
                                                  &requiredLength,
                                                  NULL); // 디바이스의 경로를 가져온다.

        if (!success)
        {
            OOPS();
            break;
        }

        hHCDev = CreateFile(deviceDetailData->DevicePath,
                            GENERIC_WRITE, // 읽기? 쓰기?
                            FILE_SHARE_WRITE, // 동시에 쓰기가능
                            NULL,
                            OPEN_EXISTING, // 있으면 열어
                            0,
                            NULL); // 해당 Path에 있는 디바이스의 Handle을 얻는다.

        // Handle 얻기를 성공한 경우 그 디바이스의 RootHub의 내용을 가져온다.
		//
        if (hHCDev != INVALID_HANDLE_VALUE)
        {
			
            EnumerateHostController(hHCDev,
                                    deviceDetailData->DevicePath,
                                    deviceInfo,
                                    &deviceInfoData);
			

            CloseHandle(hHCDev);
        }

        FREE(deviceDetailData);
    }

    SetupDiDestroyDeviceInfoList(deviceInfo); // deviceInfo 메모리 free

    *DevicesConnected = TotalDevicesConnected;

    return;
}

void
EnumerateAllDevices()
{
	// GUID(모든 하드웨어에 뷰여된 ID) 를 바탕으로 디바이스 모두 열거
	// 1. 디바이스
	// 2. 허브 
	//
    EnumerateAllDevicesWithGuid(&gDeviceList, 
                                (LPGUID)&GUID_DEVINTERFACE_USB_DEVICE); // 1

    EnumerateAllDevicesWithGuid(&gHubList, 
                                (LPGUID)&GUID_DEVINTERFACE_USB_HUB); // 2
}

void
EnumerateAllDevicesWithGuid(
    PDEVICE_GUID_LIST DeviceList, 
    LPGUID Guid
    )
{
	// 디바이스 List가 이용가능하지 않다면 List Clear
	//
    if (DeviceList->DeviceInfo != INVALID_HANDLE_VALUE)
    {
        ClearDeviceList(DeviceList);
    }

	// Device 정보 들을 다루기 위한 Handle을 return 
	// DIGCF_PRESENT: 실제 시스템에 보여지는 것
	// DIGCF_DEVICEINTERFACE : Device를 돕기위한 Device
	//
    DeviceList->DeviceInfo = SetupDiGetClassDevs(Guid,
                                     NULL,
                                     NULL,
                                     (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

    if (DeviceList->DeviceInfo != INVALID_HANDLE_VALUE) // Device 이용가능한 경우
    {
        ULONG                    index;
        DWORD error;

        error = 0;
        index = 0;

        while (error != ERROR_NO_MORE_ITEMS)
        {
            BOOL success;
            PDEVICE_INFO_NODE pNode;

            pNode = (PDEVICE_INFO_NODE)ALLOC(sizeof(DEVICE_INFO_NODE));
            if (pNode == NULL)
            {
                OOPS();
                break;
            }
            pNode->DeviceInfo = DeviceList->DeviceInfo;
            pNode->DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            pNode->DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

			// 디바이스의 정보 요소들로 구성된 구조체를 pNode->DeviceInfoData 에 넣어준다.
			// 반환 : True, False
			//
            success = SetupDiEnumDeviceInfo(DeviceList->DeviceInfo,
                                            index,
                                            &pNode->DeviceInfoData);

            index++;


            if (success == FALSE)
            {
                error = GetLastError();

                if (error != ERROR_NO_MORE_ITEMS)
                {
                    OOPS();
                }

                FreeDeviceInfoNode(&pNode);
            }
            else // 정상 루틴
            {
                BOOL   bResult;
                ULONG  requiredLength;

				// Device Descriptor를 pNode->DeviceDescNmae 로 읽어온다.
				//
                bResult = GetDeviceProperty(DeviceList->DeviceInfo,
                                            &pNode->DeviceInfoData,
                                            SPDRP_DEVICEDESC,
                                            &pNode->DeviceDescName);
                if (bResult == FALSE)
                {
                    FreeDeviceInfoNode(&pNode);
                    OOPS();
                    break;
                }

				// Device Descriptor를 pNode->DeviceDriverNmae 로 읽어온다.
				//
                bResult = GetDeviceProperty(DeviceList->DeviceInfo,
                                            &pNode->DeviceInfoData,
                                            SPDRP_DRIVER,
                                            &pNode->DeviceDriverName);
				
                if (bResult == FALSE)
                {
                    FreeDeviceInfoNode(&pNode);
                    OOPS();
                    break;
                }

                pNode->DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        
                success = SetupDiEnumDeviceInterfaces(DeviceList->DeviceInfo,
                                                      0,
                                                      Guid,
                                                      index-1,
                                                      &pNode->DeviceInterfaceData); // 디바이스 interface 열거 함수
                if (!success)
                {
                    FreeDeviceInfoNode(&pNode);
                    OOPS();
                    break;
                }
             
                success = SetupDiGetDeviceInterfaceDetail(DeviceList->DeviceInfo,
                                                          &pNode->DeviceInterfaceData,
                                                          NULL,
                                                          0,
                                                          &requiredLength,
                                                          NULL); // 디바이스 interface에 대해 data 길이 반환
        
                error = GetLastError();
                
                if (!success && error != ERROR_INSUFFICIENT_BUFFER)
                {
                    FreeDeviceInfoNode(&pNode);
                    OOPS();
                    break;
                }
                
                pNode->DeviceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)ALLOC(requiredLength);

                if (pNode->DeviceDetailData == NULL)
                {
                    FreeDeviceInfoNode(&pNode);
                    OOPS();
                    break;
                }
                
                pNode->DeviceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
                
                success = SetupDiGetDeviceInterfaceDetail(DeviceList->DeviceInfo,
                                                          &pNode->DeviceInterfaceData,
                                                          pNode->DeviceDetailData,
                                                          requiredLength,
                                                          &requiredLength,
                                                          NULL); // 디바이스 Path 읽어온다.
                if (!success)
                {
                    FreeDeviceInfoNode(&pNode);
                    OOPS();
                    break;
                }        
        
                InsertTailList(&DeviceList->ListHead, &pNode->ListEntry);
            }
        }
    }
}

VOID
EnumerateHostController (
    HANDLE                   hHCDev,    _Inout_ PCHAR            leafName,
    _In_    HANDLE           deviceInfo,
    _In_    PSP_DEVINFO_DATA deviceInfoData
)
{
    PCHAR                   driverKeyName = NULL;
    PCHAR                   rootHubName = NULL;
    PLIST_ENTRY             listEntry = NULL;
    PUSBHOSTCONTROLLERINFO  hcInfo = NULL;
    PUSBHOSTCONTROLLERINFO  hcInfoInList = NULL;
    DWORD                   dwSuccess;
    BOOL                    success = FALSE;
    ULONG                   deviceAndFunction = 0;
    PUSB_DEVICE_PNP_STRINGS DevProps = NULL;


    // 메모리 할당
    //
    hcInfo = (PUSBHOSTCONTROLLERINFO)ALLOC(sizeof(USBHOSTCONTROLLERINFO));

    // 메모리 할당 실패
	//
    if (NULL == hcInfo)
        return;

    hcInfo->DeviceInfoType = HostControllerInfo; // enum -> 0

    // host controller로 부터 해당 드라이버 key 이름을 가져온다.
    //
    driverKeyName = GetHCDDriverKeyName(hHCDev);

    if (NULL == driverKeyName)
    {
        OOPS();
        FREE(hcInfo);
        return;
    }

    // 이미 존재할 경우 다시 열거 하지 않는다.
    //
    listEntry = EnumeratedHCListHead.Flink;

    while (listEntry != &EnumeratedHCListHead)
    {
        hcInfoInList = CONTAINING_RECORD(listEntry,
                                         USBHOSTCONTROLLERINFO,
                                         ListEntry);
		/*
        if (strcmp(driverKeyName, hcInfoInList->DriverKey) == 0)
        {
            // 이미 리스트에 존재 즉, 열거 안함
            //
            FREE(driverKeyName);
            FREE(hcInfo);
            return;
        }
		*/
        listEntry = listEntry->Flink;
    }

    // host controller device 속성들을 얻음
    {
        size_t cbDriverName = 0;
        HRESULT hr = S_OK;

        hr = StringCbLength(driverKeyName, MAX_DRIVER_KEY_NAME, &cbDriverName);
        if (SUCCEEDED(hr))
        {
            DevProps = DriverNameToDeviceProperties(driverKeyName, cbDriverName);
        }
    }

	// 드라이버 키를 얻음 
	//
    hcInfo->DriverKey = driverKeyName;

    if (DevProps)
    {
        ULONG   ven, dev, subsys, rev;
        ven = dev = subsys = rev = 0;

		// DeviceId를 얻음
		//
        if (sscanf_s(DevProps->DeviceId,
                   "PCI\\VEN_%x&DEV_%x&SUBSYS_%x&REV_%x",
                   &ven, &dev, &subsys, &rev) != 4)
        {
            OOPS();
        }
		
		// 기타 정보들
		//
        hcInfo->VendorID = ven;
        hcInfo->DeviceID = dev;
        hcInfo->SubSysID = subsys;
        hcInfo->Revision = rev;
        hcInfo->UsbDeviceProperties = DevProps;
    }
    else
    {
        OOPS();
    }

    if (DevProps != NULL && DevProps->DeviceDesc != NULL)
    {
        leafName = DevProps->DeviceDesc;
    }
    else
    {
        OOPS();
    }

    // USB Host Controller의 power map을 얻는다.
	//
    dwSuccess = GetHostControllerPowerMap(hHCDev, hcInfo);

    if (ERROR_SUCCESS != dwSuccess)
    {
        OOPS();
    }


    // 버스, 디바이스 그리고 함수를 얻는다.
    //
    hcInfo->BusDeviceFunctionValid = FALSE;

	// 버스 얻는 부분
	//
    success = SetupDiGetDeviceRegistryProperty(deviceInfo,
                                               deviceInfoData,
                                               SPDRP_BUSNUMBER,
                                               NULL,
                                               (PBYTE)&hcInfo->BusNumber,
                                               sizeof(hcInfo->BusNumber),
                                               NULL);

    if (success)
    {
		// 디바이스의 주소를 얻는 부분
		//
        success = SetupDiGetDeviceRegistryProperty(deviceInfo,
                                                   deviceInfoData,
                                                   SPDRP_ADDRESS,
                                                   NULL,
                                                   (PBYTE)&deviceAndFunction,
                                                   sizeof(deviceAndFunction),
                                                   NULL);
    }

    if (success)
    {
		// 함수를 얻는 부분
        hcInfo->BusDevice = deviceAndFunction >> 16;
        hcInfo->BusFunction = deviceAndFunction & 0xffff;
        hcInfo->BusDeviceFunctionValid = TRUE;
    }

    // USB Host Controller info를 얻는 부분
    dwSuccess = GetHostControllerInfo(hHCDev, hcInfo);

    if (ERROR_SUCCESS != dwSuccess)
    {
        OOPS();
    }
	
    // 열거된 host controller들을 host controller 리스트에 추가
    //
    InsertTailList(&EnumeratedHCListHead,
                   &hcInfo->ListEntry);

    // Root Hub의 이름을 얻는 부분
    // host controller와 root hub를 열거하기 위함
    //
    rootHubName = GetRootHubName(hHCDev);

    if (rootHubName != NULL)
    {
        size_t cbHubName = 0;
        HRESULT hr = S_OK;

        hr = StringCbLength(rootHubName, MAX_DRIVER_KEY_NAME, &cbHubName);
        if (SUCCEEDED(hr))
        {
            EnumerateHub(rootHubName,
                         cbHubName,
                         NULL,       // ConnectionInfo
                         NULL,       // ConnectionInfoV2
                         NULL,       // PortConnectorProps
                         NULL,       // ConfigDesc
                         NULL,       // BosDesc
                         NULL,       // StringDescs
                         NULL);      // We do not pass DevProps for RootHub
        }
    }
    else
    {
        // Failure obtaining root hub name.

        OOPS();
    }

    return;
}

VOID
FreeDeviceInfoNode(
    _In_ PDEVICE_INFO_NODE *ppNode
    )
{
    if (ppNode == NULL)
    {
        return;
    }

    if (*ppNode == NULL)
    {
        return;
    }

    if ((*ppNode)->DeviceDetailData != NULL)
    {
        FREE((*ppNode)->DeviceDetailData);
    }

    if ((*ppNode)->DeviceDescName != NULL)
    {
        FREE((*ppNode)->DeviceDescName);
    }

    if ((*ppNode)->DeviceDriverName != NULL)
    {
        FREE((*ppNode)->DeviceDriverName);
    }

    FREE(*ppNode);
    *ppNode = NULL;
}

void
ClearDeviceList(
    PDEVICE_GUID_LIST DeviceList
    )
{
    if (DeviceList->DeviceInfo != INVALID_HANDLE_VALUE)
    {
        SetupDiDestroyDeviceInfoList(DeviceList->DeviceInfo);
        DeviceList->DeviceInfo = INVALID_HANDLE_VALUE;
    }

    while (!IsListEmpty(&DeviceList->ListHead))
    {
        PDEVICE_INFO_NODE pNode = NULL;
        PLIST_ENTRY pEntry;

        pEntry = RemoveHeadList(&DeviceList->ListHead);

        pNode = CONTAINING_RECORD(pEntry,
                                  DEVICE_INFO_NODE,
                                  ListEntry);

        //FreeDeviceInfoNode(&pNode);
    }
}

_Success_(return == TRUE)
BOOL
GetDeviceProperty(
    _In_    HDEVINFO         DeviceInfoSet,
    _In_    PSP_DEVINFO_DATA DeviceInfoData,
    _In_    DWORD            Property,
    _Outptr_  LPTSTR        *ppBuffer
    )
{
    BOOL bResult;
    DWORD requiredLength = 0;
    DWORD lastError;

    if (ppBuffer == NULL)
    {
        return FALSE;
    }

    *ppBuffer = NULL;

    bResult = SetupDiGetDeviceRegistryProperty(DeviceInfoSet,
                                               DeviceInfoData,
                                               Property ,
                                               NULL,
                                               NULL,
                                               0,
                                               &requiredLength); // Plug and Play 디바이스 속성을 검색하여 requiredLength를 읽어온다.
    lastError = GetLastError();

    if ((requiredLength == 0) || (bResult != FALSE && lastError != ERROR_INSUFFICIENT_BUFFER))
    {
        return FALSE;
    }

    *ppBuffer = (LPTSTR)ALLOC(requiredLength);

    if (*ppBuffer == NULL)
    {
        return FALSE;
    }

    bResult = SetupDiGetDeviceRegistryProperty(DeviceInfoSet,
                                                DeviceInfoData,
                                                Property ,
                                                NULL,
                                                (PBYTE) *ppBuffer,
                                                requiredLength,
                                                &requiredLength); // 다시 한번 호출해서 해당 디바이스의 속성의 실제 내용을 읽어온다.
    if(bResult == FALSE)
    {
        FREE(*ppBuffer);
        *ppBuffer = NULL;
        return FALSE;
    }

    return TRUE;
}

PCHAR GetHCDDriverKeyName (
    HANDLE  HCD
)
{
    BOOL                    success = 0;
    ULONG                   nBytes = 0;
    USB_HCD_DRIVERKEY_NAME  driverKeyName = {0};
    PUSB_HCD_DRIVERKEY_NAME driverKeyNameW = NULL;
    PCHAR                   driverKeyNameA = NULL;

    ZeroMemory(&driverKeyName, sizeof(driverKeyName));

    // Get the length of the name of the driver key of the HCD
    //
    success = DeviceIoControl(HCD,
                              IOCTL_GET_HCD_DRIVERKEY_NAME, // 해당 함수 이름
                              &driverKeyName, // inputbuffer의 주소
                              sizeof(driverKeyName), // inputbuffer 크기
                              &driverKeyName, // outputbuffer의 주소
                              sizeof(driverKeyName), // outputbuffer 크기
                              &nBytes,
                              NULL); // 제어 코드를 직접적으로 해당 디바이스에 전송

    if (!success)
    {
        OOPS();
        goto GetHCDDriverKeyNameError;
    }

    nBytes = driverKeyName.ActualLength;
    if (nBytes <= sizeof(driverKeyName))
    {
        OOPS();
        goto GetHCDDriverKeyNameError;
    }

    // 메모리 할당
    driverKeyNameW = (PUSB_HCD_DRIVERKEY_NAME)ALLOC(nBytes + 1);
    if (driverKeyNameW == NULL)
    {
        OOPS();
        goto GetHCDDriverKeyNameError;
    }

    success = DeviceIoControl(HCD,
                              IOCTL_GET_HCD_DRIVERKEY_NAME,
                              driverKeyNameW,
                              nBytes,
                              driverKeyNameW,
                              nBytes,
                              &nBytes,
                              NULL);
    if (!success)
    {
        OOPS();
        goto GetHCDDriverKeyNameError;
    }

    // Convert the driver key name
    //
    driverKeyNameA = WideStrToMultiStr(driverKeyNameW->DriverKeyName, nBytes);

    // All done, free the uncoverted driver key name and return the
    // converted driver key name
    //
    FREE(driverKeyNameW);

    return driverKeyNameA;

GetHCDDriverKeyNameError:
    // There was an error, free anything that was allocated
    //
    if (driverKeyNameW != NULL)
    {
        FREE(driverKeyNameW);
        driverKeyNameW = NULL;
    }

    return NULL;
}

/*****************************************************************************

  DriverNameToDeviceProperties()

  Returns the Device properties of the DevNode with the matching DriverName.
  Returns NULL if the matching DevNode is not found.

  The caller should free the returned structure using FREE() macro

 *****************************************************************************/
PUSB_DEVICE_PNP_STRINGS 
DriverNameToDeviceProperties( 
        _In_reads_bytes_(cbDriverName) PCHAR  DriverName, 
        _In_ size_t cbDriverName
        )
{
    HDEVINFO        deviceInfo = INVALID_HANDLE_VALUE;
    SP_DEVINFO_DATA deviceInfoData = {0};
    ULONG           len;
    BOOL            status;
    PUSB_DEVICE_PNP_STRINGS DevProps = NULL;
    DWORD           lastError;

    // 메모리 할당
    DevProps = (PUSB_DEVICE_PNP_STRINGS) ALLOC(sizeof(USB_DEVICE_PNP_STRINGS));

    if(NULL == DevProps)
    {
        status = FALSE;
        goto Done;
    }

    // device의 instance를 얻음
    status = DriverNameToDeviceInst(DriverName, cbDriverName, &deviceInfo, &deviceInfoData);
    if (status == FALSE)
    {
        goto Done;
    }

    len = 0;
    status = SetupDiGetDeviceInstanceId(deviceInfo,
                                        &deviceInfoData,
                                        NULL,
                                        0,
                                        &len);
    lastError = GetLastError();

    
    if (status != FALSE && lastError != ERROR_INSUFFICIENT_BUFFER)
    {
        status = FALSE;
        goto Done;
    }
    
    //
    // An extra byte is required for the terminating character
    //
    
    len++;
    DevProps->DeviceId = (PCHAR)ALLOC(len);

    if (DevProps->DeviceId == NULL)
    {
        status = FALSE;
        goto Done;
    }

    status = SetupDiGetDeviceInstanceId(deviceInfo,
                                        &deviceInfoData,
                                        DevProps->DeviceId,
                                        len,
                                        &len);
    if (status == FALSE)
    {
        goto Done;
    }

    status = GetDeviceProperty(deviceInfo,
                               &deviceInfoData,
                               SPDRP_DEVICEDESC,
                               &DevProps->DeviceDesc);
    
    if (status == FALSE)
    {
        goto Done;
    }
    

    //    
    // We don't fail if the following registry query fails as these fields are additional information only
    //

    GetDeviceProperty(deviceInfo,
                      &deviceInfoData,
                      SPDRP_HARDWAREID,
                      &DevProps->HwId);

    GetDeviceProperty(deviceInfo,
                      &deviceInfoData,
                      SPDRP_SERVICE,
                      &DevProps->Service);

    GetDeviceProperty(deviceInfo,
                       &deviceInfoData,
                       SPDRP_CLASS,
                       &DevProps->DeviceClass);
Done:

    if (deviceInfo != INVALID_HANDLE_VALUE)
    {
        SetupDiDestroyDeviceInfoList(deviceInfo);
    }

    if (status == FALSE)
    {
        if (DevProps != NULL)
        {
            FreeDeviceProperties(&DevProps);
        }
    }
    return DevProps;
}

//*****************************************************************************
//
// GetHostControllerPowerMap()
//
// HANDLE hHCDev
//      - handle to USB Host Controller
//
// PUSBHOSTCONTROLLERINFO hcInfo
//      - data structure to receive the Power Map Info
//
// return DWORD dwError
//      - return ERROR_SUCCESS or last error
//
//*****************************************************************************

DWORD 
GetHostControllerPowerMap(
    HANDLE hHCDev, 
    PUSBHOSTCONTROLLERINFO hcInfo)
{
    USBUSER_POWER_INFO_REQUEST UsbPowerInfoRequest;
    PUSB_POWER_INFO            pUPI = &UsbPowerInfoRequest.PowerInformation ;
    DWORD                      dwError = 0;
    DWORD                      dwBytes = 0;
    BOOL                       bSuccess = FALSE;
    int                        nIndex = 0;
    int                        nPowerState = WdmUsbPowerSystemWorking;

    for ( ; nPowerState <= WdmUsbPowerSystemShutdown; nIndex++, nPowerState++) 
    {
        // zero initialize our request
        memset(&UsbPowerInfoRequest, 0, sizeof(UsbPowerInfoRequest));

        // set the header and request sizes
        UsbPowerInfoRequest.Header.UsbUserRequest = USBUSER_GET_POWER_STATE_MAP;
        UsbPowerInfoRequest.Header.RequestBufferLength = sizeof(UsbPowerInfoRequest);
        UsbPowerInfoRequest.PowerInformation.SystemState = (WDMUSB_POWER_STATE)nPowerState;

        //
        // Now query USBHUB for the USB_POWER_INFO structure for this hub.
        // For Selective Suspend support
        //
        bSuccess = DeviceIoControl(hHCDev,
                                  IOCTL_USB_USER_REQUEST,
                                  &UsbPowerInfoRequest,
                                  sizeof(UsbPowerInfoRequest),
                                  &UsbPowerInfoRequest,
                                  sizeof(UsbPowerInfoRequest),
                                  &dwBytes,
                                  NULL);

        if (!bSuccess)
        {
            dwError = GetLastError();
            OOPS();
        }
        else
        {
            // copy the data into our USB Host Controller's info structure
            memcpy( &(hcInfo->USBPowerInfo[nIndex]), pUPI, sizeof(USB_POWER_INFO));
        }
    }

    return dwError;
}

//*****************************************************************************
//
// GetHostControllerInfo()
//
// HANDLE hHCDev
//      - handle to USB Host Controller
//
// PUSBHOSTCONTROLLERINFO hcInfo
//      - data structure to receive the Power Map Info
//
// return DWORD dwError
//      - return ERROR_SUCCESS or last error
//
//*****************************************************************************

DWORD 
GetHostControllerInfo(
    HANDLE hHCDev, 
    PUSBHOSTCONTROLLERINFO hcInfo)
{
    USBUSER_CONTROLLER_INFO_0 UsbControllerInfo;
    DWORD                      dwError = 0;
    DWORD                      dwBytes = 0;
    BOOL                       bSuccess = FALSE;

    memset(&UsbControllerInfo, 0, sizeof(UsbControllerInfo));

    // set the header and request sizes
    UsbControllerInfo.Header.UsbUserRequest = USBUSER_GET_CONTROLLER_INFO_0;
    UsbControllerInfo.Header.RequestBufferLength = sizeof(UsbControllerInfo);

    //
    // Query for the USB_CONTROLLER_INFO_0 structure
    //
    bSuccess = DeviceIoControl(hHCDev,
            IOCTL_USB_USER_REQUEST,
            &UsbControllerInfo,
            sizeof(UsbControllerInfo),
            &UsbControllerInfo,
            sizeof(UsbControllerInfo),
            &dwBytes,
            NULL);

    if (!bSuccess)
    {
        dwError = GetLastError();
        OOPS();
    }
    else
    {
        hcInfo->ControllerInfo = (PUSB_CONTROLLER_INFO_0) ALLOC(sizeof(USB_CONTROLLER_INFO_0));
        if(NULL == hcInfo->ControllerInfo)
        {
            dwError = GetLastError();
            OOPS();
        }
        else
        {
            // copy the data into our USB Host Controller's info structure
            memcpy(hcInfo->ControllerInfo, &UsbControllerInfo.Info0, sizeof(USB_CONTROLLER_INFO_0));
        }
    }
    return dwError;
}

//*****************************************************************************
//
// GetRootHubName()
//
//*****************************************************************************

PCHAR GetRootHubName (
    HANDLE HostController
)
{
    BOOL                success = 0;
    ULONG               nBytes = 0;
    USB_ROOT_HUB_NAME   rootHubName;
    PUSB_ROOT_HUB_NAME  rootHubNameW = NULL;
    PCHAR               rootHubNameA = NULL;

    // Get the length of the name of the Root Hub attached to the
    // Host Controller
    //
    success = DeviceIoControl(HostController,
                              IOCTL_USB_GET_ROOT_HUB_NAME,
                              0,
                              0,
                              &rootHubName,
                              sizeof(rootHubName),
                              &nBytes,
                              NULL);

    if (!success)
    {
        OOPS();
        goto GetRootHubNameError;
    }

    // Allocate space to hold the Root Hub name
    //
    nBytes = rootHubName.ActualLength;

    rootHubNameW = (PUSB_ROOT_HUB_NAME)ALLOC(nBytes);
    if (rootHubNameW == NULL)
    {
        OOPS();
        goto GetRootHubNameError;
    }

    // Get the name of the Root Hub attached to the Host Controller
    //
    success = DeviceIoControl(HostController,
                              IOCTL_USB_GET_ROOT_HUB_NAME,
                              NULL,
                              0,
                              rootHubNameW,
                              nBytes,
                              &nBytes,
                              NULL);
    if (!success)
    {
        OOPS();
        goto GetRootHubNameError;
    }

    // Convert the Root Hub name
    //
    rootHubNameA = WideStrToMultiStr(rootHubNameW->RootHubName, nBytes);

    // All done, free the uncoverted Root Hub name and return the
    // converted Root Hub name
    //
    FREE(rootHubNameW);

    return rootHubNameA;

GetRootHubNameError:
    // There was an error, free anything that was allocated
    //
    if (rootHubNameW != NULL)
    {
        FREE(rootHubNameW);
        rootHubNameW = NULL;
    }
    return NULL;
}

//*****************************************************************************
//
// EnumerateHub()
//
// hTreeParent - Handle of the TreeView item under which this hub should be
// added.
//
// HubName - Name of this hub.  This pointer is kept so the caller can neither
// free nor reuse this memory.
//
// ConnectionInfo - NULL if this is a root hub, else this is the connection
// info for an external hub.  This pointer is kept so the caller can neither
// free nor reuse this memory.
//
// ConfigDesc - NULL if this is a root hub, else this is the Configuration
// Descriptor for an external hub.  This pointer is kept so the caller can
// neither free nor reuse this memory.
//
// StringDescs - NULL if this is a root hub.
//
// DevProps - Device properties of the hub
//
//*****************************************************************************

VOID
EnumerateHub (
    _In_reads_(cbHubName) PCHAR                     HubName,
    _In_ size_t                                     cbHubName,
    _In_opt_ PUSB_NODE_CONNECTION_INFORMATION_EX    ConnectionInfo,
    _In_opt_ PUSB_NODE_CONNECTION_INFORMATION_EX_V2 ConnectionInfoV2,
    _In_opt_ PUSB_PORT_CONNECTOR_PROPERTIES         PortConnectorProps,
    _In_opt_ PUSB_DESCRIPTOR_REQUEST                ConfigDesc,
    _In_opt_ PUSB_DESCRIPTOR_REQUEST                BosDesc,
    _In_opt_ PSTRING_DESCRIPTOR_NODE                StringDescs,
    _In_opt_ PUSB_DEVICE_PNP_STRINGS                DevProps
    )
{
    // Initialize locals to not allocated state so the error cleanup routine
    // only tries to cleanup things that were successfully allocated.
    //
    PUSB_NODE_INFORMATION    hubInfo = NULL;
    PUSB_HUB_INFORMATION_EX  hubInfoEx = NULL;
    PUSB_HUB_CAPABILITIES_EX hubCapabilityEx = NULL;
    HANDLE                  hHubDevice = INVALID_HANDLE_VALUE;
    PVOID                   info = NULL;
    PCHAR                   deviceName = NULL;
    ULONG                   nBytes = 0;
    BOOL                    success = 0;
    DWORD                   dwSizeOfLeafName = 0;
    CHAR                    leafName[512] = {0}; 
    HRESULT                 hr = S_OK;
    size_t                  cchHeader = 0;
    size_t                  cchFullHubName = 0;

    // Allocate some space for a USBDEVICEINFO structure to hold the
    // hub info, hub name, and connection info pointers.  GPTR zero
    // initializes the structure for us.
    //
    info = ALLOC(sizeof(USBEXTERNALHUBINFO));
    if (info == NULL)
    {
        OOPS();
        goto EnumerateHubError;
    }

    // Allocate some space for a USB_NODE_INFORMATION structure for this Hub
    //
    hubInfo = (PUSB_NODE_INFORMATION)ALLOC(sizeof(USB_NODE_INFORMATION));
    if (hubInfo == NULL)
    {
        OOPS();
        goto EnumerateHubError;
    }

    hubInfoEx = (PUSB_HUB_INFORMATION_EX)ALLOC(sizeof(USB_HUB_INFORMATION_EX));
    if (hubInfoEx == NULL)
    {
        OOPS();
        goto EnumerateHubError;
    }

    hubCapabilityEx = (PUSB_HUB_CAPABILITIES_EX)ALLOC(sizeof(USB_HUB_CAPABILITIES_EX));
    if(hubCapabilityEx == NULL)
    {
        OOPS();
        goto EnumerateHubError;
    }

    // Keep copies of the Hub Name, Connection Info, and Configuration
    // Descriptor pointers
    //
    ((PUSBROOTHUBINFO)info)->HubInfo   = hubInfo;
    ((PUSBROOTHUBINFO)info)->HubName   = HubName;

    if (ConnectionInfo != NULL)
    {
        ((PUSBEXTERNALHUBINFO)info)->DeviceInfoType = ExternalHubInfo;
        ((PUSBEXTERNALHUBINFO)info)->ConnectionInfo = ConnectionInfo;
        ((PUSBEXTERNALHUBINFO)info)->ConfigDesc = ConfigDesc;
        ((PUSBEXTERNALHUBINFO)info)->StringDescs = StringDescs;
        ((PUSBEXTERNALHUBINFO)info)->PortConnectorProps = PortConnectorProps;
        ((PUSBEXTERNALHUBINFO)info)->HubInfoEx = hubInfoEx;
        ((PUSBEXTERNALHUBINFO)info)->HubCapabilityEx = hubCapabilityEx;
        ((PUSBEXTERNALHUBINFO)info)->BosDesc = BosDesc;
        ((PUSBEXTERNALHUBINFO)info)->ConnectionInfoV2 = ConnectionInfoV2;
        ((PUSBEXTERNALHUBINFO)info)->UsbDeviceProperties = DevProps;
    }
    else
    {
        ((PUSBROOTHUBINFO)info)->DeviceInfoType = RootHubInfo;
        ((PUSBROOTHUBINFO)info)->HubInfoEx = hubInfoEx;
        ((PUSBROOTHUBINFO)info)->HubCapabilityEx = hubCapabilityEx;
        ((PUSBROOTHUBINFO)info)->PortConnectorProps = PortConnectorProps;
        ((PUSBROOTHUBINFO)info)->UsbDeviceProperties = DevProps;
    }

    // Allocate a temp buffer for the full hub device name.
    //
    hr = StringCbLength("\\\\.\\", MAX_DEVICE_PROP, &cchHeader);
    if (FAILED(hr))
    {
        goto EnumerateHubError;
    }
    cchFullHubName = cchHeader + cbHubName + 1;
    deviceName = (PCHAR)ALLOC((DWORD) cchFullHubName);
    if (deviceName == NULL)
    {
        OOPS();
        goto EnumerateHubError;
    }

    // Create the full hub device name
    //
    hr = StringCchCopyN(deviceName, cchFullHubName, "\\\\.\\", cchHeader);
    if (FAILED(hr))
    {
        goto EnumerateHubError;
    }
    hr = StringCchCatN(deviceName, cchFullHubName, HubName, cbHubName);
    if (FAILED(hr))
    {
        goto EnumerateHubError;
    }

    // Try to hub the open device
    //
    hHubDevice = CreateFile(deviceName,
                            GENERIC_WRITE,
                            FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL);

    // Done with temp buffer for full hub device name
    //
    FREE(deviceName);

    if (hHubDevice == INVALID_HANDLE_VALUE)
    {
        OOPS();
        goto EnumerateHubError;
    }

    //
    // Now query USBHUB for the USB_NODE_INFORMATION structure for this hub.
    // This will tell us the number of downstream ports to enumerate, among
    // other things.
    //
    success = DeviceIoControl(hHubDevice,
                              IOCTL_USB_GET_NODE_INFORMATION,
                              hubInfo,
                              sizeof(USB_NODE_INFORMATION),
                              hubInfo,
                              sizeof(USB_NODE_INFORMATION),
                              &nBytes,
                              NULL);

    if (!success)
    {
        OOPS();
        goto EnumerateHubError;
    }

    success = DeviceIoControl(hHubDevice,
                              IOCTL_USB_GET_HUB_INFORMATION_EX,
                              hubInfoEx,
                              sizeof(USB_HUB_INFORMATION_EX),
                              hubInfoEx,
                              sizeof(USB_HUB_INFORMATION_EX),
                              &nBytes,
                              NULL);

    //
    // Fail gracefully for downlevel OS's from Win8
    //
    if (!success || nBytes < sizeof(USB_HUB_INFORMATION_EX))
    {
        FREE(hubInfoEx);
        hubInfoEx = NULL;
        if (ConnectionInfo != NULL)
        {
            ((PUSBEXTERNALHUBINFO)info)->HubInfoEx = NULL;
        }
        else
        {
            ((PUSBROOTHUBINFO)info)->HubInfoEx = NULL;
        }
    }

    //
    // Obtain Hub Capabilities
    //
    success = DeviceIoControl(hHubDevice,
                              IOCTL_USB_GET_HUB_CAPABILITIES_EX,
                              hubCapabilityEx,
                              sizeof(USB_HUB_CAPABILITIES_EX),
                              hubCapabilityEx,
                              sizeof(USB_HUB_CAPABILITIES_EX),
                              &nBytes,
                              NULL);

    //
    // Fail gracefully
    //
    if (!success || nBytes < sizeof(USB_HUB_CAPABILITIES_EX))
    {
        FREE(hubCapabilityEx);
        hubCapabilityEx = NULL;
        if (ConnectionInfo != NULL)
        {
            ((PUSBEXTERNALHUBINFO)info)->HubCapabilityEx = NULL;
        }
        else
        {
            ((PUSBROOTHUBINFO)info)->HubCapabilityEx = NULL;
        }
    }

    // Build the leaf name from the port number and the device description
    //
    dwSizeOfLeafName = sizeof(leafName);
    if (ConnectionInfo)
    {
        StringCchPrintf(leafName, dwSizeOfLeafName, "[Port%d] ", ConnectionInfo->ConnectionIndex);
        StringCchCat(leafName, 
            dwSizeOfLeafName, 
            ConnectionStatuses[ConnectionInfo->ConnectionStatus]);
        StringCchCatN(leafName, 
            dwSizeOfLeafName, 
            " :  ",
            sizeof(" :  "));
    }

    if (DevProps)
    {
        size_t cbDeviceDesc = 0;
        hr = StringCbLength(DevProps->DeviceDesc, MAX_DRIVER_KEY_NAME, &cbDeviceDesc);
        if(SUCCEEDED(hr))
        {
            StringCchCatN(leafName, 
                    dwSizeOfLeafName, 
                    DevProps->DeviceDesc,
                    cbDeviceDesc);
        }
    }
    else
    {
        if(ConnectionInfo != NULL)
        {
            // External hub
            StringCchCatN(leafName, 
                    dwSizeOfLeafName, 
                    HubName,
                    cbHubName);
        }
        else
        {
            // Root hub
            StringCchCatN(leafName, 
                    dwSizeOfLeafName, 
                    "RootHub",
                    sizeof("RootHub")); 
        }
    }
	/*
    // Now add an item to the TreeView with the PUSBDEVICEINFO pointer info
    // as the LPARAM reference value containing everything we know about the
    // hub.
    //

    if (hItem == NULL)
    {
        OOPS();
        goto EnumerateHubError;
    }
	*/
    // Now recursively enumerate the ports of this hub.
    //
    EnumerateHubPorts(
        hHubDevice,
        hubInfo->u.HubInformation.HubDescriptor.bNumberOfPorts
        );


    CloseHandle(hHubDevice);
    return;

EnumerateHubError:
    //
    // Clean up any stuff that got allocated
    //

    if (hHubDevice != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hHubDevice);
        hHubDevice = INVALID_HANDLE_VALUE;
    }

    if (hubInfo)
    {
        FREE(hubInfo);
    }

    if (hubInfoEx)
    {
        FREE(hubInfoEx);
    }

    if (info)
    {
        FREE(info);
    }

    if (HubName)
    {
        FREE(HubName);
    }

    if (ConnectionInfo)
    {
        FREE(ConnectionInfo);
    }

    if (ConfigDesc)
    {
        FREE(ConfigDesc);
    }

    if (BosDesc)
    {
        FREE(BosDesc);
    }

    if (StringDescs != NULL)
    {
        PSTRING_DESCRIPTOR_NODE Next;

        do {

            Next = StringDescs->Next;
            FREE(StringDescs);
            StringDescs = Next;

        } while (StringDescs != NULL);
    }
}

//*****************************************************************************
//
// WideStrToMultiStr()
//
//*****************************************************************************

PCHAR WideStrToMultiStr ( 
                         _In_reads_bytes_(cbWideStr) PWCHAR WideStr, 
                         _In_ size_t                   cbWideStr
                         )
{
    ULONG  nBytes = 0;
    PCHAR  MultiStr = NULL;
    PWCHAR pWideStr = NULL;

    // Use local string to guarantee zero termination
    pWideStr = (PWCHAR) ALLOC((DWORD) cbWideStr + 1);
    if (NULL == pWideStr)
    {
        return NULL;
    }
    memset(pWideStr, 0, cbWideStr + 1);
    memcpy(pWideStr, WideStr, cbWideStr);

    // Get the length of the converted string
    //
    nBytes = WideCharToMultiByte(
                 CP_ACP,
                 WC_NO_BEST_FIT_CHARS,
                 pWideStr,
                 -1,
                 NULL,
                 0,
                 NULL,
                 NULL);

    if (nBytes == 0)
    {
        FREE(pWideStr);
        return NULL;
    }

    // Allocate space to hold the converted string
    //
    MultiStr = (PCHAR)ALLOC(nBytes);
    if (MultiStr == NULL)
    {
        FREE(pWideStr);
        return NULL;
    }

    // Convert the string
    //
    nBytes = WideCharToMultiByte(
                 CP_ACP,
                 WC_NO_BEST_FIT_CHARS,
                 pWideStr,
                 -1,
                 MultiStr,
                 nBytes,
                 NULL,
                 NULL);

    if (nBytes == 0)
    {
        FREE(MultiStr);
        FREE(pWideStr);
        return NULL;
    }

    FREE(pWideStr);
    return MultiStr;
}

/*****************************************************************************

  DriverNameToDeviceInst()

  Finds the Device instance of the DevNode with the matching DriverName.
  Returns FALSE if the matching DevNode is not found and TRUE if found

 *****************************************************************************/
BOOL 
DriverNameToDeviceInst(
    _In_reads_bytes_(cbDriverName) PCHAR DriverName,
    _In_ size_t cbDriverName,
    _Out_ HDEVINFO *pDevInfo,
    _Out_writes_bytes_(sizeof(SP_DEVINFO_DATA)) PSP_DEVINFO_DATA pDevInfoData
    )
{
    HDEVINFO         deviceInfo = INVALID_HANDLE_VALUE;
    BOOL             status = TRUE;
    ULONG            deviceIndex;
    SP_DEVINFO_DATA  deviceInfoData;
    BOOL             bResult = FALSE;
    PCHAR            pDriverName = NULL;
    PSTR             buf = NULL;
    BOOL             done = FALSE;

    if (pDevInfo == NULL)
    {
        return FALSE;
    }

    if (pDevInfoData == NULL)
    {
        return FALSE;
    }

    memset(pDevInfoData, 0, sizeof(SP_DEVINFO_DATA));

    *pDevInfo = INVALID_HANDLE_VALUE;

    // Use local string to guarantee zero termination
    pDriverName = (PCHAR) ALLOC((DWORD) cbDriverName + 1);
    if (NULL == pDriverName)
    {
        status = FALSE;
        goto Done;
    }
    StringCbCopyN(pDriverName, cbDriverName + 1, DriverName, cbDriverName);

    //
    // We cannot walk the device tree with CM_Get_Sibling etc. unless we assume
    // the device tree will stabilize. Any devnode removal (even outside of USB)
    // would force us to retry. Instead we use Setup API to snapshot all
    // devices.
    //

    // Examine all present devices to see if any match the given DriverName
    //
    deviceInfo = SetupDiGetClassDevs(NULL,
            NULL,
            NULL,
            DIGCF_ALLCLASSES | DIGCF_PRESENT);

    if (deviceInfo == INVALID_HANDLE_VALUE)
    {
        status = FALSE;
        goto Done;
    }

    deviceIndex = 0;
    deviceInfoData.cbSize = sizeof(deviceInfoData);

    while (done == FALSE)
    {
        //
        // Get devinst of the next device
        //

        status = SetupDiEnumDeviceInfo(deviceInfo,
                                       deviceIndex,
                                       &deviceInfoData);

        deviceIndex++;

        if (!status)
        {
            //
            // This could be an error, or indication that all devices have been
            // processed. Either way the desired device was not found.
            //
            
            done = TRUE;
            break;
        }

        //
        // Get the DriverName value
        //

        bResult = GetDeviceProperty(deviceInfo,
                                    &deviceInfoData,
                                    SPDRP_DRIVER,
                                    &buf);
        
        // If the DriverName value matches, return the DeviceInstance
        //
        if (bResult == TRUE && buf != NULL && _stricmp(pDriverName, buf) == 0)
        {
            done = TRUE;
            *pDevInfo = deviceInfo;
            CopyMemory(pDevInfoData, &deviceInfoData, sizeof(deviceInfoData));
            FREE(buf);
            break;
        }

        if(buf != NULL)
        {
            FREE(buf);
            buf = NULL;
        }
    }

Done:

    if (bResult == FALSE)
    {
        if (deviceInfo != INVALID_HANDLE_VALUE)
        {
            SetupDiDestroyDeviceInfoList(deviceInfo);
        }
    }

    if (pDriverName != NULL)
    {
        FREE(pDriverName);
    }

    return status;
}

/*****************************************************************************

  FreeDeviceProperties()

  Free the device properties structure

 *****************************************************************************/
VOID FreeDeviceProperties(_In_ PUSB_DEVICE_PNP_STRINGS *ppDevProps)
{
    if(ppDevProps == NULL)
    {
        return;
    }

    if(*ppDevProps == NULL)
    {
        return;
    }

    if ((*ppDevProps)->DeviceId != NULL)
    {
        FREE((*ppDevProps)->DeviceId);
    }

    if ((*ppDevProps)->DeviceDesc != NULL)
    {
        FREE((*ppDevProps)->DeviceDesc);
    }

    //
    // The following are not necessary, but left in case
    // in the future there is a later failure where these
    // pointer fields would be allocated.
    //

    if ((*ppDevProps)->HwId != NULL)
    {
        FREE((*ppDevProps)->HwId);
    }

    if ((*ppDevProps)->Service != NULL)
    {
        FREE((*ppDevProps)->Service);
    }

    if ((*ppDevProps)->DeviceClass != NULL)
    {
        FREE((*ppDevProps)->DeviceClass);
    }

    if ((*ppDevProps)->PowerState != NULL)
    {
        FREE((*ppDevProps)->PowerState);
    }

    FREE(*ppDevProps);
    *ppDevProps = NULL;
}

//*****************************************************************************
//
// EnumerateHubPorts()
//
// hTreeParent - Handle of the TreeView item under which the hub port should
// be added.
//
// hHubDevice - Handle of the hub device to enumerate.
//
// NumPorts - Number of ports on the hub.
//
//*****************************************************************************

VOID
EnumerateHubPorts (
    HANDLE      hHubDevice,
    ULONG       NumPorts
)
{
    ULONG       index = 0;
    BOOL        success = 0;
    HRESULT     hr = S_OK;
    PCHAR       driverKeyName = NULL;
    PUSB_DEVICE_PNP_STRINGS DevProps;
    DWORD       dwSizeOfLeafName = 0;
    CHAR        leafName[512];
    int         icon = 0;

    PUSB_NODE_CONNECTION_INFORMATION_EX    connectionInfoEx;
    PUSB_PORT_CONNECTOR_PROPERTIES         pPortConnectorProps;
    USB_PORT_CONNECTOR_PROPERTIES          portConnectorProps;
    PUSB_DESCRIPTOR_REQUEST                configDesc;
    PUSB_DESCRIPTOR_REQUEST                bosDesc;
    PSTRING_DESCRIPTOR_NODE                stringDescs;
    PUSBDEVICEINFO                         info;
    PUSB_NODE_CONNECTION_INFORMATION_EX_V2 connectionInfoExV2;
    PDEVICE_INFO_NODE                      pNode;

    // Loop over all ports of the hub.
    //
    // Port indices are 1 based, not 0 based.
    //
    for (index = 1; index <= NumPorts; index++)
    {
        ULONG nBytesEx;
        ULONG nBytes = 0;

        connectionInfoEx = NULL;
        pPortConnectorProps = NULL;
        ZeroMemory(&portConnectorProps, sizeof(portConnectorProps));
        configDesc = NULL;
        bosDesc = NULL;
        stringDescs = NULL;
        info = NULL;
        connectionInfoExV2 = NULL;
        pNode = NULL;
        DevProps = NULL;
        ZeroMemory(leafName, sizeof(leafName));

        //
        // Allocate space to hold the connection info for this port.
        // For now, allocate it big enough to hold info for 30 pipes.
        //
        // Endpoint numbers are 0-15.  Endpoint number 0 is the standard
        // control endpoint which is not explicitly listed in the Configuration
        // Descriptor.  There can be an IN endpoint and an OUT endpoint at
        // endpoint numbers 1-15 so there can be a maximum of 30 endpoints
        // per device configuration.
        //
        // Should probably size this dynamically at some point.
        //

        nBytesEx = sizeof(USB_NODE_CONNECTION_INFORMATION_EX) +
                 (sizeof(USB_PIPE_INFO) * 30);

        connectionInfoEx = (PUSB_NODE_CONNECTION_INFORMATION_EX)ALLOC(nBytesEx);

        if (connectionInfoEx == NULL)
        {
            OOPS();
            break;
        }

        connectionInfoExV2 = (PUSB_NODE_CONNECTION_INFORMATION_EX_V2) 
                                    ALLOC(sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2));

        if (connectionInfoExV2 == NULL)
        {
            OOPS();
            FREE(connectionInfoEx);
            break;
        }
        
        //
        // Now query USBHUB for the structures
        // for this port.  This will tell us if a device is attached to this
        // port, among other things.
        // The fault tolerate code is executed first.
        //

        portConnectorProps.ConnectionIndex = index;

        success = DeviceIoControl(hHubDevice,
                                  IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES,
                                  &portConnectorProps,
                                  sizeof(USB_PORT_CONNECTOR_PROPERTIES),
                                  &portConnectorProps,
                                  sizeof(USB_PORT_CONNECTOR_PROPERTIES),
                                  &nBytes,
                                  NULL);

        if (success && nBytes == sizeof(USB_PORT_CONNECTOR_PROPERTIES)) 
        {
            pPortConnectorProps = (PUSB_PORT_CONNECTOR_PROPERTIES)
                                        ALLOC(portConnectorProps.ActualLength);

            if (pPortConnectorProps != NULL)
            {
                pPortConnectorProps->ConnectionIndex = index;
                
                success = DeviceIoControl(hHubDevice,
                                          IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES,
                                          pPortConnectorProps,
                                          portConnectorProps.ActualLength,
                                          pPortConnectorProps,
                                          portConnectorProps.ActualLength,
                                          &nBytes,
                                          NULL);

                if (!success || nBytes < portConnectorProps.ActualLength)
                {
                    FREE(pPortConnectorProps);
                    pPortConnectorProps = NULL;
                }
            }
        }
        
        connectionInfoExV2->ConnectionIndex = index;
        connectionInfoExV2->Length = sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2);
        connectionInfoExV2->SupportedUsbProtocols.Usb300 = 1;

        success = DeviceIoControl(hHubDevice,
                                  IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX_V2,
                                  connectionInfoExV2,
                                  sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2),
                                  connectionInfoExV2,
                                  sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2),
                                  &nBytes,
                                  NULL);

        if (!success || nBytes < sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2)) 
        {
            FREE(connectionInfoExV2);
            connectionInfoExV2 = NULL;
        }

        connectionInfoEx->ConnectionIndex = index;

        success = DeviceIoControl(hHubDevice,
                                  IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX,
                                  connectionInfoEx,
                                  nBytesEx,
                                  connectionInfoEx,
                                  nBytesEx,
                                  &nBytesEx,
                                  NULL);

        if (success)
        {
            //
            // Since the USB_NODE_CONNECTION_INFORMATION_EX is used to display
            // the device speed, but the hub driver doesn't support indication
            // of superspeed, we overwrite the value if the super speed
            // data structures are available and indicate the device is operating
            // at SuperSpeed.
            // 
            
            if (connectionInfoEx->Speed == UsbHighSpeed 
                && connectionInfoExV2 != NULL 
                && connectionInfoExV2->Flags.DeviceIsOperatingAtSuperSpeedOrHigher)
            {
                connectionInfoEx->Speed = UsbSuperSpeed;
            }
        } 
        else 
        {
            PUSB_NODE_CONNECTION_INFORMATION    connectionInfo = NULL;

            // Try using IOCTL_USB_GET_NODE_CONNECTION_INFORMATION
            // instead of IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX
            //

            nBytes = sizeof(USB_NODE_CONNECTION_INFORMATION) +
                     sizeof(USB_PIPE_INFO) * 30;

            connectionInfo = (PUSB_NODE_CONNECTION_INFORMATION)ALLOC(nBytes);

            if (connectionInfo == NULL) 
            {
                OOPS();

                FREE(connectionInfoEx);
                if (pPortConnectorProps != NULL)
                {
                    FREE(pPortConnectorProps);
                }
                if (connectionInfoExV2 != NULL)
                {
                    FREE(connectionInfoExV2);
                }
                continue;                
            }

            connectionInfo->ConnectionIndex = index;

            success = DeviceIoControl(hHubDevice,
                                      IOCTL_USB_GET_NODE_CONNECTION_INFORMATION,
                                      connectionInfo,
                                      nBytes,
                                      connectionInfo,
                                      nBytes,
                                      &nBytes,
                                      NULL);

            if (!success)
            {
                OOPS();

                FREE(connectionInfo);
                FREE(connectionInfoEx);
                if (pPortConnectorProps != NULL)
                {
                    FREE(pPortConnectorProps);
                }
                if (connectionInfoExV2 != NULL)
                {
                    FREE(connectionInfoExV2);
                }
                continue;
            }

            // Copy IOCTL_USB_GET_NODE_CONNECTION_INFORMATION into
            // IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX structure.
            //
            connectionInfoEx->ConnectionIndex = connectionInfo->ConnectionIndex;
            connectionInfoEx->DeviceDescriptor = connectionInfo->DeviceDescriptor;
            connectionInfoEx->CurrentConfigurationValue = connectionInfo->CurrentConfigurationValue;
            connectionInfoEx->Speed = connectionInfo->LowSpeed ? UsbLowSpeed : UsbFullSpeed;
            connectionInfoEx->DeviceIsHub = connectionInfo->DeviceIsHub;
            connectionInfoEx->DeviceAddress = connectionInfo->DeviceAddress;
            connectionInfoEx->NumberOfOpenPipes = connectionInfo->NumberOfOpenPipes;
            connectionInfoEx->ConnectionStatus = connectionInfo->ConnectionStatus;

            memcpy(&connectionInfoEx->PipeList[0],
                   &connectionInfo->PipeList[0],
                   sizeof(USB_PIPE_INFO) * 30);

            FREE(connectionInfo);
        }

        // Update the count of connected devices
        //
        if (connectionInfoEx->ConnectionStatus == DeviceConnected)
        {
            TotalDevicesConnected++;
        }

        if (connectionInfoEx->DeviceIsHub)
        {
            TotalHubs++;
        }

        // If there is a device connected, get the Device Description
        //
        if (connectionInfoEx->ConnectionStatus != NoDeviceConnected)
        {
            driverKeyName = GetDriverKeyName(hHubDevice, index);

            if (driverKeyName)
            {
                size_t cbDriverName = 0;

                hr = StringCbLength(driverKeyName, MAX_DRIVER_KEY_NAME, &cbDriverName);
                if (SUCCEEDED(hr))
                {
                    DevProps = DriverNameToDeviceProperties(driverKeyName, cbDriverName);
                    pNode = FindMatchingDeviceNodeForDriverName(driverKeyName, connectionInfoEx->DeviceIsHub);
                }
                FREE(driverKeyName);
            }

        }

        // If there is a device connected to the port, try to retrieve the
        // Configuration Descriptor from the device.
        //
        if (connectionInfoEx->ConnectionStatus == DeviceConnected)
        {
            configDesc = GetConfigDescriptor(hHubDevice,
                                             index,
                                             0);
        }
        else
        {
            configDesc = NULL;
        }

        if (configDesc != NULL &&
            connectionInfoEx->DeviceDescriptor.bcdUSB >= 0x0210)
        {
            bosDesc = GetBOSDescriptor(hHubDevice,
                                       index);
        }
        else
        {
            bosDesc = NULL;
        }

        if (configDesc != NULL &&
            AreThereStringDescriptors(&connectionInfoEx->DeviceDescriptor,
                                      (PUSB_CONFIGURATION_DESCRIPTOR)(configDesc+1)))
        {
            stringDescs = GetAllStringDescriptors (
                              hHubDevice,
                              index,
                              &connectionInfoEx->DeviceDescriptor,
                              (PUSB_CONFIGURATION_DESCRIPTOR)(configDesc+1));
        }
        else
        {
            stringDescs = NULL;
        }

        // If the device connected to the port is an external hub, get the
        // name of the external hub and recursively enumerate it.
        //
        if (connectionInfoEx->DeviceIsHub)
        {
            PCHAR extHubName;
            size_t cbHubName = 0;

            extHubName = GetExternalHubName(hHubDevice, index);
            if (extHubName != NULL)
            {
                hr = StringCbLength(extHubName, MAX_DRIVER_KEY_NAME, &cbHubName);
                if (SUCCEEDED(hr))
                {
                    EnumerateHub(//hPortItem,
                            extHubName,
                            cbHubName,
                            connectionInfoEx,
                            connectionInfoExV2,
                            pPortConnectorProps,
                            configDesc,
                            bosDesc,
                            stringDescs,
                            DevProps);
                }
            }
        }
        else
        {
            // Allocate some space for a USBDEVICEINFO structure to hold the
            // hub info, hub name, and connection info pointers.  GPTR zero
            // initializes the structure for us.
            //
            info = (PUSBDEVICEINFO) ALLOC(sizeof(USBDEVICEINFO));

            if (info == NULL)
            {
                OOPS();
                if (configDesc != NULL)
                {
                    FREE(configDesc);
                }
                if (bosDesc != NULL)
                {
                    FREE(bosDesc);
                }
                FREE(connectionInfoEx);
                
                if (pPortConnectorProps != NULL)
                {
                    FREE(pPortConnectorProps);
                }
                if (connectionInfoExV2 != NULL)
                {
                    FREE(connectionInfoExV2);
                }
                break;
            }

            info->DeviceInfoType = DeviceInfo;
            info->ConnectionInfo = connectionInfoEx;
            info->PortConnectorProps = pPortConnectorProps;
            info->ConfigDesc = configDesc;
            info->StringDescs = stringDescs;
            info->BosDesc = bosDesc;
            info->ConnectionInfoV2 = connectionInfoExV2;
            info->UsbDeviceProperties = DevProps;
            info->DeviceInfoNode = pNode;

			//결과값
			DisplayConnectionInfo(info->ConnectionInfo,
                (PUSBDEVICEINFO)info,
                info->StringDescs,
                info->ConnectionInfoV2);
			
            StringCchPrintf(leafName, sizeof(leafName), "[Port%d] ", index);

            // Add error description if ConnectionStatus is other than NoDeviceConnected / DeviceConnected
            StringCchCat(leafName, 

                sizeof(leafName), 
                ConnectionStatuses[connectionInfoEx->ConnectionStatus]);

            if (DevProps)
            {
                size_t cchDeviceDesc = 0;

                hr = StringCbLength(DevProps->DeviceDesc, MAX_DEVICE_PROP, &cchDeviceDesc);
                if (FAILED(hr))
                {
                    OOPS();
                }
                dwSizeOfLeafName = sizeof(leafName);
                StringCchCatN(leafName, 
                    dwSizeOfLeafName - 1, 
                    " :  ",
                    sizeof(" :  "));
                StringCchCatN(leafName, 
                    dwSizeOfLeafName - 1, 
                    DevProps->DeviceDesc,
                    cchDeviceDesc );
            }

            if (connectionInfoEx->ConnectionStatus == NoDeviceConnected)
            {
                if (connectionInfoExV2 != NULL &&
                    connectionInfoExV2->SupportedUsbProtocols.Usb300 == 1)
                {
                    icon = NoSsDeviceIcon;
                }
                else
                {
                    icon = NoDeviceIcon;
                }
            }
            else if (connectionInfoEx->CurrentConfigurationValue)
            {
                if (connectionInfoEx->Speed == UsbSuperSpeed)
                {
                    icon = GoodSsDeviceIcon;
                }
                else
                {
                    icon = GoodDeviceIcon;
                }
            }
            else
            {
                icon = BadDeviceIcon;
            }
        }
    } // for
}

//*****************************************************************************
//
// GetDriverKeyName()
//
//*****************************************************************************

PCHAR GetDriverKeyName (
    HANDLE  Hub,
    ULONG   ConnectionIndex
)
{
    BOOL                                success = 0;
    ULONG                               nBytes = 0;
    USB_NODE_CONNECTION_DRIVERKEY_NAME  driverKeyName;
    PUSB_NODE_CONNECTION_DRIVERKEY_NAME driverKeyNameW = NULL;
    PCHAR                               driverKeyNameA = NULL;

    // Get the length of the name of the driver key of the device attached to
    // the specified port.
    //
    driverKeyName.ConnectionIndex = ConnectionIndex;

    success = DeviceIoControl(Hub,
                              IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,
                              &driverKeyName,
                              sizeof(driverKeyName),
                              &driverKeyName,
                              sizeof(driverKeyName),
                              &nBytes,
                              NULL);

    if (!success)
    {
        OOPS();
        goto GetDriverKeyNameError;
    }

    // Allocate space to hold the driver key name
    //
    nBytes = driverKeyName.ActualLength;

    if (nBytes <= sizeof(driverKeyName))
    {
        OOPS();
        goto GetDriverKeyNameError;
    }

    driverKeyNameW = (PUSB_NODE_CONNECTION_DRIVERKEY_NAME)ALLOC(nBytes);
    if (driverKeyNameW == NULL)
    {
        OOPS();
        goto GetDriverKeyNameError;
    }

    // Get the name of the driver key of the device attached to
    // the specified port.
    //
    driverKeyNameW->ConnectionIndex = ConnectionIndex;

    success = DeviceIoControl(Hub,
                              IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,
                              driverKeyNameW,
                              nBytes,
                              driverKeyNameW,
                              nBytes,
                              &nBytes,
                              NULL);

    if (!success)
    {
        OOPS();
        goto GetDriverKeyNameError;
    }

    // Driver Key name 의 type을 변환 ( WCHAR -> PCHAR )
    //
    driverKeyNameA = WideStrToMultiStr(driverKeyNameW->DriverKeyName, nBytes);

    // 메모리 해제
    //
    FREE(driverKeyNameW);

    return driverKeyNameA;


GetDriverKeyNameError:
    // There was an error, free anything that was allocated
    //
    if (driverKeyNameW != NULL)
    {
        FREE(driverKeyNameW);
        driverKeyNameW = NULL;
    }

    return NULL;
}

PDEVICE_INFO_NODE
FindMatchingDeviceNodeForDriverName(
    _In_ PSTR   DriverKeyName,
    _In_ BOOLEAN IsHub
    )
{
    PDEVICE_INFO_NODE pNode  = NULL;
    PDEVICE_GUID_LIST pList  = NULL;
    PLIST_ENTRY       pEntry = NULL;

    pList = IsHub ? &gHubList : &gDeviceList;

    pEntry = pList->ListHead.Flink;

    while (pEntry != &pList->ListHead)
    {
        pNode = CONTAINING_RECORD(pEntry,
                                  DEVICE_INFO_NODE,
                                  ListEntry);
        if (_stricmp(DriverKeyName, pNode->DeviceDriverName) == 0)
        {
            return pNode;
        }

        pEntry = pEntry->Flink;
    }
    
    return NULL;
}

//*****************************************************************************
//
// GetConfigDescriptor()
//
// hHubDevice - Handle of the hub device containing the port from which the
// Configuration Descriptor will be requested.
//
// ConnectionIndex - Identifies the port on the hub to which a device is
// attached from which the Configuration Descriptor will be requested.
//
// DescriptorIndex - Configuration Descriptor index, zero based.
//
//*****************************************************************************

PUSB_DESCRIPTOR_REQUEST
GetConfigDescriptor (
    HANDLE  hHubDevice,
    ULONG   ConnectionIndex,
    UCHAR   DescriptorIndex
)
{
    BOOL    success = 0;
    ULONG   nBytes = 0;
    ULONG   nBytesReturned = 0;

    UCHAR   configDescReqBuf[sizeof(USB_DESCRIPTOR_REQUEST) +
                             sizeof(USB_CONFIGURATION_DESCRIPTOR)];

    PUSB_DESCRIPTOR_REQUEST         configDescReq = NULL;
    PUSB_CONFIGURATION_DESCRIPTOR   configDesc = NULL;


    // Request the Configuration Descriptor the first time using our
    // local buffer, which is just big enough for the Cofiguration
    // Descriptor itself.
    //
    nBytes = sizeof(configDescReqBuf);

    configDescReq = (PUSB_DESCRIPTOR_REQUEST)configDescReqBuf;
    configDesc = (PUSB_CONFIGURATION_DESCRIPTOR)(configDescReq+1);

    // Zero fill the entire request structure
    //
    memset(configDescReq, 0, nBytes);

    // Indicate the port from which the descriptor will be requested
    //
    configDescReq->ConnectionIndex = ConnectionIndex;

    //
    // USBHUB uses URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE to process this
    // IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION request.
    //
    // USBD will automatically initialize these fields:
    //     bmRequest = 0x80
    //     bRequest  = 0x06
    //
    // We must inititialize these fields:
    //     wValue    = Descriptor Type (high) and Descriptor Index (low byte)
    //     wIndex    = Zero (or Language ID for String Descriptors)
    //     wLength   = Length of descriptor buffer
    //
    configDescReq->SetupPacket.wValue = (USB_CONFIGURATION_DESCRIPTOR_TYPE << 8)
                                        | DescriptorIndex;

    configDescReq->SetupPacket.wLength = (USHORT)(nBytes - sizeof(USB_DESCRIPTOR_REQUEST));

    // Now issue the get descriptor request.
    //
    success = DeviceIoControl(hHubDevice,
                              IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
                              configDescReq,
                              nBytes,
                              configDescReq,
                              nBytes,
                              &nBytesReturned,
                              NULL);

    if (!success)
    {
        OOPS();
        return NULL;
    }

    if (nBytes != nBytesReturned)
    {
        OOPS();
        return NULL;
    }

    if (configDesc->wTotalLength < sizeof(USB_CONFIGURATION_DESCRIPTOR))
    {
        OOPS();
        return NULL;
    }

    // Now request the entire Configuration Descriptor using a dynamically
    // allocated buffer which is sized big enough to hold the entire descriptor
    //
    nBytes = sizeof(USB_DESCRIPTOR_REQUEST) + configDesc->wTotalLength;

    configDescReq = (PUSB_DESCRIPTOR_REQUEST)ALLOC(nBytes);

    if (configDescReq == NULL)
    {
        OOPS();
        return NULL;
    }

    configDesc = (PUSB_CONFIGURATION_DESCRIPTOR)(configDescReq+1);

    // Indicate the port from which the descriptor will be requested
    //
    configDescReq->ConnectionIndex = ConnectionIndex;

    //
    // USBHUB uses URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE to process this
    // IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION request.
    //
    // USBD will automatically initialize these fields:
    //     bmRequest = 0x80
    //     bRequest  = 0x06
    //
    // We must inititialize these fields:
    //     wValue    = Descriptor Type (high) and Descriptor Index (low byte)
    //     wIndex    = Zero (or Language ID for String Descriptors)
    //     wLength   = Length of descriptor buffer
    //
    configDescReq->SetupPacket.wValue = (USB_CONFIGURATION_DESCRIPTOR_TYPE << 8)
                                        | DescriptorIndex;

    configDescReq->SetupPacket.wLength = (USHORT)(nBytes - sizeof(USB_DESCRIPTOR_REQUEST));

    // Now issue the get descriptor request.
    //

    success = DeviceIoControl(hHubDevice,
                              IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
                              configDescReq,
                              nBytes,
                              configDescReq,
                              nBytes,
                              &nBytesReturned,
                              NULL);

    if (!success)
    {
        OOPS();
        FREE(configDescReq);
        return NULL;
    }

    if (nBytes != nBytesReturned)
    {
        OOPS();
        FREE(configDescReq);
        return NULL;
    }

    if (configDesc->wTotalLength != (nBytes - sizeof(USB_DESCRIPTOR_REQUEST)))
    {
        OOPS();
        FREE(configDescReq);
        return NULL;
    }

    return configDescReq;
}

//*****************************************************************************
//
// GetBOSDescriptor()
//
// hHubDevice - Handle of the hub device containing the port from which the
// Configuration Descriptor will be requested.
//
// ConnectionIndex - Identifies the port on the hub to which a device is
// attached from which the BOS Descriptor will be requested.
//
//*****************************************************************************

PUSB_DESCRIPTOR_REQUEST
GetBOSDescriptor (
    HANDLE  hHubDevice,
    ULONG   ConnectionIndex
)
{
    BOOL    success = 0;
    ULONG   nBytes = 0;
    ULONG   nBytesReturned = 0;

    UCHAR   bosDescReqBuf[sizeof(USB_DESCRIPTOR_REQUEST) +
                          sizeof(USB_BOS_DESCRIPTOR)];

    PUSB_DESCRIPTOR_REQUEST bosDescReq = NULL;
    PUSB_BOS_DESCRIPTOR     bosDesc = NULL;


    // Request the BOS Descriptor the first time using our
    // local buffer, which is just big enough for the BOS
    // Descriptor itself.
    //
    nBytes = sizeof(bosDescReqBuf);

    bosDescReq = (PUSB_DESCRIPTOR_REQUEST)bosDescReqBuf;
    bosDesc = (PUSB_BOS_DESCRIPTOR)(bosDescReq+1);

    // Zero fill the entire request structure
    //
    memset(bosDescReq, 0, nBytes);

    // Indicate the port from which the descriptor will be requested
    //
    bosDescReq->ConnectionIndex = ConnectionIndex;

    //
    // USBHUB uses URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE to process this
    // IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION request.
    //
    // USBD will automatically initialize these fields:
    //     bmRequest = 0x80
    //     bRequest  = 0x06
    //
    // We must inititialize these fields:
    //     wValue    = Descriptor Type (high) and Descriptor Index (low byte)
    //     wIndex    = Zero (or Language ID for String Descriptors)
    //     wLength   = Length of descriptor buffer
    //
    bosDescReq->SetupPacket.wValue = (USB_BOS_DESCRIPTOR_TYPE << 8);

    bosDescReq->SetupPacket.wLength = (USHORT)(nBytes - sizeof(USB_DESCRIPTOR_REQUEST));

    // Now issue the get descriptor request.
    //
    success = DeviceIoControl(hHubDevice,
                              IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
                              bosDescReq,
                              nBytes,
                              bosDescReq,
                              nBytes,
                              &nBytesReturned,
                              NULL);

    if (!success)
    {
        OOPS();
        return NULL;
    }

    if (nBytes != nBytesReturned)
    {
        OOPS();
        return NULL;
    }

    if (bosDesc->wTotalLength < sizeof(USB_BOS_DESCRIPTOR))
    {
        OOPS();
        return NULL;
    }

    // Now request the entire BOS Descriptor using a dynamically
    // allocated buffer which is sized big enough to hold the entire descriptor
    //
    nBytes = sizeof(USB_DESCRIPTOR_REQUEST) + bosDesc->wTotalLength;

    bosDescReq = (PUSB_DESCRIPTOR_REQUEST)ALLOC(nBytes);

    if (bosDescReq == NULL)
    {
        OOPS();
        return NULL;
    }

    bosDesc = (PUSB_BOS_DESCRIPTOR)(bosDescReq+1);

    // Indicate the port from which the descriptor will be requested
    //
    bosDescReq->ConnectionIndex = ConnectionIndex;

    //
    // USBHUB uses URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE to process this
    // IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION request.
    //
    // USBD will automatically initialize these fields:
    //     bmRequest = 0x80
    //     bRequest  = 0x06
    //
    // We must inititialize these fields:
    //     wValue    = Descriptor Type (high) and Descriptor Index (low byte)
    //     wIndex    = Zero (or Language ID for String Descriptors)
    //     wLength   = Length of descriptor buffer
    //
    bosDescReq->SetupPacket.wValue = (USB_BOS_DESCRIPTOR_TYPE << 8);

    bosDescReq->SetupPacket.wLength = (USHORT)(nBytes - sizeof(USB_DESCRIPTOR_REQUEST));

    // Now issue the get descriptor request.
    //

    success = DeviceIoControl(hHubDevice,
                              IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
                              bosDescReq,
                              nBytes,
                              bosDescReq,
                              nBytes,
                              &nBytesReturned,
                              NULL);

    if (!success)
    {
        OOPS();
        FREE(bosDescReq);
        return NULL;
    }

    if (nBytes != nBytesReturned)
    {
        OOPS();
        FREE(bosDescReq);
        return NULL;
    }

    if (bosDesc->wTotalLength != (nBytes - sizeof(USB_DESCRIPTOR_REQUEST)))
    {
        OOPS();
        FREE(bosDescReq);
        return NULL;
    }

    return bosDescReq;
}

//*****************************************************************************
//
// AreThereStringDescriptors()
//
// DeviceDesc - Device Descriptor for which String Descriptors should be
// checked.
//
// ConfigDesc - Configuration Descriptor (also containing Interface Descriptor)
// for which String Descriptors should be checked.
//
//*****************************************************************************

BOOL
AreThereStringDescriptors (
    PUSB_DEVICE_DESCRIPTOR          DeviceDesc,
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc
)
{
    PUCHAR                  descEnd = NULL;
    PUSB_COMMON_DESCRIPTOR  commonDesc = NULL;

    //
    // Check Device Descriptor strings
    //

    if (DeviceDesc->iManufacturer ||
        DeviceDesc->iProduct      ||
        DeviceDesc->iSerialNumber
       )
    {
        return TRUE;
    }


    //
    // Check the Configuration and Interface Descriptor strings
    //

    descEnd = (PUCHAR)ConfigDesc + ConfigDesc->wTotalLength;

    commonDesc = (PUSB_COMMON_DESCRIPTOR)ConfigDesc;

    while ((PUCHAR)commonDesc + sizeof(USB_COMMON_DESCRIPTOR) < descEnd &&
           (PUCHAR)commonDesc + commonDesc->bLength <= descEnd)
    {
        switch (commonDesc->bDescriptorType)
        {
            case USB_CONFIGURATION_DESCRIPTOR_TYPE:
            case USB_OTHER_SPEED_CONFIGURATION_DESCRIPTOR_TYPE:
                if (commonDesc->bLength != sizeof(USB_CONFIGURATION_DESCRIPTOR))
                {
                    OOPS();
                    break;
                }
                if (((PUSB_CONFIGURATION_DESCRIPTOR)commonDesc)->iConfiguration)
                {
                    return TRUE;
                }
                commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
                continue;

            case USB_INTERFACE_DESCRIPTOR_TYPE:
                if (commonDesc->bLength != sizeof(USB_INTERFACE_DESCRIPTOR) &&
                    commonDesc->bLength != sizeof(USB_INTERFACE_DESCRIPTOR2))
                {
                    OOPS();
                    break;
                }
                if (((PUSB_INTERFACE_DESCRIPTOR)commonDesc)->iInterface)
                {
                    return TRUE;
                }
                commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
                continue;

            default:
                commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
                continue;
        }
        break;
    }

    return FALSE;
}

//*****************************************************************************
//
// GetAllStringDescriptors()
//
// hHubDevice - Handle of the hub device containing the port from which the
// String Descriptors will be requested.
//
// ConnectionIndex - Identifies the port on the hub to which a device is
// attached from which the String Descriptors will be requested.
//
// DeviceDesc - Device Descriptor for which String Descriptors should be
// requested.
//
// ConfigDesc - Configuration Descriptor (also containing Interface Descriptor)
// for which String Descriptors should be requested.
//
//*****************************************************************************

PSTRING_DESCRIPTOR_NODE
GetAllStringDescriptors (
    HANDLE                          hHubDevice,
    ULONG                           ConnectionIndex,
    PUSB_DEVICE_DESCRIPTOR          DeviceDesc,
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc
)
{
    PSTRING_DESCRIPTOR_NODE supportedLanguagesString = NULL;
    ULONG                   numLanguageIDs = 0;
    USHORT                  *languageIDs = NULL;

    PUCHAR                  descEnd = NULL;
    PUSB_COMMON_DESCRIPTOR  commonDesc = NULL;
    UCHAR                   uIndex = 1;
    UCHAR                   bInterfaceClass = 0;
    BOOL                    getMoreStrings = FALSE;
    HRESULT                 hr = S_OK;

    //
    // Get the array of supported Language IDs, which is returned
    // in String Descriptor 0
    //
    supportedLanguagesString = GetStringDescriptor(hHubDevice,
                                                   ConnectionIndex,
                                                   0,
                                                   0);

    if (supportedLanguagesString == NULL)
    {
        return NULL;
    }

    numLanguageIDs = (supportedLanguagesString->StringDescriptor->bLength - 2) / 2;

    languageIDs = (USHORT *)(&supportedLanguagesString->StringDescriptor->bString[0]);

    //
    // Get the Device Descriptor strings
    //

    if (DeviceDesc->iManufacturer)
    {
        GetStringDescriptors(hHubDevice,
                             ConnectionIndex,
                             DeviceDesc->iManufacturer,
                             numLanguageIDs,
                             languageIDs,
                             supportedLanguagesString);
    }

    if (DeviceDesc->iProduct)
    {
        GetStringDescriptors(hHubDevice,
                             ConnectionIndex,
                             DeviceDesc->iProduct,
                             numLanguageIDs,
                             languageIDs,
                             supportedLanguagesString);
    }

    if (DeviceDesc->iSerialNumber)
    {
        GetStringDescriptors(hHubDevice,
                             ConnectionIndex,
                             DeviceDesc->iSerialNumber,
                             numLanguageIDs,
                             languageIDs,
                             supportedLanguagesString);
    }

    //
    // Get the Configuration and Interface Descriptor strings
    //

    descEnd = (PUCHAR)ConfigDesc + ConfigDesc->wTotalLength;

    commonDesc = (PUSB_COMMON_DESCRIPTOR)ConfigDesc;

    while ((PUCHAR)commonDesc + sizeof(USB_COMMON_DESCRIPTOR) < descEnd &&
           (PUCHAR)commonDesc + commonDesc->bLength <= descEnd)
    {
        switch (commonDesc->bDescriptorType)
        {
            case USB_CONFIGURATION_DESCRIPTOR_TYPE:
                if (commonDesc->bLength != sizeof(USB_CONFIGURATION_DESCRIPTOR))
                {
                    OOPS();
                    break;
                }
                if (((PUSB_CONFIGURATION_DESCRIPTOR)commonDesc)->iConfiguration)
                {
                    GetStringDescriptors(hHubDevice,
                                         ConnectionIndex,
                                         ((PUSB_CONFIGURATION_DESCRIPTOR)commonDesc)->iConfiguration,
                                         numLanguageIDs,
                                         languageIDs,
                                         supportedLanguagesString);
                }
                commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
                continue;

            case USB_IAD_DESCRIPTOR_TYPE:
                if (commonDesc->bLength < sizeof(USB_IAD_DESCRIPTOR))
                {
                    OOPS();
                    break;
                }
                if (((PUSB_IAD_DESCRIPTOR)commonDesc)->iFunction)
                {
                    GetStringDescriptors(hHubDevice,
                                         ConnectionIndex,
                                         ((PUSB_IAD_DESCRIPTOR)commonDesc)->iFunction,
                                         numLanguageIDs,
                                         languageIDs,
                                         supportedLanguagesString);
                }
                commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
                continue;

            case USB_INTERFACE_DESCRIPTOR_TYPE:
                if (commonDesc->bLength != sizeof(USB_INTERFACE_DESCRIPTOR) &&
                    commonDesc->bLength != sizeof(USB_INTERFACE_DESCRIPTOR2))
                {
                    OOPS();
                    break;
                }
                if (((PUSB_INTERFACE_DESCRIPTOR)commonDesc)->iInterface)
                {
                    GetStringDescriptors(hHubDevice,
                                         ConnectionIndex,
                                         ((PUSB_INTERFACE_DESCRIPTOR)commonDesc)->iInterface,
                                         numLanguageIDs,
                                         languageIDs,
                                         supportedLanguagesString);
                }

                //
                // We need to display more string descriptors for the following
                // interface classes
                //
                bInterfaceClass = ((PUSB_INTERFACE_DESCRIPTOR)commonDesc)->bInterfaceClass;
                if (bInterfaceClass == USB_DEVICE_CLASS_VIDEO)
                {
                    getMoreStrings = TRUE;
                }
                commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
                continue;

            default:
                commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
                continue;
        }
        break;
    }

    if (getMoreStrings)
    {
        //
        // We might need to display strings later that are referenced only in
        // class-specific descriptors. Get String Descriptors 1 through 32 (an
        // arbitrary upper limit for Strings needed due to "bad devices"
        // returning an infinite repeat of Strings 0 through 4) until one is not
        // found.
        //
        // There are also "bad devices" that have issues even querying 1-32, but
        // historically USBView made this query, so the query should be safe for
        // video devices.
        //
        for (uIndex = 1; SUCCEEDED(hr) && (uIndex < NUM_STRING_DESC_TO_GET); uIndex++) 
        {
            hr = GetStringDescriptors(hHubDevice,
                                      ConnectionIndex,
                                      uIndex,
                                      numLanguageIDs,
                                      languageIDs,
                                      supportedLanguagesString);
        }
    }

    return supportedLanguagesString;
}

//*****************************************************************************
//
// GetExternalHubName()
//
//*****************************************************************************

PCHAR GetExternalHubName (
    HANDLE  Hub,
    ULONG   ConnectionIndex
)
{
    BOOL                        success = 0;
    ULONG                       nBytes = 0;
    USB_NODE_CONNECTION_NAME    extHubName;
    PUSB_NODE_CONNECTION_NAME   extHubNameW = NULL;
    PCHAR                       extHubNameA = NULL;

    // Get the length of the name of the external hub attached to the
    // specified port.
    //
    extHubName.ConnectionIndex = ConnectionIndex;

    success = DeviceIoControl(Hub,
                              IOCTL_USB_GET_NODE_CONNECTION_NAME,
                              &extHubName,
                              sizeof(extHubName),
                              &extHubName,
                              sizeof(extHubName),
                              &nBytes,
                              NULL);

    if (!success)
    {
        OOPS();
        goto GetExternalHubNameError;
    }

    // Allocate space to hold the external hub name
    //
    nBytes = extHubName.ActualLength;

    if (nBytes <= sizeof(extHubName))
    {
        OOPS();
        goto GetExternalHubNameError;
    }

    extHubNameW = (PUSB_NODE_CONNECTION_NAME)ALLOC(nBytes);

    if (extHubNameW == NULL)
    {
        OOPS();
        goto GetExternalHubNameError;
    }

    // Get the name of the external hub attached to the specified port
    //
    extHubNameW->ConnectionIndex = ConnectionIndex;

    success = DeviceIoControl(Hub,
                              IOCTL_USB_GET_NODE_CONNECTION_NAME,
                              extHubNameW,
                              nBytes,
                              extHubNameW,
                              nBytes,
                              &nBytes,
                              NULL);

    if (!success)
    {
        OOPS();
        goto GetExternalHubNameError;
    }

    // Convert the External Hub name
    //
    extHubNameA = WideStrToMultiStr(extHubNameW->NodeName, nBytes);

    // All done, free the uncoverted external hub name and return the
    // converted external hub name
    //
    FREE(extHubNameW);

    return extHubNameA;


GetExternalHubNameError:
    // There was an error, free anything that was allocated
    //
    if (extHubNameW != NULL)
    {
        FREE(extHubNameW);
        extHubNameW = NULL;
    }

    return NULL;
}

//*****************************************************************************
//
// GetStringDescriptor()
//
// hHubDevice - Handle of the hub device containing the port from which the
// String Descriptor will be requested.
//
// ConnectionIndex - Identifies the port on the hub to which a device is
// attached from which the String Descriptor will be requested.
//
// DescriptorIndex - String Descriptor index.
//
// LanguageID - Language in which the string should be requested.
//
//*****************************************************************************

PSTRING_DESCRIPTOR_NODE
GetStringDescriptor (
    HANDLE  hHubDevice,
    ULONG   ConnectionIndex,
    UCHAR   DescriptorIndex,
    USHORT  LanguageID
)
{
    BOOL    success = 0;
    ULONG   nBytes = 0;
    ULONG   nBytesReturned = 0;

    UCHAR   stringDescReqBuf[sizeof(USB_DESCRIPTOR_REQUEST) +
                             MAXIMUM_USB_STRING_LENGTH];

    PUSB_DESCRIPTOR_REQUEST stringDescReq = NULL;
    PUSB_STRING_DESCRIPTOR  stringDesc = NULL;
    PSTRING_DESCRIPTOR_NODE stringDescNode = NULL;

    nBytes = sizeof(stringDescReqBuf);

    stringDescReq = (PUSB_DESCRIPTOR_REQUEST)stringDescReqBuf;
    stringDesc = (PUSB_STRING_DESCRIPTOR)(stringDescReq+1);

    // Zero fill the entire request structure
    //
    memset(stringDescReq, 0, nBytes);

    // Indicate the port from which the descriptor will be requested
    //
    stringDescReq->ConnectionIndex = ConnectionIndex;

    //
    // USBHUB uses URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE to process this
    // IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION request.
    //
    // USBD will automatically initialize these fields:
    //     bmRequest = 0x80
    //     bRequest  = 0x06
    //
    // We must inititialize these fields:
    //     wValue    = Descriptor Type (high) and Descriptor Index (low byte)
    //     wIndex    = Zero (or Language ID for String Descriptors)
    //     wLength   = Length of descriptor buffer
    //
    stringDescReq->SetupPacket.wValue = (USB_STRING_DESCRIPTOR_TYPE << 8)
                                        | DescriptorIndex;

    stringDescReq->SetupPacket.wIndex = LanguageID;

    stringDescReq->SetupPacket.wLength = (USHORT)(nBytes - sizeof(USB_DESCRIPTOR_REQUEST));

    // Now issue the get descriptor request.
    //
    success = DeviceIoControl(hHubDevice,
                              IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
                              stringDescReq,
                              nBytes,
                              stringDescReq,
                              nBytes,
                              &nBytesReturned,
                              NULL);

    //
    // Do some sanity checks on the return from the get descriptor request.
    //

    if (!success)
    {
        OOPS();
        return NULL;
    }

    if (nBytesReturned < 2)
    {
        OOPS();
        return NULL;
    }

    if (stringDesc->bDescriptorType != USB_STRING_DESCRIPTOR_TYPE)
    {
        OOPS();
        return NULL;
    }

    if (stringDesc->bLength != nBytesReturned - sizeof(USB_DESCRIPTOR_REQUEST))
    {
        OOPS();
        return NULL;
    }

    if (stringDesc->bLength % 2 != 0)
    {
        OOPS();
        return NULL;
    }

    //
    // Looks good, allocate some (zero filled) space for the string descriptor
    // node and copy the string descriptor to it.
    //

    stringDescNode = (PSTRING_DESCRIPTOR_NODE)ALLOC(sizeof(STRING_DESCRIPTOR_NODE) +
                                                    stringDesc->bLength);

    if (stringDescNode == NULL)
    {
        OOPS();
        return NULL;
    }

    stringDescNode->DescriptorIndex = DescriptorIndex;
    stringDescNode->LanguageID = LanguageID;

    memcpy(stringDescNode->StringDescriptor,
           stringDesc,
           stringDesc->bLength);

    return stringDescNode;
}

//*****************************************************************************
//
// GetStringDescriptors()
//
// hHubDevice - Handle of the hub device containing the port from which the
// String Descriptor will be requested.GetStringDescriptors
//
// ConnectionIndex - Identifies the port on the hub to which a device is
// attached from which the String Descriptor will be requested.
//
// DescriptorIndex - String Descriptor index.
//
// NumLanguageIDs -  Number of languages in which the string should be
// requested.
//
// LanguageIDs - Languages in which the string should be requested.
//
// StringDescNodeHead - First node in linked list of device's string descriptors
//
// Return Value: HRESULT indicating whether the string is on the list
//
//*****************************************************************************

HRESULT
GetStringDescriptors (
    _In_ HANDLE                         hHubDevice,
    _In_ ULONG                          ConnectionIndex,
    _In_ UCHAR                          DescriptorIndex,
    _In_ ULONG                          NumLanguageIDs,
    _In_reads_(NumLanguageIDs) USHORT  *LanguageIDs,
    _In_ PSTRING_DESCRIPTOR_NODE        StringDescNodeHead
)
{
    PSTRING_DESCRIPTOR_NODE tail = NULL;
    PSTRING_DESCRIPTOR_NODE trailing = NULL;
    ULONG i = 0;

    //
    // Go to the end of the linked list, searching for the requested index to
    // see if we've already retrieved it
    //
    for (tail = StringDescNodeHead; tail != NULL; tail = tail->Next)
    {
        if (tail->DescriptorIndex == DescriptorIndex)
        {
            return S_OK;
        }

        trailing = tail;
    }

    tail = trailing;

    //
    // Get the next String Descriptor. If this is NULL, then we're done (return)
    // Otherwise, loop through all Language IDs
    //
    for (i = 0; (tail != NULL) && (i < NumLanguageIDs); i++)
    {
        tail->Next = GetStringDescriptor(hHubDevice,
                                         ConnectionIndex,
                                         DescriptorIndex,
                                         LanguageIDs[i]);

        tail = tail->Next;
    }

    if (tail == NULL)
    {
        return E_FAIL;
    } else {
        return S_OK;
    }
}

// 출력함수 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID
DisplayConnectionInfo (
    _In_     PUSB_NODE_CONNECTION_INFORMATION_EX    ConnectInfo,
    _In_     PUSBDEVICEINFO                         info,
    _In_     PSTRING_DESCRIPTOR_NODE                StringDescs,
    _In_opt_ PUSB_NODE_CONNECTION_INFORMATION_EX_V2 ConnectionInfoV2
)
{

    //@@DisplayConnectionInfo - Device Information
    PCHAR                               VendorString = NULL;
    UINT                                tog = 1;
    UINT                                uIADcount = 0;

    // No device connected
    if (ConnectInfo->ConnectionStatus == NoDeviceConnected)
    {
        printf("ConnectionStatus:      NoDeviceConnected\r\n");
        return;
    }

    // This is the entry point to the device display functions.
    // First, save this device's PUSBDEVICEINFO address
    // In a future version of this test, we will keep track of the the 
    //  descriptor that we're parsing (# of bytes from beginning of info->configuration descriptor)
    // Then we can linked descriptors by reading forward through the remaining descriptors
    //  while still keeping our place in this main DisplayConnectionInfo() and called
    //  functions.
    //
    // We also initialize some global flags in uvcview.h that are used to 
    //  verify items in MJPEG, Uncompressed and Vendor Frame descriptors
    //
    InitializePerDeviceSettings(info);
    
    if(gDoAnnotation)
    {

        printf("       ---===>Device Information<===---\r\n");

        if (ConnectInfo->DeviceDescriptor.iProduct)
        {
            DisplayUSEnglishStringDescriptor(ConnectInfo->DeviceDescriptor.iProduct,
                StringDescs,
                info->DeviceInfoNode != NULL? info->DeviceInfoNode->LatestDevicePowerState: PowerDeviceUnspecified);
        }

        printf("\r\nConnectionStatus:                  %s\r\n",
            ConnectionStatuses[ConnectInfo->ConnectionStatus]);

        printf("Current Config Value:              0x%02X",
            ConnectInfo->CurrentConfigurationValue);

		
    }

    switch (ConnectInfo->Speed){
    case UsbLowSpeed:
        if(gDoAnnotation)
        {
            printf("  -> Device Bus Speed: Low\r\n");
        }
        else 
        {
            printf("\r\n");
        }
        gDeviceSpeed = UsbLowSpeed;
        break;
        
    case UsbFullSpeed:
        if(gDoAnnotation)
        {
            printf("  -> Device Bus Speed: Full");
            if (ConnectionInfoV2 != NULL)
            {
                printf(" (is %sSuperSpeed or higher capable)\r\n",
                                 ConnectionInfoV2->Flags.DeviceIsSuperSpeedCapableOrHigher 
                                 ? "" : "not ");
            }
            else
            {
                printf("\r\n");
            }
         }
        else 
        {
            printf("\r\n");
        }
        gDeviceSpeed = UsbFullSpeed;
        break;
    case UsbHighSpeed:
        if(gDoAnnotation)
        {
            printf("  -> Device Bus Speed: High");
            if (ConnectionInfoV2 != NULL)
            {
                printf(" (is %sSuperSpeed or higher capable)\r\n",
                                 ConnectionInfoV2->Flags.DeviceIsSuperSpeedCapableOrHigher 
                                 ? "" : "not ");
            }
            else
            {
                printf("\r\n");
            }
        }
        else 
        {
            printf("\r\n");
        }
        gDeviceSpeed = UsbHighSpeed;
        break;
        
    case UsbSuperSpeed:
        if(gDoAnnotation)
        {
            printf("  -> Device Bus Speed: Super\r\n");
        }
        else 
        {
            printf("\r\n");
        }
        gDeviceSpeed = UsbSuperSpeed;
        break;
    default:
        if(gDoAnnotation){printf("  -> Device Bus Speed: Unknown\r\n");}
        else {printf("\r\n");}
    }

    if(gDoAnnotation){
			printf("Device Address:                    0x%02X\r\n",
				ConnectInfo->DeviceAddress);

			printf("Open Pipes:                          %2d\r\n",
				ConnectInfo->NumberOfOpenPipes);
    }
    
    // No open pipes means the USB stack has not loaded the device
    if (ConnectInfo->NumberOfOpenPipes == 0)
    {
        printf("*!*ERROR:  No open pipes!\r\n");
    }

    printf("\r\n          ===>Device Descriptor<===\r\n");
    //@@DisplayConnectionInfo - Device Descriptor

    if (ConnectInfo->DeviceDescriptor.bLength != 18)
    {
        //@@TestCase A1.1
        //@@ERROR
        //@@Descriptor Field - bLength
        //@@The declared length in the device descriptor is not equal to the 
        //@@  required length in the USB Device Specification
        printf("*!*ERROR:  bLength of %d incorrect, should be %d\r\n",
            ConnectInfo->DeviceDescriptor.bLength,
            18);
        OOPS();
    }

    printf("bLength:                           0x%02X\r\n",
        ConnectInfo->DeviceDescriptor.bLength);

    printf("bDescriptorType:                   0x%02X\r\n",
        ConnectInfo->DeviceDescriptor.bDescriptorType);

    //@@TestCase A1.2
    //@@Not implemented - Priority 1
    //@@Descriptor Field - bcdUSB
    //@@Need to check that any UVC device is set to 0x0200 or later.
    printf("bcdUSB:                          0x%04X\r\n",
        ConnectInfo->DeviceDescriptor.bcdUSB);

    printf("bDeviceClass:                      0x%02X",
        ConnectInfo->DeviceDescriptor.bDeviceClass);

    // Quit on these device failures
    if ((ConnectInfo->ConnectionStatus == DeviceFailedEnumeration) || 
        (ConnectInfo->ConnectionStatus == DeviceGeneralFailure))
    {
        printf("\r\n*!*ERROR:  Device enumeration failure\r\n");
        return;
    }

    // Is this an IAD device?
    uIADcount = IsIADDevice((PUSBDEVICEINFO) info);

    if (uIADcount)
    {
        // this device configuration has 1 or more IAD descriptors
        if (ConnectInfo->DeviceDescriptor.bDeviceClass == USB_MISCELLANEOUS_DEVICE)
        {
            tog = 0;
            if (gDoAnnotation)
            {
                printf("  -> This is a Multi-interface Function Code Device\r\n");
            }
            else 
            {
                printf("\r\n");
            }
        } else {
            printf("\r\n*!*ERROR: device class should be Multi-interface Function 0x%02X\r\n"\
                "          When IAD descriptor is used\r\n",
                USB_MISCELLANEOUS_DEVICE);
        }
        // Is this a UVC device?
        g_chUVCversion = IsUVCDevice((PUSBDEVICEINFO) info);
    } 
    else 
    {
        // this is not an IAD device
        switch (ConnectInfo->DeviceDescriptor.bDeviceClass)
        {
        case USB_INTERFACE_CLASS_DEVICE:
            if(gDoAnnotation)
            {printf("  -> This is an Interface Class Defined Device\r\n");}
            else {printf("\r\n");}
            break;

        case USB_COMMUNICATION_DEVICE:
            tog = 0;
            if(gDoAnnotation)
            {printf("  -> This is a Communication Device\r\n");}
            else {printf("\r\n");}
            break;

        case USB_HUB_DEVICE:
            tog = 0;
            if(gDoAnnotation)
            {printf("  -> This is a HUB Device\r\n");}
            else {printf("\r\n");}
            break;

        case USB_DIAGNOSTIC_DEVICE:
            tog = 0;
            if(gDoAnnotation)
            {printf("  -> This is a Diagnostic Device\r\n");}
            else {printf("\r\n");}
            break;

        case USB_WIRELESS_CONTROLLER_DEVICE:
            tog = 0;
            if(gDoAnnotation)
            {printf("  -> This is a Wireless Controller(Bluetooth) Device\r\n");}
            else {printf("\r\n");}
            break;

        case USB_VENDOR_SPECIFIC_DEVICE:
            tog = 0;
            if(gDoAnnotation)
            {printf("  -> This is a Vendor Specific Device\r\n");}
            else {printf("\r\n");}
            break;

        case USB_MISCELLANEOUS_DEVICE:
            tog = 0;
            //@@TestCase A1.3
            //@@ERROR
            //@@Descriptor Field - bDeviceClass
            //@@Multi-interface Function code used for non-IAD device
            printf("\r\n*!*ERROR:  Multi-interface Function code %d used for "\
                "device with no IAD descriptors\r\n",
                ConnectInfo->DeviceDescriptor.bDeviceClass);
            break;

        default:
            //@@TestCase A1.4
            //@@ERROR
            //@@Descriptor Field - bDeviceClass
            //@@An unknown device class has been defined
            printf("\r\n*!*ERROR:  unknown bDeviceClass %d\r\n",
                ConnectInfo->DeviceDescriptor.bDeviceClass);
            OOPS();
            break;
        }
    }

    printf("bDeviceSubClass:                   0x%02X",
        ConnectInfo->DeviceDescriptor.bDeviceSubClass);

    // check the subclass
    if (uIADcount)
    {
        // this device configuration has 1 or more IAD descriptors
        if (ConnectInfo->DeviceDescriptor.bDeviceSubClass == USB_COMMON_SUB_CLASS)
        {
            if (gDoAnnotation)
            {
                printf("  -> This is the Common Class Sub Class\r\n");
            } else 
            {
                printf("\r\n");
            }
        } 
        else 
        {
            //@@TestCase A1.5
            //@@ERROR
            //@@Descriptor Field - bDeviceSubClass
            //@@An invalid device sub class used for Multi-interface Function (IAD) device
            printf("\r\n*!*ERROR: device SubClass should be USB Common Sub Class %d\r\n"\
                "          When IAD descriptor is used\r\n",
                USB_COMMON_SUB_CLASS);
            OOPS();
        }
    } 
    else 
    {
        // Not an IAD device, so all subclass values are invalid
        if(ConnectInfo->DeviceDescriptor.bDeviceSubClass > 0x00 && 
            ConnectInfo->DeviceDescriptor.bDeviceSubClass < 0xFF)
        {
            //@@TestCase A1.6
            //@@ERROR
            //@@Descriptor Field - bDeviceSubClass
            //@@An invalid device sub class has been defined
            printf("\r\n*!*ERROR:  bDeviceSubClass of %d is invalid\r\n",
                ConnectInfo->DeviceDescriptor.bDeviceSubClass);
            OOPS();
        } else 
        {
            printf("\r\n");

        }
    }

    printf("bDeviceProtocol:                   0x%02X",
        ConnectInfo->DeviceDescriptor.bDeviceProtocol);

    // check the protocol
    if (uIADcount)
    {
        // this device configuration has 1 or more IAD descriptors
        if (ConnectInfo->DeviceDescriptor.bDeviceProtocol == USB_IAD_PROTOCOL)
        {
            if (gDoAnnotation)
            {
                printf("  -> This is the Interface Association Descriptor protocol\r\n");
            } 
            else 
            {
                printf("\r\n");
            }
        } 
        else 
        {
            //@@TestCase A1.7
            //@@ERROR
            //@@Descriptor Field - bDeviceSubClass
            //@@An invalid device sub class used for Multi-interface Function (IAD) device
            printf("\r\n*!*ERROR: device Protocol should be USB IAD Protocol %d\r\n"\
                "          When IAD descriptor is used\r\n",
                USB_IAD_PROTOCOL);
            OOPS();
        }
    } 
    else 
    {
        // Not an IAD device, so all subclass values are invalid
        if(ConnectInfo->DeviceDescriptor.bDeviceProtocol > 0x00 && 
            ConnectInfo->DeviceDescriptor.bDeviceProtocol < 0xFF && tog==1)
        {
            //@@TestCase A1.8
            //@@ERROR
            //@@Descriptor Field - bDeviceProtocol
            //@@An invalid device protocol has been defined
            printf("\r\n*!*ERROR:  bDeviceProtocol of %d is invalid\r\n",
                ConnectInfo->DeviceDescriptor.bDeviceProtocol);
            OOPS();
        } 
        else
        {
            printf("\r\n");
        }
    }

    printf("bMaxPacketSize0:                   0x%02X",
        ConnectInfo->DeviceDescriptor.bMaxPacketSize0);

    if(gDoAnnotation)
    {
        printf(" = (%d) Bytes\r\n",
            ConnectInfo->DeviceDescriptor.bMaxPacketSize0);
    } 
    else 
    {
        printf("\r\n");
    }

    switch (gDeviceSpeed){
        case UsbLowSpeed:
            if(ConnectInfo->DeviceDescriptor.bMaxPacketSize0 != 8)
            {
                //@@TestCase A1.9
                //@@ERROR
                //@@Descriptor Field - bMaxPacketSize0
                //@@An invalid bMaxPacketSize0 has been defined for a low speed device
                printf("*!*ERROR:  Low Speed Devices require bMaxPacketSize0 = 8\r\n");
                OOPS();
            }
            break;
        case UsbFullSpeed:
            if(!(ConnectInfo->DeviceDescriptor.bMaxPacketSize0 == 8 ||
                ConnectInfo->DeviceDescriptor.bMaxPacketSize0 == 16 ||
                ConnectInfo->DeviceDescriptor.bMaxPacketSize0 == 32 ||
                ConnectInfo->DeviceDescriptor.bMaxPacketSize0 == 64))
            {
                //@@TestCase A1.10
                //@@ERROR
                //@@Descriptor Field - bMaxPacketSize0
                //@@An invalid bMaxPacketSize0 has been defined for a full speed device
                printf("*!*ERROR:  Full Speed Devices require bMaxPacketSize0 = 8, 16, 32, or 64\r\n");
                OOPS();
            }
            break;
        case UsbHighSpeed:
            if(ConnectInfo->DeviceDescriptor.bMaxPacketSize0 != 64)
            {
                //@@TestCase A1.11
                //@@ERROR
                //@@Descriptor Field - bMaxPacketSize0
                //@@An invalid bMaxPacketSize0 has been defined for a high speed device
                printf("*!*ERROR:  High Speed Devices require bMaxPacketSize0 = 64\r\n");
                OOPS();
            }
            break;
        case UsbSuperSpeed:
            if(ConnectInfo->DeviceDescriptor.bMaxPacketSize0 != 9)
            {
                printf("*!*ERROR:  SuperSpeed Devices require bMaxPacketSize0 = 9 (512)\r\n");
                OOPS();
            }
            break;
    }

    printf("idVendor:                        0x%04X",
        ConnectInfo->DeviceDescriptor.idVendor);

    if (gDoAnnotation)
    {
        VendorString = GetVendorString(ConnectInfo->DeviceDescriptor.idVendor);
        if (VendorString != NULL)
        {
            printf(" = %s\r\n",
                VendorString);
        }
    }
    else {printf("\r\n");}

    printf("idProduct:                       0x%04X\r\n",
        ConnectInfo->DeviceDescriptor.idProduct);

    printf("bcdDevice:                       0x%04X\r\n",
        ConnectInfo->DeviceDescriptor.bcdDevice);

    printf("iManufacturer:                     0x%02X\r\n",
        ConnectInfo->DeviceDescriptor.iManufacturer);

    if (ConnectInfo->DeviceDescriptor.iManufacturer && gDoAnnotation)
    {
        DisplayStringDescriptor(ConnectInfo->DeviceDescriptor.iManufacturer,
            StringDescs,
            info->DeviceInfoNode != NULL? info->DeviceInfoNode->LatestDevicePowerState: PowerDeviceUnspecified);
    }

    printf("iProduct:                          0x%02X\r\n",
        ConnectInfo->DeviceDescriptor.iProduct);

    if (ConnectInfo->DeviceDescriptor.iProduct && gDoAnnotation)
    {
        DisplayStringDescriptor(ConnectInfo->DeviceDescriptor.iProduct,
            StringDescs,
            info->DeviceInfoNode != NULL? info->DeviceInfoNode->LatestDevicePowerState: PowerDeviceUnspecified);
    }

    printf("iSerialNumber:                     0x%02X\r\n",
        ConnectInfo->DeviceDescriptor.iSerialNumber);

    if (ConnectInfo->DeviceDescriptor.iSerialNumber && gDoAnnotation)
    {
        DisplayStringDescriptor(ConnectInfo->DeviceDescriptor.iSerialNumber,
            StringDescs,
            info->DeviceInfoNode != NULL? info->DeviceInfoNode->LatestDevicePowerState: PowerDeviceUnspecified);
    }

    printf("bNumConfigurations:                0x%02X\r\n",
        ConnectInfo->DeviceDescriptor.bNumConfigurations);

    if(ConnectInfo->DeviceDescriptor.bNumConfigurations != 1)
    {
        //@@TestCase A1.12
        //@@CAUTION
        //@@Descriptor Field - bNumConfigurations
        //@@Most host controllers do not handle more than one configuration
        printf("*!*CAUTION:    Most host controllers will only work with "\
            "one configuration per speed\r\n");
        OOPS();
    }

    if (ConnectInfo->NumberOfOpenPipes)
    {
        printf("\r\n          ---===>Open Pipes<===---\r\n");
        DisplayPipeInfo(ConnectInfo->NumberOfOpenPipes,
                        ConnectInfo->PipeList);
    }

    return;
}

void
InitializePerDeviceSettings (
    PUSBDEVICEINFO info
    )
{
    // Save base address for this current device's info (including Configuration descriptor)
    CurrentUSBDeviceInfo = info;

    // Initialize Configuration descriptor length
    dwConfigLength = 0;

    // Save # of bytes from start of Configuration descriptor
    // (Update this in the descriptor parsing routines)
    dwConfigIndex = 0;

    // Flags used in dispvid.c to display default Frame descriptor for MJPEG, 
    //  Uncompressed, Vendor and FrameBased Formats
    g_chMJPEGFrameDefault = 0;
    g_chUNCFrameDefault = 0;
    g_chVendorFrameDefault = 0;
    g_chFrameBasedFrameDefault = 0;

    // Spec version of UVC device
    g_chUVCversion = 0;

    // Start and end address of the configuration descriptor and start of the string descriptors
    g_pConfigDesc  = NULL;
    g_pStringDescs = NULL;
    g_descEnd      = NULL;

    // 
    // The GetConfigDescriptor() function in enum.c does not always work
    // If that fails, the Configuration descriptor will be NULL 
    //  and we can only display the device descriptor
    //
    CurrentConfigDesc = NULL;
    if (NULL != info)
    {
         if (NULL != info->ConfigDesc)
        {
            CurrentConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)(info->ConfigDesc + 1);

            // Save the LENGTH of the Config descriptor
            // Note that IsIADDevice() saves the ADDRESS of the END of the Config desc
            // Be aware of the difference
            dwConfigLength = CurrentConfigDesc->wTotalLength;
        }
    }

    return;
}

VOID
DisplayUSEnglishStringDescriptor (
    UCHAR                       Index,
    PSTRING_DESCRIPTOR_NODE     USStringDescs,
    DEVICE_POWER_STATE          LatestDevicePowerState
    )
{
    ULONG nBytes = 0;
    BOOLEAN FoundMatchingString = FALSE;
    CHAR  pString[512];

    //@@DisplayUSEnglishStringDescriptor - String Descriptor
    while (USStringDescs)
    {
        if (USStringDescs->DescriptorIndex == Index)
        {
            if (USStringDescs->LanguageID != 0x0409)
                continue;

            FoundMatchingString = TRUE;

            printf("English product name: \"");
            memset(pString, 0, 512);
            nBytes = WideCharToMultiByte(
                CP_ACP,     // CodePage
                WC_NO_BEST_FIT_CHARS,
                USStringDescs->StringDescriptor->bString,
                (USStringDescs->StringDescriptor->bLength - 2) / 2,
                pString,
                512,
                NULL,       // lpDefaultChar
                NULL);      // pUsedDefaultChar
            if (nBytes)
                printf("%s\"\r\n", pString);
            else
                printf("\"\r\n", pString);
            return;
        }
        USStringDescs = USStringDescs->Next;
    }

    //@@TestCase A8.1
    //@@WARNING
    //@@Descriptor Field - string index
    //@@No support for english
    if (!FoundMatchingString)
    {
        if (LatestDevicePowerState == PowerDeviceD0)
        {
            printf("*!*ERROR:  No String Descriptor for index %d!\r\n", Index);
            OOPS();
        } 
        else
        {
            printf("String Descriptor for index %d not available while device is in low power state.\r\n", Index);
        }
    }
    else
    {
        printf("*!*ERROR:  The index selected does not support English(US)\r\n");
        OOPS();
    }
    return;

}

UINT
IsIADDevice (
    PUSBDEVICEINFO info
    )
{
    PUSB_CONFIGURATION_DESCRIPTOR  ConfigDesc = NULL;
    PUSB_COMMON_DESCRIPTOR         commonDesc = NULL;
    PUCHAR                         descEnd = NULL;
    UINT  uIADcount = 0;

    // 
    // The GetConfigDescriptor() function in enum.c does not always work
    // If that fails, the Configuration descriptor will be NULL 
    //  and we can only display the device descriptor
    // 
    if (NULL == info)
    {
        return 0;
    }
    if (NULL == info->ConfigDesc)
    {
        return 0;
    }

    ConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)(info->ConfigDesc + 1);
    if (NULL != ConfigDesc)
    {
        commonDesc = (PUSB_COMMON_DESCRIPTOR)ConfigDesc;
        descEnd = (PUCHAR)ConfigDesc + ConfigDesc->wTotalLength;
    }

    // return total number of IAD descriptors in this device configuration
    while ((PUCHAR)commonDesc + sizeof(USB_COMMON_DESCRIPTOR) < descEnd &&
        (PUCHAR)commonDesc + commonDesc->bLength <= descEnd)
    {
        if (commonDesc->bDescriptorType == USB_IAD_DESCRIPTOR_TYPE)
        {
            uIADcount++;
        }
        commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
    }
    return (uIADcount);
}

UINT
IsUVCDevice (
    PUSBDEVICEINFO info
    )
{
    PUSB_CONFIGURATION_DESCRIPTOR  ConfigDesc = NULL;
    PUSB_COMMON_DESCRIPTOR         commonDesc = NULL;
    PUCHAR                         descEnd = NULL;
    UINT  uUVCversion = 0;

    // 
    // The GetConfigDescriptor() function in enum.c does not always work
    // If that fails, the Configuration descriptor will be NULL 
    //  and we can only display the device descriptor
    // 
    if (NULL == info)
    {
        return 0;
    }
    if (NULL == info->ConfigDesc)
    {
        return 0;
    }
    ConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)(info->ConfigDesc + 1);
    if (NULL == ConfigDesc)
    {
        return 0;
    }

    // We've got a good Configuration Descriptor
    commonDesc = (PUSB_COMMON_DESCRIPTOR)ConfigDesc;
    descEnd = (PUCHAR)ConfigDesc + ConfigDesc->wTotalLength;

    // walk through all the descriptors looking for the VIDEO_CONTROL_HEADER_UNIT
    while ((PUCHAR)commonDesc + sizeof(USB_COMMON_DESCRIPTOR) < descEnd &&
        (PUCHAR)commonDesc + commonDesc->bLength <= descEnd)
    {
        if ((commonDesc->bDescriptorType == CS_INTERFACE) && 
            (commonDesc->bLength > sizeof(VIDEO_CONTROL_HEADER_UNIT)))
        {
            // Right type, size. Now check subtype 
            PVIDEO_CONTROL_HEADER_UNIT pCSVC = NULL;
            pCSVC = (PVIDEO_CONTROL_HEADER_UNIT) commonDesc;
            if (VC_HEADER == pCSVC->bDescriptorSubtype)
            {
                // found the Class-specific VC Interface Header descriptor
                uUVCversion = pCSVC->bcdVideoSpec;
                // Save the version to global
                g_chUVCversion = uUVCversion;
                // We're done
                break;
            }
        }
        commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
    }
    return (uUVCversion);
}

PCHAR
GetVendorString (
    USHORT     idVendor
    )
{
    PUSBVENDORID vendorID = NULL;

    if (idVendor == 0x0000)
    {
        return NULL;
    }

    vendorID = USBVendorIDs;

    while (vendorID->usVendorID != 0x0000)
    {
        if (vendorID->usVendorID == idVendor)
        {
            break;
        }
        vendorID++;
    }

    return (vendorID->szVendor);
}

VOID
DisplayStringDescriptor (
    UCHAR                    Index,
    PSTRING_DESCRIPTOR_NODE  StringDescs,
    DEVICE_POWER_STATE       LatestDevicePowerState
    )
{
    ULONG nBytes = 0;
    BOOLEAN FoundMatchingString = FALSE;
    PCHAR pStr = NULL;
    CHAR  pString[512];

    //@@DisplayStringDescriptor - String Descriptor

    while (StringDescs)
    {
        if (StringDescs->DescriptorIndex == Index)
        {
            FoundMatchingString = TRUE;
            if(gDoAnnotation)
            {
                pStr= GetLangIDString(StringDescs->LanguageID);
                if(pStr)
                {
                    printf("     %s  \"",
                        pStr);
                }
                else
                {
                    //@@TestCase A9.1
                    //@@WARNING
                    //@@Descriptor Field - string index
                    //@@The Language ID does not match any known languages supported by USB ORG
                    printf("*!*WARNING:  %d is an invalid Language ID\r\n",
                        Index);
                    OOPS();
                }
            }
            else
            {
                printf("     0x%04X:  \"", StringDescs->LanguageID);
            }
            memset(pString, 0, 512);
            nBytes = WideCharToMultiByte(
                CP_ACP,     // CodePage
                WC_NO_BEST_FIT_CHARS,
                StringDescs->StringDescriptor->bString,
                (StringDescs->StringDescriptor->bLength - 2) / 2,
                pString,
                512,
                NULL,       // lpDefaultChar
                NULL);      // pUsedDefaultChar
            if (nBytes)
                printf("%s\"\r\n", pString);
            else
                printf("\"\r\n", pString);
        }
        StringDescs = StringDescs->Next;
    }

    if (!FoundMatchingString)
    {
        if (LatestDevicePowerState == PowerDeviceD0)
        {
            printf("*!*ERROR:  No String Descriptor for index %d!\r\n", Index);
            OOPS();
        } 
        else
        {
            printf("String Descriptor for index %d not available while device is in low power state.\r\n", Index);
        }
    }
}

VOID
DisplayPipeInfo (
    ULONG           NumPipes,
    USB_PIPE_INFO  *PipeInfo
    )
{
    ULONG i = 0;

    for (i = 0; i < NumPipes; i++)
    {
        DisplayEndpointDescriptor(&PipeInfo[i].EndpointDescriptor, NULL, 0, FALSE);
    }

}

PCHAR
GetLangIDString (
    USHORT     idLang
    )
{
    PUSBLANGID langID = NULL;

    if (idLang != 0x0000)
    {
        langID = USBLangIDs;

        while (langID->usLangID != 0x0000)
        {
            if (langID->usLangID == idLang)
            {
                return (langID->szLanguage);
            }
            langID++;
        }
    }

    return NULL;
}

VOID
DisplayEndpointDescriptor (
    _In_     PUSB_ENDPOINT_DESCRIPTOR    
                        EndpointDesc,
    _In_opt_ PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR 
                        EpCompDesc,
    _In_     UCHAR      InterfaceClass,
    _In_     BOOLEAN    EpCompDescAvail
    )
{
    UCHAR epType = EndpointDesc->bmAttributes & USB_ENDPOINT_TYPE_MASK;
    PUSB_HIGH_SPEED_MAXPACKET hsMaxPacket;

    printf("\r\n          ===>Endpoint Descriptor<===\r\n");
    //@@DisplayEndpointDescriptor - Endpoint Descriptor
    //length checked in DisplayConfigDesc()

    printf("bLength:                           0x%02X\r\n",
        EndpointDesc->bLength);

    printf("bDescriptorType:                   0x%02X\r\n",
        EndpointDesc->bDescriptorType);

    printf("bEndpointAddress:                  0x%02X",
        EndpointDesc->bEndpointAddress);

    if(gDoAnnotation)
    {
        if(USB_ENDPOINT_DIRECTION_OUT(EndpointDesc->bEndpointAddress))
        {
            printf("  -> Direction: OUT - EndpointID: %d\r\n",
                (EndpointDesc->bEndpointAddress & USB_ENDPOINT_ADDRESS_MASK));
        }
        else if(USB_ENDPOINT_DIRECTION_IN(EndpointDesc->bEndpointAddress))
        {
            printf("  -> Direction: IN - EndpointID: %d\r\n",
                (EndpointDesc->bEndpointAddress & USB_ENDPOINT_ADDRESS_MASK));
        }
        else
        {
            //@@TestCase A6.1
            //@@ERROR
            //@@Descriptor Field - bEndpointAddress
            //@@An invalid endpoint addressl has been defined
            printf("\r\n*!*ERROR:  This appears to be an invalid bEndpointAddress\r\n");
            OOPS();
        }
    }
    else {printf("\r\n");}

    printf("bmAttributes:                      0x%02X",
        EndpointDesc->bmAttributes);

    if(gDoAnnotation)
    {
        printf("  -> ");

        switch (epType)
        {
        case USB_ENDPOINT_TYPE_CONTROL:
            printf("Control Transfer Type\r\n");
            if (EndpointDesc->bmAttributes & USB_ENDPOINT_TYPE_CONTROL_RESERVED_MASK)
            {
                printf("\r\n*!*ERROR:     Bits 7..2 are reserved and must be set to 0\r\n");
                OOPS();
            }
            break;

        case USB_ENDPOINT_TYPE_ISOCHRONOUS:
            printf("Isochronous Transfer Type, Synchronization Type = ");

            switch (USB_ENDPOINT_TYPE_ISOCHRONOUS_SYNCHRONIZATION(EndpointDesc->bmAttributes))
            {
            case USB_ENDPOINT_TYPE_ISOCHRONOUS_SYNCHRONIZATION_NO_SYNCHRONIZATION:
                printf("No Synchronization");
                break;

            case USB_ENDPOINT_TYPE_ISOCHRONOUS_SYNCHRONIZATION_ASYNCHRONOUS:
                printf("Asynchronous");
                break;

            case USB_ENDPOINT_TYPE_ISOCHRONOUS_SYNCHRONIZATION_ADAPTIVE:
                printf("Adaptive");
                break;

            case USB_ENDPOINT_TYPE_ISOCHRONOUS_SYNCHRONIZATION_SYNCHRONOUS:
                printf("Synchronous");
                break;
            }
            printf(", Usage Type = ");

            switch (USB_ENDPOINT_TYPE_ISOCHRONOUS_USAGE(EndpointDesc->bmAttributes))
            {
            case USB_ENDPOINT_TYPE_ISOCHRONOUS_USAGE_DATA_ENDOINT:
                printf("Data Endpoint\r\n");
                break;

            case USB_ENDPOINT_TYPE_ISOCHRONOUS_USAGE_FEEDBACK_ENDPOINT:
                printf("Feedback Endpoint\r\n");
                break;

            case USB_ENDPOINT_TYPE_ISOCHRONOUS_USAGE_IMPLICIT_FEEDBACK_DATA_ENDPOINT:
                printf("Implicit Feedback Data Endpoint\r\n");
                break;

            case USB_ENDPOINT_TYPE_ISOCHRONOUS_USAGE_RESERVED:
                //@@TestCase A6.2
                //@@ERROR
                //@@Descriptor Field - bmAttributes
                //@@A reserved bit has a value
                printf("\r\n*!*ERROR:     This value is Reserved\r\n");
                OOPS();
                break;
            }
            if (EndpointDesc->bmAttributes & USB_ENDPOINT_TYPE_ISOCHRONOUS_RESERVED_MASK)
            {
                printf("\r\n*!*ERROR:     Bits 7..6 are reserved and must be set to 0\r\n");
                OOPS();
            }
            break;

        case USB_ENDPOINT_TYPE_BULK:
            printf("Bulk Transfer Type\r\n");
            if (EndpointDesc->bmAttributes & USB_ENDPOINT_TYPE_BULK_RESERVED_MASK)
            {
                printf("\r\n*!*ERROR:     Bits 7..2 are reserved and must be set to 0\r\n");
                OOPS();
            }
            break;

        case USB_ENDPOINT_TYPE_INTERRUPT:

            if (gDeviceSpeed != UsbSuperSpeed)
            {
                printf("Interrupt Transfer Type\r\n");
                if (EndpointDesc->bmAttributes & USB_20_ENDPOINT_TYPE_INTERRUPT_RESERVED_MASK)
                {
                    printf("\r\n*!*ERROR:     Bits 7..2 are reserved and must be set to 0\r\n");
                    OOPS();
                }
            }
            else
            {
                printf("Interrupt Transfer Type, Usage Type = ");
                
                switch (USB_30_ENDPOINT_TYPE_INTERRUPT_USAGE(EndpointDesc->bmAttributes))
                {
                case USB_30_ENDPOINT_TYPE_INTERRUPT_USAGE_PERIODIC:
                    printf("Periodic\r\n");
                    break;

                case USB_30_ENDPOINT_TYPE_INTERRUPT_USAGE_NOTIFICATION:
                    printf("Notification\r\n");
                    break;

                case USB_30_ENDPOINT_TYPE_INTERRUPT_USAGE_RESERVED10:
                case USB_30_ENDPOINT_TYPE_INTERRUPT_USAGE_RESERVED11:
                    printf("\r\n*!*ERROR:     This value is Reserved\r\n");
                    OOPS();
                    break;
                }

                if (EndpointDesc->bmAttributes & USB_30_ENDPOINT_TYPE_INTERRUPT_RESERVED_MASK)
                {
                    printf("\r\n*!*ERROR:     Bits 7..6 and 3..2 are reserved and must be set to 0\r\n");
                    OOPS();
                }

                if (EpCompDescAvail && EpCompDesc == NULL)
                {
                    printf("\r\n*!*ERROR:     Endpoint Companion Descriptor missing\r\n");
                    OOPS();
                }
            }
            break;

        }
    } 
    else
    {
        printf("\r\n");
    }

    //@@TestCase A6.3
    //@@Priority 1
    //@@Descriptor Field - bInterfaceNumber
    //@@Question - Should we test to verify bInterfaceNumber is valid?
    printf("wMaxPacketSize:                  0x%04X",
        EndpointDesc->wMaxPacketSize);
    if(gDoAnnotation)
    {
        switch (gDeviceSpeed)
        {
        case UsbSuperSpeed:
            switch (epType)
            {
            case USB_ENDPOINT_TYPE_BULK:
                if (EndpointDesc->wMaxPacketSize != USB_ENDPOINT_SUPERSPEED_BULK_MAX_PACKET_SIZE)
                {
                    printf("\r\n*!*ERROR:     SuperSpeed Bulk endpoints must be %d bytes\r\n",
                        USB_ENDPOINT_SUPERSPEED_BULK_MAX_PACKET_SIZE);
                }
                else
                {
                    printf("\r\n");
                }
                break;
                
            case USB_ENDPOINT_TYPE_CONTROL:
                if (EndpointDesc->wMaxPacketSize != USB_ENDPOINT_SUPERSPEED_CONTROL_MAX_PACKET_SIZE)
                {
                    printf("\r\n*!*ERROR:     SuperSpeed Control endpoints must be %d bytes\r\n",
                        USB_ENDPOINT_SUPERSPEED_CONTROL_MAX_PACKET_SIZE);
                }
                else
                {
                    printf("\r\n");
                }
                break;
                
            case USB_ENDPOINT_TYPE_ISOCHRONOUS:
                
                if (EpCompDesc != NULL)
                {
                    if (EpCompDesc->bMaxBurst > 0)
                    {
                        if (EndpointDesc->wMaxPacketSize != USB_ENDPOINT_SUPERSPEED_ISO_MAX_PACKET_SIZE)
                        {
                            printf("\r\n*!*ERROR:     SuperSpeed isochronous endpoints must have wMaxPacketSize value of %d bytes\r\n",
                                USB_ENDPOINT_SUPERSPEED_ISO_MAX_PACKET_SIZE);
                            printf("                  when the SuperSpeed endpoint companion descriptor bMaxBurst value is greater than 0\r\n");
                        }
                        else
                        {
                            printf("\r\n");
                        }
                    }
                    else if (EndpointDesc->wMaxPacketSize > USB_ENDPOINT_SUPERSPEED_ISO_MAX_PACKET_SIZE)
                    {
                        printf("\r\n*!*ERROR:     Invalid SuperSpeed isochronous maximum packet size\r\n");
                    }
                    else
                    {
                        printf("\r\n");
                    }
                }
                else
                {
                    printf("\r\n");
                }
                break;
                
            case USB_ENDPOINT_TYPE_INTERRUPT:

                if (EpCompDesc != NULL)
                {
                    if (EpCompDesc->bMaxBurst > 0)
                    {
                        if (EndpointDesc->wMaxPacketSize != USB_ENDPOINT_SUPERSPEED_INTERRUPT_MAX_PACKET_SIZE)
                        {
                            printf("\r\n*!*ERROR:     SuperSpeed interrupt endpoints must have wMaxPacketSize value of %d bytes\r\n",
                                USB_ENDPOINT_SUPERSPEED_INTERRUPT_MAX_PACKET_SIZE);
                            printf("                  when the SuperSpeed endpoint companion descriptor bMaxBurst value is greater than 0\r\n");
                        }
                        else
                        {
                            printf("\r\n");
                        }
                    }
                    else if (EndpointDesc->wMaxPacketSize > USB_ENDPOINT_SUPERSPEED_INTERRUPT_MAX_PACKET_SIZE)
                    {
                        printf("\r\n*!*ERROR:     Invalid SuperSpeed interrupt maximum packet size\r\n");
                    }
                    else
                    {
                        printf("\r\n");
                    }
                }
                else
                {
                    printf("\r\n");
                }
                break;
            }
            break;
           /* 
        case UsbHighSpeed:
            hsMaxPacket = (PUSB_HIGH_SPEED_MAXPACKET)&EndpointDesc->wMaxPacketSize;

            switch (epType)
            {
            case USB_ENDPOINT_TYPE_ISOCHRONOUS:
            case USB_ENDPOINT_TYPE_INTERRUPT:
                switch (hsMaxPacket->HSmux) {
                case 0:
                    if ((hsMaxPacket->MaxPacket < 1) || (hsMaxPacket->MaxPacket >1024)) 
                    {
                        printf("*!*ERROR:  Invalid maximum packet size, should be between 1 and 1024\r\n");
                    }
                    break;

                case 1:
                    if ((hsMaxPacket->MaxPacket < 513) || (hsMaxPacket->MaxPacket >1024)) 
                    {
                        printf("*!*ERROR:  Invalid maximum packet size, should be between 513 and 1024\r\n");
                    }
                    break;

                case 2:
                    if ((hsMaxPacket->MaxPacket < 683) || (hsMaxPacket->MaxPacket >1024)) 
                    {
                        printf("*!*ERROR:  Invalid maximum packet size, should be between 683 and 1024\r\n");
                    }
                    break;
                        
                case 3:
                    printf("*!*ERROR:  Bits 12-11 set to Reserved value in wMaxPacketSize\r\n");
                    break;
                }

                printf(" = %d transactions per microframe, 0x%02X max bytes\r\n", hsMaxPacket->HSmux + 1, hsMaxPacket->MaxPacket);
                break;
       
            case USB_ENDPOINT_TYPE_BULK:
            case USB_ENDPOINT_TYPE_CONTROL:
                printf(" = 0x%02X max bytes\r\n", hsMaxPacket->MaxPacket);
                break;
            }
            break;
			*/
        
        case UsbFullSpeed:
            // full speed
            printf(" = 0x%02X bytes\r\n",
                EndpointDesc->wMaxPacketSize & 0x7FF);
            break;
        default:
            // low or invalid speed
            if (InterfaceClass == USB_DEVICE_CLASS_VIDEO)
            {
                printf(" = Invalid bus speed for USB Video Class\r\n");
            }
            else
            {
                printf("\r\n");
            }
            break;
        }
    }
    else 
    {
        printf("\r\n");
    }

    if (EndpointDesc->wMaxPacketSize & 0xE000) 
    {
        //@@TestCase A6.4
        //@@Priority 1
        //@@OTG Descriptor Field - wMaxPacketSize
        //@@Attribute bits D7-2 reserved (reset to 0)
        printf("*!*ERROR:  wMaxPacketSize bits 15-13 should be 0\r\n");
    }

    if (EndpointDesc->bLength == sizeof(USB_ENDPOINT_DESCRIPTOR))
    {
        //@@TestCase A6.5
        //@@Priority 1
        //@@Descriptor Field - bInterfaceNumber
        //@@Question - Should we test to verify bInterfaceNumber is valid?
        printf("bInterval:                         0x%02X\r\n",
            EndpointDesc->bInterval);
    }
    else
    {
        PUSB_ENDPOINT_DESCRIPTOR2 endpointDesc2;

        endpointDesc2 = (PUSB_ENDPOINT_DESCRIPTOR2)EndpointDesc;

        printf("wInterval:                       0x%04X\r\n",
            endpointDesc2->wInterval);

        printf("bSyncAddress:                      0x%02X\r\n",
            endpointDesc2->bSyncAddress);
    }

    if (EpCompDesc != NULL)
    {
        DisplayEndointCompanionDescriptor(EpCompDesc, epType);        
    }

}

VOID
DisplayEndointCompanionDescriptor (
    PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR EpCompDesc,
    UCHAR                                         DescType
    )
{
    printf("\r\n ===>SuperSpeed Endpoint Companion Descriptor<===\r\n");

    printf("bLength:                           0x%02X\r\n",
        EpCompDesc->bLength);

    printf("bDescriptorType:                   0x%02X\r\n",
        EpCompDesc->bDescriptorType);

    printf("bMaxBurst:                         0x%02X\r\n",
        EpCompDesc->bMaxBurst);

    printf("bmAttributes:                      0x%02X",
        EpCompDesc->bmAttributes.AsUchar);
    if(gDoAnnotation)
    {
        switch (DescType)
        {
        case USB_ENDPOINT_TYPE_CONTROL:
        case USB_ENDPOINT_TYPE_INTERRUPT:
            if (EpCompDesc->bmAttributes.AsUchar != 0)
            {
                printf("*!*ERROR:  Control/Interrupt SuperSpeed endpoints do not support streams\r\n");
            }
            else 
            {
                printf("\r\n");
            }
            break;
        case USB_ENDPOINT_TYPE_BULK:
            if(EpCompDesc->bmAttributes.Bulk.MaxStreams == 0)
            {
                printf("The bulk endpoint does not define streams (MaxStreams == 0)\r\n");
            }
            else
            {
                printf(" = %d streams supported\r\n", 1 << EpCompDesc->bmAttributes.Bulk.MaxStreams);
            }

            if (EpCompDesc->bmAttributes.Bulk.Reserved1 != 0)
            {
                printf("*!*ERROR:  bmAttributes bits 7-5 should be 0\r\n");
            }
            break;

        case USB_ENDPOINT_TYPE_ISOCHRONOUS:
            printf(" = %d maximum number of packets within a service interval\r\n", 
                (EpCompDesc->bmAttributes.Isochronous.Mult+1)*EpCompDesc->bMaxBurst);

            if (EpCompDesc->bmAttributes.Isochronous.Mult > USB_SUPERSPEED_ISOCHRONOUS_MAX_MULTIPLIER)
            {
                printf("*!*ERROR:  Maximum SuperSpeed isochronous endpoint multiplier value exceeded\r\n");
            }

            if (EpCompDesc->bmAttributes.Isochronous.Reserved2 != 0)
            {
                printf("*!*ERROR:  bmAttributes bits 7-2 should be 0\r\n");
            }
            else 
            {
                printf("\r\n");
            }
            break;
        }
    }
    printf("wBytesPerInterval:                 0x%04X\r\n",
        EpCompDesc->wBytesPerInterval);

}
