Unit ImageListView;

Interface

Uses
	System.SysUtils, System.Classes, Winapi.Windows, Vcl.Controls, Vcl.ComCtrls, Winapi.CommCtrl, Vcl.ImgList,
	Winapi.Messages, Winapi.GDIPAPI, Winapi.GDIPOBJ, Winapi.UxTheme;

Type
	TILVItemSelected     = Procedure(Sender: TObject; Index: Integer) Of Object;
	TILVItemActivated    = Procedure(Sender: TObject; Index: Integer) Of Object;
	TILVDraw             = Procedure(Sender: TObject; Item: PNMCustomDraw) Of Object;
	TILVDrawItem         = Procedure(Sender: TObject; Item: PNMCustomDraw) Of Object;
	TILVShowMenu         = Procedure(Sender: TObject; Index: Integer) Of Object;
	TILVItemCountChanged = Procedure(Sender: TObject; Count: Integer) Of Object;

	TImageListView = Class(TWinControl)
	Private
		{ Private declarations }
		FImgList           : HIMAGELIST;
		FItemHeight        : Integer;
		FOnDraw            : TILVDraw;
		FOnDrawItem        : TILVDrawItem;
		FOnItemSelected    : TILVItemSelected;
		FOnItemActivated   : TILVItemActivated;
		FOnShowMenu        : TILVShowMenu;
		FOnItemCountChanged: TILVItemCountChanged;
		FCount             : Integer;
		FLastSelected      : Integer;
		FCustomDrawItem    : Boolean;
		Procedure SetItemHeight(Height: Integer);
	Protected
		{ Protected declarations }
		Procedure CNNotify(Var Message: TWMNotifyLV); Message CN_NOTIFY;
		Procedure CreateParams(Var Params: TCreateParams); Override;
		Procedure CreateWnd; Override;
	Public
		{ Public declarations }
		Constructor Create(AOwner: TComponent); Override;
		Destructor Destroy; Override;

	Public
		Function GetItemRect(Index: Integer): TRect;
		Procedure SetItemCount(Count: Integer);
		Procedure DeleteItem(Index: Integer);
		Procedure SelectItem(Index: Integer);
		Function GetSelected(): Integer;
		Procedure RedrawItem(Index: Integer);
		Procedure RedrawItemEx(iFirst, iLast: Integer);
		Function GetItemState(Index: Integer): UINT;

	Published
		{ Published declarations }
		Property Align;
		Property CustomDrawItem    : Boolean Read FCustomDrawItem Write FCustomDrawItem Default FALSE;
		Property ItemHeight        : Integer Read FItemHeight Write SetItemHeight Default 32;
		Property OnDraw            : TILVDraw Read FOnDraw Write FOnDraw;
		Property OnDrawItem        : TILVDrawItem Read FOnDrawItem Write FOnDrawItem;
		Property OnItemSelected    : TILVItemSelected Read FOnItemSelected Write FOnItemSelected;
		Property OnItemActivated   : TILVItemActivated Read FOnItemActivated Write FOnItemActivated;
		Property OnShowMenu        : TILVShowMenu Read FOnShowMenu Write FOnShowMenu;
		Property OnItemCountChanged: TILVItemCountChanged Read FOnItemCountChanged Write FOnItemCountChanged;
	Public
		Property ItemIndex               : Integer Read GetSelected Write SelectItem;
		Property Count                   : Integer Read FCount Write SetItemCount;
		Property ItemRect[Index: Integer]: TRect Read GetItemRect;
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TImageListView]);
End;

Constructor TImageListView.Create(AOwner: TComponent);
Begin
	Inherited;
	FLastSelected := -1;
	ControlStyle := ControlStyle - [csCaptureMouse] + [csDisplayDragImage, csAcceptsControls, csReflector, csPannable];
	If FItemHeight = 0 Then FItemHeight := 32;
	FImgList := ImageList_Create(1, FItemHeight, ILC_COLOR32, 0, 1);
	DoubleBuffered := TRUE;
	TabStop := TRUE;
End;

Destructor TImageListView.Destroy();
Begin
	ImageList_Destroy(FImgList);
	Inherited;
End;

Function TImageListView.GetItemRect(Index: Integer): TRect;
Begin
	ListView_GetItemRect(Handle, Index, Result, LVIR_BOUNDS);
End;

Function TImageListView.GetSelected: Integer;
Begin
	Result := FLastSelected;
End;

Procedure TImageListView.CreateParams(Var Params: TCreateParams);
Begin
	InitCommonControl(ICC_LISTVIEW_CLASSES);
	Inherited CreateParams(Params);
	CreateSubClass(Params, WC_LISTVIEW);
	Params.Style := WS_CHILD Or WS_TABSTOP Or WS_CLIPCHILDREN Or WS_CLIPSIBLINGS Or WS_VSCROLL Or LVS_SHAREIMAGELISTS Or LVS_REPORT Or LVS_SINGLESEL Or LVS_SHOWSELALWAYS Or LVS_NOCOLUMNHEADER Or LVS_NOSORTHEADER Or LVS_OWNERDATA;
	Params.ExStyle := Params.ExStyle Or WS_EX_CLIENTEDGE;
	Params.WindowClass.Style := Params.WindowClass.Style And Not(CS_HREDRAW Or CS_VREDRAW);
End;

Procedure TImageListView.CreateWnd();
Var
	Styles: DWORD;
	Col   : TLVColumn;
Begin
	Inherited;
	Styles := LVS_EX_DOUBLEBUFFER Or LVS_EX_FULLROWSELECT Or LVS_EX_AUTOSIZECOLUMNS Or LVS_EX_BORDERSELECT;
	ListView_SetExtendedListViewStyle(Handle, Styles);
	ListView_SetImageList(Handle, FImgList, LVSIL_SMALL);
	SetWindowTheme(Handle, 'explorer', Nil);

	FillChar(Col, sizeof(Col), 0);
	Col.mask := LVCF_FMT Or LVCF_WIDTH;
	Col.fmt := LVCFMT_FILL Or LVCFMT_NO_TITLE;
	Col.cx := Width - 21;
	ListView_InsertColumn(Handle, 0, Col);

	ListView_SetItemCount(Handle, FCount);
	If FLastSelected <> -1 Then
	Begin
		ListView_SetItemState(Handle, FLastSelected, LVIS_SELECTED, LVIS_SELECTED);
	End;
End;

Procedure TImageListView.CNNotify(Var Message: TWMNotifyLV);
Var
	lpnmitem: PNMITEMACTIVATE;
Begin
	Case Message.NMHdr.code Of
		NM_CUSTOMDRAW:
			Begin
				Message.Result := CDRF_DODEFAULT;
				If (Message.NMCustomDraw.dwDrawStage = CDDS_PREPAINT) Then
				Begin
					If Assigned(FOnDraw) Then
					Begin
						FOnDraw(Self, Message.NMCustomDraw);
					End;
					Message.Result := CDRF_NOTIFYITEMDRAW;
				End Else If (Message.NMCustomDraw.dwDrawStage = CDDS_ITEMPREPAINT) Then
				Begin
					If (FCustomDrawItem) And Assigned(FOnDrawItem) Then
					Begin
						FOnDrawItem(Self, Message.NMCustomDraw);
						Message.Result := CDRF_SKIPDEFAULT;
					End Else Begin
						Message.Result := CDRF_NOTIFYPOSTPAINT;
					End;
				End Else If Message.NMCustomDraw.dwDrawStage = CDDS_ITEMPOSTPAINT Then
				Begin
					If (FCustomDrawItem = FALSE) And Assigned(FOnDrawItem) Then
					Begin
						FOnDrawItem(Self, Message.NMCustomDraw);
					End;
				End;
			End;

		LVN_ITEMCHANGED:
			If (Message.NMListView.uChanged = LVIF_STATE) Then
			Begin
				If (Message.NMListView.uOldState And LVIS_SELECTED = 0) And
				  (Message.NMListView.uNewState And LVIS_SELECTED <> 0) And
				  (FLastSelected <> Message.NMListView.iItem)
				Then
				Begin
					FLastSelected := Message.NMListView.iItem;
					If Assigned(FOnItemSelected) Then FOnItemSelected(Self, Message.NMListView.iItem);
				End;
			End;

		LVN_ITEMACTIVATE:
			Begin
				lpnmitem := PNMITEMACTIVATE(Message.NMListView);
				If Assigned(FOnItemActivated) And (lpnmitem.iItem <> -1) Then
				Begin
					FOnItemActivated(Self, lpnmitem.iItem);
				End;
				Message.Result := 0;
			End;

		NM_CLICK:
			Begin
				lpnmitem := PNMITEMACTIVATE(Message.NMListView);
				If (lpnmitem.iItem = -1) And (FLastSelected <> -1) Then
				Begin
					ListView_SetItemState(Handle, FLastSelected, LVIS_SELECTED, LVIS_SELECTED);
				End;
				Message.Result := 0;
			End;

		NM_RCLICK:
			Begin
				lpnmitem := PNMITEMACTIVATE(Message.NMListView);
				If (lpnmitem.iItem = -1) And (FLastSelected <> -1) Then
				Begin
					ListView_SetItemState(Handle, FLastSelected, LVIS_SELECTED, LVIS_SELECTED);
				End;
				If Assigned(FOnShowMenu) Then
				Begin
					FOnShowMenu(Self, lpnmitem.iItem);
				End;
				Message.Result := 0;
			End;

		NM_DBLCLK, NM_RDBLCLK:
			Begin
				lpnmitem := PNMITEMACTIVATE(Message.NMListView);
				If (lpnmitem.iItem = -1) And (FLastSelected <> -1) Then
				Begin
					ListView_SetItemState(Handle, FLastSelected, LVIS_SELECTED, LVIS_SELECTED);
				End;
				Message.Result := 0;
			End;
	End;
End;

Procedure TImageListView.SetItemCount(Count: Integer);
Var
	Reselected: Integer;
Begin
	FCount := Count;
	Reselected := FLastSelected;
	If FLastSelected >= FCount Then
	Begin
		FLastSelected := FCount - 1;
	End;
	ListView_SetItemCount(Handle, FCount);
	If (FCount > 0) And (Reselected <> FLastSelected) Then
	Begin
		SelectItem(FLastSelected);
	End;
	If Assigned(FOnItemCountChanged) Then
	Begin
		FOnItemCountChanged(Self, FCount);
	End;
End;

Procedure TImageListView.SetItemHeight(Height: Integer);
Begin
	If Height <> FItemHeight Then
	Begin
		FItemHeight := Height;
		ImageList_Destroy(FImgList);
		FImgList := ImageList_Create(1, FItemHeight, ILC_COLOR32, 0, 1);
		If HandleAllocated Then
		Begin
			ListView_SetImageList(Handle, FImgList, LVSIL_SMALL);
		End;
	End;
End;

Procedure TImageListView.DeleteItem(Index: Integer);
Begin
	If Index < FCount Then
	Begin
		ListView_DeleteItem(Handle, Index);
		FCount := FCount - 1;
		If FCount > 0 Then
		Begin
			If FLastSelected >= FCount Then
			Begin
				SelectItem(FCount - 1);
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
		If Assigned(FOnItemCountChanged) Then
		Begin
			FOnItemCountChanged(Self, FCount);
		End;
	End Else Begin
		Raise Exception.Create('Error Message');
	End;
End;

Procedure TImageListView.SelectItem(Index: Integer);
Begin
	FLastSelected := Index;
	ListView_SetItemState(Handle, Index, LVIS_SELECTED, LVIS_SELECTED);
	ListView_EnsureVisible(Handle, Index, FALSE);
	If Assigned(FOnItemSelected) Then
	Begin
		FOnItemSelected(Self, Index);
	End;
End;

Procedure TImageListView.RedrawItem(Index: Integer);
Begin
	ListView_RedrawItems(Handle, Index, Index);
End;

Procedure TImageListView.RedrawItemEx(iFirst, iLast: Integer);
Begin
	ListView_RedrawItems(Handle, iFirst, iLast);
End;

Function TImageListView.GetItemState(Index: Integer): UINT;
Begin
	Result := ListView_GetItemState(Handle, Index, LVIS_SELECTED Or LVIS_FOCUSED);
End;

End.
