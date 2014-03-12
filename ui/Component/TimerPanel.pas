Unit TimerPanel;

Interface

Uses
	System.SysUtils, System.Classes, Vcl.Controls, Winapi.Messages, Winapi.Windows,
	Winapi.GDIPAPI, Winapi.GDIPOBJ;

Const
	TIMER_PANEL_WIDTH  = 640;
	TIMER_PANEL_HEIGHT = 480;

Type
	TTimerPanel = Class(TWinControl)
	Private
		{ Private declarations }
		FImage      : TGPBitmap;
		FImageBuffer: PDWORD;
		FPointer    : TGPGraphicsPath;
		FOrnament   : TGPGraphicsPath;
		FTickMark   : TGPGraphicsPath;
	Protected
		{ Protected declarations }
		Procedure WMEraseBkgnd(Var Message: TWmEraseBkgnd); Message WM_ERASEBKGND;
		Procedure WMPaint(Var Message: TWMPaint); Message WM_PAINT;
	Private
		curvature: Single;
		speed    : Single;
		ticks    : Integer;
		Procedure CreatePointer();
		Procedure CreateOrnament();
		Procedure CreateTickMark();
	Public
		{ Public declarations }
		Constructor Create(AOwner: TComponent); Override;
		Destructor Destroy; Override;
	Published
		{ Published declarations }
	End;

Procedure Register;

Implementation

Procedure Register;
Begin
	RegisterComponents('Samples', [TTimerPanel]);
End;

{ TTimerPanel }

Constructor TTimerPanel.Create(AOwner: TComponent);
Begin
	Inherited;
	Width := TIMER_PANEL_WIDTH;
	Height := TIMER_PANEL_HEIGHT;
	FImageBuffer := GetMemory(TIMER_PANEL_WIDTH * TIMER_PANEL_HEIGHT * 4);
	FillChar(FImageBuffer^, TIMER_PANEL_WIDTH * TIMER_PANEL_HEIGHT * 4, $FF);
	FImage := TGPBitmap.Create(TIMER_PANEL_WIDTH, TIMER_PANEL_HEIGHT, TIMER_PANEL_WIDTH * 4, PixelFormat32bppRGB, PBYTE(FImageBuffer));
	CreatePointer();
	CreateOrnament();
	CreateTickMark();

	speed := 30.0;
	curvature := 56.0;
	ticks := 9;

End;

Destructor TTimerPanel.Destroy;
Begin
	FImage.Free();
	FPointer.Free();
	FOrnament.Free();
	FTickMark.Free();
	FreeMemory(FImageBuffer);
	Inherited;
End;

Procedure TTimerPanel.CreatePointer();
Const
	curveToPoints: Array [0 .. 30] Of TGPPointF =
	  (
	  (X: 134.39; Y: 218.05),

	  (X: 134.39; Y: 218.05),
	  (X: 137.95; Y: 219.75),
	  (X: 137.95; Y: 219.75),

	  (X: 137.95; Y: 219.75),
	  (X: 141.78; Y: 357.55),
	  (X: 141.78; Y: 357.55),

	  (X: 141.78; Y: 357.55),
	  (X: 145.39; Y: 359.54),
	  (X: 151.13; Y: 356.31),

	  (X: 156.87; Y: 353.08),
	  (X: 158.95; Y: 349.86),
	  (X: 158.95; Y: 349.86),

	  (X: 158.95; Y: 349.86),
	  (X: 134.49; Y: 415.99),
	  (X: 134.49; Y: 415.99),

	  (X: 134.49; Y: 415.99),
	  (X: 110.02; Y: 349.86),
	  (X: 110.02; Y: 349.86),

	  (X: 110.02; Y: 349.86),
	  (X: 112.10; Y: 353.08),
	  (X: 117.84; Y: 356.31),

	  (X: 123.58; Y: 359.54),
	  (X: 127.19; Y: 357.55),
	  (X: 127.19; Y: 357.55),

	  (X: 127.19; Y: 357.55),
	  (X: 131.02; Y: 219.75),
	  (X: 131.02; Y: 219.75),

	  (X: 131.02; Y: 219.75),
	  (X: 134.39; Y: 218.05),
	  (X: 134.39; Y: 218.05)
	  );
Var
	points: PGPPointF;
Begin
	FPointer := TGPGraphicsPath.Create();
	points := @curveToPoints[0];
	FPointer.AddBeziers(points, 31);
	FPointer.CloseFigure();
End;

Procedure TTimerPanel.CreateOrnament();
Const
	curveToPoints: Array [0 .. 18] Of TGPPointF =
	  (
	  (X: 251.77; Y: 135.25),

	  (X: 252.88; Y: 144.62),
	  (X: 260.31; Y: 146.12),
	  (X: 260.31; Y: 146.12),

	  (X: 260.31; Y: 146.12),
	  (X: 266.06; Y: 343.75),
	  (X: 266.06; Y: 343.75),

	  (X: 266.06; Y: 343.75),
	  (X: 257.38; Y: 346.25),
	  (X: 251.79; Y: 355.06),

	  (X: 245.5; Y: 345.88),
	  (X: 237.52; Y: 343.75),
	  (X: 237.52; Y: 343.75),

	  (X: 237.52; Y: 343.75),
	  (X: 243.27; Y: 146.12),
	  (X: 243.27; Y: 146.12),

	  (X: 243.27; Y: 146.12),
	  (X: 250.25; Y: 144.75),
	  (X: 251.77; Y: 135.25)
	  );
Var
	points: PGPPointF;
Begin
	FOrnament := TGPGraphicsPath.Create();
	points := @curveToPoints[0];
	FOrnament.AddBeziers(points, 31);
	FOrnament.CloseFigure();
End;

Procedure TTimerPanel.CreateTickMark();
Const
	curveToPoints: Array [0 .. 12] Of TGPPointF =
	  (
	  (X: 225.81; Y: 358.28),

	  (X: 225.81; Y: 358.28),
	  (X: 222.7; Y: 385.11),
	  (X: 222.7; Y: 385.11),

	  (X: 222.7; Y: 385.11),
	  (X: 235.97; Y: 385.11),
	  (X: 235.97; Y: 385.11),

	  (X: 235.97; Y: 385.11),
	  (X: 232.86; Y: 358.28),
	  (X: 232.86; Y: 358.28),

	  (X: 232.86; Y: 358.28),
	  (X: 225.81; Y: 358.28),
	  (X: 225.81; Y: 358.28)
	  );
Var
	points: PGPPointF;
Begin
	FTickMark := TGPGraphicsPath.Create();
	points := @curveToPoints[0];
	FTickMark.AddBeziers(points, 31);
	FTickMark.CloseFigure();
End;

Procedure TTimerPanel.WMEraseBkgnd(Var Message: TWmEraseBkgnd);
Begin
	Message.Result := 1;
End;

function fabs(x:Single):Single;
begin
	if x < 0 then Result := -x
    else Result := x;
end;

Procedure TTimerPanel.WMPaint(Var Message: TWMPaint);
Var
	DC      : HDC;
	PS      : TPaintStruct;
	Graphics: TGPGraphics;
	pen     : TGPPen;

Const
	inset       = 8.0;
	shadowAngle = -35.0;

Var
	sweepAngle: Single;
	sAngle    : Single;
	eAngle    : Single;
    center:TGPPointF;
    spread, radius, dip:Single;

	tickSize : Single;
	labelSize : Single;
	indicatorSize : Single;
	centerSize : Single;
	ornamentSize : Single;
	paddingSize : Single;
    bottomOfText:Single;

	ornamentTop:Single;
	ornamentBottom:Single;

	armTop:Single;
	armBottom:Single;

Begin
	DC := BeginPaint(Handle, PS);
	Graphics := TGPGraphics.Create(DC);
	Graphics.SetInterpolationMode(InterpolationModeHighQuality);
	Graphics.SetCompositingQuality(CompositingQualityHighQuality);
	Graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	Graphics.DrawImage(FImage, 0, 0, TIMER_PANEL_WIDTH, TIMER_PANEL_HEIGHT);
	pen := TGPPen.Create($FFFF0000, 2.0);
	pen.SetLineJoin(LineJoinRound);
	pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
	Graphics.DrawPath(pen, FPointer);
	Graphics.Free();
	EndPaint(Handle, PS);


	sweepAngle = 270.0 * (curvature / 100.0) + 45.0;
	sAngle = 90 - sweepAngle / 2;
	eAngle = 90 + sweepAngle / 2;

	center.x = TIMER_PANEL_WIDTH/2.0;
	spread = TIMER_PANEL_WIDTH;
	dip = fabs(sin(sAngle*pi/180));

	radius = (TIMER_PANEL_HEIGHT/(dip+1.0)) - inset;
	center.y = radius*dip + (inset/2.0);

	if (radius > spread/2.0 - inset) then radius = spread/2.0 - inset;


	 tickSize = radius * 5.0/100.0;
	labelSize = radius * 9.0/100.0;
	indicatorSize = radius * 55.0/100.0;
	centerSize = radius * 15.0/100.0;
	ornamentSize = radius * 40.0/100.0;
	paddingSize = radius * 2.0/100.0;

	 bottomOfText = radius - tickSize - labelSize - paddingSize*3;


	ornamentTop = (bottomOfText + centerSize + ornamentSize)/2.0;
	ornamentBottom = ornamentTop - ornamentSize;

	armTop = (bottomOfText + centerSize + indicatorSize)/2.0;
	armBottom = armTop - indicatorSize;


	NSBezierPath *frame = [[[NSBezierPath alloc] init] autorelease];
	[frame appendBezierPathWithArcWithCenter:center radius:centerSize startAngle:eAngle endAngle:sAngle clockwise:YES];
	[frame appendBezierPathWithArcWithCenter:center radius:radius startAngle:sAngle endAngle:eAngle];
	[frame closePath];
	[frame setLineWidth: 0.5];
	[frame setLineJoinStyle:NSRoundLineJoinStyle];

		(* fill with light blue, stroke with black.*)
	[[NSColor colorWithCalibratedRed:.95 green:.95 blue:1.0 alpha: 1.0] set];
	[frame fillWithShadowAtDegrees:shadowAngle withDistance: inset/2];
    [[NSColor blackColor] set];
	[frame stroke];

		(* save a copy of the bounding frame *)
	[self setBoundingFrame: frame];

		(* construct a tick mark path centered at the origin *)
	NSBezierPath *tickmark = self.tickMarkPath;
	[tickmark transformUsingAffineTransform:
		[[NSAffineTransform transform]
				scaleBounds: [tickmark bounds] toHeight: tickSize centeredAboveOrigin: (radius - paddingSize - tickSize)]];

		(* construct a small background decoration centered at the origin *)
	NSBezierPath *ornament = [self ornamentPath];
	[ornament transformUsingAffineTransform:
		[[NSAffineTransform transform]
				scaleBounds: [ornament bounds] toHeight: ornamentSize centeredAboveOrigin: ornamentBottom]];

		(* construct a the indicator pointer centered at the origin *)
	NSBezierPath *speedPointer = [self speedPointerPath];
	[speedPointer transformUsingAffineTransform:
		[[NSAffineTransform transform]
				scaleBounds: [speedPointer bounds] toHeight: indicatorSize centeredAboveOrigin: armBottom]];

		(* blending colors for the ornaments and tick marks *)
	NSColor *startColor = [NSColor greenColor];
	NSColor *midColor = [NSColor yellowColor];
	NSColor *endColor = [NSColor redColor];

		(* calculate the font to use for the label *)
	NSFont *labelFont = [[NSFont labelFontOfSize:labelSize] printerFont];

		(* transforms used during drawing *)
	NSAffineTransform *transform;
	NSAffineTransform *identity = [NSAffineTransform transform];

		(* calculate the pointer arm's total sweep *)
	float pointerWidth = speedPointer.bounds.size.width;
		 (* border on each end of sweep to accomodate width of pointer *)
	float tickoutside = ((pointerWidth*.67) / (radius/2.0)) * 180/pi;
		 (* total arm sweep will be background sweep minus border on each side *)
	float armSweep = sweepAngle - tickoutside*2;

		(* calculate the number of tick mark labels *)
	float ornamentWidth = ornament.bounds.size.width;
		 (* border on each end of sweep to accomodate width of pointer *)
	float ornamentDegrees = (ornamentWidth / ornamentBottom) * 180/pi;
		(* calculate the maximum number of ornaments that will fit *)
	int maxTicks = truncf(armSweep/ornamentDegrees);
		(* limit the number of ticks we'll draw by the maximum *)
	int limitedTicks = ((self.ticks > maxTicks) ? maxTicks : self.ticks);
		(* calculate the number of degrees between tickmarks *)
	float tickdegrees = (armSweep)/((float)limitedTicks-1.0);

		(* loop drawing tick mark labels and ornaments *)
	int i;
    for (i=0; i < limitedTicks; i++) {

			(* set up the transform matrix so we're drawing
			at the appropriate angle.  Here, we reset the xform matrix,
			center it on the axis of our dial, and then rotate it to the
			nth position. *)
		transform = [[NSAffineTransform alloc] initWithTransform: identity]; (* reset the xform matrix *)
		[transform translateXBy:center.x yBy:center.y]; (* set the center to the center of our dial *)
		[transform rotateByDegrees: ( (limitedTicks-i-1)*tickdegrees + tickoutside + sAngle - 90 ) ];
		[transform concat];

		(* calculate the label string to display *)
		float displayedValue = roundf((float) (100.0/(limitedTicks-1))*i);
		NSString *theLabel = [NSString stringWithFormat:@"%.0f", displayedValue];

		(* draw the tick mark label string using a NSBezierPath *)
		NSBezierPath *nthLabelPath = [theLabel bezierWithFont:labelFont];
		[nthLabelPath transformUsingAffineTransform:
			[[NSAffineTransform transform]
					scaleBounds: [nthLabelPath bounds] toHeight:[nthLabelPath bounds].size.height
						centeredAboveOrigin:bottomOfText-[labelFont descender]]];
		[nthLabelPath setLineWidth: 0.5];
		[[NSColor blueColor] set];
		[nthLabelPath fill];
		[[NSColor blackColor] set];
		[nthLabelPath stroke];

			(* draw the ornament.
			Ramp from green to yellow and then from yellow to red. *)
		float cfraction = ((float) i / (float)(limitedTicks-1));
		if ( cfraction <= 0.5 )
			[[startColor blendedColorWithFraction:cfraction*2 ofColor:midColor] set];
		else
            [[midColor blendedColorWithFraction:(cfraction-0.5)*2 ofColor:endColor] set];

			(* fill the tickmark and ornament *)
		[ornament fill];
		[tickmark fill];

			(* stroke the tickmark and ornament *)
		[[NSColor blackColor] set];
		[tickmark stroke];
		[ornament stroke];

			(* set the coordinates back the way they were *)
		[transform invert];
		[transform concat];

        [transform release];
	}

		(* translate and rotate the indicator arrow to its final position *)
	NSAffineTransform *positionSpeedometer = [NSAffineTransform transform];
	[positionSpeedometer translateXBy:center.x yBy:center.y]; (* set the center to the center of our dial *)
	[positionSpeedometer rotateByDegrees: (armSweep+tickoutside-(armSweep/100)*speed + sAngle) - 90 ];
	[speedPointer transformUsingAffineTransform: positionSpeedometer];

		(* draw the pointer in red, stroke in black *)
	[[NSColor redColor] set];
	[speedPointer fillWithShadowAtDegrees:shadowAngle withDistance: inset/2];
    [[NSColor blackColor] set];
	[speedPointer stroke];

		(* record arm information for the drag routine *)
	[self saveSweepWithCenter:center startAngle:sAngle+tickoutside endAngle:sAngle+tickoutside+armSweep];

End;

End.
