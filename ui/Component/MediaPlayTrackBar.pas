unit MediaPlayTrackBar;

interface

uses
    System.SysUtils, System.Classes, System.Types, Vcl.Controls, Vcl.ComCtrls, Vcl.Themes,
    Winapi.Messages, Winapi.Windows, Winapi.CommCtrl;

type
    TMediaPlayTrackBar = class(TWinControl)
    private
        { Private declarations }
        FOnMouseDragBegin: TNotifyEvent;
        FOnMouseDragEnd  : TNotifyEvent;
        FMouseDrag       : Boolean;
        FPosition        : Integer;
        FMin             : Integer;
        FMax             : Integer;
        FFrequency       : Integer;
        FOnChange        : TNotifyEvent;
        FShowSelRange    : Boolean;
        FSelStart        : Integer;
        FSelEnd          : Integer;
        FShowTick        : Boolean;

        procedure SetMin(Value: Integer);
        procedure SetMax(Value: Integer);
        procedure SetPosition(Value: Integer);
        procedure SetShowSelRange(const Value: Boolean);
        procedure SetShowTick(const Value: Boolean);
        procedure UpdateSelection();

    protected
        procedure CreateParams(var Params: TCreateParams); override;
        procedure CreateWnd; override;

    protected
        { Protected declarations }
        procedure WMLButtonDown(var Message: TWMLButtonDown); message WM_LBUTTONDOWN;
        procedure WMLButtonUp(var Message: TWMLButtonUp); message WM_LBUTTONUP;
        procedure WMCaptureChanged(var Message: TMessage); message WM_CAPTURECHANGED;
        procedure CNHScroll(var Message: TWMHScroll); message CN_HSCROLL;
        procedure CNVScroll(var Message: TWMVScroll); message CN_VSCROLL;

    public
        constructor Create(AOwner: TComponent); override;

    public
        { Public declarations }
        property Min: Integer read FMax write SetMin;
        property Max: Integer read FMax write SetMax;
        procedure SetSelection(SelStart, SelEnd: Integer);

    published
        { Published declarations }
        property OnMouseDragBegin: TNotifyEvent read FOnMouseDragBegin write FOnMouseDragBegin;
        property OnMouseDragEnd  : TNotifyEvent read FOnMouseDragEnd write FOnMouseDragEnd;
        property OnChange        : TNotifyEvent read FOnChange write FOnChange;
        property ShowSelRange    : Boolean read FShowSelRange write SetShowSelRange default True;
        property ShowTick        : Boolean read FShowTick write SetShowTick default True;
        property Position        : Integer read FPosition write SetPosition default 0;

    published
        property SelStart: Integer read FSelStart;
        property SelEnd  : Integer read FSelEnd;

    private
        procedure WMEraseBkGnd(var Message: TWMEraseBkGnd); message WM_ERASEBKGND;
        procedure CNNotify(var Message: TWMNotifyTRB); message CN_NOTIFY;

    published
        property Align;
        property Anchors;
        property BorderWidth;
        property Ctl3D;
        property DoubleBuffered;
        property DragCursor;
        property DragKind;
        property DragMode;
        property Enabled;
        property Constraints;
        property ParentCtl3D;
        property ParentDoubleBuffered;
        property ParentShowHint;
        property Hint;
        property ShowHint;
        property TabOrder;
        property TabStop default True;
        property Visible;
    end;

procedure Register;

const
    MediaPlayTrackBarHint =
      '-->: Forward one second;'#13#10 +
      '<--: Backward one second;'#13#10 +
      'Page Down: Forward 10 seconds;'#13#10 +
      'Page Up: Backward 10 seconds';

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TMediaPlayTrackBar]);
end;

constructor TMediaPlayTrackBar.Create(AOwner: TComponent);
begin
    inherited Create(AOwner);
    Width := 150;
    Height := 45;
    TabStop := True;
    FMin := 0;
    FMax := 10;
    FPosition := 0;
    FFrequency := 1;
    FSelStart := 0;
    FSelEnd := 0;
    ControlStyle := ControlStyle - [csDoubleClicks, csGestures];
    FShowTick := True;
    FShowSelRange := True;
end;

procedure TMediaPlayTrackBar.CreateParams(var Params: TCreateParams);
begin
    InitCommonControl(ICC_BAR_CLASSES);
    inherited CreateParams(Params);
    CreateSubClass(Params, TRACKBAR_CLASS);
    Params.Style := Params.Style or TBS_BOTH or TBS_HORZ or TBS_FIXEDLENGTH;
    if FShowSelRange then Params.Style := Params.Style or TBS_ENABLESELRANGE;
    if FShowTick then
    begin
        Params.Style := Params.Style or TBS_AUTOTICKS;
    end else begin
        Params.Style := Params.Style or TBS_NOTICKS;
    end;
    Params.WindowClass.Style := Params.WindowClass.Style and not(CS_HREDRAW or CS_VREDRAW or CS_DBLCLKS);
end;

procedure TMediaPlayTrackBar.CreateWnd;
begin
    inherited CreateWnd;
    if HandleAllocated then
    begin
        SendMessage(Handle, TBM_SETTHUMBLENGTH, 20, 0);
        SendMessage(Handle, TBM_SETLINESIZE, 0, 1);
        SendMessage(Handle, TBM_SETPAGESIZE, 0, 10);
        SendMessage(Handle, TBM_SETRANGEMIN, 1, FMin);
        SendMessage(Handle, TBM_SETRANGEMAX, 1, FMax);
        if FShowSelRange then UpdateSelection();
        SendMessage(Handle, TBM_SETPOS, 1, FPosition);
        SendMessage(Handle, TBM_SETPAGESIZE, 0, 10);
        SendMessage(Handle, TBM_SETTICFREQ, FFrequency, 1);
    end;
end;

procedure TMediaPlayTrackBar.WMLButtonDown(var Message: TWMLButtonDown);
begin
    inherited;
    if (FMouseDrag = FALSE) and (Assigned(FOnMouseDragBegin)) then
    begin
        FMouseDrag := True;
        FOnMouseDragBegin(Self);
    end;
end;

procedure TMediaPlayTrackBar.SetShowSelRange(const Value: Boolean);
begin
    if Value <> FShowSelRange then
    begin
        FShowSelRange := Value;
        RecreateWnd;
    end;
end;

procedure TMediaPlayTrackBar.UpdateSelection();
begin
    if HandleAllocated then
    begin
        if (FSelStart = 0) and (FSelEnd = 0) then
        begin
            SendMessage(Handle, TBM_CLEARSEL, 1, 0)
        end else begin
            SendMessage(Handle, TBM_SETSELSTART, WPARAM(FALSE), FSelStart);
            SendMessage(Handle, TBM_SETSELEND, WPARAM(True), FSelEnd);
        end;
    end;
end;

procedure TMediaPlayTrackBar.SetSelection(SelStart, SelEnd: Integer);
begin
    if (SelStart <> FSelStart) or (SelEnd <> FSelEnd) then
    begin
        FSelStart := SelStart;
        FSelEnd := SelEnd;
        UpdateSelection();
    end;
end;

procedure TMediaPlayTrackBar.SetShowTick(const Value: Boolean);
begin
    if Value <> FShowTick then
    begin
        FShowTick := Value;
        RecreateWnd;
    end;
end;

procedure TMediaPlayTrackBar.SetPosition(Value: Integer);
Begin
    if Value <> FPosition then
    begin
        FPosition := Value;
        if HandleAllocated then SendMessage(Handle, TBM_SETPOS, 1, Value);
    end;
End;

procedure TMediaPlayTrackBar.SetMin(Value: Integer);
begin
    if Value <> FMin then
    begin
        FMin := Value;
        SendMessage(Handle, TBM_SETRANGEMIN, 1, Value);
    end;
end;

procedure TMediaPlayTrackBar.SetMax(Value: Integer);
begin
    if Value <> FMax then
    begin
        FMax := Value;
        if FMax < 10 then
        begin
            FFrequency := 1;
        end else if FMax < 100 then
        begin
            FFrequency := 5;
        end else if FMax < 200 then
        begin
            FFrequency := 10;
        end else if FMax < 1200 then
        begin
            FFrequency := 60;
        end else if FMax < 3600 then
        begin
            FFrequency := 300;
        end else
        begin
            FFrequency := 600;
        end;
        if HandleAllocated then
        begin
            SendMessage(Handle, TBM_SETTICFREQ, FFrequency, 1);
            SendMessage(Handle, TBM_SETRANGEMAX, 1, Value);
        end;
    end;
END;

procedure TMediaPlayTrackBar.WMLButtonUp(var Message: TWMLButtonUp);
begin
    inherited;
    if (FMouseDrag) and (Assigned(FOnMouseDragEnd)) then
    begin
        FMouseDrag := FALSE;
        FOnMouseDragEnd(Self);
    end;
end;

procedure TMediaPlayTrackBar.WMCaptureChanged(var Message: TMessage);
begin
    inherited;
    if (FMouseDrag) and (Assigned(FOnMouseDragEnd)) then
    begin
        FMouseDrag := FALSE;
        FOnMouseDragEnd(Self);
    end;
end;

procedure TMediaPlayTrackBar.CNHScroll(var Message: TWMHScroll);
var
    Position: Integer;
begin
    Message.Result := 0;
    Position := SendMessage(Handle, TBM_GETPOS, 0, 0);
    if FPosition = Position then Exit;
    FPosition := Position;
    if FMouseDrag then
    begin
        if Assigned(FOnChange) then
        begin
            FOnChange(Self);
        end;
    end else begin
        if Assigned(FOnMouseDragBegin) then
        begin
            FOnMouseDragBegin(Self);
        end;
        if Assigned(FOnChange) then
        begin
            FOnChange(Self);
        end;
        if Assigned(FOnMouseDragEnd) then
        begin
            FOnMouseDragEnd(Self);
        end;
    end;
end;

procedure TMediaPlayTrackBar.CNVScroll(var Message: TWMVScroll);
begin
    Message.Result := 0;
end;

procedure TMediaPlayTrackBar.WMEraseBkGnd(var Message: TWMEraseBkGnd);
var
    R: TRect;
begin
    if StyleServices.Enabled then
    begin
        R := ClientRect;
        if Focused and ((Perform(WM_QUERYUISTATE, 0, 0) and UISF_HIDEFOCUS) = 0) then
              InflateRect(R, -1, -1);
        StyleServices.DrawParentBackground(Handle, Message.DC, nil, FALSE, R);
        Message.Result := 1;
    end
    else
          inherited;
end;

procedure TMediaPlayTrackBar.CNNotify(var Message: TWMNotifyTRB);
var
    R      : TRect;
    Rgn    : HRGN;
    Details: TThemedElementDetails;
    Offset : Integer;
{$IFDEF CLR}
    Info: TNMCustomDraw;
{$ELSE}
    Info: PNMCustomDraw;
{$ENDIF}
begin
    if StyleServices.Enabled then
    begin
        with Message do
            if NMHdr.code = NM_CUSTOMDRAW then
            begin
                Info := NMCustomDraw;
                case Info.dwDrawStage of
                    CDDS_PREPAINT:
                        Result := CDRF_NOTIFYITEMDRAW;
                    CDDS_ITEMPREPAINT:
                        begin
{$IFDEF CLR}
                            case Info.dwItemSpec.ToInt64 of
{$ELSE}
                            case Info.dwItemSpec of
{$ENDIF}
                                TBCD_TICS:
                                    begin
                                        R := ClientRect;
                                        if Focused and ((Perform(WM_QUERYUISTATE, 0, 0) and UISF_HIDEFOCUS) = 0) then
                                        InflateRect(R, -1, -1);
                                        StyleServices.DrawParentBackground(Handle, Info.hDC, nil, FALSE, R)
                                    end;
                                TBCD_CHANNEL:
                                    begin
                                        SendGetStructMessage(Handle, TBM_GETTHUMBRECT, 0, R);
                                        Offset := 0;
                                        if Focused then
                                        Inc(Offset);
                                        R.Left := ClientRect.Left + Offset;
                                        R.Right := ClientRect.Right - Offset;
                                        with R do
                                        Rgn := CreateRectRgn(Left, Top, Right, Bottom);
                                        SelectClipRgn(Info.hDC, Rgn);
                                        Details := StyleServices.GetElementDetails(ttbThumbTics);
                                        StyleServices.DrawParentBackground(Handle, Info.hDC, Details, FALSE);
                                        DeleteObject(Rgn);
                                        SelectClipRgn(Info.hDC, 0);
                                    end;
                            end;
                            Result := CDRF_DODEFAULT;
                        end;
                else
                    Result := CDRF_DODEFAULT;
                end;
            end;
    end
    else
          inherited;
end;

end.
