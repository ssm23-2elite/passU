
// totalDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "total.h"
#include "totalDlg.h"
#include "afxdialogex.h"

#include "uvcview.h"
#include "vndrlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

USBSENDDEVICEDESC sendToDeviceDescData;

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CtotalDlg ��ȭ ����



CtotalDlg::CtotalDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtotalDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtotalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtotalDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CtotalDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CtotalDlg �޽��� ó����

BOOL CtotalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ���� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CtotalDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CtotalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CtotalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CtotalDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GetDesc();
}

int GetDesc()
{
	HANDLE                           hHCDev = NULL;
	HDEVINFO                         deviceInfo = NULL;
	SP_DEVINFO_DATA                  deviceInfoData;
	SP_DEVICE_INTERFACE_DATA         deviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA deviceDetailData = NULL;
	ULONG                            index = 0;
	ULONG                            requiredLength = 0;
	BOOL                             success;

	gHubList.DeviceInfo = INVALID_HANDLE_VALUE;
    InitializeListHead(&gHubList.ListHead);
    gDeviceList.DeviceInfo = INVALID_HANDLE_VALUE;
    InitializeListHead(&gDeviceList.ListHead);
	
	TotalDevicesConnected = 0;
    TotalHubs = 0;

	// GUID ��� �������̽��� ����Ͽ� ȣ��Ʈ ��Ʈ�ѷ��� �ݺ�
    // Handle ��ȯ
	//
    deviceInfo = SetupDiGetClassDevs((LPGUID)&GUID_CLASS_USB_HOST_CONTROLLER,
                                     NULL,
                                     NULL,
                                     (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (index=0;
         SetupDiEnumDeviceInfo(deviceInfo,
                               index,
                               &deviceInfoData); // ����̽� ���� ��Ҹ� �����ϴ� ����ü ��ȯ
         index++)
    {
        deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        success = SetupDiEnumDeviceInterfaces(deviceInfo,
                                              0,
                                              (LPGUID)&GUID_CLASS_USB_HOST_CONTROLLER,
                                              index,
                                              &deviceInterfaceData); // ����̽� interfaces �� ����
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
                                                  NULL); // ����̽� interface�� ������ �о�´�. ũ��� �Բ�

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
                                                  NULL); // ����̽��� ��θ� �����´�.

        if (!success)
        {
            OOPS();
            break;
        }

        hHCDev = CreateFile(deviceDetailData->DevicePath,
                            GENERIC_WRITE, // �б�? ����?
                            FILE_SHARE_WRITE, // ���ÿ� ���Ⱑ��
                            NULL,
                            OPEN_EXISTING, // ������ ����
                            0,
                            NULL); // �ش� Path�� �ִ� ����̽��� Handle�� ��´�.

        // Handle ��⸦ ������ ��� �� ����̽��� RootHub�� ������ �����´�.
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

    SetupDiDestroyDeviceInfoList(deviceInfo); // deviceInfo �޸� free

	return 0;
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


    // �޸� �Ҵ�
    //
    hcInfo = (PUSBHOSTCONTROLLERINFO)ALLOC(sizeof(USBHOSTCONTROLLERINFO));

    // �޸� �Ҵ� ����
	//
    if (NULL == hcInfo)
        return;

    hcInfo->DeviceInfoType = HostControllerInfo; // enum -> 0

    // host controller�� ���� �ش� ����̹� key �̸��� �����´�.
    //
    driverKeyName = GetHCDDriverKeyName(hHCDev);

    if (NULL == driverKeyName)
    {
        OOPS();
        FREE(hcInfo);
        return;
    }

    // �̹� ������ ��� �ٽ� ���� ���� �ʴ´�.
    //
    listEntry = EnumeratedHCListHead.Flink;

    while (listEntry != &EnumeratedHCListHead)
    {
        hcInfoInList = CONTAINING_RECORD(listEntry,
                                         USBHOSTCONTROLLERINFO,
                                         ListEntry);

        if (strcmp(driverKeyName, hcInfoInList->DriverKey) == 0)
        {
            // �̹� ����Ʈ�� ���� ��, ���� ����
            //
            FREE(driverKeyName);
            FREE(hcInfo);
            return;
        }

        listEntry = listEntry->Flink;
    }

    // host controller device �Ӽ����� ����
    {
        size_t cbDriverName = 0;
        HRESULT hr = S_OK;

        hr = StringCbLength(driverKeyName, MAX_DRIVER_KEY_NAME, &cbDriverName);
        if (SUCCEEDED(hr))
        {
            DevProps = DriverNameToDeviceProperties(driverKeyName, cbDriverName);
        }
    }

	// ����̹� Ű�� ���� 
	//
    hcInfo->DriverKey = driverKeyName;

    if (DevProps)
    {
        ULONG   ven, dev, subsys, rev;
        ven = dev = subsys = rev = 0;

		// DeviceId�� ����
		//
        if (sscanf_s(DevProps->DeviceId,
                   "PCI\\VEN_%x&DEV_%x&SUBSYS_%x&REV_%x",
                   &ven, &dev, &subsys, &rev) != 4)
        {
            OOPS();
        }
		
		// ��Ÿ ������
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

    // USB Host Controller�� power map�� ��´�.
	//
    dwSuccess = GetHostControllerPowerMap(hHCDev, hcInfo);

    if (ERROR_SUCCESS != dwSuccess)
    {
        OOPS();
    }


    // ����, ����̽� �׸��� �Լ��� ��´�.
    //
    hcInfo->BusDeviceFunctionValid = FALSE;

	// ���� ��� �κ�
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
		// ����̽��� �ּҸ� ��� �κ�
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
		// �Լ��� ��� �κ�
        hcInfo->BusDevice = deviceAndFunction >> 16;
        hcInfo->BusFunction = deviceAndFunction & 0xffff;
        hcInfo->BusDeviceFunctionValid = TRUE;
    }

    // USB Host Controller info�� ��� �κ�
    dwSuccess = GetHostControllerInfo(hHCDev, hcInfo);

    if (ERROR_SUCCESS != dwSuccess)
    {
        OOPS();
    }
	
    // ���ŵ� host controller���� host controller ����Ʈ�� �߰�
    //
    InsertTailList(&EnumeratedHCListHead,
                   &hcInfo->ListEntry);

    // Root Hub�� �̸��� ��� �κ�
    // host controller�� root hub�� �����ϱ� ����
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
                              IOCTL_GET_HCD_DRIVERKEY_NAME, // �ش� �Լ� �̸�
                              &driverKeyName, // inputbuffer�� �ּ�
                              sizeof(driverKeyName), // inputbuffer ũ��
                              &driverKeyName, // outputbuffer�� �ּ�
                              sizeof(driverKeyName), // outputbuffer ũ��
                              &nBytes,
                              NULL); // ���� �ڵ带 ���������� �ش� ����̽��� ����

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

    // �޸� �Ҵ�
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

  DriverNameToDeviceProperties()

  DriverName�� ��ġ�ϴ� DevNode �� ��ġ �Ӽ����� return.
  �´� DevNode�� ������ NULL return.

  ȣ���ڴ� FREE()�� �̿��Ͽ� ��ȯ�� ����ü�� �ݵ�� free.

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

    // �޸� �Ҵ�
    DevProps = (PUSB_DEVICE_PNP_STRINGS) ALLOC(sizeof(USB_DEVICE_PNP_STRINGS));

    if(NULL == DevProps)
    {
        status = FALSE;
        goto Done;
    }

    // device�� instance�� ����
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

/*****************************************************************************

  DriverNameToDeviceInst()

  DriverName�� ��ġ�ϴ� DevNode�� Device instance�� ã�´�.
  DevNode�� ��ġ�ϴ� ���� ������ FALSE�� �ִٸ� True�� return.

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

	// zero termination �� �����ϱ� ���� ���� ���ڿ� ���� ���
    pDriverName = (PCHAR) ALLOC((DWORD) cbDriverName + 1);
    if (NULL == pDriverName)
    {
        status = FALSE;
        goto Done;
    }
    StringCbCopyN(pDriverName, cbDriverName + 1, DriverName, cbDriverName);

    // ���� �־��� DriverName�� ��ġ�ϴ� � ��ġ�� �ִٸ� 
	// ��� ���� ����� ���̴� ��ġ���� �����Ѵ�.
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
		// ���� ��ġ�� devinst�� ����
        //

        status = SetupDiEnumDeviceInfo(deviceInfo,
                                       deviceIndex,
                                       &deviceInfoData);

        deviceIndex++;

        if (!status)
        {
            //
			// �̰��� ���� �ų� ��� ��ġ���� ó���ǰ� ������ �ȳ� �ϴ� ���� �� �ִ�.
			// �Ǵ� ���ϴ� ��ġ�� ã�� ���ߴٴ� ���� �� �մ�.
            //
            
            done = TRUE;
            break;
        }

        //
        // DriverName ���� ����.
        //

        bResult = GetDeviceProperty(deviceInfo,
                                    &deviceInfoData,
                                    SPDRP_DRIVER,
                                    &buf);
        
        // DriverName �� ���� ��ġ�ϴ� ��� DeviceInstance ��  return.
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

  device properties ����ü�� free.

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
    // ���� ���� ���� �ʿ�ġ �ʴ�. but left in case
    // ������ ������ �Ҵ�� ���� ������ �ʵ���� 
	// ���� ��� ���߿� ���а� ����̴�. 
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
// GetHostControllerPowerMap()
//
// HANDLE hHCDev
//      - USB Host Controller �� �ٷ�� ���� handle.
//
// PUSBHOSTCONTROLLERINFO hcInfo
//      - Power Map Info�� �ޱ� ���� ������ ����ü
//
// return DWORD dwError
//      - return ERROR_SUCCESS �Ǵ� last error
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
        // 0���� �ʱ�ȭ
        memset(&UsbPowerInfoRequest, 0, sizeof(UsbPowerInfoRequest));

        // set the header and request sizes
        UsbPowerInfoRequest.Header.UsbUserRequest = USBUSER_GET_POWER_STATE_MAP;
        UsbPowerInfoRequest.Header.RequestBufferLength = sizeof(UsbPowerInfoRequest);
        UsbPowerInfoRequest.PowerInformation.SystemState = (WDMUSB_POWER_STATE)nPowerState;

        //
		// ���� hub�� ���� USB_POWER_INFO ����ü�� ���� USBHUB�� �˾Ƴ���.
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
			// ��ǻ���� USB Host Controller �� info structure �� data�� copy
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
//      - USB Host Controller �� �ٷ�� ���� handle.
//
// PUSBHOSTCONTROLLERINFO hcInfo
//      - Power Map Info�� �ޱ� ���� ������ ����ü
//
// return DWORD dwError
//      - return ERROR_SUCCESS �Ǵ� last error
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

	// header �׸��� ��û�ϴ� ũ����� ����
    UsbControllerInfo.Header.UsbUserRequest = USBUSER_GET_CONTROLLER_INFO_0;
    UsbControllerInfo.Header.RequestBufferLength = sizeof(UsbControllerInfo);

    //
    // USB_CONTROLLER_INFO_0 structure �� ����
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
			// ��ǻ���� USB Host Controller �� info structure �� data�� copy
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

    // Host Controller �� �پ� �ִ� Root Hub�� �̸��� ���̸� ����
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

    // Root Hub name�� ���� ���� �Ҵ� 
    //
    nBytes = rootHubName.ActualLength;

    rootHubNameW = (PUSB_ROOT_HUB_NAME)ALLOC(nBytes);
    if (rootHubNameW == NULL)
    {
        OOPS();
        goto GetRootHubNameError;
    }

	// Host Controller�� �پ� �ִ� Root Hub�� �̸��� ����
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

	// Root Hub�� �̸��� ��ȯ
    //
    rootHubNameA = WideStrToMultiStr(rootHubNameW->RootHubName, nBytes);

	// ��� ���� ������, ��ȯ�� Root Hub�� �̸��� ��ȯ�ϰ� ��ȯ���� ���� ���� free.
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
// HubName - ���� Hub�� �̸�. �� �����ʹ� ���� �Ǿ�� �ϹǷ� ȣ���ڰ� �� �޸𸮸�
// free �ϰų� ���� �� �� ����.
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

			//�����
	
			PUSB_DESCRIPTOR_REQUEST ConfigReqDesc = ((PUSBEXTERNALHUBINFO)info)->ConfigDesc;
			PUSB_COMMON_DESCRIPTOR  commonDesc = (PUSB_COMMON_DESCRIPTOR)(ConfigReqDesc + 1);
			PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)commonDesc;
			PUSB_INTERFACE_DESCRIPTOR   InterfaceDesc;

			memcpy(&sendToDeviceDescData.DeviceDescriptor, &info->ConnectionInfo->DeviceDescriptor, sizeof(sendToDeviceDescData.DeviceDescriptor) );

			memcpy(&sendToDeviceDescData.ConfigDesc, ConfigDesc, sizeof(sendToDeviceDescData.ConfigDesc) );
			
			commonDesc = (PUSB_COMMON_DESCRIPTOR)((PUCHAR)commonDesc + commonDesc->bLength);
			InterfaceDesc = (PUSB_INTERFACE_DESCRIPTOR)commonDesc;

			memcpy(&sendToDeviceDescData.InterfaceDesc, InterfaceDesc, sizeof(sendToDeviceDescData.InterfaceDesc) );

			memcpy(&sendToDeviceDescData.EndpointDescriptor[0], &((PUSB_PIPE_INFO)info->ConnectionInfo->PipeList)[0], sizeof(sendToDeviceDescData.EndpointDescriptor[0]) );
			memcpy(&sendToDeviceDescData.EndpointDescriptor[1], &((PUSB_PIPE_INFO)info->ConnectionInfo->PipeList)[1], sizeof(sendToDeviceDescData.EndpointDescriptor[0]) );

			strcpy_s(sendToDeviceDescData.DeviceId, info->UsbDeviceProperties->DeviceId);
			strcpy_s(sendToDeviceDescData.DeviceDesc, info->UsbDeviceProperties->DeviceDesc);
			strcpy_s(sendToDeviceDescData.HwId, info->UsbDeviceProperties->HwId);
			strcpy_s(sendToDeviceDescData.Service, info->UsbDeviceProperties->Service);
			strcpy_s(sendToDeviceDescData.DeviceClass, info->UsbDeviceProperties->DeviceClass);

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

    // Driver Key name �� type�� ��ȯ ( WCHAR -> PCHAR )
    //
    driverKeyNameA = WideStrToMultiStr(driverKeyNameW->DriverKeyName, nBytes);

    // �޸� ����
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
                                               &requiredLength); // Plug and Play ����̽� �Ӽ��� �˻��Ͽ� requiredLength�� �о�´�.
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
                                                &requiredLength); // �ٽ� �ѹ� ȣ���ؼ� �ش� ����̽��� �Ӽ��� ���� ������ �о�´�.
    if(bResult == FALSE)
    {
        FREE(*ppBuffer);
        *ppBuffer = NULL;
        return FALSE;
    }

    return TRUE;
}