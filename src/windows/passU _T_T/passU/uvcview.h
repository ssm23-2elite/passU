/*++

Copyright (c) 1997-2008 Microsoft Corporation

Module Name:

    UVCVIEW.H

Abstract:

    This is the header file for UVCVIEW

Environment:

    user mode

Revision History:

    04-25-97 : created
    04/13/2005 : major bug fixing

--*/

#ifndef   __UVCVIEW_H__
#define   __UVCVIEW_H__

/*****************************************************************************
 I N C L U D E S
*****************************************************************************/
#include <windows.h>
#include <windowsx.h>
#include <initguid.h>
#include <devioctl.h>
#include <dbt.h>
#include <stdio.h>
#include <commctrl.h>
#include <usbioctl.h>
#include <usbiodef.h>
#include <intsafe.h>
#include <strsafe.h>
#include <specstrings.h>
#include <usb.h>
#include <usbuser.h>
#include <basetyps.h>
#include <wtypes.h>
#include <objbase.h>
#include <io.h>
#include <conio.h>
#include <shellapi.h>
#include <cfgmgr32.h>
#include <shlwapi.h>
#include <SetupAPI.h>
#include <winioctl.h>
#include <devpkey.h>
#include <math.h>

// This is mostly a private USB Audio descriptor header
#include "usbdesc.h"

// This is the inbox USBVideo driver descriptor header (copied locally)
#include "uvcdesc.h"

/*****************************************************************************
 P R A G M A S
*****************************************************************************/

#pragma once

/*****************************************************************************
 D E F I N E S
*****************************************************************************/

// define H264_SUPPORT to add H.264 support to uvcview.exe
#define H264_SUPPORT

#define TEXT_ITEM_LENGTH 64

#define  OOPS()

#define ALLOC(dwBytes) GlobalAlloc(GPTR,(dwBytes))

#define REALLOC(hMem, dwBytes) GlobalReAlloc((hMem), (dwBytes), (GMEM_MOVEABLE|GMEM_ZEROINIT))

#define FREE(hMem)  GlobalFree((hMem))

#define CHECKFORLEAKS()


#define DEVICE_CONFIGURATION_TEXT_LENGTH 10240

#define STR_INVALID_POWER_STATE    "(invalid state) "
#define STR_UNKNOWN_CONTROLLER_FLAVOR "Unknown"

FORCEINLINE
VOID
InitializeListHead(
    _Out_ PLIST_ENTRY ListHead
    )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

//
//  BOOLEAN
//  IsListEmpty(
//      PLIST_ENTRY ListHead
//      );
//

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

//
//  PLIST_ENTRY
//  RemoveHeadList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}

//
//  VOID
//  RemoveEntryList(
//      PLIST_ENTRY Entry
//      );
//

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

//
//  VOID
//  InsertTailList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

// global version for USB Video Class spec version (pre-release)
#define BCDVDC     0x0083

// A.2  Video Interface Subclass Codes
#define SC_VIDEO_INTERFACE_COLLECTION  0x03

// A.3  Video Interface Protocol Codes
#define PC_PROTOCOL_UNDEFINED     0x00

// USB Video Class spec version
#define NOT_UVC 0x0
#define UVC10   0x100
#define UVC11   0x110

#ifdef H264_SUPPORT
#define UVC15   0x150
#endif

#define OUTPUT_MESSAGE_MAX_LENGTH    1024
#define MAX_DEVICE_PROP 200
#define MAX_DRIVER_KEY_NAME 256

/*****************************************************************************
 T Y P E D E F S
*****************************************************************************/

typedef struct
{
	USB_DEVICE_DESCRIPTOR			 DeviceDescriptor;
    USB_CONFIGURATION_DESCRIPTOR	 ConfigDesc;
	USB_INTERFACE_DESCRIPTOR	     InterfaceDesc;
	USB_ENDPOINT_DESCRIPTOR	         EndpointDescriptor[2];
	CHAR DeviceId[50];
	CHAR DeviceDesc[40];
	CHAR HwId[80];
	CHAR Service[20];
	CHAR DeviceClass[20];
	
} USBSENDDEVICEDESC, *PUSBSENDDEVICEDESC;

typedef enum _TREEICON
{
    ComputerIcon,
    HubIcon,
    NoDeviceIcon,
    GoodDeviceIcon,
    BadDeviceIcon,
    GoodSsDeviceIcon,
    NoSsDeviceIcon
} TREEICON;

// Callback function for walking TreeView items
//
typedef VOID
(*LPFNTREECALLBACK)(
    HWND        hTreeWnd,
    PVOID       pContext
);


// Callback notification function called at end of every tree depth
typedef VOID
(*LPFNTREENOTIFYCALLBACK)(PVOID pContext);

//
// Structure used to build a linked list of String Descriptors
// retrieved from a device.
//

typedef struct _STRING_DESCRIPTOR_NODE
{
    struct _STRING_DESCRIPTOR_NODE *Next;
    UCHAR                           DescriptorIndex;
    USHORT                          LanguageID;
    USB_STRING_DESCRIPTOR           StringDescriptor[1];
} STRING_DESCRIPTOR_NODE, *PSTRING_DESCRIPTOR_NODE;

//
// A collection of device properties. The device can be hub, host controller or usb device
//
typedef struct _USB_DEVICE_PNP_STRINGS
{
    PCHAR DeviceId;
    PCHAR DeviceDesc;
    PCHAR HwId;
    PCHAR Service;
    PCHAR DeviceClass;
    PCHAR PowerState;
} USB_DEVICE_PNP_STRINGS, *PUSB_DEVICE_PNP_STRINGS;

typedef struct _DEVICE_INFO_NODE {
    HDEVINFO                         DeviceInfo;
    LIST_ENTRY                       ListEntry;
    SP_DEVINFO_DATA                  DeviceInfoData;
    SP_DEVICE_INTERFACE_DATA         DeviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceDetailData;
    PSTR                             DeviceDescName;
    ULONG                            DeviceDescNameLength;
    PSTR                             DeviceDriverName;
    ULONG                            DeviceDriverNameLength;
    DEVICE_POWER_STATE               LatestDevicePowerState;
} DEVICE_INFO_NODE, *PDEVICE_INFO_NODE;

//
// Structures assocated with TreeView items through the lParam.  When an item
// is selected, the lParam is retrieved and the structure it which it points
// is used to display information in the edit control.
//

typedef enum _USBDEVICEINFOTYPE
{
    HostControllerInfo,
    RootHubInfo,
    ExternalHubInfo,
    DeviceInfo
} USBDEVICEINFOTYPE, *PUSBDEVICEINFOTYPE;

typedef struct _USBHOSTCONTROLLERINFO
{
    USBDEVICEINFOTYPE                   DeviceInfoType;
    LIST_ENTRY                          ListEntry;
    PCHAR                               DriverKey;
    ULONG                               VendorID;
    ULONG                               DeviceID;
    ULONG                               SubSysID;
    ULONG                               Revision;
    USB_POWER_INFO                      USBPowerInfo[6];
    BOOL                                BusDeviceFunctionValid;
    ULONG                               BusNumber;
    USHORT                              BusDevice;
    USHORT                              BusFunction;
    PUSB_CONTROLLER_INFO_0              ControllerInfo;
    PUSB_DEVICE_PNP_STRINGS             UsbDeviceProperties;
} USBHOSTCONTROLLERINFO, *PUSBHOSTCONTROLLERINFO;

typedef struct _USBROOTHUBINFO
{
    USBDEVICEINFOTYPE                   DeviceInfoType;
    PUSB_NODE_INFORMATION               HubInfo;
    PUSB_HUB_INFORMATION_EX             HubInfoEx;
    PCHAR                               HubName;
    PUSB_PORT_CONNECTOR_PROPERTIES      PortConnectorProps;
    PUSB_DEVICE_PNP_STRINGS             UsbDeviceProperties;
    PDEVICE_INFO_NODE                   DeviceInfoNode;
    PUSB_HUB_CAPABILITIES_EX            HubCapabilityEx;

} USBROOTHUBINFO, *PUSBROOTHUBINFO;

typedef struct _USBEXTERNALHUBINFO
{
    USBDEVICEINFOTYPE                      DeviceInfoType;
    PUSB_NODE_INFORMATION                  HubInfo;
    PUSB_HUB_INFORMATION_EX                HubInfoEx;
    PCHAR                                  HubName;
    PUSB_NODE_CONNECTION_INFORMATION_EX    ConnectionInfo;
    PUSB_PORT_CONNECTOR_PROPERTIES         PortConnectorProps;
    PUSB_DESCRIPTOR_REQUEST                ConfigDesc;
    PUSB_DESCRIPTOR_REQUEST                BosDesc;
    PSTRING_DESCRIPTOR_NODE                StringDescs;
    PUSB_NODE_CONNECTION_INFORMATION_EX_V2 ConnectionInfoV2; // NULL if root HUB
    PUSB_DEVICE_PNP_STRINGS                UsbDeviceProperties;
    PDEVICE_INFO_NODE                      DeviceInfoNode;
    PUSB_HUB_CAPABILITIES_EX               HubCapabilityEx;
} USBEXTERNALHUBINFO, *PUSBEXTERNALHUBINFO;


// HubInfo, HubName may be in USBDEVICEINFOTYPE, so they can be removed
typedef struct
{
    USBDEVICEINFOTYPE                      DeviceInfoType;
    PUSB_NODE_INFORMATION                  HubInfo;          // NULL if not a HUB
    PUSB_HUB_INFORMATION_EX                HubInfoEx;        // NULL if not a HUB
    PCHAR                                  HubName;          // NULL if not a HUB
    PUSB_NODE_CONNECTION_INFORMATION_EX    ConnectionInfo;   // NULL if root HUB
    PUSB_PORT_CONNECTOR_PROPERTIES         PortConnectorProps;
    PUSB_DESCRIPTOR_REQUEST                ConfigDesc;       // NULL if root HUB
    PUSB_DESCRIPTOR_REQUEST                BosDesc;          // NULL if root HUB
    PSTRING_DESCRIPTOR_NODE                StringDescs;
    PUSB_NODE_CONNECTION_INFORMATION_EX_V2 ConnectionInfoV2; // NULL if root HUB
    PUSB_DEVICE_PNP_STRINGS                UsbDeviceProperties;
    PDEVICE_INFO_NODE                      DeviceInfoNode;
    PUSB_HUB_CAPABILITIES_EX               HubCapabilityEx;  // NULL if not a HUB
} USBDEVICEINFO, *PUSBDEVICEINFO;

typedef struct _STRINGLIST
{
#ifdef H264_SUPPORT
    ULONGLONG       ulFlag;
#else
    ULONG           ulFlag;
#endif
    PCHAR     pszString;
    PCHAR     pszModifier;

} STRINGLIST, * PSTRINGLIST;

typedef struct _DEVICE_GUID_LIST {
    HDEVINFO   DeviceInfo;
    LIST_ENTRY ListHead;
} DEVICE_GUID_LIST, *PDEVICE_GUID_LIST;


/*****************************************************************************
 G L O B A L S
*****************************************************************************/

int  TotalHubs;

BOOL gDoConfigDesc;

// The following flags/variables are all initialized in Display.c InitializePerDeviceSettings() 
//
// Save the default frame from the MJPEG, Uncompressed, Vendor and Frame Based Format descriptor
// Check for this when processing the individual Frame descriptors
UCHAR   g_chMJPEGFrameDefault;
UCHAR   g_chUNCFrameDefault;
UCHAR   g_chVendorFrameDefault;
UCHAR   g_chFrameBasedFrameDefault;

// Spec version of UVC device
UINT g_chUVCversion;

// Base address of the USBDEVICEINFO for device we're parsing
PUSBDEVICEINFO CurrentUSBDeviceInfo;

// Base address of the Configuration descriptor we're parsing
PUSB_CONFIGURATION_DESCRIPTOR  CurrentConfigDesc;

// Length of the current configuration descriptor
DWORD dwConfigLength;
// Our current position from the beginning of the config descriptor
DWORD dwConfigIndex;

// Save the current Configuration starting and ending addresses
// Used in ValidateDescAddress() 
//
PUSB_CONFIGURATION_DESCRIPTOR   g_pConfigDesc;
PSTRING_DESCRIPTOR_NODE         g_pStringDescs;
PUCHAR                          g_descEnd;

/*****************************************************************************
 F U N C T I O N    P R O T O T Y P E S
*****************************************************************************/

//
// ENUM.C
//

VOID
EnumerateHostControllers (
    ULONG     *DevicesConnected
    );


DEVICE_POWER_STATE
AcquireDevicePowerState(
    _Inout_ PDEVICE_INFO_NODE pNode
    );

_Success_(return == TRUE)
BOOL
GetDeviceProperty(
    _In_    HDEVINFO         DeviceInfoSet,
    _In_    PSP_DEVINFO_DATA DeviceInfoData,
    _In_    DWORD            Property,
    _Outptr_ LPTSTR         *ppBuffer
    );

void
ClearDeviceList(
    PDEVICE_GUID_LIST DeviceList
    );

VOID
EnumerateHostController (
    HANDLE                   hHCDev,    _Inout_ PCHAR            leafName,
    _In_    HANDLE           deviceInfo,
    _In_    PSP_DEVINFO_DATA deviceInfoData
);

PCHAR GetHCDDriverKeyName (
    HANDLE  HCD
);

PCHAR WideStrToMultiStr ( 
                         _In_reads_bytes_(cbWideStr) PWCHAR WideStr, 
                         _In_ size_t                   cbWideStr
                         );

PUSB_DEVICE_PNP_STRINGS 
DriverNameToDeviceProperties( 
        _In_reads_bytes_(cbDriverName) PCHAR  DriverName, 
        _In_ size_t cbDriverName
        );

BOOL 
DriverNameToDeviceInst(
    _In_reads_bytes_(cbDriverName) PCHAR DriverName,
    _In_ size_t cbDriverName,
    _Out_ HDEVINFO *pDevInfo,
    _Out_writes_bytes_(sizeof(SP_DEVINFO_DATA)) PSP_DEVINFO_DATA pDevInfoData
    );

VOID FreeDeviceProperties(_In_ PUSB_DEVICE_PNP_STRINGS *ppDevProps);

DWORD 
GetHostControllerPowerMap(
    HANDLE hHCDev, 
    PUSBHOSTCONTROLLERINFO hcInfo);

DWORD 
GetHostControllerInfo(
    HANDLE hHCDev, 
    PUSBHOSTCONTROLLERINFO hcInfo);

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

#endif   //__UVCVIEW_H__