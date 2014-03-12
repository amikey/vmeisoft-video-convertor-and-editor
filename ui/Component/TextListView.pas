Unit TextListView;

Interface

Uses
	System.SysUtils, System.Classes, Winapi.Windows, Vcl.Controls, Vcl.ComCtrls, Winapi.CommCtrl, Vcl.ImgList,
	Winapi.Messages, Winapi.GDIPAPI, Winapi.GDIPOBJ, Winapi.UxTheme, Vcl.StdCtrls, Vcl.Graphics;

Type
	TTLVRecreateItem          = Procedure(Sender: TObject) Of Object;
	TTLVItemSelected          = Procedure(Sender: TObject; Index: Integer) Of Object;
	TTLVItemActivated         = Procedure(Sender: TObject; Index: Integer) Of Object;
	TTLVShowMenu              = Procedure(Sender: TObject; Index: Integer) Of Object;
	TTLVItemCheckStateChanged = Procedure(Sender: TObject; Index: Integer; fCheck: Boolean) Of Object;
	TTLVGetHeaderText         = Function(Sender: TObject; Index: Integer): String Of Object;

	TTextListViewView     = Class;
	TTextListViewHreader  = Class;
	TTextListViewCheckBox = Class;

	TTextListView = Class(TWinControl)
	Private
		FList        : TTextListViewView;
		FHeader      : TTextListViewHreader;
		FCheckBox    : TTextListViewCheckBox;
		FLastSelected: Integer;
		FLocked      : Boolean;
	Private
		FOnRecreateItem         : TTLVRecreateItem;
		FOnItemSelected         : TTLVItemSelected;
		FOnItemActivated        : TTLVItemActivated;
		FOnShowMenu             : TTLVShowMenu;
		FOnItemCheckStateChanged: TTLVItemCheckStateChanged;
		FOnGetHeaderText        : TTLVGetHeaderText;
	Private
		Procedure SelectItem(Index: Integer);
		Procedure RecheckState();
		Procedure OnCheckBoxClick(fChecked: Boolean);

	Protected
		Procedure CreateParams(Var Params: TCreateParams); Override;
		Procedure CreateWnd; Override;
		Procedure WMSize(Var Message: TWMSize); Message WM_SIZE;

	Public
		Constructor Create(AOwner: TComponent); Override;

	Public
		Procedure DeleteItem(Index: Integer);
		Procedure AddItem(Str1, Str2: String; Checked: Boolean);
		Procedure ChangeItem(Index: Integer; Str1, Str2: String; Checked: Boolean);
		Procedure SetItemSetCheckState(Index: Integer; fCheck: Boolean);
		Function GetItemSetCheckState(Index: Integer): Boolean;
		Function GetCount(): Integer;

	Published
		Property Align;
		Property Font;
		Property OnRecreateItem         : TTLVRecreateItem Read FOnRecreateItem Write FOnRecreateItem;
		Property OnItemSelected         : TTLVItemSelected Read FOnItemSelected Write FOnItemSelected;
		Property OnItemActivated        : TTLVItemActivated Read FOnItemActivated Write FOnItemActivated;
		Property OnShowMenu             : TTLVShowMenu Read FOnShowMenu Write FOnShowMenu;
		Property OnItemCheckStateChanged: TTLVItemCheckStateChanged Read FOnItemCheckStateChanged Write FOnItemCheckStateChanged;
		Property OnGetHeaderText        : TTLVGetHeaderText Read FOnGetHeaderText Write FOnGetHeaderText;

	Public
		Property ItemIndex: Integer Read FLastSelected Write SelectItem;

	End;

	TTextListViewHreader = Class(TWinControl)
	Private
		{ Private declarations }
		FTextListView: TTextListView;
	Protected
		{ Protected declarations }
		Procedure CreateParams(Var Params: TCreateParams); Override;
		Procedure CreateWnd; Override;
	Public
		{ Public declarations }
		Constructor Create(AOwner: TTextListView);
	End;

	TTextListViewView = Class(TWinControl)
	Private
		{ Private declarations }
		FTextListView: TTextListView;
	Protected
		{ Protected declarations }
		Procedure CNNotify(Var Message: TWMNotifyLV); Message CN_NOTIFY;
		Procedure CreateParams(Var Params: TCreateParams); Override;
		Procedure CreateWnd; Override;
	Public
		{ Public declarations }
		Constructor Create(AOwner: TTextListView);
	End;

	TTextListViewCheckBox = Class(TWinControl)
	Private
		{ Private declarations }
		FTextListView: TTextListView;
		FState       : Integer;
	Protected
		{ Protected declarations }
		Procedure CreateParams(Var Params: TCreateParams); Override;
		Procedure CreateWnd; Override;
		Procedure CNCommand(Var Message: TWMCommand); Message CN_COMMAND;
	Public
		{ Public declarations }
		Constructor Create(AOwner: TTextListView);
		Procedure SetState(State: Integer);
	End;

Procedure Register;

Implementation

Const
	HEADER_HEIGHT = 22;
	HEADER1_WIDTH = 23;

Procedure Register;
Begin
	RegisterComponents('Samples', [TTextListView]);
End;

procedure TTextListView.ChangeItem(Index: Integer; Str1, Str2: String;  Checked: Boolean);
begin
 	ListView_SetItemText(FList.Handle, Index, 1, PWideChar(Str1));
	ListView_SetItemText(FList.Handle, Index, 2, PWideChar(Str2));
end;

Constructor TTextListView.Create(AOwner: TComponent);
Begin
	Inherited;
	InitCommonControl(ICC_LISTVIEW_CLASSES);
	FLastSelected := -1;
	DoubleBuffered := TRUE;
	FHeader := TTextListViewHreader.Create(Self);
	FList := TTextListViewView.Create(Self);
	FCheckBox := TTextListViewCheckBox.Create(Self);
	FHeader.Parent := Self;
	FList.Parent := Self;
	FCheckBox.Parent := FHeader;
	FHeader.Visible := TRUE;
	FList.Visible := TRUE;
	FCheckBox.Visible := TRUE;
	FCheckBox.Enabled := FALSE;
	TabStop := FALSE;
End;

Procedure TTextListView.CreateParams(Var Params: TCreateParams);
Begin
	Inherited;
	Params.Style := WS_CHILD Or WS_CLIPCHILDREN Or WS_CLIPSIBLINGS;
	Params.ExStyle := WS_EX_CLIENTEDGE;
	Params.WindowClass.Style := 0;
End;

Procedure TTextListView.CreateWnd;
Begin
	Inherited;
	SetWindowTheme(Handle, 'explorer', Nil);
End;

Procedure TTextListView.DeleteItem(Index: Integer);
Var
	Count: Integer;
Begin
	Count := GetCount();
	If Index < Count Then
	Begin
		ListView_DeleteItem(FList.Handle, Index);
		Count := Count - 1;
		If Count > 0 Then
		Begin
			If FLastSelected >= Count Then
			Begin
				SelectItem(Count - 1);
			End Else Begin
				SelectItem(FLastSelected);
			End;
		End Else Begin
			FLastSelected := -1;
			If Assigned(FOnItemSelected) Then
			Begin
				FOnItemSelected(Self, -1);
			End;
		End;
	End Else Begin
		Raise Exception.Create('Error Message');
	End;
	If GetCount() > 0 Then
	Begin
		FCheckBox.Enabled := TRUE;
		RecheckState();
	End Else Begin
		FCheckBox.Enabled := FALSE;
	End;
End;

Procedure TTextListView.AddItem(Str1, Str2: String; Checked: Boolean);
Var
	Item: TLVItem;
Begin
	FLocked := TRUE;
	FillChar(Item, sizeof(Item), 0);
	Item.iItem := GetCount();
	ListView_InsertItem(FList.Handle, Item);
	ListView_SetItemText(FList.Handle, Item.iItem, 1, PWideChar(Str1));
	ListView_SetItemText(FList.Handle, Item.iItem, 2, PWideChar(Str2));
	If (Checked) Then ListView_SetCheckState(FList.Handle, Item.iItem, TRUE);
	FCheckBox.Enabled := TRUE;
	RecheckState();
	FLocked := FALSE;
End;

Procedure TTextListView.SetItemSetCheckState(Index: Integer; fCheck: Boolean);
Begin
	ListView_SetCheckState(FList.Handle, Index, fCheck);
	RecheckState();
End;

Procedure TTextListView.WMSize(Var Message: TWMSize);
Var
	i  : Integer;
	Col: TLVColumn;
	Hdr: THDItem;
Begin
	Inherited;
	FHeader.SetBounds(0, 0, ClientWidth, HEADER_HEIGHT);
	FList.SetBounds(0, HEADER_HEIGHT, ClientWidth, ClientHeight - HEADER_HEIGHT);
	FCheckBox.SetBounds(2, 3, 16, 16);

	FillChar(Col, sizeof(Col), 0);
	FillChar(Hdr, sizeof(Hdr), 0);
	For i := 0 To 2 Do
	Begin
		Hdr.mask := HDI_WIDTH;
		Col.mask := LVCF_WIDTH;
		Col.iSubItem := i;
		Case i Of
			0:
				Begin
					Hdr.cxy := HEADER1_WIDTH;
					Col.cx := HEADER1_WIDTH;
				End;
			1:
				Begin
					Hdr.cxy := (ClientWidth - HEADER1_WIDTH - 18) Div 2;
					Col.cx := Hdr.cxy;
				End;
			2:
				Begin
					Hdr.cxy := ClientWidth - ((ClientWidth - HEADER1_WIDTH - 20) Div 2) - HEADER1_WIDTH - 18;
					Col.cx := Hdr.cxy;
				End;
		End;
		Header_SetItem(FHeader.Handle, i, Hdr);
		ListView_SetColumn(FList.Handle, i, Col);
	End;
End;

Function TTextListView.GetCount: Integer;
Begin
	Result := ListView_GetItemCount(FList.Handle);
End;

Function TTextListView.GetItemSetCheckState(Index: Integer): Boolean;
Begin
	Result := ListView_GetCheckState(FList.Handle, Index) <> 0;
End;

Procedure TTextListView.SelectItem(Index: Integer);
Begin
	FLastSelected := Index;
	ListView_SetItemState(FList.Handle, Index, LVIS_SELECTED, LVIS_SELECTED);
	ListView_EnsureVisible(FList.Handle, Index, FALSE);
	If Assigned(FOnItemSelected) Then
	Begin
		FOnItemSelected(Self, Index);
	End;
End;

Procedure TTextListView.RecheckState();
Var
	i        : Integer;
	Checked  : Integer;
	Unchecked: Integer;
	Item     : THDItem;
	Count    : Integer;
Begin
	Count := GetCount();
	Checked := 0;
	Unchecked := 0;
	For i := 0 To Count - 1 Do
	Begin
		If ListView_GetCheckState(FList.Handle, i) = 0 Then
		Begin
			Unchecked := Unchecked + 1;
		End Else Begin
			Checked := Checked + 1;
		End;
	End;
	Item.mask := HDI_FORMAT;
	If Count = Checked Then
	Begin
		FCheckBox.SetState(BST_CHECKED);
	End Else If Unchecked = Count Then
	Begin
		FCheckBox.SetState(BST_UNCHECKED);
	End Else Begin
		FCheckBox.SetState(BST_INDETERMINATE);
	End;
End;

Procedure TTextListView.OnCheckBoxClick(fChecked: Boolean);
Var
	i: Integer;
Begin
	FLocked := TRUE;
	For i := 0 To GetCount() Do
	Begin
		ListView_SetCheckState(FList.Handle, i, fChecked);
	End;
	FLocked := FALSE;
End;

{ TTextHreader }
Constructor TTextListViewHreader.Create(AOwner: TTextListView);
Begin
	Inherited Create(AOwner);
	FTextListView := AOwner;
	TabStop := FALSE;
End;

Procedure TTextListViewHreader.CreateParams(Var Params: TCreateParams);
Begin
	Inherited CreateParams(Params);
	CreateSubClass(Params, WC_HEADER);
	Params.Style := WS_CHILD Or WS_CLIPCHILDREN Or WS_CLIPSIBLINGS Or HDS_NOSIZING;
	Params.WindowClass.Style := 0;
End;

Procedure TTextListViewHreader.CreateWnd;
Var
	i   : Integer;
	Item: THDItem;
Begin
	Inherited;
	SetWindowTheme(Handle, 'explorer', Nil);

	FillChar(Item, sizeof(Item), 0);
	For i := 0 To 2 Do
	Begin
		If i = 0 Then
		Begin
			Item.mask := HDI_WIDTH;
			Item.cxy := HEADER1_WIDTH;
		End Else Begin
			Item.mask := HDI_TEXT;
			If Assigned(FTextListView.FOnGetHeaderText) Then
			Begin
				Text := FTextListView.FOnGetHeaderText(FTextListView, i);
			End Else Begin
				Text := Format('Column %d', [i]);
			End;
			Item.pszText := PWideChar(Text);
		End;
		Header_InsertItem(Handle, i, Item);
	End;
End;

{ TTextList }

Procedure TTextListViewView.CNNotify(Var Message: TWMNotifyLV);
Var
	lpnmitem: PNMITEMACTIVATE;
Begin
	Case Message.NMHdr.code Of
		NM_CUSTOMDRAW:
			Begin
				Message.Result := CDRF_DODEFAULT;
				If (Message.NMCustomDraw.dwDrawStage = CDDS_PREPAINT) Then
				Begin
					Message.Result := CDRF_NOTIFYITEMDRAW;
				End Else If (Message.NMCustomDraw.dwDrawStage = CDDS_ITEMPREPAINT) Then
				Begin
					If ListView_GetCheckState(Handle, Message.NMCustomDraw.dwItemSpec) = 0 Then
					Begin
						Message.NMLVCustomDraw.clrText := $808080;
						Message.Result := CDRF_NEWFONT;
					End;
				End;
			End;

		LVN_ITEMCHANGED:
			If (FTextListView.FLocked = FALSE) And (Message.NMListView.uChanged = LVIF_STATE) Then
			Begin
				If (Message.NMListView.uOldState And LVIS_SELECTED = 0) And
				  (Message.NMListView.uNewState And LVIS_SELECTED <> 0) And
				  (FTextListView.FLastSelected <> Message.NMListView.iItem)
				Then
				Begin
					FTextListView.FLastSelected := Message.NMListView.iItem;
					If Assigned(FTextListView.FOnItemSelected) Then FTextListView.FOnItemSelected(Self, Message.NMListView.iItem);
				End;
				If (Message.NMListView.uOldState And LVIS_STATEIMAGEMASK) <> (Message.NMListView.uNewState And LVIS_STATEIMAGEMASK) Then
				Begin
					FTextListView.RecheckState();
					If Assigned(FTextListView.FOnItemCheckStateChanged) Then FTextListView.FOnItemCheckStateChanged(Self, Message.NMListView.iItem, (Message.NMListView.uNewState And LVIS_STATEIMAGEMASK) = $2000);
				End;
			End;

		LVN_ITEMACTIVATE:
			Begin
				lpnmitem := PNMITEMACTIVATE(Message.NMListView);
				If Assigned(FTextListView.FOnItemActivated) And (lpnmitem.iItem <> -1) Then
				Begin
					FTextListView.FOnItemActivated(Self, lpnmitem.iItem);
				End;
				Message.Result := 0;
			End;

		NM_CLICK:
			Begin
				lpnmitem := PNMITEMACTIVATE(Message.NMListView);
				If (lpnmitem.iItem = -1) And (FTextListView.FLastSelected <> -1) Then
				Begin
					ListView_SetItemState(Handle, FTextListView.FLastSelected, LVIS_SELECTED, LVIS_SELECTED);
				End;
				Message.Result := 0;
			End;

		NM_RCLICK:
			Begin
				lpnmitem := PNMITEMACTIVATE(Message.NMListView);
				If (lpnmitem.iItem = -1) And (FTextListView.FLastSelected <> -1) Then
				Begin
					ListView_SetItemState(Handle, FTextListView.FLastSelected, LVIS_SELECTED, LVIS_SELECTED);
				End;
				If Assigned(FTextListView.FOnShowMenu) Then
				Begin
					FTextListView.FOnShowMenu(Self, lpnmitem.iItem);
				End;
				Message.Result := 0;
			End;

		NM_DBLCLK, NM_RDBLCLK:
			Begin
				lpnmitem := PNMITEMACTIVATE(Message.NMListView);
				If (lpnmitem.iItem = -1) And (FTextListView.FLastSelected <> -1) Then
				Begin
					ListView_SetItemState(Handle, FTextListView.FLastSelected, LVIS_SELECTED, LVIS_SELECTED);
				End;
				Message.Result := 0;
			End;
	End;
End;

Constructor TTextListViewView.Create(AOwner: TTextListView);
Begin
	Inherited Create(AOwner);
	ControlStyle := ControlStyle - [csCaptureMouse] + [csReflector, csPannable];
	FTextListView := AOwner;
	TabStop := TRUE;
End;

Procedure TTextListViewView.CreateParams(Var Params: TCreateParams);
Begin
	Inherited CreateParams(Params);
	CreateSubClass(Params, WC_LISTVIEW);
	Params.Style := WS_CHILD Or WS_TABSTOP Or WS_CLIPCHILDREN Or WS_CLIPSIBLINGS Or WS_VSCROLL Or LVS_SHAREIMAGELISTS Or LVS_REPORT Or LVS_SINGLESEL Or LVS_SHOWSELALWAYS Or LVS_NOSORTHEADER Or LVS_NOCOLUMNHEADER;
	Params.WindowClass.Style := Params.WindowClass.Style And Not(CS_HREDRAW Or CS_VREDRAW);
End;

Procedure TTextListViewView.CreateWnd();
Var
	Styles: DWORD;
	Col   : TLVColumn;
	i     : Integer;
	Text  : String;
Begin
	Inherited;
	Styles := LVS_EX_DOUBLEBUFFER Or LVS_EX_FULLROWSELECT Or LVS_EX_CHECKBOXES Or LVS_EX_BORDERSELECT;
	ListView_SetExtendedListViewStyle(Handle, Styles);
	SetWindowTheme(Handle, 'explorer', Nil);

	FillChar(Col, sizeof(Col), 0);
	For i := 0 To 2 Do
	Begin
		Col.iSubItem := i;
		Col.mask := LVCF_WIDTH;
		If i = 0 Then
		Begin
			Col.cx := 21;
		End Else Begin
			Col.cx := (Width - 21 - 21) Div 2;
		End;
		ListView_InsertColumn(Handle, i, Col);
	End;

	If Assigned(FTextListView.FOnRecreateItem) Then
	Begin
		FTextListView.FOnRecreateItem(Self);
	End;

	If FTextListView.FLastSelected <> -1 Then
	Begin
		ListView_SetItemState(Handle, FTextListView.FLastSelected, LVIS_SELECTED, LVIS_SELECTED);
	End;

End;

{ TTextListViewCheckBox }

Procedure TTextListViewCheckBox.CNCommand(Var Message: TWMCommand);
Begin
	If Message.NotifyCode = BN_CLICKED Then
	Begin
		If SendMessage(Handle, BM_GETCHECK, 0, 0) = BST_CHECKED Then
		Begin
			FState := BST_UNCHECKED;
			SendMessage(Handle, BM_SETCHECK, BST_UNCHECKED, 0);
			FTextListView.OnCheckBoxClick(FALSE);
		End Else Begin
			FState := BST_CHECKED;
			SendMessage(Handle, BM_SETCHECK, BST_CHECKED, 0);
			FTextListView.OnCheckBoxClick(TRUE);
		End;
	End;
End;

Constructor TTextListViewCheckBox.Create(AOwner: TTextListView);
Begin
	Inherited Create(AOwner);
	FTextListView := AOwner;
	TabStop := FALSE;
End;

Procedure TTextListViewCheckBox.CreateParams(Var Params: TCreateParams);
Begin
	Inherited;
	CreateSubClass(Params, WC_BUTTON);
	Params.Style := WS_CHILD Or BS_3STATE;
	If Self.Enabled = FALSE Then Params.Style := Params.Style Or WS_DISABLED;
	Params.ExStyle := 0;
	Params.WindowClass.Style := 0;
End;

Procedure TTextListViewCheckBox.CreateWnd;
Begin
	Inherited;
	SendMessage(Handle, BM_SETCHECK, FState, 0);
End;

Procedure TTextListViewCheckBox.SetState(State: Integer);
Begin
	FState := State;
	SendMessage(Handle, BM_SETCHECK, State, 0);
End;

End.
