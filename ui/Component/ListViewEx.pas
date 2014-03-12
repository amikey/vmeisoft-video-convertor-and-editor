unit ListViewEx;

interface

uses
    System.SysUtils, System.Classes, Winapi.Windows, Vcl.Controls, Vcl.ComCtrls, Winapi.CommCtrl,
    Winapi.GDIPAPI, Winapi.GDIPOBJ;

type
    TListItemEx = Class
    private
        FImage  : TGPBitmap;
        FInfoTip: string;
    public
        property Image  : TGPBitmap read FImage write FImage;
        property InfoTip: String read FInfoTip write FInfoTip;
    end;

    TListViewEx = class(TListView)
    private
        { Private declarations }
        FNoScroll  : Boolean;
        FIconWidth : Integer;
        FIconHeight: Integer;
        procedure InnerOnInfoTip(Sender: TObject; Item: TListItem; var InfoTip: string);
    protected
        { Protected declarations }
        procedure CNNotify(var Message: TWMNotifyLV); message CN_NOTIFY;
        procedure CreateParams(var Params: TCreateParams); override;
        procedure CreateWnd; override;
    public
        { Public declarations }
        constructor Create(AOwner: TComponent); override;
        destructor Destroy; override;
        function AddIcon(Caption: String; Image: TGPBitmap; InfoTip: String): TListItem;
        procedure SetIcon(index: Integer; Image: TGPBitmap; InfoTip: String);
    published
        { Published declarations }
        property NoScroll  : Boolean read FNoScroll write FNoScroll default False;
        property IconWidth : Integer read FIconWidth write FIconWidth default 0;
        property IconHeight: Integer read FIconHeight write FIconHeight default 0;
    end;

procedure Register;

implementation

procedure Register;
begin
    RegisterComponents('Samples', [TListViewEx]);
end;

constructor TListViewEx.Create(AOwner: TComponent);
begin
    inherited;
    if not(csDesigning in ComponentState) then
    begin
        OnInfoTip := InnerOnInfoTip;
    end;
    Self.DoubleBuffered := TRUE;
end;

destructor TListViewEx.Destroy;
var
    i     : Integer;
    ItemEx: TListItemEx;
begin
    for i := 0 to Items.Count - 1 do
    begin
        ItemEx := Items[i].Data;
        if ItemEx <> Nil then ItemEx.Free();
    end;
    inherited;
end;

procedure TListViewEx.CreateParams(var Params: TCreateParams);
begin
    inherited CreateParams(Params);
    if FNoScroll then Params.Style := Params.Style or LVS_NOSCROLL;
end;

procedure TListViewEx.CreateWnd;
begin
    inherited;
    if (FIconWidth > 0) or (FIconHeight > 0) then
    begin
        ListView_SetIconSpacing(Handle, FIconWidth, FIconHeight);
    end;
end;

procedure TListViewEx.CNNotify(var Message: TWMNotifyLV);
var
    ItemEx  : TListItemEx;
    Rect    : TRect;
    Graphics: TGPGraphics;
    x, y    : Integer;
    index   : Integer;
begin
    inherited;
    if Message.NMHdr.code <> NM_CUSTOMDRAW then exit;
    if (Message.NMCustomDraw.dwDrawStage = CDDS_PREPAINT) then
    begin
        Message.Result := CDRF_NOTIFYITEMDRAW;;
    end else if (Message.NMCustomDraw.dwDrawStage = CDDS_ITEMPREPAINT) then
    begin
        Message.Result := CDRF_NOTIFYPOSTPAINT;;
    end else if Message.NMCustomDraw.dwDrawStage = CDDS_ITEMPOSTPAINT then
    begin
        index := Message.NMCustomDraw.dwItemSpec;
        if index = -1 then exit;
        ItemEx := Self.Items[index].Data;
        if ItemEx = nil then exit;
        if ItemEx.Image = nil then exit;
        ListView_GetItemRect(Handle, index, Rect, LVIR_ICON);
        Graphics := TGPGraphics.Create(Message.NMCustomDraw.hdc);
        x := (Rect.Width - ItemEx.Image.GetWidth()) div 2;
        y := (Rect.Height - ItemEx.Image.GetHeight()) div 2;
        Graphics.DrawImage(ItemEx.Image, x + Rect.Left, y + Rect.Top);
        Graphics.Free();
    end;
end;

procedure TListViewEx.InnerOnInfoTip(Sender: TObject; Item: TListItem; var InfoTip: string);
var
    ItemEx: TListItemEx;
begin
    ItemEx := Item.Data;
    if ItemEx = nil then exit;
    InfoTip := ItemEx.InfoTip;
end;

function TListViewEx.AddIcon(Caption: String; Image: TGPBitmap; InfoTip: String): TListItem;
var
    ItemEx: TListItemEx;
begin
    ItemEx := TListItemEx.Create();
    ItemEx.Image := Image;
    ItemEx.InfoTip := InfoTip;

    Result := Self.Items.Add();
    Result.Data := ItemEx;
    Result.Caption := Caption;
end;

procedure TListViewEx.SetIcon(index: Integer; Image: TGPBitmap; InfoTip: String);
var
    ItemEx: TListItemEx;
    li    : TListItem;
begin
    li := Self.Items[index];
    ItemEx := li.Data;
    if ItemEx = nil then
    begin
        ItemEx := TListItemEx.Create();
        li.Data := ItemEx;
    end;
    ItemEx.Image := Image;
    ItemEx.InfoTip := InfoTip;
    li.Update();
end;

end.
