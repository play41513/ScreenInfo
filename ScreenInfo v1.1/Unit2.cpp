//---------------------------------------------------------------------------
#include <windows.h>    // 安全移除USB裝置用 *要比 vcl.h 早編譯
#include <SetupAPI.h> // 安全移除USB裝置用 *要比 vcl.h 早編譯
#include <cfgmgr32.h> // 安全移除USB裝置用 *要比 vcl.h 早編譯
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>		//檔案輸出入用
#include <fstream>
#include <iostream>
#include <string>
#include "Unit2.h"
//---------------------------------------------------------------------------
using namespace std;
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
#include <windows.h>
bool bStart = false;

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
	moDebug->Lines->Add("[螢幕比對資料]");
	getMonitor(true);
	moDebug->Lines->Add("\r\n\r\n[DisplayLink比對資料]");
	getDisplayLinkCardName(true);
	bStart = true;
}
//---------------------------------------------------------------------------
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
	DISPLAY_DEVICE dd;
	TDeviceModeA lpDevMode;
	dd.cb = sizeof(dd);
	DWORD dev = 0;
	int MONITORAMOUNT = 0;
	while (EnumDisplayDevices(0, dev, &dd, 0))
	{
		DISPLAY_DEVICE ddMon;
		ZeroMemory(&ddMon, sizeof(ddMon));
		ddMon.cb = sizeof(ddMon);
		DWORD devMon = 0;
		while (EnumDisplayDevices(dd.DeviceName, devMon, &ddMon, 0))
		{
			DeviceDetail = "";
			if (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE &&
			!(ddMon.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
			{
				DeviceID = ddMon.DeviceID;
				DeviceID = DeviceID.SubString(9,DeviceID.Pos("\\")-1);
				EnumDisplaySettings(dd.DeviceName,ENUM_CURRENT_SETTINGS,&lpDevMode);
				if(MONITORAMOUNT>0)
					((TPanel*)frmMain->FindComponent("plSetFreq"+AnsiString(MONITORAMOUNT+1)))->Visible = true;
				((TPanel*)frmMain->FindComponent("pl_pels"+AnsiString(MONITORAMOUNT+1)))->Caption
					= (AnsiString)lpDevMode.dmPelsWidth + " X "+ (AnsiString)lpDevMode.dmPelsHeight;
				((TPanel*)frmMain->FindComponent("pl_freq"+AnsiString(MONITORAMOUNT+1)))->Caption
					= lpDevMode.dmDisplayFrequency;
				((TPanel*)frmMain->FindComponent("pl_name"+AnsiString(MONITORAMOUNT+1)))->Caption
					= DeviceID;

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
		if(i>0)
			((TPanel*)frmMain->FindComponent("plSetFreq"+AnsiString(i+1)))->Visible = false;
		((TPanel*)frmMain->FindComponent("pl_pels"+AnsiString(i+1)))->Caption
			= "";
		((TPanel*)frmMain->FindComponent("pl_freq"+AnsiString(i+1)))->Caption
			= "";
		((TPanel*)frmMain->FindComponent("pl_name"+AnsiString(i+1)))->Caption
			= "";
	}
	return MONITORAMOUNT;
}
void __fastcall TfrmMain::btn_freq1_30Click(TObject *Sender)
{
	TButton* btn = (TButton*)Sender;
	changefreq(btn->Hint.ToInt(),btn->Caption.ToInt());
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::changefreq(int Devicenum, int freq){
	TDeviceModeA lpDevMode2;
	AnsiString displayname = "\\\\.\\DISPLAY" + IntToStr(Devicenum);
	AnsiString msg = "此裝置不支援"+IntToStr(freq)+"赫茲";
	EnumDisplaySettings(displayname.c_str(),ENUM_CURRENT_SETTINGS,&lpDevMode2);
	lpDevMode2.dmDisplayFrequency = freq;
	if(ChangeDisplaySettingsEx(displayname.c_str(), &lpDevMode2, NULL, 0, NULL)==DISP_CHANGE_BADMODE){
		MessageBox(0,msg.c_str(),"提示",MB_OK);
	}
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
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, temppath.c_str(), 0, KEY_READ,&hSoftKey) == ERROR_SUCCESS)
		{
			DWORD dwType=REG_SZ;
			DWORD dwCount=0;
			LONG lResult = RegQueryValueEx(hSoftKey, "FriendlyName", NULL, &dwType,NULL, &dwCount);
			if (lResult == ERROR_SUCCESS)
			{
				LPBYTE company_Get=new BYTE [dwCount];
				lResult = RegQueryValueEx(hSoftKey, "FriendlyName", NULL, &dwType,
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
	DEBUG("[ USB裝置列舉 ]");
	//--------------------------------------------------------------------------
	//   獲取設備資訊
	hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_USB,
	0,   //   Enumerator
	0,
	DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
	//--------------------------------------------------------------------------
	SP_DEVICE_INTERFACE_DATA   ifdata;
	SP_DEVINFO_DATA            spdd;
	DeviceInfoData.cbSize   =   sizeof(SP_DEVINFO_DATA);
	for (i=0;SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData);i++)	//   枚舉每個USB設備
	{
		ifdata.cbSize   =   sizeof(ifdata);
		if (SetupDiEnumDeviceInterfaces(								//   枚舉符合該GUID的設備介面
		hDevInfo,           //   設備資訊集控制碼
		NULL,                         //   不需額外的設備描述
		(LPGUID)&GUID_USB,//GUID_CLASS_USB_DEVICE,                     //   GUID
		(ULONG)i,       //   設備資訊集裏的設備序號
		&ifdata))                 //   設備介面資訊
		{
			ULONG predictedLength   =   0;
			ULONG requiredLength   =   0;
			//   取得該設備介面的細節(設備路徑)
			SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   設備資訊集控制碼
				&ifdata,          //   設備介面資訊
				NULL,             //   設備介面細節(設備路徑)
				0,         	      //   輸出緩衝區大小
				&requiredLength,  //   不需計算輸出緩衝區大小(直接用設定值)
				NULL);            //   不需額外的設備描述
			//   取得該設備介面的細節(設備路徑)
			predictedLength=requiredLength;
			pDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT,   predictedLength);
			pDetail->cbSize   =   sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			ZeroMemory(&spdd, sizeof(spdd));
			spdd.cbSize = sizeof(spdd);

			if(SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   設備資訊集控制碼
				&ifdata,             //   設備介面資訊
				pDetail,             //   設備介面細節(設備路徑)
				predictedLength,     //   輸出緩衝區大小
				&requiredLength,     //   不需計算輸出緩衝區大小(直接用設定值)
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
					DEBUG(SS+"列舉失敗");
				}

			}
			else
			{
				DEBUG("SetupDiGetInterfaceDeviceDetail F");
			}
			GlobalFree(pDetail);
		}
		else
		{
			DEBUG("SetupDiEnumDeviceInterfaces F");
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
	moDebug->Lines->Add("[螢幕比對資料]");
	getMonitor(true);
	moDebug->Lines->Add("\r\n\r\n[DisplayLink比對資料]");
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
	if(!moDebug->Height)
	{
		plResult->Align = alTop;
		plResult->Height = 100;
		moDebug->Align	= alClient;
		btnReset->Enabled = true;
	}
	else
	{
		plResult->Align = alClient;
		moDebug->Align	= alTop;
		moDebug->Height	= 0;
		btnReset->Enabled = false;
    }
}
//---------------------------------------------------------------------------


