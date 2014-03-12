unit ToolBarEx;

interface

uses
    System.SysUtils, System.Classes, Vcl.Controls, Vcl.ToolWin, Vcl.ComCtrls,
    Winapi.Messages, Winapi.Windows, Winapi.CommCtrl;

const
    CM_SET_TOOL_BUTTON_INFO = WM_USER + 350;

type
    TToolBarEx = class(TToolBar)
    private
        { Private declarations }
        FDropdown: Boolean;
    protected
        { Protected declarations }
        procedure CNNotify(var Message: TWMNotifyTLB); message CN_NOTIFY;
        procedure CreateWnd; override;
        procedure ResetButtonInfo(var Message: TMessage); message CM_SET_TOOL_BUTTON_INFO;
    public
        { Public declarations }
    published
        { Published declarations }
    end;

var
    test_value: Integer;

procedure Register;

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TToolBarEx]);
end;

{ TToolBarEx }

procedure TToolBarEx.CNNotify(var Message: TWMNotifyTLB);
var
    TBButton  : TTBButton;
    ToolButton: TToolButton;
begin
    if Message.NMHdr.code = TBN_DROPDOWN then
    begin
        if FDropdown then Exit;
        if Perform(TB_GETBUTTON, Message.NMToolBar.iItem, Winapi.Windows.LPARAM(@TBButton)) <> 0 then
        begin
            ToolButton := TToolButton(TBButton.dwData);
            if (ToolButton <> nil) and (ToolButton.DropdownMenu = nil) and (ToolButton.MenuItem = nil) then
            begin
                FDropdown := TRUE;
                ToolButton.Click();
                FDropdown := FALSE;
                Exit;
            end;
        end;
        test_value := test_value + 1;
    end;
    inherited;
end;

procedure TToolBarEx.CreateWnd;
begin
    inherited;
    PostMessage(Handle, CM_SET_TOOL_BUTTON_INFO, 0, 0);
end;

procedure TToolBarEx.ResetButtonInfo(var Message: TMessage);
var
    TBButton: TTBButton;
    Info    : TBBUTTONINFO;
    Count   : Integer;
    i       : Integer;
begin
    Count := Perform(TB_BUTTONCOUNT, 0, 0);
    for i := 0 to Count - 1 do
    begin
        if Perform(TB_GETBUTTON, i, Winapi.Windows.LPARAM(@TBButton)) = 0 then continue;
        if TBButton.fsStyle <> BTNS_DROPDOWN then continue;
        Info.cbSize := sizeof(Info);
        Info.dwMask := TBIF_STYLE;
        Info.fsStyle := BTNS_WHOLEDROPDOWN;
        Perform(TB_SETBUTTONINFO, i, LPARAM(@Info));
    end;
    Self.Realign();
end;

end.
