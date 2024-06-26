//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Registry.hpp>
#include <pngimage.hpp>
#include <Buttons.hpp>
const char GUID_USB_HUB[] = "{F18A0E88-C30C-11D0-8815-00A0C906BED8}";
const char GUID_USB_DEVICE[] = "{A5DCBF10-6530-11D2-901F-00C04FB951ED}";
const char GUID_HID[] = "{4d1e55b2-f16f-11cf-88cb-001111000030}";
const char GUID_LAN[] = "{D35F7840-6A0C-11d2-B841-00C04FAD5171}";
const char GUID_CD[]  = "{53F56308-B6BF-11D0-94F2-00A0C91EFB8B}";

#define WAIT 0
#define PASS 1
#define DEBUG(String)    //moDebug->Lines->Add(String)
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TTimer *Timer1;
	TPanel *plDPI1;
	TPanel *pl_name1;
	TPanel *pl_freq1;
	TPanel *pl_pels1;
	TPanel *Panel14;
	TPanel *Panel15;
	TPanel *Panel16;
	TPanel *Panel17;
	TPanel *plDPI4;
	TPanel *pl_name4;
	TPanel *pl_freq4;
	TPanel *pl_pels4;
	TPanel *plDPI3;
	TPanel *pl_name3;
	TPanel *pl_freq3;
	TPanel *pl_pels3;
	TPanel *plDPI2;
	TPanel *pl_name2;
	TPanel *pl_freq2;
	TPanel *pl_pels2;
	TPanel *Panel4;
	TButton *btn_freq2_30;
	TButton *btn_freq2_60;
	TPanel *Panel3;
	TPanel *plDisplayLinkCard1;
	TPanel *plDisplayLinkCard2;
	TPanel *plDPI;
	TPanel *plDisplay;
	TPanel *Panel1;
	TPanel *Panel2;
	TPanel *Panel5;
	TPanel *plSwitchDebug;
	TPanel *Panel6;
	TMemo *moDebug;
	TBitBtn *btnReset;
	TPanel *plSetFreq1;
	TButton *btn_freq1_30;
	TButton *btn_freq1_60;
	TPanel *plSetFreq2;
	TPanel *plSetFreq3;
	TButton *btn_freq3_30;
	TButton *btn_freq3_60;
	TPanel *plSetFreq4;
	TButton *btn_freq4_30;
	TButton *btn_freq4_60;
	TPanel *Panel7;
	TImage *Image1;
	TPanel *Panel8;
	TPanel *plResult;
	TPanel *Panel9;
	TPanel *pl_name5;
	TPanel *pl_freq5;
	TPanel *pl_pels5;
	TPanel *plSetFreq5;
	TButton *btn_freq5_30;
	TButton *btn_freq5_60;
	void __fastcall Timer1Timer(TObject *Sender);
	int __fastcall getMonitor(bool bInit);
	void __fastcall btn_freq1_30Click(TObject *Sender);
	void __fastcall changefreq(int Devicenum, int freq);
	void __fastcall btnResetClick(TObject *Sender);
	void __fastcall plSwitchDebugClick(TObject *Sender);
	void __fastcall plResultDblClick(TObject *Sender);

private:	// User declarations
	TStringList *TListDisplayLink_Dev,*TListInit_Screen,*TListInit_DisplayLink,*TListScreen_Dev,*TListDisplayLink_Path;

	void TfrmMain::getDisplayLinkCardName(bool bInit);
	void TfrmMain::EnumUSB();
	void TfrmMain::CheckDevData();
	void Delay(DWORD iMilliSeconds);
	void setRegDriver(AnsiString DevVPID);

public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
