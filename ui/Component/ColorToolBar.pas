unit ColorToolBar;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, Vcl.ToolWin, Winapi.CommCtrl,
    Vcl.Menus, Winapi.GDIPAPI, Winapi.GDIPOBJ, Vcl.ImgList, Vcl.StdCtrls;

const
    COLOR_TOOLBAR_WIDTH  = 50;
    COLOR_TOOLBAR_HEIGHT = 23;

const
    CM_COLOR_TOOLBAR_SETCOLOR = WM_USER + 101;

type
    TColorToolBar      = class;
    TColorToolDropdown = procedure(Sender: TColorToolBar) of object;
    TColorToolChanged  = procedure(Sender: TColorToolBar; Color: DWORD) of object;

    TColorToolBar = class(TWinControl)
    private
        { Private declarations }
        FOnDropdown    : TColorToolDropdown;
        FOnColorChanged: TColorToolChanged;
        FSelectedColor : DWORD;
        FDropdown      : Boolean;
        procedure SetSelectedColor(Value: DWORD);
    protected
        { Protected declarations }
        procedure OnSetColor(var Message: TMessage); message CM_COLOR_TOOLBAR_SETCOLOR;
        procedure CNNotify(var Message: TWMNotifyTLB); message CN_NOTIFY;
        procedure CustomDraw(dc: HDC);
        procedure CustomDrawButton(dc: HDC);
        procedure CreateWnd; override;
    public
        { Public declarations }
        constructor Create(AOwner: TComponent); override;
        procedure CreateParams(var Params: TCreateParams); override;
    published
        { Published declarations }
        property OnDropdown    : TColorToolDropdown read FOnDropdown write FOnDropdown;
        property OnColorChanged: TColorToolChanged read FOnColorChanged write FOnColorChanged;
        property SelectedColor : DWORD read FSelectedColor Write SetSelectedColor;
    end;

procedure Register;

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TColorToolBar]);
end;

constructor TColorToolBar.Create(AOwner: TComponent);
begin
    inherited Create(AOwner);
    Width := COLOR_TOOLBAR_WIDTH + 25;
    Height := COLOR_TOOLBAR_HEIGHT;
end;

procedure TColorToolBar.CreateParams(var Params: TCreateParams);
begin
    Vcl.ComCtrls.InitCommonControl(ICC_BAR_CLASSES);
    inherited CreateParams(Params);
    CreateSubClass(Params, TOOLBARCLASSNAME);
    Params.Style := WS_CHILD or WS_VISIBLE or
      TBSTYLE_FLAT or
      CCS_NOPARENTALIGN or CCS_NOMOVEY or CCS_NORESIZE or CCS_NODIVIDER;
    Params.ExStyle := 0;
    Params.WindowClass.Style := 0;
end;

procedure TColorToolBar.CreateWnd;
var
    ButtonInfo: TTBBUTTON;
    bResult   : LRESULT;
    Size      : TSIZE;
begin
    inherited CreateWnd;
    SendMessage(Handle, TB_BUTTONSTRUCTSIZE, sizeof(TTBBUTTON), 0);
    bResult := SendMessage(Handle, TB_SETBITMAPSIZE, 0, 0);
    bResult := SendMessage(Handle, TB_SETBUTTONSIZE, 0, MakeLParam(COLOR_TOOLBAR_WIDTH, COLOR_TOOLBAR_HEIGHT));
    bResult := SendMessage(Handle, TB_SETINDENT, 0, 0);
    FillChar(ButtonInfo, sizeof(ButtonInfo), 0);
    ButtonInfo.fsState := TBSTATE_ENABLED;
    ButtonInfo.fsStyle := BTNS_WHOLEDROPDOWN;
    bResult := SendStructMessage(Handle, TB_INSERTBUTTON, 0, ButtonInfo);
    bResult := SendStructMessage(Handle, TB_GETMAXSIZE, 0, Size);
    SetWindowPos(Handle, 0, 0, 0, Size.cx, Size.cy, SWP_NOZORDER or SWP_NOMOVE);
end;

procedure TColorToolBar.CNNotify(var Message: TWMNotifyTLB);
var
    TBCustomDraw: PNMTBCustomDraw;
begin
    if Message.NMHdr.code = TBN_DROPDOWN then
    begin
        if Assigned(FOnDropdown) then
        begin
            if FDropdown = FALSE then
            begin
                FDropdown := TRUE;
                FOnDropdown(Self);
                FDropdown := FALSE;
            end;
        end;
    end else if Message.NMHdr.code = NM_CUSTOMDRAW then
    begin
        Message.Result := CDRF_DODEFAULT;
        TBCustomDraw := Message.NMTBCustomDraw;
        case TBCustomDraw.nmcd.dwDrawStage of
            CDDS_PREPAINT:
                begin
                    Message.Result := CDRF_NOTIFYITEMDRAW;
                    CustomDraw(TBCustomDraw.nmcd.HDC);
                end;
            CDDS_ITEMPREPAINT:
                begin
                    Message.Result := CDRF_NOTIFYPOSTPAINT;
                end;
            CDDS_ITEMPOSTPAINT:
                begin
                    if TBCustomDraw.nmcd.dwItemSpec <> 0 then Exit;
                    CustomDrawButton(TBCustomDraw.nmcd.HDC);
                end;
        end;
        Exit;
    end;
    inherited;
end;

procedure TColorToolBar.SetSelectedColor(Value: DWORD);
var
    dc: HDC;
begin
    if FSelectedColor = Value then Exit;
    FSelectedColor := Value;
    dc := GetDC(Self.Handle);
    CustomDrawButton(dc);
    ReleaseDC(Self.Handle, dc);
end;

procedure TColorToolBar.OnSetColor(var Message: TMessage);
begin
    SetSelectedColor(Message.WParam);
    if Assigned(FOnColorChanged) then
    begin
        FOnColorChanged(Self, FSelectedColor);
    end;
end;

procedure TColorToolBar.CustomDraw(dc: HDC);
const
    Color1 = $FFF0F0F0;
    Color2 = $FFC0C0C0;
var
    Graphics           : TGPGraphics;
    LinearGradientBrush: TGPLinearGradientBrush;
    GradientRect       : TGPRect;
    ARect              : TRect;
begin
    Winapi.Windows.GetClientRect(Handle, ARect);
    GradientRect.X := ARect.Left;
    GradientRect.Y := ARect.Top;
    GradientRect.Width := ARect.Width;
    GradientRect.Height := ARect.Height - 1;
    LinearGradientBrush := TGPLinearGradientBrush.Create(GradientRect, Color1, Color2, LinearGradientModeVertical);
    Graphics := TGPGraphics.Create(dc);
    Graphics.FillRectangle(LinearGradientBrush, GradientRect);
    LinearGradientBrush.Free();
    Graphics.Free();
end;

procedure TColorToolBar.CustomDrawButton(dc: HDC);
const
    BORDER_COLOR = $FF828790;
var
    Graphics  : TGPGraphics;
    SolidBrush: TGPSolidBrush;
    Pen       : TGPPen;
    ButtonRect: TGPRect;
    ARect     : TRect;
begin
    SendStructMessage(Handle, TB_GETRECT, 0, ARect);
    ButtonRect.X := ARect.Left + 5;
    ButtonRect.Y := ARect.Top + 5;
    ButtonRect.Width := ARect.Width - 23;
    ButtonRect.Height := ARect.Height - 11;
    SolidBrush := TGPSolidBrush.Create(FSelectedColor or $FF000000);
    Pen := TGPPen.Create(BORDER_COLOR, 1);
    Graphics := TGPGraphics.Create(dc);
    Graphics.FillRectangle(SolidBrush, ButtonRect);
    Graphics.DrawRectangle(Pen, ButtonRect);
    Pen.Free();
    SolidBrush.Free();
    Graphics.Free();
end;

end.
