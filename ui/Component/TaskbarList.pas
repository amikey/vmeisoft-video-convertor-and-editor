unit TaskbarList;

interface

uses
    System.SysUtils, System.Classes, Winapi.Messages, Winapi.Windows, Winapi.ShlObj, System.Win.ComObj;

type
    TTaskbarList = class(TComponent)
    private
        { Private declarations }
        FTaskBarList: ITaskbarList3;
        FHandle     : HWND;
    protected
        { Protected declarations }
    public
        { Public declarations }
        constructor Create(AOwner: TComponent); override;
        destructor Destroy; override;
    published
        { Published declarations }
        property WindowHandle: HWND write FHandle;
        procedure SetProgressValue(ullCompleted: ULONGLONG; ullTotal: ULONGLONG);
    end;

procedure Register;

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TTaskbarList]);
end;

constructor TTaskbarList.Create(AOwner: TComponent);
begin
    inherited Create(AOwner);
    if Win32MajorVersion >= 6 then
    begin
        FTaskBarList := CreateComObject(CLSID_TaskbarList) as ITaskbarList3;
    end;
end;

destructor TTaskbarList.Destroy;
begin
    if FTaskBarList <> nil then
    begin
        FTaskBarList.SetProgressValue(FHandle, 0, 100);
        FTaskBarList.SetProgressState(FHandle, TBPF_NOPROGRESS);
        FTaskBarList := nil;
    end;
    inherited Destroy;
end;

procedure TTaskbarList.SetProgressValue(ullCompleted: ULONGLONG; ullTotal: ULONGLONG);
begin
    if FTaskBarList <> nil then
    begin
        FTaskBarList.SetProgressValue(FHandle, ullCompleted, ullTotal);
    end;
end;

end.
