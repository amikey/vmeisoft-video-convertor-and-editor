unit PanelList;

interface

uses
    System.SysUtils, System.Classes, Vcl.Controls, Vcl.ExtCtrls;

type
    TPanelList = class(TPanel)
    private
        { Private declarations }
        FPages      : TList;
        FActivePanel: TPanel;

        procedure ChangeActivePage(Page: TPanel);
        procedure DeleteTab(Page: TPanelList; Index: Integer);
        function GetActivePageIndex: Integer;
        function GetPage(Index: Integer): TPanel;
        function GetPageCount: Integer;

    protected
        { Protected declarations }
    public
        { Public declarations }
        constructor Create(AOwner: TComponent); override;
        destructor Destroy; override;
    published
        { Published declarations }
    end;

procedure Register;

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TPanelList]);
end;

constructor TPanelList.Create(AOwner: TComponent);
begin
    inherited Create(AOwner);
    ControlStyle := [csDoubleClicks, csOpaque, csPannable, csGestures];
    FPages := TList.Create;
end;

destructor TPanelList.Destroy;
begin
    FPages.Free;
    inherited Destroy;
end;

end.
