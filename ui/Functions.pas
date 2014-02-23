unit Functions;

interface

uses
    Winapi.Windows, Winapi.Messages, Winapi.CommCtrl, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics, Vcl.Controls, Vcl.Forms,
    Vcl.Dialogs, Vcl.ImgList, Vcl.Menus, Vcl.StdCtrls, Vcl.ExtCtrls, Vcl.OleCtrls, SHDocVw, Vcl.ComCtrls, Vcl.Buttons, Winapi.GDIPAPI, Winapi.GDIPOBJ,
    VideoConverterInt, SubtitleInt;

var
    ApplicationPath: String;
    ResourcePath   : String;

procedure InitApplication();
function GetDurationString(d: Integer): String;
function GetAVTimeString(t: Int64): String;
function GetDurationProcString(d1, d2: Integer): String;
function IsSupportImage(FileName: String; var Width, Height: Integer): Boolean;
function RoundVideoSize(Value: Integer): Integer;
function MakeHtmlString(Str: String): String;
function GetVersionString(v: Uint64): String;
function GetApplicationVersionValue(): Uint64;
function GetApplicationVersionString(): String;
function GetFontString(Name: String; Size: Integer; Style: Integer): String;

function BGRToTColor(color: ColorRef): TColor;
procedure TColorToBGR(var ref: ColorRef; color: TColor);

implementation

uses
    Defines;

function RoundVideoSize(Value: Integer): Integer;
begin
    Result := (Value + 15) and (not 15);
end;

procedure InitApplication();
begin
    ApplicationPath := ExtractFilePath(Application.ExeName);
    ResourcePath := ApplicationPath + 'Resource\';
end;

function GetAVTimeString(t: Int64): String;
var
    h, m, s, d: Integer;
begin
    t := t div 100000;
    d := t mod 10;
    t := t div 10;
    s := t mod 60;
    t := t div 60;
    m := t mod 60;
    t := t div 60;
    h := t;
    Result := Format('%d:%.2d:%.2d.%d', [h, m, s, d]);
end;

function GetDurationString(d: Integer): String;
var
    h, m, s: Integer;
begin
    h := d div 3600;
    d := d mod 3600;
    m := d div 60;
    s := d mod 60;
    Result := Format('%d:%.2d:%.2d', [h, m, s]);
end;

function zdgys(x, y: Integer): Integer;
var
    n: Integer;
begin
    n := y;
    if x < y then
    begin
        y := x;
        x := n;
    end;

    while n > 0 do
    begin
        n := x mod y;
        x := y;
        if n > 0 then y := n;
    end;
    Result := y;
end;

function GetDurationProcString(d1, d2: Integer): String;
begin
    Result := GetDurationString(d1) + ' / ' + GetDurationString(d2);
end;

function IsSupportImage(FileName: String; var Width, Height: Integer): Boolean;
var
    Image: TGPImage;
begin
    Image := TGPImage.Create(FileName);
    Result := Image.GetLastStatus() = Winapi.GDIPAPI.Ok;
    Width := Image.GetWidth();
    Height := Image.GetHeight();
    Result := (Result) and (Width <> 0) and (Height <> 0);
    Image.Free();
end;

function MakeHtmlString(Str: String): String;
var
    i, l: Integer;
    s   : UTF8String;
begin
    Result := '';
    s := UTF8String(Str);
    l := length(s);
    for i := 1 to l do
    begin
        if (s[i] >= '0') and (s[i] <= '9') then
        begin
            Result := Result + s[i];
        end else if (s[i] >= 'a') and (s[i] <= 'z') then
        begin
            Result := Result + s[i];
        end else if (s[i] >= 'A') and (s[i] <= 'Z') then
        begin
            Result := Result + s[i];
        end else begin
            Result := Result + Format('%%%x', [BYTE(s[i])]);
        end;
    end;
end;

function GetApplicationVersionValue(): Uint64;
var
    VerInfoSize : DWORD;
    VerInfo     : Pointer;
    VerValueSize: DWORD;
    Dummy       : DWORD;
    VerValue    : PVSFixedFileInfo;
    FileName    : string;
begin
    Result := 0;
    FileName := Application.ExeName;
    VerInfoSize := GetFileVersionInfoSize(PChar(FileName), Dummy);
    if VerInfoSize = 0 then Exit;
    GetMem(VerInfo, VerInfoSize);
    GetFileVersionInfo(PChar(FileName), 0, VerInfoSize, VerInfo);
    VerQueryValue(VerInfo, '\', Pointer(VerValue), VerValueSize);
    Result := VerValue^.dwFileVersionMS;
    Result := (Result shl 32) + VerValue^.dwFileVersionLS;
    FreeMem(VerInfo);
end;

function GetVersionString(v: Uint64): string;
begin
    Result := Format('%d.%d.%d.%d', [(v shr 48) and $FFFF, (v shr 32) and $FFFF, (v shr 16) and $FFFF, v and $FFFF]);
end;

function GetApplicationVersionString(): String;
begin
    Result := GetVersionString(GetApplicationVersionValue());
end;

function GetFontString(Name: String; Size: Integer; Style: Integer): String;
var
    b: Boolean;
begin
    Result := 'Font:' + Name + '; Size:' + IntToStr(Size) + '; Style:';
    if Style = 0 then
    begin
        Result := Result + 'Regular.';
    end else begin
        b := FALSE;
        if (Style and FontStyleBold) <> 0 then
        begin
            Result := Result + 'Bold';
            b := TRUE;
        end;
        if (Style and FontStyleItalic) <> 0 then
        begin
            if b then Result := Result + ',';
            Result := Result + 'Italic';
            b := TRUE;
        end;
        if (Style and FontStyleUnderline) <> 0 then
        begin
            if b then Result := Result + ',';
            Result := Result + 'Underline';
            b := TRUE;
        end;
        if (Style and FontStyleStrikeout) <> 0 then
        begin
            if b then Result := Result + ',';
            Result := Result + 'Strikeout';
            b := TRUE;
        end;
        Result := Result + '.';
    end;
end;

function BGRToTColor(color: ColorRef): TColor;
begin
    Result := color.R + (color.G shl 8) + (color.b shl 16);
end;

procedure TColorToBGR(var ref: ColorRef; color: TColor);
var
    c: ColorRef;
begin
    c.Color := ColorToRGB(color);
    ref.B := c.R;
    ref.G := c.G;
    ref.R := c.B;
end;

end.
