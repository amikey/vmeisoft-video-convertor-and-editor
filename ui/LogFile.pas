{$WARN SYMBOL_PLATFORM OFF}
unit LogFile;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics, Vcl.Controls, Vcl.Forms,
    Vcl.Dialogs, Vcl.ImgList, Vcl.Menus, Vcl.StdCtrls, Vcl.ExtCtrls, Vcl.OleCtrls, SHDocVw, Vcl.ComCtrls, Vcl.Buttons;

function GetMultipleInatance(): Int32; stdcall;
function GetMainWindowHandle(): HWND; stdcall;
procedure SetMainWindowHandle(hwndMain: HWND); stdcall;
procedure WriteErrorInfo(ErrorInfo: String);
procedure WriteErrorLog(Info: PAnsiChar); stdcall;

implementation

uses Defines;

function GetMultipleInatance; external DLL_NAME name 'GetMultipleInatance';
function GetMainWindowHandle; external DLL_NAME name 'GetMainWindowHandle';
procedure SetMainWindowHandle; external DLL_NAME name 'SetMainWindowHandle';
procedure WriteErrorLog; external DLL_NAME name 'WriteErrorLog';

procedure WriteErrorInfo(ErrorInfo: String);
begin
    WriteErrorLog(PAnsiChar(AnsiString(ErrorInfo)));
end;

end.
