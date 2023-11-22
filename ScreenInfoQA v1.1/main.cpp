//---------------------------------------------------------------------------
#include <windows.h>    // �w������USB�˸m�� *�n�� vcl.h ���sĶ
#include <SetupAPI.h> // �w������USB�˸m�� *�n�� vcl.h ���sĶ
#include <cfgmgr32.h> // �w������USB�˸m�� *�n�� vcl.h ���sĶ
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>		//�ɮ׿�X�J��
#include <fstream>
#include <iostream>
#include <string>
#include "main.h"
#include <DXGI.h>
#include <vector>
#include <cmath>
//---------------------------------------------------------------------------
TfrmMain *frmMain;
#pragma package(smart_init)
#pragma link "IdBaseComponent"
#pragma link "IdComponent"
#pragma link "IdIcmpClient"
#pragma link "IdRawBase"
#pragma link "IdRawClient"
#pragma resource "*.dfm"
#pragma comment(lib, "dxgi.lib")

using namespace std;
bool bStart = false;
DWORD dwDevMsgCount =0;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
	TListInit_Screen 		= new TStringList;
	TListInit_DisplayLink 	= new TStringList;
	TListScreen_Dev 		= new TStringList;
	TListDisplayLink_Dev 	= new TStringList;
	//
	TListDisplayLink_Path 	= new TStringList;
	moDebug->Lines->Add("[�ù������]");
	getMonitor(true);
	moDebug->Lines->Add("\r\n\r\n[DisplayLink�����]");
	getDisplayLinkCardName(true);
	bStart = true;

	OldWindowProc = WindowProc;
	WindowProc = MyWindowProc;
	GUID guid;
	guid =StringToGUID(GUID_USB_HUB);
	g_DeviceNogification.RegisterWindowsDeviceInterfaceNotification(
		Handle,guid);
	guid =StringToGUID(GUID_USB_DEVICE);
	g_DeviceNogification.RegisterWindowsDeviceInterfaceNotification(
		Handle,guid);
	guid =StringToGUID(GUID_VIDEO);
	g_DeviceNogification.RegisterWindowsDeviceInterfaceNotification(
		Handle,guid);
	guid =StringToGUID(GUID_USBSTOR);
	g_DeviceNogification.RegisterWindowsDeviceInterfaceNotification(
		Handle,guid);


	//https://www.itread01.com/content/1550426413.html                                                                                               hhh
	//https://stackoom.com/question/3G7vr/DXGI%E6%98%BE%E7%A4%BA%E5%99%A8%E6%9E%9A%E4%B8%BE%E6%9C%AA%E6%8F%90%E4%BE%9BDell-P-Q%E6%98%BE%E7%A4%BA%E5%99%A8%E7%9A%84%E5%AE%8C%E6%95%B4%E5%B0%BA%E5%AF%B8
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::MyWindowProc(TMessage& Message)
{
	PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
	AnsiString DBCC_Name,SS;

	pDevInf =(PDEV_BROADCAST_DEVICEINTERFACE)Message.LParam;

	//
	if (Message.Msg==WM_DEVICECHANGE){
		if(Message.WParam ==DBT_DEVICEARRIVAL){
			if(AnsiString(pDevInf->dbcc_name).Length()>0)
			{
				SS = SS.sprintf("%04d",dwDevMsgCount);
				DBCC_Name =AnsiString(pDevInf->dbcc_name).UpperCase();
				if(DBCC_Name.Pos("#"))
				{
					dwDevMsgCount++;
					DEBUG_DEV_MSG("["+SS+"]( IN  )["+FormatDateTime("mm-dd_hh-nn-ss", Now())+"]");
					DEBUG_DEV_MSG(DBCC_Name);
				}
				if(dwDevMsgCount%5==0)
					DEBUG_DEV_MSG("----------");
			}
		}
		else if(Message.WParam ==DBT_DEVICEREMOVECOMPLETE){
			dwDevMsgCount++;
			if(AnsiString(pDevInf->dbcc_name).Length()>0)
			{
				SS = SS.sprintf("%04d",dwDevMsgCount);
				DBCC_Name =AnsiString(pDevInf->dbcc_name).UpperCase();
				DEBUG_DEV_MSG("["+SS+"](OUT)["+FormatDateTime("mm-dd_hh-nn-ss", Now())+"]");
				DEBUG_DEV_MSG(DBCC_Name);
				if(dwDevMsgCount%5==0)
					DEBUG_DEV_MSG("----------");
			}
		}

	}
	// �~���l�T��������
	TfrmMain::WndProc(Message);
}
void __fastcall TfrmMain::Timer1Timer(TObject *Sender)
{
	if(bStart)
	{
		bStart = false;
		getMonitor(false);
		getDisplayLinkCardName(false);
		CheckDevData();
		bStart = true;
	}
}
//---------------------------------------------------------------------------
int __fastcall TfrmMain::getMonitor(bool bInit)
{
	AnsiString DeviceID,DeviceDetail;
	TListScreen_Dev->Clear();
	DISPLAY_DEVICEA dd;
	TDeviceModeA lpDevMode;
	dd.cb = sizeof(dd);
	DWORD dev = 0;
	int MONITORAMOUNT = 0;
	while (EnumDisplayDevicesA(0, dev, &dd, 0))
	{
		DISPLAY_DEVICEA ddMon;
		ZeroMemory(&ddMon, sizeof(ddMon));
		ddMon.cb = sizeof(ddMon);
		DWORD devMon = 0;
		while (EnumDisplayDevicesA(dd.DeviceName, devMon, &ddMon, 0))
		{
			DeviceDetail = "";
			if (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE &&
			!(ddMon.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
			{
				DeviceID = ddMon.DeviceID;
				DeviceID = DeviceID.SubString(9,DeviceID.Pos("\\")-1);
				EnumDisplaySettingsA(dd.DeviceName,ENUM_CURRENT_SETTINGS,&lpDevMode);

				((TPanel*)frmMain->FindComponent("pl_pels"+AnsiString(MONITORAMOUNT+1)))->Caption
					= (AnsiString)lpDevMode.dmPelsWidth + " X "+ (AnsiString)lpDevMode.dmPelsHeight;
				((TPanel*)frmMain->FindComponent("pl_freq"+AnsiString(MONITORAMOUNT+1)))->Caption
					= lpDevMode.dmDisplayFrequency;
				TPanel* plName = ((TPanel*)frmMain->FindComponent("pl_name"+AnsiString(MONITORAMOUNT+1)));
				plName->Caption  = DeviceID;
				plName->Hint = dd.DeviceName;

				DeviceDetail = DeviceID+":"
							+(AnsiString)lpDevMode.dmPelsWidth + "x"+ (AnsiString)lpDevMode.dmPelsHeight
							+"-"+lpDevMode.dmDisplayFrequency;
				MONITORAMOUNT++;
				if(MONITORAMOUNT>1&&bInit)
				{
					moDebug->Lines->Add(DeviceDetail);
					TListInit_Screen->Add(DeviceDetail);
				}
				if(MONITORAMOUNT>1)
					TListScreen_Dev->Add(DeviceDetail);

			}
			devMon++;
			ZeroMemory(&ddMon, sizeof(ddMon));
			ddMon.cb = sizeof(ddMon);
		}
		ZeroMemory(&dd, sizeof(dd));
		dd.cb = sizeof(dd);
		dev++;
	}
	for(int i = MONITORAMOUNT;i<5;i++)
	{
		((TPanel*)frmMain->FindComponent("pl_pels"+AnsiString(i+1)))->Caption
			= "";
		((TPanel*)frmMain->FindComponent("pl_freq"+AnsiString(i+1)))->Caption
			= "";
		((TPanel*)frmMain->FindComponent("pl_name"+AnsiString(i+1)))->Caption
			= "";
	}
	return MONITORAMOUNT;
}
void TfrmMain::getDisplayLinkCardName(bool bInit)
{
	EnumUSB();
	AnsiString DevInfo="";
	TListDisplayLink_Dev->Clear();

	HKEY hSoftKey = NULL;
	for(int i = 0;i<TListDisplayLink_Path->Count;i++)
	{
		hSoftKey = NULL;
		TPanel* plDisplayLinkCard = (TPanel*)frmMain->FindComponent("plDisplayLinkCard"+AnsiString(i+1));
		setRegDriver(TListDisplayLink_Path->Strings[i]);
		AnsiString temppath = "system\\CurrentControlSet\\Enum\\"+TListDisplayLink_Path->Strings[i];
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, temppath.c_str(), 0, KEY_READ,&hSoftKey) == ERROR_SUCCESS)
		{
			DWORD dwType=REG_SZ;
			DWORD dwCount=0;
			LONG lResult = RegQueryValueExA(hSoftKey, "FriendlyName", NULL, &dwType,NULL, &dwCount);
			if (lResult == ERROR_SUCCESS)
			{
				LPBYTE company_Get=new BYTE [dwCount];
				lResult = RegQueryValueExA(hSoftKey, "FriendlyName", NULL, &dwType,
				company_Get, &dwCount);
				if(lResult != ERROR_SUCCESS)
					plDisplayLinkCard->Caption = "(!)Error";
				else
				{
					plDisplayLinkCard->Caption = AnsiString((char*)company_Get);
					if(bInit)
					{
						TListInit_DisplayLink->Add(AnsiString((char*)company_Get));
						moDebug->Lines->Add(AnsiString(plDisplayLinkCard->Caption));
					}
					TListDisplayLink_Dev->Add(AnsiString((char*)company_Get));
				}

				delete company_Get;
			}
		}
	}
	//
	frmMain->AutoSize = false;
	switch(TListDisplayLink_Dev->Count)
	{
		case 0:
			plDisplay->Height	= 0;
			frmMain->Height = 453;
			break;
		case 1:
			plDisplay->Height	= 83;
			frmMain->Height = 536;
			break;
		case 2:
			plDisplay->Height	= 125;
			frmMain->Height = 578;
			break;
	}
	frmMain->AutoSize = true;
	//
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
}
void TfrmMain::EnumUSB()
{
	TListDisplayLink_Path->Clear();
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i,j;
	AnsiString SS,USBPath;
	PSP_DEVICE_INTERFACE_DETAIL_DATA   pDetail   =NULL;
	GUID GUID_USB = StringToGUID(GUID_USB_DEVICE);
	//DEBUG("[ USB�˸m�C�| ]");
	//--------------------------------------------------------------------------
	//   ����]�Ƹ�T
	hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_USB,
	0,   //   Enumerator
	0,
	DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
	//--------------------------------------------------------------------------
	SP_DEVICE_INTERFACE_DATA   ifdata;
	SP_DEVINFO_DATA            spdd;
	DeviceInfoData.cbSize   =   sizeof(SP_DEVINFO_DATA);
	for (i=0;SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData);i++)	//   �T�|�C��USB�]��
	{
		ifdata.cbSize   =   sizeof(ifdata);
		if (SetupDiEnumDeviceInterfaces(								//   �T�|�ŦX��GUID���]�Ƥ���
		hDevInfo,           //   �]�Ƹ�T������X
		NULL,                         //   �����B�~���]�ƴy�z
		(LPGUID)&GUID_USB,//GUID_CLASS_USB_DEVICE,                     //   GUID
		(ULONG)i,       //   �]�Ƹ�T���ت��]�ƧǸ�
		&ifdata))                 //   �]�Ƥ�����T
		{
			ULONG predictedLength   =   0;
			ULONG requiredLength   =   0;
			//   ���o�ӳ]�Ƥ������Ӹ`(�]�Ƹ��|)
			SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   �]�Ƹ�T������X
				&ifdata,          //   �]�Ƥ�����T
				NULL,             //   �]�Ƥ����Ӹ`(�]�Ƹ��|)
				0,         	      //   ��X�w�İϤj�p
				&requiredLength,  //   ���ݭp���X�w�İϤj�p(�����γ]�w��)
				NULL);            //   �����B�~���]�ƴy�z
			//   ���o�ӳ]�Ƥ������Ӹ`(�]�Ƹ��|)
			predictedLength=requiredLength;
			pDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT,   predictedLength);
			pDetail->cbSize   =   sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			ZeroMemory(&spdd, sizeof(spdd));
			spdd.cbSize = sizeof(spdd);

			if(SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   �]�Ƹ�T������X
				&ifdata,             //   �]�Ƥ�����T
				pDetail,             //   �]�Ƥ����Ӹ`(�]�Ƹ��|)
				predictedLength,     //   ��X�w�İϤj�p
				&requiredLength,     //   ���ݭp���X�w�İϤj�p(�����γ]�w��)
				&spdd))               //
			{
				try
				{
					char ch[512];
					for(j=0;j<predictedLength;j++)
					{
					ch[j]=*(pDetail->DevicePath+8+j);
					}
					SS=ch;
					if(SS.LowerCase().Pos("vid_17e9"))
					{
						DEVINST DevInstChild = 0;
						CM_Get_Child(&DevInstChild, spdd.DevInst, 0);
						char szDeviceIdString[MAX_PATH];
						CM_Get_Device_IDA(DevInstChild, szDeviceIdString, MAX_PATH, 0);
						TListDisplayLink_Path->Add(AnsiString(szDeviceIdString));
					}
				}
				catch(...)
				{
					//DEBUG(SS+"�C�|����");
				}

			}
			else
			{
				//DEBUG("SetupDiGetInterfaceDeviceDetail F");
			}
			GlobalFree(pDetail);
		}
		else
		{
			//DEBUG("SetupDiEnumDeviceInterfaces F");
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
}
void TfrmMain::CheckDevData()
{
	pl_name2->Tag = WAIT;
	pl_name3->Tag = WAIT;
	pl_name4->Tag = WAIT;
	pl_name5->Tag = WAIT;
	plDisplayLinkCard1->Tag = WAIT;
	plDisplayLinkCard2->Tag = WAIT;
	int ScreenPASS = 0;
	int DisplayPASS= 0;
	//
	for(int i=0;i<TListInit_Screen->Count;i++)
	{
		for(int j=0;j<TListScreen_Dev->Count;j++)
		{
			if(TListScreen_Dev->Strings[j] == TListInit_Screen->Strings[i])
			{
				TListScreen_Dev->Delete(j);
				ScreenPASS++;
			}
		}
	}
	for(int i=0;i<TListInit_DisplayLink->Count;i++)
	{
		for(int j=0;j<TListDisplayLink_Dev->Count;j++)
		{
			if(TListDisplayLink_Dev->Strings[j] == TListInit_DisplayLink->Strings[i])
			{
				TListDisplayLink_Dev->Delete(j);
				DisplayPASS++;
				//((TPanel*)frmMain->FindComponent("plDisplayLinkCard"+AnsiString(i+1)))->Font->Color = clGreen;
			}
		}
	}
	//
	bool bPASS = true;
	bool bHAVE = false;
	if(ScreenPASS == TListInit_Screen->Count)
	{
		for(int i=0;i<TListInit_Screen->Count;i++)
		{
			((TPanel*)frmMain->FindComponent("pl_pels"+AnsiString(i+2)))->Font->Color = clGreen;
			((TPanel*)frmMain->FindComponent("pl_freq"+AnsiString(i+2)))->Font->Color = clGreen;
			((TPanel*)frmMain->FindComponent("pl_name"+AnsiString(i+2)))->Font->Color = clGreen;
			bHAVE = true;
		}
	}
	else
	{
		bPASS = false;
		for(int i=0;i<TListInit_Screen->Count;i++)
		{
			((TPanel*)frmMain->FindComponent("pl_pels"+AnsiString(i+2)))->Font->Color = clWindowText;
			((TPanel*)frmMain->FindComponent("pl_freq"+AnsiString(i+2)))->Font->Color = clWindowText;
			((TPanel*)frmMain->FindComponent("pl_name"+AnsiString(i+2)))->Font->Color = clWindowText;
		}
	}
	//
	if(DisplayPASS == TListInit_DisplayLink->Count)
	{
		for(int i=0;i<TListInit_DisplayLink->Count;i++)
		{
			((TPanel*)frmMain->FindComponent("plDisplayLinkCard"+AnsiString(i+1)))->Font->Color = clGreen;
			bHAVE = true;
		}
	}
	else
	{
		bPASS = false;
		for(int i=0;i<TListInit_DisplayLink->Count;i++)
		{
			((TPanel*)frmMain->FindComponent("plDisplayLinkCard"+AnsiString(i+1)))->Font->Color = clWindowText;
		}
	}
	if(!bHAVE) bPASS = false;
	if(TListDisplayLink_Dev->Count || TListScreen_Dev->Count)
		bPASS = false;
	plResult->Caption = bPASS ? "PASS":"WAIT";
	plResult->Color   = bPASS ? clLime:clCream;
	//
	frmMain->Refresh();
}



void __fastcall TfrmMain::btnResetClick(TObject *Sender)
{
	btnReset->Enabled = false;
	bStart = false;
	Delay(1000);
	TListInit_Screen->Clear();
	TListInit_DisplayLink->Clear();
	moDebug->Lines->Clear();
	moDebug->Lines->Add("[�ù������]");
	getMonitor(true);
	moDebug->Lines->Add("\r\n\r\n[DisplayLink�����]");
	getDisplayLinkCardName(true);
	bStart = true;
	btnReset->Enabled = true;
}
//---------------------------------------------------------------------------
void TfrmMain::Delay(DWORD iMilliSeconds) //
{
	DWORD iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
}
void __fastcall TfrmMain::plSwitchDebugClick(TObject *Sender)
{
	frmMain->AutoSize = false;
	if(plSwitchDebug->Caption.Pos(">"))
	{
		plSwitchDebug->Caption = "<";
		frmMain->Width = 847;
	}
	else
	{
		plSwitchDebug->Caption = ">";
		frmMain->Width = 602;
	}
	frmMain->AutoSize = true;
}
//---------------------------------------------------------------------------
void TfrmMain::setRegDriver(AnsiString DevVPID)
{
	TRegistry *reg = new TRegistry();
	reg->RootKey = HKEY_LOCAL_MACHINE;
	AnsiString temppath = "SYSTEM\\CurrentControlSet\\Control\\usbflags";
	reg->OpenKey(temppath.c_str(), false);
	AnsiString tempIgnoreHWSerNum = "IgnoreHWSerNum"+DevVPID.SubString(9,4)+DevVPID.SubString(18,4);
	if(!reg->ValueExists (tempIgnoreHWSerNum))
	{
		BYTE buf[1];
		buf[0] = 0x01;
		reg->WriteBinaryData(tempIgnoreHWSerNum,buf,1);
	}
	reg->CloseKey();
	delete reg;
}
void __fastcall TfrmMain::plResultDblClick(TObject *Sender)
{
	if(!moDevMessage->Height)
	{
		plResult->Align = alTop;
		plResult->Height = 44;
		moDevMessage->Align	= alClient;
		btnReset->Enabled = true;
	}
	else
	{
		plResult->Align = alClient;
		moDevMessage->Align	= alTop;
		moDevMessage->Height	= 0;
		btnReset->Enabled = false;
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::Panel2DblClick(TObject *Sender)
{
	DISPLAY_DEVICE DisplayDevice1;
	DISPLAY_DEVICE DisplayDevice2;
	DEVMODE lpDevMode1;
	DEVMODE lpDevMode2;

	memset(&DisplayDevice1, 0, sizeof(DisplayDevice1));
	memset(&DisplayDevice2, 0, sizeof(DisplayDevice2));

	ZeroMemory(&lpDevMode1, sizeof(DEVMODE));
	ZeroMemory(&lpDevMode2, sizeof(DEVMODE));

	DisplayDevice1.cb = sizeof(DisplayDevice1);
	DisplayDevice2.cb = sizeof(DisplayDevice2);

	EnumDisplayDevices(NULL, 0, &DisplayDevice1, 0);
	EnumDisplayDevices(NULL, 1, &DisplayDevice2, 0);

	EnumDisplaySettings(DisplayDevice1.DeviceName, ENUM_CURRENT_SETTINGS, &lpDevMode1);
	EnumDisplaySettings(DisplayDevice2.DeviceName, ENUM_CURRENT_SETTINGS, &lpDevMode2);

	lpDevMode1.dmSize = sizeof(DEVMODE);
	lpDevMode1.dmDriverExtra = 0;
	lpDevMode1.dmFields = DM_POSITION | DM_PELSHEIGHT | DM_PELSWIDTH;
	lpDevMode1.dmPelsWidth = 1366;
	lpDevMode1.dmPelsHeight = 768;

	lpDevMode2.dmSize = sizeof(DEVMODE);
	lpDevMode2.dmDriverExtra = 0;
	lpDevMode2.dmFields = DM_POSITION | DM_PELSHEIGHT | DM_PELSWIDTH;
	lpDevMode2.dmPelsWidth = 2560;
	lpDevMode2.dmPelsHeight = 1440;
	//lpDevMode2.dmPosition.x = 1366;
	//lpDevMode2.dmPosition.y

	ChangeDisplaySettingsEx (DisplayDevice1.DeviceName, &lpDevMode1, NULL, (CDS_UPDATEREGISTRY | CDS_NORESET), NULL);
	ChangeDisplaySettingsEx (DisplayDevice2.DeviceName, &lpDevMode2, NULL, (CDS_UPDATEREGISTRY | CDS_NORESET), NULL);
	ChangeDisplaySettingsEx (NULL, NULL, NULL, 0, NULL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Panel2Click(TObject *Sender)
{
	DEVMODEA devMode;
	LONG modeSwitch;
	LONG i;
	CHAR buf[256];
	i = 0;
	AnsiString sDevMode;
	do
	{
		modeSwitch = EnumDisplaySettingsA("\\\\.\\DISPLAY2", i, &devMode);
		i++;
		if(devMode.dmDisplayFrequency == 30 || devMode.dmDisplayFrequency == 60)
		{
		sDevMode.printf("%d x %d, %d",
		 devMode.dmPelsWidth, devMode.dmPelsHeight,
		 devMode.dmDisplayFrequency);
		//Memo1->Lines->Add(sDevMode);
		}
	}
	while(devMode.dmBitsPerPel!=16);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::pl_name1DblClick(TObject *Sender)
{
	TPanel * pl = (TPanel*) Sender;
	DWORD dwDisplayIndex = pl->Name.SubString(pl->Name.Length(),1).ToInt();
	plScreenSettingTitle->Tag = dwDisplayIndex;
	plScreenSettingTitle->Caption = "< Screen "+AnsiString(dwDisplayIndex)+"  : "+pl->Caption+" >";
	plSetDisplay->Left  = 145;
	plSetDisplay->Visible = true;
	EmnuDisplay(dwDisplayIndex);
	//SettingDisplayValue(dwDisplayIndex);
}
//---------------------------------------------------------------------------
void TfrmMain::SettingDisplayValue(DWORD dwDisplayIndex)
{
	plSetDisplay->Left  = 145;
	plSetDisplay->Visible = true;
	DEVMODEA devMode;
	LONG modeSwitch;
	LONG i;
	CHAR buf[256];
	i = 0;
	AnsiString sDevMode;
	lboxDisplaySetting->Items->Clear();
	AnsiString SS = "\\\\.\\DISPLAY"+AnsiString(dwDisplayIndex);
	while(EnumDisplaySettingsA(SS.c_str(), i, &devMode))
	{
		i++;
		if(devMode.dmBitsPerPel == 16)
			continue;
		if(devMode.dmDisplayFrequency == 30 || devMode.dmDisplayFrequency == 60)
		{
			//DWORD dw11 = devMode.dmDisplayFlags;
			if(devMode.dmPelsWidth >= 1000 && devMode.dmPelsHeight >= 1000)
				sDevMode.printf("%d x %d  -%dHz",devMode.dmPelsWidth, devMode.dmPelsHeight,devMode.dmDisplayFrequency);
			else if(devMode.dmPelsWidth >= 1000 && devMode.dmPelsHeight < 1000)
				sDevMode.printf("%d x   %d  -%dHz",devMode.dmPelsWidth, devMode.dmPelsHeight,devMode.dmDisplayFrequency);
			else if(devMode.dmPelsWidth < 1000 && devMode.dmPelsHeight >= 1000)
				sDevMode.printf("  %d x %d  -%dHz",devMode.dmPelsWidth, devMode.dmPelsHeight,devMode.dmDisplayFrequency);
			else if(devMode.dmPelsWidth < 1000 && devMode.dmPelsHeight < 1000)
				sDevMode.printf("  %d x   %d  -%dHz",devMode.dmPelsWidth, devMode.dmPelsHeight,devMode.dmDisplayFrequency);
			if(lboxDisplaySetting->Items->Count)
			{
				if(lboxDisplaySetting->Items->Strings[lboxDisplaySetting->Items->Count-1]
					!= sDevMode)
				lboxDisplaySetting->Items->Add(sDevMode);
			}
			else
				lboxDisplaySetting->Items->Add(sDevMode);
		}
	}

	TStringList* list = new TStringList;
	list->Text = lboxDisplaySetting->Items->Text;
	lboxDisplaySetting->Items->Clear();
	for(int i = list->Count-1 ; i > 0 ; i--)
		lboxDisplaySetting->Items->Add(list->Strings[i]);

	lboxDisplaySetting->ItemIndex = 0;
	if(list->Count)
		plSettingResolution->Caption = lboxDisplaySetting->Items->Strings[lboxDisplaySetting->ItemIndex];
	delete list;
}

void __fastcall TfrmMain::plSettingOkMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	TPanel* pl = (TPanel*)frmMain->FindComponent
		(((TPanel*)Sender)->Name + "Title");
	pl->Left++;
	pl->Top++;
	pl->Width--;
	pl->Height--;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::plSettingOkMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	TPanel* pl = (TPanel*)frmMain->FindComponent
		(((TPanel*)Sender)->Name + "Title");
	pl->Left--;
	pl->Top--;
	pl->Width++;
	pl->Height++;
}
//---------------------------------------------------------------------------



void __fastcall TfrmMain::plSettingOkClick(TObject *Sender)
{
	DWORD dwScreenIndex = plScreenSettingTitle->Tag-1;
	DWORD dwWidth,dwHeight,dwFreq;
	AnsiString SS = lboxDisplaySetting->Items->Strings[lboxDisplaySetting->ItemIndex];
	dwWidth = SS.SubString(1,SS.Pos("x")-1).Trim().ToInt();
	SS.Delete(1,SS.Pos("x"));
	SS = SS.Trim();
	dwHeight = SS.SubString(1,SS.Pos("-")-1).Trim().ToInt();
	SS.Delete(1,SS.Pos("-"));
	SS = SS.Trim();
	dwFreq = SS.SubString(1,SS.Pos("Hz")-1).Trim().ToInt();

	DISPLAY_DEVICEA DisplayDevice1;
	DEVMODEA lpDevMode1;

	memset(&DisplayDevice1, 0, sizeof(DisplayDevice1));

	ZeroMemory(&lpDevMode1, sizeof(DEVMODE));

	DisplayDevice1.cb = sizeof(DisplayDevice1);

	EnumDisplayDevicesA(NULL, dwScreenIndex, &DisplayDevice1, 0);
	TPanel* plName = (TPanel*)frmMain->FindComponent("pl_name"+AnsiString(plScreenSettingTitle->Tag));
	EnumDisplaySettingsA(AnsiString(plName->Hint).c_str(), ENUM_CURRENT_SETTINGS, &lpDevMode1);

	lpDevMode1.dmSize = sizeof(DEVMODE);
	//lpDevMode1.dmDriverExtra = 0;
	lpDevMode1.dmFields = DM_POSITION | DM_PELSHEIGHT | DM_PELSWIDTH | DM_DISPLAYFREQUENCY;
	lpDevMode1.dmPelsWidth = dwWidth;
	lpDevMode1.dmPelsHeight = dwHeight;
	lpDevMode1.dmDisplayFrequency = dwFreq;
	//DEBUG(AnsiString(plName->Hint).c_str());
	//DEBUG(dwWidth);
	//DEBUG(dwHeight);
	//DEBUG(dwFreq);
	DWORD dwResult = ChangeDisplaySettingsExA(AnsiString(plName->Hint).c_str(), &lpDevMode1, NULL, 0, NULL);
	switch(dwResult)
	{
		case DISP_CHANGE_SUCCESSFUL:
			plSetDisplay->Visible = false;
			DEBUG("DISP_CHANGE_SUCCESSFUL");
		break;
		case DISP_CHANGE_BADDUALVIEW:
			plSetDisplay->Visible = false;
			DEBUG("DISP_CHANGE_BADDUALVIEW");
		break;
		case DISP_CHANGE_BADFLAGS:
			plSetDisplay->Visible = false;
			DEBUG("DISP_CHANGE_BADFLAGS");
		break;
		case DISP_CHANGE_BADMODE:
			plSetDisplay->Visible = false;
			DEBUG("DISP_CHANGE_BADMODE");
		break;
		case DISP_CHANGE_BADPARAM:
			plSetDisplay->Visible = false;
			DEBUG("DISP_CHANGE_BADPARAM");
		break;
		case DISP_CHANGE_FAILED:
			plSetDisplay->Visible = false;
			DEBUG("DISP_CHANGE_FAILED");
		break;
		case DISP_CHANGE_NOTUPDATED:
			plSetDisplay->Visible = false;
			DEBUG("DISP_CHANGE_NOTUPDATED");
		break;
		case DISP_CHANGE_RESTART:
			plSetDisplay->Visible = false;
			DEBUG("DISP_CHANGE_RESTART");
		break;
    }
	plSetDisplay->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::plSettingCancelClick(TObject *Sender)
{
	plSetDisplay->Visible = false;
}
//---------------------------------------------------------------------------
void TfrmMain::EmnuDisplay(DWORD dwDisplayIndex)
{
	// �޼Ʃw�q
	IDXGIFactory * pFactory;
	IDXGIAdapter * pAdapter;
	std::vector <IDXGIAdapter*> vAdapters;            // ��ܥd
	AnsiString sDevMode;
	lboxDisplaySetting->Clear();

	// ��ܥd���ƶq
	int iAdapterNum = 0;


	// �إߤ@��DXGI�u�t
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));

	if (FAILED(hr))
		return ;

	// �C�|�����d
	while (pFactory->EnumAdapters(iAdapterNum, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		vAdapters.push_back(pAdapter);
		++iAdapterNum;
	}

	// ��T��X
	//Memo1->Lines->Add("===============�����" + AnsiString(iAdapterNum)+ "����ܥd===============" );
	//cout << "===============�����" << iAdapterNum << "����ܥd===============" << endl;
	for (size_t i = 0; i < vAdapters.size(); i++)
	{
		// �����T
		DXGI_ADAPTER_DESC adapterDesc;
		vAdapters[i]->GetDesc(&adapterDesc);
		wstring aa(adapterDesc.Description);
		/*std::string bb = WStringToString(aa);
		// ��X��ܥd��T
		cout << "�t�ε��T�O����:" << adapterDesc.DedicatedSystemMemory / 1024 / 1024 << "M" << endl;
		cout << "�M�ε��T�O����:" << adapterDesc.DedicatedVideoMemory / 1024 / 1024 << "M" << endl;
		cout << "�@�ɨt�ΰO����:" << adapterDesc.SharedSystemMemory / 1024 / 1024 << "M" << endl;
		cout << "�˸m�y�z:" << bb.c_str()<< endl;
		cout << "�˸mID:" << adapterDesc.DeviceId << endl;
		cout << "PCI ID�ץ�����:" << adapterDesc.Revision << endl;
		cout << "�l�t��PIC ID:" << adapterDesc.SubSysId << endl;
		cout << "�t�ӽs��:" << adapterDesc.VendorId << endl;*/

		// ��X�˸m
		IDXGIOutput * pOutput;
		std::vector<IDXGIOutput*> vOutputs;
		// ��X�˸m�ƶq
		int iOutputNum = 0;
		while (vAdapters[i]->EnumOutputs(iOutputNum, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			vOutputs.push_back(pOutput);
			iOutputNum++;
		}
		//cout << "�����" << iOutputNum << "����ܸ˸m:" << endl;
		for (size_t n = 0; n < vOutputs.size(); n++)
		{
			// �����ܸ˸m��T
			DXGI_OUTPUT_DESC outputDesc;
			vOutputs[n]->GetDesc(&outputDesc);

			// ����˸m�䴩
			UINT uModeNum = 0;
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
			UINT flags = DXGI_ENUM_MODES_INTERLACED;

			vOutputs[n]->GetDisplayModeList(format, flags, &uModeNum, 0);
			DXGI_MODE_DESC * pModeDescs = new DXGI_MODE_DESC[uModeNum];
			vOutputs[n]->GetDisplayModeList(format, flags, &uModeNum, pModeDescs);

			//Memo1->Lines->Add("�W��:" + AnsiString(outputDesc.DeviceName));
			TPanel* plName = ((TPanel*)frmMain->FindComponent("pl_name"+AnsiString(dwDisplayIndex)));
			//DEBUG(plName->Hint + "|" + AnsiString(outputDesc.DeviceName));
			if(plName->Hint == AnsiString(outputDesc.DeviceName))
			{
				//DEBUG("���\");
				sDevMode = AnsiString(outputDesc.DesktopCoordinates.right - outputDesc.DesktopCoordinates.left) + "x" + AnsiString(outputDesc.DesktopCoordinates.bottom - outputDesc.DesktopCoordinates.top);
				TPanel* plNowFeq = (TPanel*)frmMain->FindComponent("pl_freq"+AnsiString(dwDisplayIndex));
				TPanel* plNowPel = (TPanel*)frmMain->FindComponent("pl_pels"+AnsiString(dwDisplayIndex));
				sDevMode = plNowPel->Caption.LowerCase()+" -"+plNowFeq->Caption+"Hz";
				plSettingResolution->Caption = sDevMode;

				// �Ҥ䴩���ѪR�׸�T
				AnsiString buf;
				double dbFreq;
				DWORD  dwFreq;
				for (UINT m = 0; m < uModeNum; m++)
				{
					dbFreq = double(pModeDescs[m].RefreshRate.Numerator) / double(pModeDescs[m].RefreshRate.Denominator);
					sprintf(buf.c_str(),"%.0f",dbFreq);
					dwFreq = atoi(buf.c_str());

					if(pModeDescs[m].Width >= 1000 && pModeDescs[m].Height >= 1000)
						sDevMode.printf("%d x %d  -%dHz",pModeDescs[m].Width, pModeDescs[m].Height,dwFreq);
					else if(pModeDescs[m].Width >= 1000 && pModeDescs[m].Height < 1000)
						sDevMode.printf("%d x   %d  -%dHz",pModeDescs[m].Width, pModeDescs[m].Height,dwFreq);
					else if(pModeDescs[m].Width < 1000 && pModeDescs[m].Height >= 1000)
						sDevMode.printf("  %d x %d  -%dHz",pModeDescs[m].Width, pModeDescs[m].Height,dwFreq);
					else if(pModeDescs[m].Width < 1000 && pModeDescs[m].Height < 1000)
						sDevMode.printf("  %d x   %d  -%dHz",pModeDescs[m].Width, pModeDescs[m].Height,dwFreq);
					if(lboxDisplaySetting->Items->Count)
					{
						if(lboxDisplaySetting->Items->Strings[lboxDisplaySetting->Items->Count-1]
							!= sDevMode)
						lboxDisplaySetting->Items->Add(sDevMode);
					}
					else
						lboxDisplaySetting->Items->Add(sDevMode);
				}
				break;
			}
		}
		vOutputs.clear();

	}
	vAdapters.clear();

	TStringList* list = new TStringList;
	list->Text = lboxDisplaySetting->Items->Text;
	lboxDisplaySetting->Items->Clear();
	for(int i = list->Count-1 ; i > 0 ; i--)
		lboxDisplaySetting->Items->Add(list->Strings[i]);

	lboxDisplaySetting->ItemIndex = 0;
	delete list;

	return ;
}

void __fastcall TfrmMain::btnClearMsgClick(TObject *Sender)
{
	moDevMessage->Clear();
	dwDevMsgCount = 0;
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::btnPingClick(TObject *Sender)
{
	SetBatValue();
	WinExec("ping.bat", SW_SHOW);
	SetBatValue();
}
void TfrmMain::SetBatValue()
{
	AnsiString FILE_BAT_INI = "ping.bat";
	AnsiString SS = "ping -t "+edtIP->Text;
	fstream fp;
	fp.open(FILE_BAT_INI.c_str(), ios::out); // �}���ɮ�

	fp << "cd/" << endl;
	fp << SS.c_str() << endl;
	fp.close(); // �����ɮ�
}
bool TfrmMain::Ping(AnsiString IP,bool bLast)
{
   /*	szLanMsg = "";
   moDebug->Lines->Add("Pinging \""+IP+"\"...");
   int avg_time = 0;
   gPingTime = 0;
   try
   {
	  IdIcmpClient->Host=IP;
	  for(int i=0; i<3; i++)
      {
		 try
		 {
			AnsiString buff;
			IdIcmpClient->Ping(buff,0);
			Application->ProcessMessages();
		 }
         catch(Exception &e)
		 {
			szLanMsg = "PING ����(PING FAIL)";
			moDebug->Lines->Add("Request timed out.");
		 }
      }
   }
   __finally
   {
	  avg_time = gPingTime/3;
	  moDebug->Lines->Add(avg_time);
   }
   moDebug->Lines->Add("End");
   if(szLanMsg == "")
   {
	   if(avg_time>10)
	   {
			szLanMsg = "���t���F��(SpeedOfPingFail)";
	   }
	   else
	   {
			if(szLanMsg=="")
				return true;
	   }
   }
   if(bLast)
		frmMain->ERROR_MSG = szLanMsg; */
   return false;

}
void __fastcall TfrmMain::IdIcmpClientReply(TComponent *ASender, const TReplyStatus *AReplyStatus)

{
   /*AnsiString szReport;
   if(AReplyStatus->FromIpAddress=="0.0.0.0" && AReplyStatus->TimeToLive==0)
   {
		szLanMsg = "PING����(PING-FAIL)";
		szReport="Request timed out.";
   }
   else
   {
	  if(AReplyStatus->MsRoundTripTime==0)
	  {
		 szReport.sprintf("Reply from %s: bytes=%d time<1ms TTL=%d",
			AReplyStatus->FromIpAddress, AReplyStatus->BytesReceived,
			AReplyStatus->TimeToLive);
	  }
	  else
	  {
		 szReport.sprintf("Reply from %s: bytes=%d time=%dms TTL=%d",
			AReplyStatus->FromIpAddress, AReplyStatus->BytesReceived,
			AReplyStatus->MsRoundTripTime, AReplyStatus->TimeToLive);
	  }
	  gPingTime+= AReplyStatus->MsRoundTripTime;
   }
   moDebug->Lines->Add(szReport); */
}
//---------------------------------------------------------------------------
