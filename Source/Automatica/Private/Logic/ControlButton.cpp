// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/ControlButton.h"
#include "Core/CAutomatica.h"


FControlButtonSetup::FControlButtonSetup()
{
	Command = Move_Forward;
	Modifier = Add;
	Type = EControlButtonType::Command;
	RelatedCounterButton = nullptr;
}

AControlButton::AControlButton()
{
	PrimaryActorTick.bCanEverTick = true;
	//bUseChannelBusOutput = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Button Holder"));

	CFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Frame"));
	CFace = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Face"));
	CText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text"));
	CIcon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Icon"));

	CFrame->SetupAttachment(RootComponent);
	CFace->SetupAttachment(RootComponent);
	CText->SetupAttachment(RootComponent);
	CIcon->SetupAttachment(CFace);
	CText->SetVisibility(true);

	CFace->SetCollisionProfileName("Interactable");

	PPressed = FParamAnimator(this);

	ScaleModifier = 0.4;
	IconScaleModifier = 3;

	bIsToggleMode = false;
}

void AControlButton::Press()
{
	if (Unit == nullptr || !PPressed.IsIdle()) return;

	PPressed.Set(1 - PPressed.GetTarget());

	switch (Setup.Type)
	{
	case Command:
		
		break;
	}
	Unit->AddCommand(Setup.Command);
}

void AControlButton::SetButtonSetup(const FControlButtonSetup ButtonSetup)
{
	Setup = ButtonSetup;
	OnConstruction(GetTransform());
}

void AControlButton::InteractSetup_Bind(UInteractionCatcher* Catcher)
{
	if (CatchersActive.Contains(Catcher)) return;
	CatchersActive.Add(Catcher);
	HandleInteractionCatcherUpdate();
}

void AControlButton::InteractSetup_Release(UInteractionCatcher* Catcher)
{
	if (!CatchersActive.Contains(Catcher)) return;
	CatchersActive.Remove(Catcher);
	HandleInteractionCatcherUpdate();
}

void AControlButton::BeginPlay()
{
	Super::BeginPlay();

	PPressed.OverrideValue(0);
	PPressed.SetInterpolator(AnimationCurve);
	PPressed.SetTime(0.2);
	
	CText->SetVisibility(false);
	if (ButtonOverlayInstance)
		ButtonOverlayInstance->SetScalarParameterValue("Visible", 0);
}

void AControlButton::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ButtonOverlay)
		ButtonOverlayInstance = UMaterialInstanceDynamic::Create(ButtonOverlay, this);

	if (PlaneMesh)
		CIcon->SetStaticMesh(PlaneMesh);

	float Scale = 1;
	float IconScale = 1;
	float Rotate = 0;
	bIsToggleMode = false;
	switch(Setup.Type)
	{
	case Command:
		if (FCommandSettingDrawer Drawer; CommandMap && CommandMap->GetCommandSettingDrawer(Setup.Command, Drawer))
		{
			CIcon->SetMaterial(0, Drawer.Material);
			CText->SetText(Drawer.DisplayName);
			if (Drawer.bUseInnerExtent)
			{
				ConstructShape(Round);
				bIsToggleMode = true;
			} else
			{
				ConstructShape(Squared);
				IconScale = 1.35;
			}
		} else
		{
			CText->SetText(NSLOCTEXT("Button", "Unknown", "Unbekannt"));
		}
		break;
	case Backspace:
		Scale = 0.7;
		CIcon->SetMaterial(0, MatIconBackspace);
		ConstructShape(Squared);
		Rotate = 45;
		CText->SetText(NSLOCTEXT("Button", "Backspace", "Löschen"));
		break;
	case CounterModifier:
		Scale = 0.3;
		ConstructShape(Round);
		CIcon->SetMaterial(0, Setup.Modifier == Subtract ? MatIconMinus : MatIconPlus);
		CText->SetText(FText::FromString(""));
		break;
	default:
		Scale = 1;
		ConstructShape(Squared);
		CText->SetText(NSLOCTEXT("Button", "Unknown", "Unbekannt"));
		break;
	}
	
	Scale *= ScaleModifier;

	CText->SetWorldSize(10);
	CText->TextRenderColor = FColor::White;
	CText->HorizontalAlignment = EHTA_Center;
	CText->VerticalAlignment = EVRTA_TextBottom;
	CText->SetRelativeLocation(FVector(0, -100 * Scale, 20));
	CText->SetRelativeRotation(FRotator(30, 90, 0));

	if (TextMaterial)
		CText->SetTextMaterial(TextMaterial);
	
	CIcon->SetRelativeLocation(FVector(0, 0, 53));
	CIcon->SetRelativeRotation(FRotator(0, -Rotate, 0));
	CIcon->SetRelativeScale3D(FVector(IconScaleModifier * IconScale));

	const FVector Scale3D(Scale);
	CFrame->SetRelativeScale3D(Scale3D);
	CFrame->SetRelativeRotation(FRotator(0, Rotate, 0));
	CFace->SetRelativeScale3D(Scale3D);
	CFace->SetRelativeRotation(FRotator(0, Rotate, 0));
}

void AControlButton::ConstructShape(const EControlButtonShape Shape) const
{
	if (Shape == Round)
	{
		if (FrameMeshRound) CFrame->SetStaticMesh(FrameMeshRound);
		if (FaceMeshRound) CFace->SetStaticMesh(FaceMeshRound);
	} else {
		if (FrameMeshSquare) CFrame->SetStaticMesh(FrameMeshSquare);
		if (FaceMeshSquare) CFace->SetStaticMesh(FaceMeshSquare);
	}
	CFrame->SetOverlayMaterial(ButtonOverlayInstance);
}

void AControlButton::HandleInteractionCatcherUpdate() const
{
	const bool bIsActive = CatchersActive.Num() > 0;

	CText->SetVisibility(bIsActive);
	if (ButtonOverlayInstance)
		ButtonOverlayInstance->SetScalarParameterValue("Visible", bIsActive);
}

void AControlButton::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	double Value;
	if (!PPressed.Get(Value))
	{
		if (Value == 1 && !bIsToggleMode)
		{
			PPressed.Set(0);
		} else
		{
			SetActorTickEnabled(false);
			return;	
		}
	}

	ButtonOverlayInstance->SetScalarParameterValue("Pressed", Value);
}

