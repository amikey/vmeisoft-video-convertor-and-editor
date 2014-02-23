unit Merge;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
    Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Vcl.ExtCtrls, Winapi.Shellapi,
    Vcl.ComCtrls, VideoConverterInt;

type
    TfrmMerge = class(TForm)
        btnCancel: TButton;
        ProgressBar1: TProgressBar;
        Timer1: TTimer;
        lblTime: TLabel;
        lblOutput: TLinkLabel;
        btnOK: TButton;
        procedure FormResize(Sender: TObject);
        procedure Timer1Timer(Sender: TObject);
        procedure FormShow(Sender: TObject);
        procedure btnCancelClick(Sender: TObject);
        procedure btnOKClick(Sender: TObject);
        procedure FormCreate(Sender: TObject);
        procedure lblOutputLinkClick(Sender: TObject; const Link: string; LinkType: TSysLinkType);
    private
        { Private declarations }
        procedure OnComplete(var msg: TMessage); message VC_MERGE_COMPLETE;
    public
        { Public declarations }
        FItems     : array [0 .. 1024 - 1] of PVCItem;
        FCount     : Integer;
        FTimerCount: Int64;
    end;

implementation

{$R *.dfm}


uses
    Functions, Defines, CommonData;

procedure TfrmMerge.btnCancelClick(Sender: TObject);
begin
    if Application.MessageBox('Abort ?', PWideChar(APP_TITLE), MB_ICONQUESTION or MB_YESNO or MB_DEFBUTTON2) = IDYES then
    begin
        vcAbortMerge();
        Close();
    end;
end;

procedure TfrmMerge.btnOKClick(Sender: TObject);
begin
    Close();
end;

procedure TfrmMerge.FormCreate(Sender: TObject);
begin
    btnOK.Left := btnCancel.Left;
    btnOK.Top := btnCancel.Top;
    lblOutput.Left := ProgressBar1.Left;
    lblOutput.Top := ProgressBar1.Top;
    lblOutput.Width := ProgressBar1.Width;
end;

procedure TfrmMerge.FormResize(Sender: TObject);
begin
    if Self.WindowState = wsMinimized then
    begin
        Application.Minimize();
    end;
end;

procedure TfrmMerge.FormShow(Sender: TObject);
begin
    _MergeInfo.m_hwndMessage := Self.Handle;
    if vcStartMerge(FItems[0], FCount) <> S_OK then
    begin
        Application.MessageBox(FAILED_FOUND_FLASH, PWideChar(APP_TITLE), MB_ICONERROR);
        Close();
    end;
    Timer1.Enabled := TRUE;
end;

procedure TfrmMerge.lblOutputLinkClick(Sender: TObject; const Link: string;
  LinkType: TSysLinkType);
var
    Parameters: WideString;
begin
    Parameters := '/select,' + _MergeInfo.m_OutFile;
    ShellExecute(0, nil, 'explorer.exe', PWideChar(Parameters), nil, SW_SHOWNORMAL);
end;

procedure TfrmMerge.OnComplete(var msg: TMessage);
begin
    Timer1.Enabled := FALSE;
    btnOK.Visible := TRUE;
    lblOutput.Visible := TRUE;
    btnCancel.Visible := FALSE;
    ProgressBar1.Visible := FALSE;
    lblTime.Visible := FALSE;
    case msg.LParam of
        0:
            begin
                lblOutput.Caption := 'Complete:<a>' + _MergeInfo.m_OutFile + '</a>';
            end;
        -99:
            begin
                //
            end;
    else
        begin
            lblOutput.Caption := FAILED_FOUND_FLASH;
            lblOutput.Font.Color := clRed;
        end;
    end;
end;

procedure TfrmMerge.Timer1Timer(Sender: TObject);
var
    Estimated: Integer;
begin
    if _MergeInfo.m_Duration = 0 then exit;
    FTimerCount := FTimerCount + 1;
    ProgressBar1.Position := _MergeInfo.m_Time * 100 div _MergeInfo.m_Duration;
    if (ProgressBar1.Position > 2) and (_MergeInfo.m_Time < _MergeInfo.m_Duration) then
    begin
        Estimated := (FTimerCount * _MergeInfo.m_Duration div _MergeInfo.m_Time) - FTimerCount;
        lblTime.Caption := 'Elapsed:[' + GetDurationString(FTimerCount) + ']; Estimated:[' + GetDurationString(Estimated) + ']';
    end else begin
        lblTime.Caption := 'Elapsed:[' + GetDurationString(FTimerCount) + ']';
    end;
end;

end.
