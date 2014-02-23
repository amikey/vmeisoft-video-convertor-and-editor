
#define CompanyName          "VMeisoft"
#define ProductName          "Video Converter and Video Editor"
#define FullProductName      "VMeisoft Video Converter and Video Editor"
#define AppVersion()        ParseVersion(AddBackslash(SourcePath) + "..\ui\Win64\Release\VideoConverter.exe", Local[0], Local[1], Local[2], Local[3]), str(Local[0]) + "." + str(Local[1]) + "." + str(Local[2]) + "." + str(Local[3])

[Setup]
AppName={#FullProductName}
AppMutex=VMeisoftVideoConverterMutex
AppVersion={#AppVersion}
VersionInfoVersion={#AppVersion}
AppCopyright=Copyright (C) 2014 VMeisoft, Inc.
DefaultDirName={pf}\{#CompanyName}\{#ProductName}
DefaultGroupName={#CompanyName}
Compression=lzma2/ultra
SolidCompression=yes
OutputDir=.
OutputBaseFilename=converter
MinVersion=5.1
AppComments={#FullProductName} Setup
AppPublisher={#CompanyName}
AppPublisherURL=http://www.vmeisoft.com/
AppSupportURL=http://www.vmeisoft.com/
AppendDefaultGroupName=no
AllowCancelDuringInstall=no
DisableDirPage=auto
DisableProgramGroupPage=yes
SetupIconFile=VideoConverter.ico
UninstallDisplayIcon={app}\VideoConverter.exe
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: "..\Resource\*.*";                           DestDir: "{app}\Resource"; Flags: ignoreversion recursesubdirs;
Source: "..\ui\Win64\Release\VideoConverter.exe";    DestDir: "{app}"; Flags: ignoreversion;   Check: Is64BitInstallMode
Source: "..\ui\Win32\Release\VideoConverter.exe";    DestDir: "{app}"; Flags: ignoreversion;   Check: not Is64BitInstallMode
Source: "..\src\x64\Release\VideoConverter.dll";     DestDir: "{app}"; Flags: ignoreversion;   Check: Is64BitInstallMode
Source: "..\src\Release\VideoConverter.dll";         DestDir: "{app}"; Flags: ignoreversion;   Check: not Is64BitInstallMode

[Icons]
Name: "{group}\{#ProductName}"; Filename: "{app}\VideoConverter.exe"

[Run]
Filename: "{app}\VideoConverter.exe"; Description: "Launch application";        Flags: postinstall nowait shellexec; WorkingDir: "{app}"
Filename: {code:CreateDesktopIcon};   Description: "Create a desktop icon";     Flags: postinstall nowait runhidden skipifdoesntexist

[UninstallDelete]
Type: files; Name: "{commondesktop}\{#FullProductName}.lnk"

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultStr: String;
  ResultCode: Integer;
begin
  if CurStep = ssInstall then
  begin
    if RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#FullProductName}_is1', 'UninstallString', ResultStr) then
    begin
      ResultStr := RemoveQuotes(ResultStr);
      Exec(ResultStr, '/VERYSILENT', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
    end;
  end;
end;

function CreateDesktopIcon(Param: String): String;
begin
  CreateShellLink(
    ExpandConstant('{commondesktop}\{#FullProductName}.lnk'),
    '{#FullProductName}',
    ExpandConstant('{app}\VideoConverter.exe'),
    '',
    ExpandConstant('{app}'),
    '',
    0,
    SW_SHOWNORMAL);
  Result := '';
end;
