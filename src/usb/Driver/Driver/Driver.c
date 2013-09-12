/*++

Module Name:

    driver.c

Abstract:

    This file contains the driver entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "Driver.h"
#include "Driver.tmh"

#include <ntddk.h>
#include <usb100.h>
#include <usb200.h>
#include <usbdi.h>

#define CHILDDEVICECOUNT 1
#define BUSDRIVERDEVICE 0
#define CHILDDRIVERDEVICE 1
//#define CHILDDEVICEDEVICEID L"USB\\VID_1234&PID_5679" // PID를 수정하였습니다
//#define CHILDDEVICEHARDWAREID L"USB\\VID_1234&PID_5679"
#define CHILDDEVICECOMPATIBLEID L"USB\\Class_08&SubClass_06&Prot_50"
#define CHILDDEVICEINSTANCEID1 L"0001"
#define CHILDDEVICETEXT  L"USB Device"

#define CONFIGHANDLE 0xAAAAAAAA
#define INTERFACEHANDLE 0x55555555
#define PIPE0HANDLE 0x11111111
#define PIPE1HANDLE 0x22222222

#include "devioctl.h"
#include <initguid.h>

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, DriverEvtDeviceAdd)
#pragma alloc_text (PAGE, DriverEvtDriverContextCleanup)
#endif

// 응용프로그램과 주고받을 ControlCode를 정의합니다
#define IOCTL_BUSDRIVER_ADDDEVICE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_BUSDRIVER_REMOVEDEVICE	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_BUSDRIVER_INSERTDATA		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4000, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define near

#undef FAR
#undef  NEAR
#define FAR                 far
#define NEAR                near
#ifndef CONST
#define CONST               const
#endif

#pragma warning (disable : 4100 4101 4133 4267 4305 4366 4701 4703 4995)
#pragma pack(1)
typedef unsigned long       DWORD;
typedef DWORD near          *PDWORD;

typedef struct
{
	USB_DEVICE_DESCRIPTOR			 DeviceDescriptor;
    USB_CONFIGURATION_DESCRIPTOR	 ConfigDesc;
	USB_INTERFACE_DESCRIPTOR	     InterfaceDesc;
	USB_ENDPOINT_DESCRIPTOR	         EndpointDescriptor[2];
	CHAR DeviceId[50];
	WCHAR DeviceDesc[20];
	CHAR HwId[40];
	CHAR Service[20];
	CHAR DeviceClass[20];

} USBDEVICEDESC, *PUSBDEVICEDESC;

typedef struct
{
	int DeviceObjectType;
	PDEVICE_OBJECT 	NextLayerDeviceObject;
	PDEVICE_OBJECT ChildDeviceObject[CHILDDEVICECOUNT];
	PDEVICE_OBJECT ParentDeviceObject;
	PDEVICE_OBJECT PhysicalDeviceObject;

	// USB IO
	PUSB_DEVICE_DESCRIPTOR pDeviceDescriptor;
	PUSB_CONFIGURATION_DESCRIPTOR pConfigurationDescriptor;
#define NOT_CONFIGURED 0
	char CurrentConfigValue;

	ULONG ConfigurationHandle;
	PUSBD_INTERFACE_INFORMATION pInterfaceInformation;

#define MAXIMUMLOOPBACKDATASIZE 0x100000
	unsigned long SizeofHolding;
	unsigned char *pLoopBackDataBuffer;

	LIST_ENTRY Pipe0IrpListHead;
	LIST_ENTRY Pipe1IrpListHead;

	KTIMER Pipe0Timer;
	KTIMER Pipe1Timer;

	KDPC Pipe0Dpc;
	KDPC Pipe1Dpc;

	BOOLEAN Removed;
	BOOLEAN HasChild;

	// Symbolic Link
	UNICODE_STRING UnicodeString; 
	// 사용자에게 허용될 DeviceStack에 대한 이름을 보관하는곳

}DEVICE_EXTENSION, *PDEVICE_EXTENSION;
#pragma pack()

typedef struct
{
	PIRP Irp;
	PIO_WORKITEM pIoWorkItem;
}WORKITEM_CONTEXT, *PWORKITEM_CONTEXT;

USBDEVICEDESC receiveDesc;

// Prototype
VOID
Pipe0TimerRoutine(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

VOID
Pipe1TimerRoutine(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

void
DRIVER_WorkItemRoutine
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PVOID Context
	);

NTSTATUS
DRIVER_DeviceControlDispatch
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	);

NTSTATUS
DRIVER_InternalDeviceControlDispatch
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	);

NTSTATUS
USBIO
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	);

NTSTATUS
DRIVER_AddDevice
	(
	IN PDRIVER_OBJECT DriverObject,         
	IN PDEVICE_OBJECT PhysicalDeviceObject  
	)
{
	NTSTATUS returnStatus = STATUS_SUCCESS;         
	PDEVICE_OBJECT DeviceObject = NULL;             
	PDEVICE_EXTENSION deviceExtension;

	DbgPrint("추가한다");

	returnStatus = IoCreateDevice                                       
			(                                               
				DriverObject,                               
				sizeof(DEVICE_EXTENSION),                
				NULL,                   
				FILE_DEVICE_UNKNOWN,
				0,                                          
				FALSE,                                      
				&DeviceObject       
			);

	deviceExtension = DeviceObject->DeviceExtension;
	deviceExtension->PhysicalDeviceObject = PhysicalDeviceObject;
	deviceExtension->DeviceObjectType = BUSDRIVERDEVICE;
	deviceExtension->HasChild = FALSE;

	deviceExtension->NextLayerDeviceObject =
		IoAttachDeviceToDeviceStack (
					DeviceObject,
					PhysicalDeviceObject	
	);

	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING; 
	DeviceObject->Flags |= deviceExtension->NextLayerDeviceObject->Flags & 
							( DO_POWER_PAGABLE  | DO_POWER_INRUSH); // DeviceObject의 Flag를 변경합니다

	IoRegisterDeviceInterface( PhysicalDeviceObject, &GUID_SAMPLE, NULL, &deviceExtension->UnicodeString );

	return returnStatus;
}

NTSTATUS
MyCompletionRoutine
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp,
	IN PVOID Context
	)
{
	PKEVENT pEvent;
	pEvent = (PKEVENT)Context;
	KeSetEvent( pEvent, 0, FALSE );
	return STATUS_MORE_PROCESSING_REQUIRED;
}

void
CreateChildDevice
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	)
{
	PIO_STACK_LOCATION pStack; 
	PDEVICE_EXTENSION deviceExtension; 
	PDEVICE_OBJECT NextLayerDeviceObject;
	NTSTATUS returnStatus;
	unsigned short wTotalLength;
	PUSB_CONFIGURATION_DESCRIPTOR pConfigurationDescriptor;
	PUSB_INTERFACE_DESCRIPTOR pInterfaceDescriptor;
	PUSB_ENDPOINT_DESCRIPTOR pEndpointDescriptor;
	PUSBD_INTERFACE_INFORMATION pInterfaceInformation;
	PDEVICE_EXTENSION childDeviceExtension;
	
	pStack = IoGetCurrentIrpStackLocation ( Irp );
	deviceExtension = DeviceObject->DeviceExtension;
	NextLayerDeviceObject = deviceExtension->NextLayerDeviceObject;

	// New Child Device 열거
	IoCreateDevice                                       
	(                                               
		DeviceObject->DriverObject,                               
		sizeof(DEVICE_EXTENSION),                
		NULL,                   
		FILE_DEVICE_UNKNOWN,
		FILE_AUTOGENERATED_DEVICE_NAME,                                          
		FALSE,                                      
		&deviceExtension->ChildDeviceObject[0]       
	);

	deviceExtension->ChildDeviceObject[0]->Flags |= DO_POWER_PAGABLE;
	deviceExtension->ChildDeviceObject[0]->Flags &= ~DO_DEVICE_INITIALIZING;

	childDeviceExtension = (PDEVICE_EXTENSION)deviceExtension->ChildDeviceObject[0]->DeviceExtension;
	childDeviceExtension->DeviceObjectType = CHILDDRIVERDEVICE;
	childDeviceExtension->ParentDeviceObject = DeviceObject;
	childDeviceExtension->Removed = FALSE;

	// USB Descriptor IO
	childDeviceExtension->pDeviceDescriptor = (PUSB_DEVICE_DESCRIPTOR)ExAllocatePool( 
		NonPagedPool, 
		sizeof(USB_DEVICE_DESCRIPTOR) 
		);

	wTotalLength = sizeof(USB_CONFIGURATION_DESCRIPTOR) + sizeof(USB_INTERFACE_DESCRIPTOR) + sizeof(USB_ENDPOINT_DESCRIPTOR) * 2;

	childDeviceExtension->pConfigurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR)ExAllocatePool( 
		NonPagedPool, 
		wTotalLength );

	childDeviceExtension->CurrentConfigValue = NOT_CONFIGURED;

	// Descriptor Setup

	// DeviceDescriptor
	childDeviceExtension->pDeviceDescriptor->bLength = receiveDesc.DeviceDescriptor.bLength;
	childDeviceExtension->pDeviceDescriptor->bDescriptorType = receiveDesc.DeviceDescriptor.bDescriptorType;
	childDeviceExtension->pDeviceDescriptor->bcdUSB = receiveDesc.DeviceDescriptor.bcdUSB;
	childDeviceExtension->pDeviceDescriptor->bDeviceClass = receiveDesc.DeviceDescriptor.bDeviceClass;
	childDeviceExtension->pDeviceDescriptor->bDeviceSubClass = receiveDesc.DeviceDescriptor.bDeviceSubClass;
	childDeviceExtension->pDeviceDescriptor->bDeviceProtocol = receiveDesc.DeviceDescriptor.bDeviceProtocol;
	childDeviceExtension->pDeviceDescriptor->bMaxPacketSize0 = receiveDesc.DeviceDescriptor.bMaxPacketSize0; // 64
	childDeviceExtension->pDeviceDescriptor->idVendor = receiveDesc.DeviceDescriptor.idVendor;
	childDeviceExtension->pDeviceDescriptor->idProduct = receiveDesc.DeviceDescriptor.idProduct;
	childDeviceExtension->pDeviceDescriptor->bcdDevice = receiveDesc.DeviceDescriptor.bcdDevice;
	childDeviceExtension->pDeviceDescriptor->iManufacturer = receiveDesc.DeviceDescriptor.iManufacturer;
	childDeviceExtension->pDeviceDescriptor->iProduct = receiveDesc.DeviceDescriptor.iProduct;
	childDeviceExtension->pDeviceDescriptor->iSerialNumber = receiveDesc.DeviceDescriptor.iSerialNumber;
	childDeviceExtension->pDeviceDescriptor->bNumConfigurations = receiveDesc.DeviceDescriptor.bNumConfigurations;

	// ConfigurationDescriptor
	pConfigurationDescriptor = childDeviceExtension->pConfigurationDescriptor;
	pConfigurationDescriptor->bLength = receiveDesc.ConfigDesc.bLength;
	pConfigurationDescriptor->bDescriptorType = receiveDesc.ConfigDesc.bDescriptorType;
	pConfigurationDescriptor->wTotalLength = receiveDesc.ConfigDesc.wTotalLength;
	pConfigurationDescriptor->bNumInterfaces = receiveDesc.ConfigDesc.bNumInterfaces;
	pConfigurationDescriptor->bConfigurationValue = receiveDesc.ConfigDesc.bConfigurationValue;
	pConfigurationDescriptor->iConfiguration = receiveDesc.ConfigDesc.iConfiguration;
	pConfigurationDescriptor->bmAttributes = receiveDesc.ConfigDesc.bmAttributes; // BusPowered
	pConfigurationDescriptor->MaxPower = receiveDesc.ConfigDesc.MaxPower; // 500mA

	// InterfaceDescriptor
	pInterfaceDescriptor = (PUSB_INTERFACE_DESCRIPTOR)(pConfigurationDescriptor+1);
	pInterfaceDescriptor->bLength = receiveDesc.InterfaceDesc.bLength;
	pInterfaceDescriptor->bDescriptorType = receiveDesc.InterfaceDesc.bDescriptorType;
	pInterfaceDescriptor->bInterfaceNumber = receiveDesc.InterfaceDesc.bInterfaceNumber;
	pInterfaceDescriptor->bAlternateSetting = receiveDesc.InterfaceDesc.bAlternateSetting;
	pInterfaceDescriptor->bNumEndpoints = receiveDesc.InterfaceDesc.bNumEndpoints;
	pInterfaceDescriptor->bInterfaceClass = receiveDesc.InterfaceDesc.bInterfaceClass;
	pInterfaceDescriptor->bInterfaceSubClass = receiveDesc.InterfaceDesc.bInterfaceSubClass;
	pInterfaceDescriptor->bInterfaceProtocol = receiveDesc.InterfaceDesc.bInterfaceProtocol;
	pInterfaceDescriptor->iInterface = receiveDesc.InterfaceDesc.iInterface;

	// EndpointDescriptor
	pEndpointDescriptor = (PUSB_ENDPOINT_DESCRIPTOR)(pInterfaceDescriptor+1);
	pEndpointDescriptor->bLength = receiveDesc.EndpointDescriptor[0].bLength;
	pEndpointDescriptor->bDescriptorType = receiveDesc.EndpointDescriptor[0].bDescriptorType;
	pEndpointDescriptor->bEndpointAddress = receiveDesc.EndpointDescriptor[0].bEndpointAddress;
	pEndpointDescriptor->bmAttributes = receiveDesc.EndpointDescriptor[0].bmAttributes;
	pEndpointDescriptor->wMaxPacketSize = receiveDesc.EndpointDescriptor[0].wMaxPacketSize; // 512
	pEndpointDescriptor->bInterval = receiveDesc.EndpointDescriptor[0].bInterval;

	pEndpointDescriptor++;
	pEndpointDescriptor->bLength = receiveDesc.EndpointDescriptor[1].bLength;
	pEndpointDescriptor->bDescriptorType = receiveDesc.EndpointDescriptor[1].bDescriptorType;
	pEndpointDescriptor->bEndpointAddress = receiveDesc.EndpointDescriptor[1].bEndpointAddress;
	pEndpointDescriptor->bmAttributes = receiveDesc.EndpointDescriptor[1].bmAttributes;
	pEndpointDescriptor->wMaxPacketSize = receiveDesc.EndpointDescriptor[1].wMaxPacketSize; // 512
	pEndpointDescriptor->bInterval = receiveDesc.EndpointDescriptor[1].bInterval;

	// USB Pipe준비
	childDeviceExtension->ConfigurationHandle = CONFIGHANDLE;
	childDeviceExtension->pInterfaceInformation = (PUSBD_INTERFACE_INFORMATION)ExAllocatePool( 
		NonPagedPool, 
		sizeof(USBD_INTERFACE_INFORMATION) + sizeof(USBD_PIPE_INFORMATION) );
	
	pInterfaceInformation = childDeviceExtension->pInterfaceInformation;
	RtlZeroMemory( pInterfaceInformation, sizeof(USBD_INTERFACE_INFORMATION) + sizeof(USBD_PIPE_INFORMATION) );
	pInterfaceInformation->Length = sizeof(USBD_INTERFACE_INFORMATION) + sizeof(USBD_PIPE_INFORMATION);
	pInterfaceInformation->InterfaceNumber = 0;
	pInterfaceInformation->AlternateSetting = 0;
	pInterfaceInformation->Class = 0xFF;
	pInterfaceInformation->SubClass = 0;
	pInterfaceInformation->Protocol = 0xFE;
	pInterfaceInformation->InterfaceHandle = (PVOID)INTERFACEHANDLE;
	pInterfaceInformation->NumberOfPipes = 2;
	pInterfaceInformation->Pipes[0].MaximumPacketSize = 0x200;
	pInterfaceInformation->Pipes[0].EndpointAddress = 0x81;
	pInterfaceInformation->Pipes[0].Interval = 0;
	pInterfaceInformation->Pipes[0].PipeType = USB_ENDPOINT_TYPE_BULK;
	pInterfaceInformation->Pipes[0].PipeHandle = (PVOID)PIPE0HANDLE;
	pInterfaceInformation->Pipes[1].MaximumPacketSize = 0x200;
	pInterfaceInformation->Pipes[1].EndpointAddress = 0x02;
	pInterfaceInformation->Pipes[1].Interval = 0;
	pInterfaceInformation->Pipes[1].PipeType = USB_ENDPOINT_TYPE_BULK;
	pInterfaceInformation->Pipes[1].PipeHandle = (PVOID)PIPE1HANDLE;

	InitializeListHead( &childDeviceExtension->Pipe0IrpListHead );
	InitializeListHead( &childDeviceExtension->Pipe1IrpListHead );

	KeInitializeTimer( &childDeviceExtension->Pipe0Timer );
	KeInitializeTimer( &childDeviceExtension->Pipe1Timer );

	KeInitializeDpc( &childDeviceExtension->Pipe0Dpc, Pipe0TimerRoutine, deviceExtension->ChildDeviceObject[0] );
	KeInitializeDpc( &childDeviceExtension->Pipe1Dpc, Pipe1TimerRoutine, deviceExtension->ChildDeviceObject[0] );

	KeSetTimerEx( &childDeviceExtension->Pipe0Timer, RtlConvertLongToLargeInteger( -1 * 100000), 1, &childDeviceExtension->Pipe0Dpc );
	KeSetTimerEx( &childDeviceExtension->Pipe1Timer, RtlConvertLongToLargeInteger( -1 * 100000), 1, &childDeviceExtension->Pipe1Dpc );

	// LoopBackBuffer 준비
	childDeviceExtension->pLoopBackDataBuffer = (unsigned char *)ExAllocatePool( NonPagedPool, MAXIMUMLOOPBACKDATASIZE+1 );
	childDeviceExtension->SizeofHolding = 0;
}

NTSTATUS
DRIVER_CreateClose
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	)
{
	PIO_STACK_LOCATION pStack; 
	PDEVICE_EXTENSION deviceExtension; 
	NTSTATUS status=STATUS_INVALID_DEVICE_REQUEST;
	
	pStack = IoGetCurrentIrpStackLocation ( Irp );
	deviceExtension = DeviceObject->DeviceExtension;

	if( deviceExtension->DeviceObjectType == BUSDRIVERDEVICE )
	{ // BusDriverDevice
		Irp->IoStatus.Information = 0;
		status = STATUS_SUCCESS;
	}
	Irp->IoStatus.Status = status;
	IoCompleteRequest( Irp, IO_NO_INCREMENT );
	return status;
}

NTSTATUS
DriverDevice_PnpDispatch
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	)
{
	PIO_STACK_LOCATION pStack; 
	PDEVICE_EXTENSION deviceExtension; 
	PDEVICE_OBJECT NextLayerDeviceObject;
	NTSTATUS returnStatus;
	
	pStack = IoGetCurrentIrpStackLocation ( Irp );
	deviceExtension = DeviceObject->DeviceExtension;
	NextLayerDeviceObject = deviceExtension->NextLayerDeviceObject;

	switch ( pStack->MinorFunction )
	{
		case IRP_MN_START_DEVICE :
		{
			{
				KEVENT Event;
				PDEVICE_EXTENSION childDeviceExtension;

				KeInitializeEvent( &Event, NotificationEvent, FALSE );

				IoCopyCurrentIrpStackLocationToNext( Irp );
				IoSetCompletionRoutine( 
					Irp, 
					MyCompletionRoutine, 
					&Event, 
					TRUE, 
					TRUE, 
					TRUE );
				returnStatus = IoCallDriver( NextLayerDeviceObject, Irp );
				if( returnStatus == STATUS_PENDING )
				{
					KeWaitForSingleObject( 
						&Event, 
						Executive, 
						KernelMode, 
						FALSE, 
						NULL );
					returnStatus = Irp->IoStatus.Status;
				}
				if( NT_SUCCESS( returnStatus ) )
				{
					IoSetDeviceInterfaceState( &deviceExtension->UnicodeString, TRUE ); 
					// 사용자가 우리의 DeviceStack에 접근하는 것을 허용
				}

				IoCompleteRequest( Irp, IO_NO_INCREMENT );
				return returnStatus;
			}			
		}
		break;			

		case IRP_MN_REMOVE_DEVICE :
		{ 
			IoSetDeviceInterfaceState( &deviceExtension->UnicodeString, FALSE ); 
			// 사용자가 우리의 DeviceStack에 접근하는 것을 허용

			RtlFreeUnicodeString( &deviceExtension->UnicodeString );
			// 사용자가 접근하는 이름을 메모리에서 해제한다

			IoDetachDevice ( NextLayerDeviceObject );								
			IoDeleteDevice ( DeviceObject );									
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

		case IRP_MN_QUERY_DEVICE_RELATIONS :
		{ 
			{
				PDEVICE_RELATIONS pRelations;
			
				if( pStack->Parameters.QueryDeviceRelations.Type == BusRelations )
				{
					if( deviceExtension->HasChild == TRUE)
					{
						pRelations = (PDEVICE_RELATIONS)ExAllocatePool( 
							PagedPool, 
							sizeof(ULONG) + sizeof(PDEVICE_OBJECT) * (CHILDDEVICECOUNT + 1) );
						pRelations->Count = CHILDDEVICECOUNT;
						pRelations->Objects[0] = deviceExtension->ChildDeviceObject[0];
						pRelations->Objects[1] = 0;

						ObReferenceObject( pRelations->Objects[0] );
					}
					else
					{
						pRelations = (PDEVICE_RELATIONS)ExAllocatePool( 
							PagedPool, 
							sizeof(ULONG) + sizeof(PDEVICE_OBJECT) );
						pRelations->Count = 0;
						pRelations->Objects[0] = 0;
					}
					Irp->IoStatus.Information = (ULONG)pRelations;				
					Irp->IoStatus.Status = STATUS_SUCCESS;
				}
			}
		}
		break;
	} 

	IoSkipCurrentIrpStackLocation( Irp ); 
	returnStatus = IoCallDriver(NextLayerDeviceObject, Irp);

	return returnStatus;
}

NTSTATUS
QueryDeviceCaps( IN  PIRP   Irp )
{
	
    PIO_STACK_LOCATION      pStack;
    PDEVICE_CAPABILITIES    deviceCapabilities;
    NTSTATUS                status;
    
    pStack = IoGetCurrentIrpStackLocation (Irp);
	
    deviceCapabilities = pStack->Parameters.DeviceCapabilities.Capabilities;
	
    if(deviceCapabilities->Version != 1 || 
		deviceCapabilities->Size < sizeof(DEVICE_CAPABILITIES))
    {
		return STATUS_UNSUCCESSFUL; 
    }
    
    deviceCapabilities->DeviceState[PowerSystemWorking] = PowerDeviceD0;
    deviceCapabilities->DeviceState[PowerSystemSleeping1] = PowerDeviceD3;
    deviceCapabilities->DeviceState[PowerSystemSleeping2] = PowerDeviceD3;
    deviceCapabilities->DeviceState[PowerSystemSleeping3] = PowerDeviceD3;
    deviceCapabilities->SystemWake = PowerSystemUnspecified;
    deviceCapabilities->DeviceWake = PowerDeviceUnspecified;
    deviceCapabilities->DeviceD1 = FALSE;
    deviceCapabilities->DeviceD2 = FALSE;
    deviceCapabilities->WakeFromD0 = FALSE;
    deviceCapabilities->WakeFromD1 = FALSE;
    deviceCapabilities->WakeFromD2 = FALSE;
    deviceCapabilities->WakeFromD3 = FALSE;
    deviceCapabilities->D1Latency = 0;
    deviceCapabilities->D2Latency = 0;
    deviceCapabilities->D3Latency = 0;
    deviceCapabilities->EjectSupported = FALSE;
    deviceCapabilities->HardwareDisabled = FALSE;
    deviceCapabilities->Removable = TRUE;
	deviceCapabilities->SurpriseRemovalOK = FALSE;
    deviceCapabilities->UniqueID = TRUE;
    deviceCapabilities->SilentInstall = FALSE;
    return STATUS_SUCCESS;
}

NTSTATUS
ChildDriverDevice_PnpDispatch
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	)
{
	PIO_STACK_LOCATION pStack; 
	PDEVICE_EXTENSION deviceExtension; 
	PDEVICE_OBJECT NextLayerDeviceObject;
	KIRQL OldIrql;
	LIST_ENTRY Dequeued0IrpListHead, Dequeued1IrpListHead;
	PLIST_ENTRY pListEntry;
	PIRP CancelIrp;

	NTSTATUS returnStatus = Irp->IoStatus.Status;
	
	pStack = IoGetCurrentIrpStackLocation ( Irp );
	deviceExtension = DeviceObject->DeviceExtension;

	switch ( pStack->MinorFunction )
	{
		case IRP_MN_QUERY_CAPABILITIES:
		{
			returnStatus = QueryDeviceCaps( Irp );
			break;
		}

		case IRP_MN_START_DEVICE :
		case IRP_MN_QUERY_STOP_DEVICE:
		case IRP_MN_QUERY_REMOVE_DEVICE:
		{ 
			returnStatus = STATUS_SUCCESS;
			break;
		}

		case IRP_MN_REMOVE_DEVICE :
		{ 
			if( deviceExtension->Removed == TRUE )
			{
				returnStatus = STATUS_SUCCESS;
				break;
			}
			deviceExtension->Removed = TRUE;

			{
				PDEVICE_OBJECT ParentDeviceObject;
				PDEVICE_EXTENSION ParentDeviceExtension;
				ParentDeviceObject = deviceExtension->ParentDeviceObject;
				ParentDeviceExtension = (PDEVICE_EXTENSION)ParentDeviceObject->DeviceExtension;

				if( ParentDeviceExtension->HasChild == TRUE )
				{
					ParentDeviceExtension->HasChild = FALSE;
				}
			}

			InitializeListHead( &Dequeued0IrpListHead );
			InitializeListHead( &Dequeued1IrpListHead );

			IoAcquireCancelSpinLock( &OldIrql );
			KeCancelTimer( &deviceExtension->Pipe0Timer );
			KeCancelTimer( &deviceExtension->Pipe1Timer );

			while( !IsListEmpty( &deviceExtension->Pipe0IrpListHead ) )
			{
				pListEntry = RemoveHeadList(&deviceExtension->Pipe0IrpListHead);
				InsertTailList( &Dequeued0IrpListHead, pListEntry );
			}

			while( !IsListEmpty( &deviceExtension->Pipe1IrpListHead ) )
			{
				pListEntry = RemoveHeadList(&deviceExtension->Pipe1IrpListHead);
				InsertTailList( &Dequeued1IrpListHead, pListEntry );
			}

			IoReleaseCancelSpinLock( OldIrql );

			while( !IsListEmpty( &Dequeued0IrpListHead ) )
			{
				pListEntry = RemoveHeadList(&Dequeued0IrpListHead);
				CancelIrp = CONTAINING_RECORD( pListEntry, IRP, Tail.Overlay.ListEntry );
				CancelIrp->IoStatus.Status = STATUS_CANCELLED;
				CancelIrp->IoStatus.Information = 0;
				IoCompleteRequest( CancelIrp, IO_NO_INCREMENT );
			}

			while( !IsListEmpty( &Dequeued1IrpListHead ) )
			{
				pListEntry = RemoveHeadList(&Dequeued1IrpListHead);
				CancelIrp = CONTAINING_RECORD( pListEntry, IRP, Tail.Overlay.ListEntry );
				CancelIrp->IoStatus.Status = STATUS_CANCELLED;
				CancelIrp->IoStatus.Information = 0;
				IoCompleteRequest( CancelIrp, IO_NO_INCREMENT );
			}

			if( deviceExtension->pDeviceDescriptor )
			{
				ExFreePool( deviceExtension->pDeviceDescriptor );
				deviceExtension->pDeviceDescriptor = NULL;
			}
			if( deviceExtension->pConfigurationDescriptor )
			{
				ExFreePool( deviceExtension->pConfigurationDescriptor );
				deviceExtension->pConfigurationDescriptor = NULL;
			}
			if( deviceExtension->pInterfaceInformation )
			{
				ExFreePool( deviceExtension->pInterfaceInformation );
				deviceExtension->pInterfaceInformation = NULL;
			}
			if( deviceExtension->pLoopBackDataBuffer )
			{
				ExFreePool( deviceExtension->pLoopBackDataBuffer );
				deviceExtension->pLoopBackDataBuffer = NULL;
				deviceExtension->SizeofHolding = 0;
			}			

			IoDeleteDevice ( DeviceObject );
			returnStatus = STATUS_SUCCESS;
			break;
		}

		case IRP_MN_QUERY_BUS_INFORMATION:
		{
			Irp->IoStatus.Information = 0;
			returnStatus = STATUS_NOT_SUPPORTED;
			break;
		}

		case IRP_MN_QUERY_DEVICE_RELATIONS :
		{ 
			PDEVICE_RELATIONS pRelations;

			if( pStack->Parameters.QueryDeviceRelations.Type == TargetDeviceRelation )
			{
				pRelations = (PDEVICE_RELATIONS)ExAllocatePool( 
					PagedPool, 
					sizeof(ULONG) + sizeof(PDEVICE_OBJECT) * 2 );
				pRelations->Count = 1;
				pRelations->Objects[0] = DeviceObject;
				pRelations->Objects[1] = 0;

				ObReferenceObject( pRelations->Objects[0] );

				Irp->IoStatus.Information = (ULONG)pRelations;				
				returnStatus = STATUS_SUCCESS;
			}
			break;
		}

	case IRP_MN_QUERY_DEVICE_TEXT:
		{
			PWCHAR string=0;
			ULONG dwSize;
			PDEVICE_EXTENSION ParentDeviceExtension;

			ParentDeviceExtension = (PDEVICE_EXTENSION)deviceExtension->ParentDeviceObject->DeviceExtension;
			
			if( pStack->Parameters.QueryDeviceText.DeviceTextType != DeviceTextDescription  )
				break;

			if( ParentDeviceExtension->ChildDeviceObject[0] == DeviceObject )				
			{
				int i, len = wcslen(receiveDesc.DeviceDesc);
				string = (WCHAR *)ExAllocatePool( PagedPool, 2*len+2 );
				RtlZeroMemory( string, 2*len+2 );

				wcscpy(string, receiveDesc.DeviceDesc);
				//RtlMoveMemory( string, receiveDesc.DeviceDesc, strlen(receiveDesc.DeviceDesc) );
				returnStatus = STATUS_SUCCESS;
			}
			else
				break;

			Irp->IoStatus.Information = (ULONG)string;				
			returnStatus = STATUS_SUCCESS;
			break;
		}

		case IRP_MN_QUERY_ID :
		{ 
			WCHAR * pID = NULL;
			PDEVICE_EXTENSION ParentDeviceExtension;

			ParentDeviceExtension = (PDEVICE_EXTENSION)deviceExtension->ParentDeviceObject->DeviceExtension;

			switch( pStack->Parameters.QueryId.IdType )
			{
				case BusQueryDeviceID:
					if( ParentDeviceExtension->ChildDeviceObject[0] == DeviceObject )
					{
						int i, len = strlen(receiveDesc.DeviceId);
						pID = (WCHAR *)ExAllocatePool( PagedPool, 2*strlen(receiveDesc.DeviceId)+2 );
						RtlZeroMemory( pID, 2*len+2 );
						
						for( i = 0 ; i < len ; i++ )
						{
							pID[i] = receiveDesc.DeviceId[i];
						}
						//RtlMoveMemory( pID, receiveDesc.DeviceId, strlen(receiveDesc.DeviceId) );
						returnStatus = STATUS_SUCCESS;
					}	
					break;

				case BusQueryHardwareIDs:
					if( ParentDeviceExtension->ChildDeviceObject[0] == DeviceObject )
					{
						int i, len = strlen(receiveDesc.HwId);
						pID = (WCHAR *)ExAllocatePool( PagedPool, 2*strlen(receiveDesc.HwId)+4 );
						RtlZeroMemory( pID, 2*len+4 );

						for( i = 0 ; i < len ; i++ )
						{
							pID[i] = receiveDesc.HwId[i];
						}
						//RtlMoveMemory( pID, receiveDesc.HwId, strlen(receiveDesc.HwId) );
						returnStatus = STATUS_SUCCESS;
					}
					break;

				case BusQueryCompatibleIDs:
					if( ParentDeviceExtension->ChildDeviceObject[0] == DeviceObject )
					{
						pID = (WCHAR *)ExAllocatePool( PagedPool, sizeof(CHILDDEVICECOMPATIBLEID)+4 );
						RtlZeroMemory( pID, sizeof(CHILDDEVICECOMPATIBLEID)+4 );
						RtlMoveMemory( pID, CHILDDEVICECOMPATIBLEID, sizeof(CHILDDEVICECOMPATIBLEID) );
						returnStatus = STATUS_SUCCESS;
					}
					break;
				case BusQueryInstanceID:
					if( ParentDeviceExtension->ChildDeviceObject[0] == DeviceObject )
					{
						pID = (WCHAR *)ExAllocatePool( PagedPool, sizeof(CHILDDEVICEINSTANCEID1)+2 );
						RtlZeroMemory( pID, sizeof(CHILDDEVICEINSTANCEID1)+2 );
						RtlMoveMemory( pID, CHILDDEVICEINSTANCEID1, sizeof(CHILDDEVICEINSTANCEID1) );
						returnStatus = STATUS_SUCCESS;
					}
					break;
			}
			Irp->IoStatus.Information = (ULONG)pID;				
			break;
		}
	} 

	Irp->IoStatus.Status = returnStatus;
	IoCompleteRequest( Irp, IO_NO_INCREMENT );

	return returnStatus;
}

NTSTATUS
DRIVER_PowerDispatch
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	)
{
	PIO_STACK_LOCATION pStack; 
	PDEVICE_EXTENSION deviceExtension; 
	
	pStack = IoGetCurrentIrpStackLocation ( Irp );
	deviceExtension = DeviceObject->DeviceExtension;

	if( deviceExtension->DeviceObjectType == BUSDRIVERDEVICE )
	{ // BusDriverDevice
		PoStartNextPowerIrp( Irp );
		IoSkipCurrentIrpStackLocation( Irp );
		return PoCallDriver( deviceExtension->NextLayerDeviceObject, Irp );
	}
	else
	{ // ChildDriverDevice
		PoStartNextPowerIrp( Irp );
		Irp->IoStatus.Status = STATUS_SUCCESS;
		IoCompleteRequest( Irp, IO_NO_INCREMENT );
		return STATUS_SUCCESS;
	}			
}

NTSTATUS
DRIVER_PnpDispatch
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	)
{
	PIO_STACK_LOCATION pStack; 
	PDEVICE_EXTENSION deviceExtension; 
	
	pStack = IoGetCurrentIrpStackLocation ( Irp );
	deviceExtension = DeviceObject->DeviceExtension;

	if( deviceExtension->DeviceObjectType == BUSDRIVERDEVICE )
	{ // BusDriverDevice
		return DriverDevice_PnpDispatch( DeviceObject, Irp );
	}
	else
	{ // ChildDriverDevice
		{
			PDEVICE_OBJECT ParentDeviceObject;
			PDEVICE_EXTENSION ParentDeviceExtension;
			ParentDeviceObject = deviceExtension->ParentDeviceObject;
			ParentDeviceExtension = (PDEVICE_EXTENSION)ParentDeviceObject->DeviceExtension;

			if( ParentDeviceExtension->HasChild == FALSE )
			{
				Irp->IoStatus.Status = STATUS_NO_SUCH_DEVICE;
				IoCompleteRequest( Irp, IO_NO_INCREMENT );
				return STATUS_NO_SUCH_DEVICE;
			}
		}
		return ChildDriverDevice_PnpDispatch( DeviceObject, Irp );
	}			
}

NTSTATUS
DRIVER_DeviceControlDispatch
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	)
{
	PIO_STACK_LOCATION pStack; 
	PDEVICE_EXTENSION deviceExtension; 
	ULONG Information=0;
	NTSTATUS returnStatus=STATUS_INVALID_DEVICE_REQUEST;
	PDWORD inBuf;
	ULONG inBufLength = 0;
	//CHAR data[1000] = {0};
	
	pStack = IoGetCurrentIrpStackLocation ( Irp );
	deviceExtension = DeviceObject->DeviceExtension;

	if( deviceExtension->DeviceObjectType == BUSDRIVERDEVICE )
	{ // BusDriverDevice
		switch( pStack->Parameters.DeviceIoControl.IoControlCode )
		// 응용프로그램이 전달하는 ControlCode를 확인합니다. 
		// 이곳에서는 파라미터의 유효성을 확인합니다.
		{
			case IOCTL_BUSDRIVER_ADDDEVICE:
				if( deviceExtension->HasChild == FALSE )
				{
					deviceExtension->HasChild = TRUE;
					CreateChildDevice( DeviceObject, Irp );
					IoInvalidateDeviceRelations( deviceExtension->PhysicalDeviceObject, BusRelations );
					returnStatus = STATUS_SUCCESS;
				}
				break;
			case IOCTL_BUSDRIVER_REMOVEDEVICE:
				deviceExtension->HasChild = FALSE;
				IoInvalidateDeviceRelations( deviceExtension->PhysicalDeviceObject, BusRelations );
				returnStatus = STATUS_SUCCESS;
				break;
			case IOCTL_BUSDRIVER_INSERTDATA:
			
				inBufLength = pStack->Parameters.DeviceIoControl.InputBufferLength;
				inBuf = (PDWORD)Irp->AssociatedIrp.SystemBuffer;

				if (inBufLength != 0) {
					RtlCopyBytes(&receiveDesc, inBuf, inBufLength);
				} else {
					DbgPrint("[!] IOCTL : IOCTL_BUSDRIVER_INSERTDATA - inBufLength Fail\n");
				}
				returnStatus = STATUS_SUCCESS;

				break;
		}
	}

	Irp->IoStatus.Status = returnStatus;
	Irp->IoStatus.Information = Information;
	IoCompleteRequest( Irp, IO_NO_INCREMENT );
	return returnStatus;
}

VOID                                                                
DRIVER_Unload                                                       
	(                                                               
	IN PDRIVER_OBJECT DriverObject 
	)
{
}

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:
    DriverEntry initializes the driver and is the first routine called by the
    system after the driver is loaded. DriverEntry specifies the other entry
    points in the function driver, such as EvtDevice and DriverUnload.

Parameters Description:

    DriverObject - represents the instance of the function driver that is loaded
    into memory. DriverEntry must initialize members of DriverObject before it
    returns to the caller. DriverObject is allocated by the system before the
    driver is loaded, and it is released by the system after the system unloads
    the function driver from memory.

    RegistryPath - represents the driver specifiNew Computerc path in the Registry.
    The function driver can use the path to store driver related data between
    reboots. The path does not store hardware instance specific data.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
{
    NTSTATUS returnStatus = STATUS_SUCCESS;    

	DbgPrint("DriverEntry");

	DriverObject->DriverUnload = DRIVER_Unload;
	DriverObject->DriverExtension->AddDevice = DRIVER_AddDevice;
	DriverObject->MajorFunction[IRP_MJ_PNP] = DRIVER_PnpDispatch;
	DriverObject->MajorFunction[IRP_MJ_POWER] = DRIVER_PowerDispatch;

	// APPIO
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DRIVER_DeviceControlDispatch;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DRIVER_CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLEANUP] = DRIVER_CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DRIVER_CreateClose;

	// USBIO
	DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = DRIVER_InternalDeviceControlDispatch;
	return returnStatus;
}

NTSTATUS
DriverEvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a device object to
    represent a new instance of the device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    status = DriverCreateDevice(DeviceInit);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

VOID
DriverEvtDriverContextCleanup(
    _In_ WDFOBJECT DriverObject
    )
/*++
Routine Description:

    Free all the resources allocated in DriverEntry.

Arguments:

    DriverObject - handle to a WDF Driver object.

Return Value:

    VOID.

--*/
{
    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE ();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    //
    // Stop WPP Tracing
    //
    //WPP_CLEANUP( WdfDriverWdmGetDriverObject(DriverObject) );

}

VOID
Pipe0TimerRoutine(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
{ // BulkIn
	NTSTATUS status;
	KIRQL OldIrql;
	PDEVICE_OBJECT DeviceObject;
	PDEVICE_EXTENSION deviceExtension; 
	PLIST_ENTRY pListEntry=NULL;
	PIRP Irp=NULL;

	DeviceObject = (PDEVICE_OBJECT)DeferredContext;
	deviceExtension = DeviceObject->DeviceExtension;

	IoAcquireCancelSpinLock( &OldIrql );

	if( !IsListEmpty( &deviceExtension->Pipe0IrpListHead ) )
	{
		pListEntry = RemoveHeadList(&deviceExtension->Pipe0IrpListHead);
		Irp = CONTAINING_RECORD( pListEntry, IRP, Tail.Overlay.ListEntry );
	}
	IoReleaseCancelSpinLock( OldIrql );

	if( !Irp )
		return;

	// Handling
	status = USBIO( DeviceObject, Irp );
	if( status == STATUS_PENDING )
	{
		if( deviceExtension->Removed != TRUE )
		{
			IoAcquireCancelSpinLock( &OldIrql );
			InsertHeadList(&deviceExtension->Pipe0IrpListHead, pListEntry);
			IoReleaseCancelSpinLock( OldIrql );
		}
	}
}

VOID
Pipe1TimerRoutine(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
{ // BulkOut
	KIRQL OldIrql;
	PDEVICE_OBJECT DeviceObject;
	PDEVICE_EXTENSION deviceExtension; 
	PLIST_ENTRY pListEntry=NULL;
	PIRP Irp=NULL;

	DeviceObject = (PDEVICE_OBJECT)DeferredContext;
	deviceExtension = DeviceObject->DeviceExtension;

	IoAcquireCancelSpinLock( &OldIrql );

	if( !IsListEmpty( &deviceExtension->Pipe1IrpListHead ) )
	{
		pListEntry = RemoveHeadList(&deviceExtension->Pipe1IrpListHead);
		Irp = CONTAINING_RECORD( pListEntry, IRP, Tail.Overlay.ListEntry );
	}
	IoReleaseCancelSpinLock( OldIrql );

	if( !Irp )
		return;

	// Handling
	USBIO( DeviceObject, Irp );
}

NTSTATUS USBComplete( PIRP Irp, PURB pUrb, NTSTATUS IrpStatus, NTSTATUS UsbStatus )
{
	IoSetCancelRoutine( Irp, NULL );
	if( pUrb )
		pUrb->UrbHeader.Status = UsbStatus;
	Irp->IoStatus.Status = IrpStatus;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest( Irp, IO_NO_INCREMENT );
	return IrpStatus;
}

NTSTATUS
checkURB
	(
	PURB pUrb
	)
{
	if( !pUrb )
		return STATUS_INVALID_PARAMETER;

	if( !pUrb->UrbHeader.Length )
		return STATUS_INVALID_PARAMETER;

	switch( pUrb->UrbHeader.Function )
	{
	case URB_FUNCTION_SELECT_CONFIGURATION:
	case URB_FUNCTION_ABORT_PIPE:
	case URB_FUNCTION_GET_CURRENT_FRAME_NUMBER:
	case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
	case URB_FUNCTION_RESET_PIPE:
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
	case URB_FUNCTION_GET_CONFIGURATION:
		break;
	default:
		return STATUS_NOT_SUPPORTED;
	}

	return STATUS_SUCCESS;
}

NTSTATUS 
USBIO_Select_Configuration( 
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp, 
	PURB pUrb 
	)
{
	PDEVICE_EXTENSION deviceExtension; 
	deviceExtension = DeviceObject->DeviceExtension;

	if( pUrb->UrbSelectConfiguration.Hdr.Length != (sizeof(struct _URB_SELECT_CONFIGURATION) + sizeof( USBD_PIPE_INFORMATION) ) )
	{
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	if( !pUrb->UrbSelectConfiguration.ConfigurationDescriptor )
	{
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	if( pUrb->UrbSelectConfiguration.ConfigurationDescriptor->wTotalLength != deviceExtension->pConfigurationDescriptor->wTotalLength )
	{
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	deviceExtension->pInterfaceInformation->Pipes[0].MaximumTransferSize = pUrb->UrbSelectConfiguration.Interface.Pipes[0].MaximumTransferSize;
	deviceExtension->pInterfaceInformation->Pipes[1].MaximumTransferSize = pUrb->UrbSelectConfiguration.Interface.Pipes[1].MaximumTransferSize;

	pUrb->UrbSelectConfiguration.ConfigurationHandle = (PVOID)deviceExtension->ConfigurationHandle;
	RtlMoveMemory( &(pUrb->UrbSelectConfiguration.Interface), deviceExtension->pInterfaceInformation, sizeof( USBD_INTERFACE_INFORMATION ) + sizeof( USBD_PIPE_INFORMATION) );

	return USBComplete( Irp, pUrb, STATUS_SUCCESS, USBD_STATUS_SUCCESS );
}

NTSTATUS 
USBIO_ResetPipe( 
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp, 
	PURB pUrb 
	)
{
	return USBComplete( Irp, pUrb, STATUS_SUCCESS, USBD_STATUS_SUCCESS );
}

NTSTATUS 
USBIO_AbortPipe( 
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp, 
	PURB pUrb 
	)
{
	PDEVICE_EXTENSION deviceExtension; 
	PVOID TargetBuffer;
	ULONG TargetBufferRequestSize;
	LIST_ENTRY DequeuedIrpListHead;
	KIRQL OldIrql;
	PLIST_ENTRY pListEntry;
	PIRP CancelIrp;

	deviceExtension = DeviceObject->DeviceExtension;

	InitializeListHead( &DequeuedIrpListHead );

	if( pUrb->UrbPipeRequest.PipeHandle == (PVOID)PIPE0HANDLE )
	{
		IoAcquireCancelSpinLock( &OldIrql );

		while( !IsListEmpty( &deviceExtension->Pipe0IrpListHead ) )
		{
			pListEntry = RemoveHeadList(&deviceExtension->Pipe0IrpListHead);
			InsertTailList( &DequeuedIrpListHead, pListEntry );
		}

		IoReleaseCancelSpinLock( OldIrql );
	}
	else
	if( pUrb->UrbPipeRequest.PipeHandle == (PVOID)PIPE1HANDLE )
	{
		IoAcquireCancelSpinLock( &OldIrql );

		while( !IsListEmpty( &deviceExtension->Pipe1IrpListHead ) )
		{
			pListEntry = RemoveHeadList(&deviceExtension->Pipe1IrpListHead);
			InsertTailList( &DequeuedIrpListHead, pListEntry );
		}

		IoReleaseCancelSpinLock( OldIrql );
	}
	else
	{
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	while( !IsListEmpty( &DequeuedIrpListHead ) )
	{
		pListEntry = RemoveHeadList(&DequeuedIrpListHead);
		CancelIrp = CONTAINING_RECORD( pListEntry, IRP, Tail.Overlay.ListEntry );
		CancelIrp->IoStatus.Status = STATUS_CANCELLED;
		CancelIrp->IoStatus.Information = 0;
		IoCompleteRequest( CancelIrp, IO_NO_INCREMENT );
	}

	return USBComplete( Irp, pUrb, STATUS_SUCCESS, USBD_STATUS_SUCCESS );
}

NTSTATUS 
USBIO_BulkOrInterruptTransfer( 
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp, 
	PURB pUrb 
	)
{
	PVOID TargetBuffer;
	ULONG TargetBufferRequestSize;
	PDEVICE_EXTENSION deviceExtension; 
	deviceExtension = DeviceObject->DeviceExtension;

	if( !pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength )
	{
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	if( !pUrb->UrbBulkOrInterruptTransfer.TransferBuffer && !pUrb->UrbBulkOrInterruptTransfer.TransferBufferMDL )
	{
		pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength = 0;
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	if( pUrb->UrbBulkOrInterruptTransfer.TransferBuffer && pUrb->UrbBulkOrInterruptTransfer.TransferBufferMDL )
	{
		pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength = 0;
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	if( pUrb->UrbBulkOrInterruptTransfer.TransferBuffer )
	{
		TargetBuffer = pUrb->UrbBulkOrInterruptTransfer.TransferBuffer;
	}
	else if( pUrb->UrbBulkOrInterruptTransfer.TransferBufferMDL )
	{
		TargetBuffer = MmGetSystemAddressForMdl( pUrb->UrbBulkOrInterruptTransfer.TransferBufferMDL );
	}
	
	TargetBufferRequestSize = pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength;
	
	if( pUrb->UrbBulkOrInterruptTransfer.PipeHandle == (PVOID)PIPE0HANDLE )
	{ // Bulk IN
		TargetBufferRequestSize = (TargetBufferRequestSize >( deviceExtension->SizeofHolding ))?
			deviceExtension->SizeofHolding : TargetBufferRequestSize;

		if( !TargetBufferRequestSize )
		{
			IoMarkIrpPending( Irp );
			return STATUS_PENDING;
		}

		RtlMoveMemory( TargetBuffer, &deviceExtension->pLoopBackDataBuffer[0], TargetBufferRequestSize );
		deviceExtension->SizeofHolding -= TargetBufferRequestSize;

		RtlMoveMemory( &deviceExtension->pLoopBackDataBuffer[0], &deviceExtension->pLoopBackDataBuffer[TargetBufferRequestSize], deviceExtension->SizeofHolding );

		pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength = TargetBufferRequestSize;
		return USBComplete( Irp, pUrb, STATUS_SUCCESS, USBD_STATUS_SUCCESS );
	}
	else
	if( pUrb->UrbBulkOrInterruptTransfer.PipeHandle == (PVOID)PIPE1HANDLE )
	{ // Bulk OUT
		TargetBufferRequestSize = (TargetBufferRequestSize >( MAXIMUMLOOPBACKDATASIZE- deviceExtension->SizeofHolding ))?
			MAXIMUMLOOPBACKDATASIZE- deviceExtension->SizeofHolding : TargetBufferRequestSize;

		if( !TargetBufferRequestSize )
		{
			pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength = 0;
			return USBComplete( Irp, pUrb, STATUS_INSUFFICIENT_RESOURCES, USBD_STATUS_BUFFER_OVERRUN );
		}

		RtlMoveMemory( &deviceExtension->pLoopBackDataBuffer[deviceExtension->SizeofHolding], TargetBuffer, TargetBufferRequestSize );
		pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength = TargetBufferRequestSize;
		deviceExtension->SizeofHolding += TargetBufferRequestSize;
		return USBComplete( Irp, pUrb, STATUS_SUCCESS, USBD_STATUS_SUCCESS );
	}
	else
	{
		pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength = 0;
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}
}

NTSTATUS 
USBIO_GetCurrentFrameNumber( 
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp, 
	PURB pUrb 
	)
{
	ULONGLONG dwwTime;

	dwwTime = KeQueryInterruptTime();
	dwwTime = dwwTime / 10000;
	pUrb->UrbGetCurrentFrameNumber.FrameNumber = (ULONG)dwwTime;

	return USBComplete( Irp, pUrb, STATUS_SUCCESS, USBD_STATUS_SUCCESS );
}

NTSTATUS 
USBIO_GetDescriptorFromDevice( 
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp, 
	PURB pUrb 
	)
{
	PVOID TargetBuffer;
	ULONG TargetBufferRequestSize;
	PDEVICE_EXTENSION deviceExtension; 
	deviceExtension = DeviceObject->DeviceExtension;

	if( !pUrb->UrbControlDescriptorRequest.TransferBufferLength )
	{
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	if( !pUrb->UrbControlDescriptorRequest.TransferBuffer && !pUrb->UrbControlDescriptorRequest.TransferBufferMDL )
	{
		pUrb->UrbControlDescriptorRequest.TransferBufferLength = 0;
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	if( pUrb->UrbControlDescriptorRequest.TransferBuffer && pUrb->UrbControlDescriptorRequest.TransferBufferMDL )
	{
		pUrb->UrbControlDescriptorRequest.TransferBufferLength = 0;
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	if( pUrb->UrbControlDescriptorRequest.TransferBuffer )
	{
		TargetBuffer = pUrb->UrbControlDescriptorRequest.TransferBuffer;
	}
	else if( pUrb->UrbControlDescriptorRequest.TransferBufferMDL )
	{
		TargetBuffer = MmGetSystemAddressForMdl( pUrb->UrbControlDescriptorRequest.TransferBufferMDL );
	}
	
	TargetBufferRequestSize = pUrb->UrbControlDescriptorRequest.TransferBufferLength;

	if( pUrb->UrbControlDescriptorRequest.DescriptorType == USB_DEVICE_DESCRIPTOR_TYPE )
	{
		TargetBufferRequestSize = ( TargetBufferRequestSize > deviceExtension->pDeviceDescriptor->bLength )? 
			deviceExtension->pDeviceDescriptor->bLength : TargetBufferRequestSize;

		RtlMoveMemory( TargetBuffer, deviceExtension->pDeviceDescriptor, TargetBufferRequestSize );

		pUrb->UrbControlDescriptorRequest.TransferBufferLength = TargetBufferRequestSize;
		return USBComplete( Irp, pUrb, STATUS_SUCCESS, USBD_STATUS_SUCCESS );
	}
	else if( pUrb->UrbControlDescriptorRequest.DescriptorType == USB_CONFIGURATION_DESCRIPTOR_TYPE )
	{
		if( pUrb->UrbControlDescriptorRequest.Index != 0 )
		{
			pUrb->UrbControlDescriptorRequest.TransferBufferLength = 0;
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
		}
		TargetBufferRequestSize = ( TargetBufferRequestSize > deviceExtension->pConfigurationDescriptor->wTotalLength )? 
			deviceExtension->pConfigurationDescriptor->wTotalLength : TargetBufferRequestSize;

		RtlMoveMemory( TargetBuffer, deviceExtension->pConfigurationDescriptor, TargetBufferRequestSize );

		pUrb->UrbControlDescriptorRequest.TransferBufferLength = TargetBufferRequestSize;
		return USBComplete( Irp, pUrb, STATUS_SUCCESS, USBD_STATUS_SUCCESS );
	}
	else
	{
		pUrb->UrbControlDescriptorRequest.TransferBufferLength = 0;
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}
}

NTSTATUS 
USBIO_GetConfiguration( 
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp, 
	PURB pUrb 
	)
{
	PDEVICE_EXTENSION deviceExtension; 
	PVOID TargetBuffer;
	ULONG TargetBufferRequestSize;
	deviceExtension = DeviceObject->DeviceExtension;

	if( pUrb->UrbControlGetConfigurationRequest.TransferBufferLength != 1 )
	{
		pUrb->UrbControlGetConfigurationRequest.TransferBufferLength = 0;
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	if( !pUrb->UrbControlGetConfigurationRequest.TransferBuffer && !pUrb->UrbControlGetConfigurationRequest.TransferBufferMDL )
	{
		pUrb->UrbControlGetConfigurationRequest.TransferBufferLength = 0;
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	if( pUrb->UrbControlGetConfigurationRequest.TransferBuffer && pUrb->UrbControlGetConfigurationRequest.TransferBufferMDL )
	{
		pUrb->UrbControlGetConfigurationRequest.TransferBufferLength = 0;
		return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
	}

	if( pUrb->UrbControlGetConfigurationRequest.TransferBuffer )
	{
		TargetBuffer = pUrb->UrbControlGetConfigurationRequest.TransferBuffer;
	}
	else if( pUrb->UrbControlGetConfigurationRequest.TransferBufferMDL )
	{
		TargetBuffer = MmGetSystemAddressForMdl( pUrb->UrbControlGetConfigurationRequest.TransferBufferMDL );
	}
	
	TargetBufferRequestSize = pUrb->UrbControlGetConfigurationRequest.TransferBufferLength;

	*(unsigned char *)TargetBuffer = deviceExtension->CurrentConfigValue;

	return USBComplete( Irp, pUrb, STATUS_SUCCESS, USBD_STATUS_SUCCESS );
}

NTSTATUS
USBIO
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	)
{
	PIO_STACK_LOCATION pStack; 
	PDEVICE_EXTENSION deviceExtension; 
	PURB pUrb;
	NTSTATUS status;

	pStack = IoGetCurrentIrpStackLocation ( Irp );
	deviceExtension = DeviceObject->DeviceExtension;

	pUrb = (PURB)pStack->Parameters.Others.Argument1;
	
	switch( pUrb->UrbHeader.Function )
	{
	case URB_FUNCTION_SELECT_CONFIGURATION:
		return USBIO_Select_Configuration( DeviceObject, Irp, pUrb );
	case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
		return USBIO_BulkOrInterruptTransfer( DeviceObject, Irp, pUrb );
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
		return USBIO_GetDescriptorFromDevice( DeviceObject, Irp, pUrb );
	case URB_FUNCTION_GET_CONFIGURATION:
		return USBIO_GetConfiguration( DeviceObject, Irp, pUrb );
	case URB_FUNCTION_RESET_PIPE:
		return USBIO_ResetPipe( DeviceObject, Irp, pUrb );
	}	
	return USBComplete( Irp, pUrb, STATUS_NOT_SUPPORTED, USBD_STATUS_INVALID_PARAMETER );
}

void
BUSDRIVER_WorkItemRoutine
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PVOID Context
	)
{
	PIRP Irp;

	PWORKITEM_CONTEXT pWorkItemContext = (PWORKITEM_CONTEXT)Context;

	Irp = pWorkItemContext->Irp;

	USBIO( DeviceObject, Irp );

	ExFreePool( pWorkItemContext->pIoWorkItem );
	ExFreePool( pWorkItemContext );
}

void
Pipe0CancelRoutine
	(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp
	)
{
	RemoveEntryList( &Irp->Tail.Overlay.ListEntry );

	IoReleaseCancelSpinLock( Irp->CancelIrql );
	USBComplete( Irp, 0, STATUS_CANCELLED, 0 );
}

void
Pipe1CancelRoutine
	(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp
	)
{
	RemoveEntryList( &Irp->Tail.Overlay.ListEntry );

	IoReleaseCancelSpinLock( Irp->CancelIrql );
	USBComplete( Irp, 0, STATUS_CANCELLED, 0 );
}

NTSTATUS
DRIVER_InternalDeviceControlDispatch
	(
	IN PDEVICE_OBJECT DeviceObject,   
	IN PIRP Irp                       
	)
{
	PIO_STACK_LOCATION pStack; 
	PDEVICE_EXTENSION deviceExtension; 
	PURB pUrb;
	NTSTATUS status;
	PWORKITEM_CONTEXT pWorkItemContext;
	KIRQL OldIrql;
	
	pStack = IoGetCurrentIrpStackLocation ( Irp );
	deviceExtension = DeviceObject->DeviceExtension;

	if( deviceExtension->DeviceObjectType == BUSDRIVERDEVICE )
	{ // BusDriverDevice
		return USBComplete( Irp, 0, STATUS_INVALID_DEVICE_REQUEST, 0 );
	}
	else
	{ // ChildDriverDevice
		if( deviceExtension->Removed == TRUE )
			return USBComplete( Irp, 0, STATUS_DEVICE_REMOVED, 0 );

		switch( pStack->Parameters.DeviceIoControl.IoControlCode )
		{
			case IOCTL_INTERNAL_USB_SUBMIT_URB:
				// 현 문맥에서 처리할 수 있는 명령어를 구분한다
				pUrb = (PURB)pStack->Parameters.Others.Argument1;

				status = checkURB( pUrb );
				if( !NT_SUCCESS( status ) )
				{
					return USBComplete( Irp, pUrb, status, USBD_STATUS_INVALID_PARAMETER );
				}

				switch( pUrb->UrbHeader.Function )
				{
				case URB_FUNCTION_ABORT_PIPE:
					return USBIO_AbortPipe( DeviceObject, Irp, pUrb );
				case URB_FUNCTION_GET_CURRENT_FRAME_NUMBER:
					return USBIO_GetCurrentFrameNumber( DeviceObject, Irp, pUrb );
				case URB_FUNCTION_RESET_PIPE:
				case URB_FUNCTION_SELECT_CONFIGURATION:
				case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
				case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
				case URB_FUNCTION_GET_CONFIGURATION:
					break;
				default:
					return USBComplete( Irp, pUrb, STATUS_NOT_SUPPORTED, USBD_STATUS_INVALID_PARAMETER );
				}	
				
				IoMarkIrpPending( Irp );

				// Timer를 이용해야 하는 요청을 구분한다
				if( pUrb->UrbHeader.Function == URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER )
				{
					IoAcquireCancelSpinLock( &OldIrql );

					if( pUrb->UrbBulkOrInterruptTransfer.PipeHandle == (PVOID)PIPE0HANDLE )
					{
						InsertTailList( &deviceExtension->Pipe0IrpListHead, &Irp->Tail.Overlay.ListEntry );
						IoSetCancelRoutine( Irp, Pipe0CancelRoutine );
						IoReleaseCancelSpinLock( OldIrql );
					}
					else
					if( pUrb->UrbBulkOrInterruptTransfer.PipeHandle == (PVOID)PIPE1HANDLE )
					{
						IoSetCancelRoutine( Irp, Pipe1CancelRoutine );
						InsertTailList( &deviceExtension->Pipe1IrpListHead, &Irp->Tail.Overlay.ListEntry );
						IoReleaseCancelSpinLock( OldIrql );
					}
					else
					{
						IoReleaseCancelSpinLock( OldIrql );
						return USBComplete( Irp, pUrb, STATUS_INVALID_PARAMETER, USBD_STATUS_INVALID_PARAMETER );
					}
				}
				else
				{ // 나머지는 WorkItem을 이용한다
					pWorkItemContext = (PWORKITEM_CONTEXT)ExAllocatePool( NonPagedPool, sizeof(WORKITEM_CONTEXT) );
					pWorkItemContext->Irp = Irp;
					pWorkItemContext->pIoWorkItem = IoAllocateWorkItem( DeviceObject );
					IoQueueWorkItem( pWorkItemContext->pIoWorkItem, BUSDRIVER_WorkItemRoutine, CriticalWorkQueue, pWorkItemContext );
				}

				return STATUS_PENDING;
		}
		return USBComplete( Irp, 0, STATUS_NOT_SUPPORTED, 0 );
	}			
}
