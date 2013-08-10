; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSimpleUDPDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "SimpleUDP.h"

ClassCount=4
Class1=CSimpleUDPApp
Class2=CSimpleUDPDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_SIMPLEUDP_DIALOG
Class4=CClientSend
Resource4=IDD_DIALOG1

[CLS:CSimpleUDPApp]
Type=0
HeaderFile=SimpleUDP.h
ImplementationFile=SimpleUDP.cpp
Filter=N

[CLS:CSimpleUDPDlg]
Type=0
HeaderFile=SimpleUDPDlg.h
ImplementationFile=SimpleUDPDlg.cpp
Filter=D
LastObject=IDC_EDIT1
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=SimpleUDPDlg.h
ImplementationFile=SimpleUDPDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SIMPLEUDP_DIALOG]
Type=1
Class=CSimpleUDPDlg
ControlCount=3
Control1=IDCANCEL,button,1342242816
Control2=IDC_BUTTON1,button,1342242816
Control3=IDC_EDIT1,edit,1352728644

[DLG:IDD_DIALOG1]
Type=1
Class=CClientSend
ControlCount=3
Control1=IDCANCEL,button,1342242816
Control2=IDC_BUTTON1,button,1342242816
Control3=IDC_EDIT1,edit,1350631552

[CLS:CClientSend]
Type=0
HeaderFile=ClientSend.h
ImplementationFile=ClientSend.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_BUTTON1

