unit CommonData;

interface

uses
    Winapi.Windows, Winapi.Messages, System.Types, System.SysUtils, System.Variants, System.Classes,
    Winapi.GDIPAPI, Winapi.GDIPOBJ, Winapi.ShellAPI,
    Profile, VideoConverterInt;

var
    _Global        : PVCGlobal;
    _ProfleSettings: PProfileSettings;
    _CodecOptions  : PCodecOptions;
    _MergeInfo     : PMergeInfo;

function OutputWidth(): Integer;
function OutputHeight(): Integer;
procedure InitCommonData();

implementation

procedure InitCommonData();
begin
    _Global := vcGetGlobal();
    _ProfleSettings := GetProfileSettings();
    _CodecOptions := vcGetCodecOptions();
    _MergeInfo := vcGetMergeInfo();
end;

function OutputWidth(): Integer;
begin
    Result := _ProfleSettings.m_ResCustom.m_Width;
end;

function OutputHeight(): Integer;
begin
    Result := _ProfleSettings.m_ResCustom.m_Height;
end;

end.
