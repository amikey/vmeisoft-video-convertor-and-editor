Unit SysLinkEx;

Interface

Uses
	Winapi.Messages, Winapi.Windows, System.SysUtils, System.Classes, System.Contnrs, System.Types, System.UITypes,
	Vcl.Controls, Vcl.Forms, Vcl.Menus, Vcl.Graphics, Vcl.StdCtrls, Vcl.GraphUtil, Winapi.ShellAPI, Winapi.CommCtrl,
	Vcl.ExtCtrls;

Type
	TSysLinkEventEx = Procedure(Sender: TObject; Link: PNMLink) Of Object;

	TSysLinkEx = Class(TWinControl)
	Private
		FAutoSize      : Boolean;
		FAutoHeight    : Boolean;
		FUseVisualStyle: Boolean;
		FOnLinkClick   : TSysLinkEventEx;
		Procedure AdjustBounds;
		Procedure CMFontChanged(Var Message: TMessage); Message CM_FONTCHANGED;
		Procedure CMTextChanged(Var Message: TMessage); Message CM_TEXTCHANGED;
		Procedure CMNotify(Var Message: TWMNotify); Message CN_NOTIFY;

	Protected
		Procedure CreateParams(Var Params: TCreateParams); Override;
		Procedure CreateWnd; Override;
		Procedure SetAutoSize(Value: Boolean); Override;
		Procedure SetAutoHeight(Value: Boolean);

	Public
		Constructor Create(AOwner: TComponent); Override;

	Published
		Property AutoSize   : Boolean Read FAutoSize Write SetAutoSize Default False;
		Property AutoHeiht  : Boolean Read FAutoHeight Write SetAutoHeight Default False;
		Property OnLinkClick: TSysLinkEventEx Read FOnLinkClick Write FOnLinkClick;

	Published
		Property Caption;
		Property Visible;
		Property Color Nodefault;
		Property Enabled;
		Property Font;
		Property ParentColor;
		Property ParentFont;
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TSysLinkEx]);
End;

Constructor TSysLinkEx.Create(AOwner: TComponent);
Begin
	Inherited;
	FAutoSize := True;
	FUseVisualStyle := False;
	ParentDoubleBuffered := False;
	Width := 53;
	Height := 17;
End;

Procedure TSysLinkEx.AdjustBounds;
Var
	TextSize: TSize;
Begin
	If HandleAllocated Then
	Begin
		If FAutoSize Then
		Begin
			SendGetStructMessage(Handle, LM_GETIDEALSIZE, MaxInt, TextSize);
			SetBounds(Left, Top, TextSize.cx + (GetSystemMetrics(SM_CXBORDER) * 4), TextSize.cy + (GetSystemMetrics(SM_CYBORDER) * 4));
		End Else If FAutoSize Then
		Begin
			Height := SendMessage(Handle, LM_GETIDEALHEIGHT, Width, 0) + GetSystemMetrics(SM_CXBORDER) * 4;
		End;
	End;
End;

Procedure TSysLinkEx.CreateParams(Var Params: TCreateParams);
Begin
	Inherited CreateParams(Params);
	CreateSubClass(Params, WC_LINK);
	Params.Style := Params.Style Or LWS_USEVISUALSTYLE Or LWS_TRANSPARENT;
	Params.WindowClass.Style := Params.WindowClass.Style And (Not CS_VREDRAW);
End;

Procedure TSysLinkEx.CreateWnd;
Begin
	Inherited;
	AdjustBounds;
End;

Procedure TSysLinkEx.CMFontChanged(Var Message: TMessage);
Begin
	Inherited;
	AdjustBounds;
End;

Procedure TSysLinkEx.CMTextChanged(Var Message: TMessage);
Begin
	Inherited;
	AdjustBounds;
	Invalidate;
End;

Procedure TSysLinkEx.CMNotify(Var Message: TWMNotify);
Var
	Item: TLItem;
Begin
	Case Message.NMHdr.code Of
		NM_CLICK, NM_RETURN:
			Begin
				If Assigned(FOnLinkClick) Then
					  FOnLinkClick(Self, PNMLink(Message.NMHdr));
			End;
	End;
End;

Procedure TSysLinkEx.SetAutoHeight(Value: Boolean);
Begin
	If FAutoHeight <> Value Then
	Begin
		FAutoHeight := Value;
		If Value Then AdjustBounds;
	End;
End;

Procedure TSysLinkEx.SetAutoSize(Value: Boolean);
Begin
	If FAutoSize <> Value Then
	Begin
		FAutoSize := Value;
		If Value Then AdjustBounds;
	End;
End;

End.
