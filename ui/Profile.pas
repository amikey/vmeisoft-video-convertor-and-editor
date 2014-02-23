unit Profile;

interface

{$WARN SYMBOL_PLATFORM OFF}


uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics, Vcl.Controls, Vcl.Forms,
    Vcl.Dialogs, Vcl.ImgList, Vcl.Menus, Vcl.StdCtrls, Vcl.ExtCtrls, Vcl.OleCtrls, SHDocVw, Vcl.ComCtrls, Vcl.Buttons;


function ProfileVideo_Size():Integer; stdcall;
function ProfileAudio_Size():Integer; stdcall;
function ProfileTarget_Size():Integer; stdcall;
function ProfileTag_Size():Integer; stdcall;
function ProfileSettings_Size():Integer; stdcall;


const
    ASPECT_4x3  = 1;
    ASPECT_16x9 = 2;

const
    GROUP_NONE   = -1;
    GROUP_HD     = 0;
    GROUP_VIDEO  = 1;
    GROUP_DEVICE = 2;
    GROUP_WEB    = 3;
    GROUP_APP    = 4;
    GROUP_AUDIO  = 5;
    GROUP_MAX    = 6;

const
    THUMBNAIL_WIDTH  = 48;
    THUMBNAIL_HEIGHT = 48;

const
    VIDEO_RES_CUSTOM    = -1;

const
    CUSTOM_STRING    = 'Custom ...';

type

    ProfileRes = record
        m_Width: Integer;
        m_Height: Integer;
    end;

    ProfileFrameRate = record
        m_Num: Integer;
        m_Den: Integer;
    end;

    ProfileCodec = record
        m_Name: array [0 .. 31] of WideChar;
        m_Codec: array [0 .. 31] of WideChar;
    end;

    ProfileVideo = record
        m_FixedSize: Integer;
        m_FixedBitRate: Integer;
        m_Aspect: Integer;

        m_cCodec: Integer;
        m_Codec: array of ProfileCodec;
        m_DefaultCodec: Integer;

        m_cRes: Integer;
        m_Res: array of ProfileRes;
        m_DefaultRes: Integer;

        m_cFrameRate: Integer;
        m_FrameRate: array of ProfileFrameRate;
        m_DefaultFrameRate: Integer;

        m_cBitRate: Integer;
        m_BitRate: array of Integer;
        m_DefaultBitRate: Integer;
    end;

    PProfileVideo = ^ProfileVideo;

    ProfileAudio = record
        m_FixedFormat: Integer;
        m_FixedBitRate: Integer;

        m_cCodec: Integer;
        m_Codec: array of ProfileCodec;
        m_DefaultCodec: Integer;

        m_cSampleRate: Integer;
        m_SampleRate: array of Integer;
        m_DefaultSampleRate: Integer;

        m_cChannel: Integer;
        m_Channel: array of Integer;
        m_DefaultChannel: Integer;

        m_cBitRate: Integer;
        m_BitRate: array of Integer;
        m_DefaultBitRate: Integer;
    end;

    PProfileAudio = ^ProfileAudio;

    ProfileTarget = record
        m_Name: array [0 .. 63] of WideChar;
        m_Description: array [0 .. 127] of WideChar;

        m_Video: ProfileVideo;
        m_Audio: ProfileAudio;
    end;

    PProfileTarget = ^ProfileTarget;

    ProfileTag = record
        m_Id: TGUID;

        m_Name: array [0 .. 255] of WideChar;
        m_Description: array [0 .. 1023] of WideChar;
        m_Format: array [0 .. 63] of WideChar;
        m_ExtName: array [0 .. 15] of WideChar;

        m_cTarget: Integer;
        m_Target: array of ProfileTarget;

        m_ImageFile: array [0 .. MAX_PATH - 1] of WideChar;
        m_Bitmap: Pointer;
    end;

    PProfileTag = ^ProfileTag;

    ProfileSettings = record

        m_IdProfile: TGUID;

        m_Group: Integer;
        m_Profile: Integer;
        m_Target: Integer;

        m_VCodec: Integer;
        m_Res: Integer;
        m_ResCustom: ProfileRes;
        m_FrameRate: Integer;
        m_VBitRate: Integer;

        m_ACodec: Integer;
        m_SampleRate: Integer;
        m_Channel: Integer;
        m_ABitRate: Integer;
    end;

    PProfileSettings = ^ProfileSettings;



procedure GetDefaultProfileSetting1(Settings: PProfileSettings; nGroup: Integer); stdcall;
procedure GetDefaultProfileSetting2(Settings: PProfileSettings; nGroup, nProfile: Integer); stdcall;
procedure GetDefaultProfileSetting3(Settings: PProfileSettings; nGroup, nProfile, nTarget: Integer); stdcall;
function GetProfofileCount(nGroup: Integer): Integer; stdcall;
function GetProfile(nGroup, Profile: Integer): PProfileTag; stdcall;
function GetProfileTarget(nGroup, Profile, Target: Integer): PProfileTarget; stdcall;
procedure DrawProfileThumbnail(Profile: PProfileTarget; hdc: hdc; x, y: Integer); stdcall;

function GetProfileSettings(): PProfileSettings; stdcall;

function GetProfofileCountEx(Settings: PProfileSettings): Integer; stdcall;
function GetProfileEx(Settings: PProfileSettings): PProfileTag; stdcall;
function GetProfileTargetEx(Settings: PProfileSettings): PProfileTarget; stdcall;
function GetProfileVideoEx(Settings: PProfileSettings): PProfileVideo; stdcall;
function GetProfileAudioEx(Settings: PProfileSettings): PProfileAudio; stdcall;
function IsFixedVideoSizeEx(Settings: PProfileSettings): BOOL; stdcall;
procedure SetCustomRes(Settings: PProfileSettings; Width, Height: Integer); stdcall;

function CustomVideoSize(p: PProfileSettings): Boolean;
function GetProfileDesc(p: PProfileSettings): String;

procedure AddBitRateStrins(Strings: TStrings; var a: array of Integer; count: Integer);

procedure AddProfileStrins(p: PProfileSettings; Strings: TStrings);
procedure AddTargetStrins(p: PProfileSettings; Strings: TStrings);
procedure AddVideoCodecStrins(p: PProfileSettings; Strings: TStrings);
procedure AddVideoResolutionStrins(p: PProfileSettings; Strings: TStrings);
procedure AddVideoFrameRateStrins(p: PProfileSettings; Strings: TStrings);
procedure AddVideoBitRateStrins(p: PProfileSettings; Strings: TStrings);

procedure AddAudioCodecStrins(p: PProfileSettings; Strings: TStrings);
procedure AddAudioSampleRateStrins(p: PProfileSettings; Strings: TStrings);
procedure AddAudioChannelStrins(p: PProfileSettings; Strings: TStrings);
procedure AddAudioBitRateStrins(p: PProfileSettings; Strings: TStrings);

function GetVideoResolutionIndex(p: PProfileSettings): Integer;
function GetAspectString(Width, Height: Integer; IsLink: Boolean): String;
function GetVideoAspectString(p: PProfileSettings): String;

function GetResolutionIndex(p: PProfileSettings): Integer;
procedure SetResolutionByIndex(p: PProfileSettings; index: Integer);

function GetFormatImageFile(Settings: PProfileSettings): String;
function GetFormatName(Settings: PProfileSettings): String;
function GetFormatDescription(Settings: PProfileSettings): String;

implementation

uses Defines;

function ProfileVideo_Size; external DLL_NAME name 'ProfileVideo_Size';
function ProfileAudio_Size; external DLL_NAME name 'ProfileAudio_Size';
function ProfileTarget_Size; external DLL_NAME name 'ProfileTarget_Size';
function ProfileTag_Size; external DLL_NAME name 'ProfileTag_Size';
function ProfileSettings_Size; external DLL_NAME name 'ProfileSettings_Size';

procedure GetDefaultProfileSetting1; external DLL_NAME name 'GetDefaultProfileSetting1';
procedure GetDefaultProfileSetting2; external DLL_NAME name 'GetDefaultProfileSetting2';
procedure GetDefaultProfileSetting3; external DLL_NAME name 'GetDefaultProfileSetting3';
function GetProfofileCount; external DLL_NAME name 'GetProfofileCount';
function GetProfile; external DLL_NAME name 'GetProfile';
function GetProfileTarget; external DLL_NAME name 'GetProfileTarget';
procedure DrawProfileThumbnail; external DLL_NAME name 'DrawProfileThumbnail';

function GetProfileSettings; external DLL_NAME name 'GetProfileSettings';

function GetProfofileCountEx; external DLL_NAME name 'GetProfofileCountEx';
function GetProfileEx; external DLL_NAME name 'GetProfileEx';
function GetProfileTargetEx; external DLL_NAME name 'GetProfileTargetEx';
function GetProfileVideoEx; external DLL_NAME name 'GetProfileVideoEx';
function GetProfileAudioEx; external DLL_NAME name 'GetProfileAudioEx';
function IsFixedVideoSizeEx; external DLL_NAME name 'IsFixedVideoSizeEx';
procedure SetCustomRes; external DLL_NAME name 'SetCustomRes';

function GetProfileDesc(p: PProfileSettings): String;
var
    o: PProfileTag;
begin
    o := GetProfileEx(p);
    Result := o.m_Description;
end;

procedure AddProfileStrins(p: PProfileSettings; Strings: TStrings);
var
    i: Integer;
    n: Integer;
    o: PProfileTag;
begin
    Strings.Clear();
    n := GetProfofileCount(p.m_Group);
    for i := 0 to n - 1 do
    begin
        o := GetProfile(p.m_Group, i);
        Strings.AddObject(o.m_Name, TObject(o));
    end;
end;

procedure AddTargetStrins(p: PProfileSettings; Strings: TStrings);
var
    i: Integer;
    n: Integer;
    o: PProfileTag;
begin
    Strings.Clear();
    o := GetProfileEx(p);
    n := o.m_cTarget;
    for i := 0 to n - 1 do
    begin
        Strings.Add(o.m_Target[i].m_Description);
    end;
end;

procedure AddVideoCodecStrins(p: PProfileSettings; Strings: TStrings);
var
    i: Integer;
    n: Integer;
    o: PProfileTarget;
begin
    Strings.Clear();
    o := GetProfileTargetEx(p);
    n := o.m_Video.m_cCodec;
    for i := 0 to n - 1 do
    begin
        Strings.Add(o.m_Video.m_Codec[i].m_Name);
    end;
end;

function CustomVideoSize(p: PProfileSettings): Boolean;
begin
    Result := GetProfileTargetEx(p).m_Video.m_FixedSize = 0;
end;

procedure AddVideoResolutionStrins(p: PProfileSettings; Strings: TStrings);
var
    i: Integer;
    n: Integer;
    o: PProfileVideo;
begin
    Strings.Clear();
    o := GetProfileVideoEx(p);
    n := o.m_cRes;
    for i := 0 to n - 1 do
    begin
        Strings.AddObject(Format('%d x %d', [o.m_Res[i].m_Width, o.m_Res[i].m_Height]), TObject(i));
    end;
    if CustomVideoSize(p) then
    begin
        Strings.AddObject(CUSTOM_STRING, TObject(VIDEO_RES_CUSTOM));
    end;
end;

procedure AddVideoFrameRateStrins(p: PProfileSettings; Strings: TStrings);
var
    i: Integer;
    n: Integer;
    o: PProfileTarget;
begin
    Strings.Clear();
    o := GetProfileTargetEx(p);
    n := o.m_Video.m_cFrameRate;
    for i := 0 to n - 1 do
    begin
        if o.m_Video.m_FrameRate[i].m_Num = 1 then
        begin
            Strings.Add(Format('%d fps', [o.m_Video.m_FrameRate[i].m_Den]));
        end else begin
            Strings.Add(Format('%.2f fps', [o.m_Video.m_FrameRate[i].m_Den / o.m_Video.m_FrameRate[i].m_Num]));
        end;
    end;
end;

procedure AddBitRateStrins(Strings: TStrings; var a: array of Integer; count: Integer);
var
    i: Integer;
begin
    Strings.Clear();
    for i := 0 to count - 1 do
    begin
        if a[i] < 1000000 then
        begin
            if (a[i] mod 1000) = 0 then
            begin
                Strings.Add(Format('%d Kbps', [a[i] div 1000]));
            end else begin
                Strings.Add(Format('%.1f Kbps', [a[i] / 1000.0]));
            end;
        end else begin
            if (a[i] mod 1000000) = 0 then
            begin
                Strings.Add(Format('%d Mbps', [a[i] div 1000000]));
            end else begin
                Strings.Add(Format('%.1f Mbps', [a[i] / 1000000.0]));
            end;
        end;
    end;
end;

procedure AddVideoBitRateStrins(p: PProfileSettings; Strings: TStrings);
var
    o: PProfileTarget;
begin
    o := GetProfileTargetEx(p);
    AddBitRateStrins(Strings, o.m_Video.m_BitRate, o.m_Video.m_cBitRate);
end;

procedure AddAudioCodecStrins(p: PProfileSettings; Strings: TStrings);
var
    i: Integer;
    n: Integer;
    o: PProfileTarget;
begin
    Strings.Clear();
    o := GetProfileTargetEx(p);
    n := o.m_Audio.m_cCodec;
    for i := 0 to n - 1 do
    begin
        Strings.Add(o.m_Audio.m_Codec[i].m_Name);
    end;
end;

procedure AddAudioSampleRateStrins(p: PProfileSettings; Strings: TStrings);
var
    i: Integer;
    n: Integer;
    o: PProfileTarget;
begin
    Strings.Clear();
    o := GetProfileTargetEx(p);
    n := o.m_Audio.m_cSampleRate;
    for i := 0 to n - 1 do
    begin
        case o.m_Audio.m_SampleRate[i] of
            8000: Strings.Add('Phone Quality (8000)');
            22050: Strings.Add('Low Quality (22050)');
            44100: Strings.Add('CD Quality (44100)');
            48000: Strings.Add('DVD Quality (48000)');
        else
            Strings.Add(Format('%dHz,', [o.m_Audio.m_SampleRate[i]]));
        end;
    end;
end;

procedure AddAudioChannelStrins(p: PProfileSettings; Strings: TStrings);
var
    i: Integer;
    n: Integer;
    o: PProfileTarget;
begin
    Strings.Clear();
    o := GetProfileTargetEx(p);
    n := o.m_Audio.m_cChannel;
    for i := 0 to n - 1 do
    begin
        case o.m_Audio.m_Channel[i] of
            1: Strings.Add('Mono');
            2: Strings.Add('Stereo');
        else
            Strings.Add(Format('%dChannels', [o.m_Audio.m_Channel[i]]));
        end;
    end;
end;

procedure AddAudioBitRateStrins(p: PProfileSettings; Strings: TStrings);
var
    o: PProfileTarget;
begin
    o := GetProfileTargetEx(p);
    AddBitRateStrins(Strings, o.m_Audio.m_BitRate, o.m_Audio.m_cBitRate);
end;

function GetVideoResolutionIndex(p: PProfileSettings): Integer;
var
    o: PProfileTarget;
    i: Integer;
    n: Integer;
begin
    o := GetProfileTargetEx(p);
    n := o.m_Video.m_cRes;
    for i := 0 to n - 1 do
    begin
        if (p.m_ResCustom.m_Width = o.m_Video.m_Res[i].m_Width) and (p.m_ResCustom.m_Height = o.m_Video.m_Res[i].m_Height) then
        begin
            Result := i;
            Exit;
        end;
    end;
    Result := n;
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

function GetAspectString(Width, Height: Integer; IsLink: Boolean): String;
var
    n   : Integer;
    w, h: Integer;
begin
    if (Width = 0) or (Height = 0) then
    begin
        Result := '';
        Exit;
    end;

    if (Width = 854) and (Height = 480) then
    begin
        if IsLink then
        begin
            Result := '<a>16:9</a>';
        end else begin
            Result := '16:9';
        end;
        Exit;
    end;

    if (Width = 1920) and (Height = 1088) then
    begin
        if IsLink then
        begin
            Result := '<a>16:9</a>';
        end else begin
            Result := '16:9';
        end;
        Exit;
    end;

    w := Width;
    h := Height;
    n := zdgys(w, h);
    if IsLink then
    begin
        Result := Format('<a>%d:%d</a>', [w div n, h div n]);
    end else begin
        Result := Format('%d:%d', [w div n, h div n]);
    end;
end;

function GetVideoAspectString(p: PProfileSettings): String;
begin
    Result := GetAspectString(p.m_ResCustom.m_Width, p.m_ResCustom.m_Height, p.m_Res = VIDEO_RES_CUSTOM);
end;

function GetResolutionIndex(p: PProfileSettings): Integer;
var
    p1: PProfileVideo;
begin
    if p.m_Res >= 0 then
    begin
        Result := p.m_Res;
    end else begin
        p1 := GetProfileVideoEx(p);
        Result := p1.m_cRes - p.m_Res;
    end;
end;

procedure SetResolutionByIndex(p: PProfileSettings; index: Integer);
var
    p1: PProfileVideo;
begin
    p1 := GetProfileVideoEx(p);
    p.m_ResCustom := p1.m_Res[index];
end;

function GetFormatImageFile(Settings: PProfileSettings): String;
var
    p: PProfileTag;
begin
    p := GetProfileEx(Settings);
    Result := p.m_ImageFile;
end;

function GetFormatName(Settings: PProfileSettings): String;
var
    p: PProfileTag;
begin
    p := GetProfileEx(Settings);
    Result := p.m_Name;
end;

function GetFormatDescription(Settings: PProfileSettings): String;
var
    p: PProfileTag;
    o: PProfileTarget;
    n: Integer;
begin
    p := GetProfileEx(Settings);
    o := GetProfileTargetEx(Settings);
    Result := '';
    if Settings.m_VCodec <> -1 then
    begin
        Result := Result + Format('Video:%dx%d; ', [Settings.m_ResCustom.m_Width, Settings.m_ResCustom.m_Height]);
    end else begin
        Result := Result + 'Video:None; ';
    end;

    if Settings.m_ACodec <> -1 then
    begin
        Result := Result + 'Audio:';
        n := o.m_Audio.m_SampleRate[Settings.m_SampleRate];
        case n of
            8000: Result := Result + 'Phone Quality,';
            22050: Result := Result + 'Low Quality,';
            44100: Result := Result + 'CD Quality,';
            48000: Result := Result + 'DVD Quality,';
        else
            Result := Result + Format('%dHz,', [n]);
        end;

        n := o.m_Audio.m_Channel[Settings.m_Channel];
        case n of
            1: Result := Result + 'Mono';
            2: Result := Result + 'Stereo';
        else
            Result := Result + Format('%dChannels', [n]);
        end;
    end else begin
        Result := Result + 'Audio:None';
    end;
end;

end.
