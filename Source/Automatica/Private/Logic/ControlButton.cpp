// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/ControlButton.h"

#include "FMODBlueprintStatics.h"
#include "Core/CAutomatica.h"
#include "Kismet/GameplayStatics.h"

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
	if (!PPressed.IsIdle()) return;

	OnPressed.Broadcast();

	PPressed.Set(1 - PPressed.GetTarget());
	SetActorTickEnabled(true);

	UFMODEvent* EventToPlay = BtSoundHi;

	if (Unit)
	{
		switch (Setup.Type)
		{
		case Command:
			if (!Unit->AddCommand(Setup.Command))
				EventToPlay = BtSoundFail;
			break;
		case Function:
			switch (Setup.Function)
			{
		case Backspace:
			Unit->Backspace();
				EventToPlay = BtSoundLo;
				break;
		case RunSequence:
			PlaySequences();
				break;
		default: break;
			}
			break;
		default: break;;
		}
	}

	UFMODBlueprintStatics::PlayEventAttached(EventToPlay, RootComponent, NAME_None, FVector::Zero(), EAttachLocation::SnapToTarget, true, true, true);
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
	PPressed.SetTime(0.1);
	
	CFrame->SetCustomDepthStencilValue(0);
	CFace->SetCustomDepthStencilValue(0);
	
	CText->SetVisibility(false);
}

void AControlButton::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (PlaneMesh)
		CIcon->SetStaticMesh(PlaneMesh);

	float Scale = 1;
	float IconScale = 1;
	float Rotate = 0;
	bIsToggleMode = false;

	float FaceMatColorMultiplier = 1;

	FLinearColor FaceMatColor = FColor::White;
	
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
	case Function:
		Scale = 0.7;
		ConstructShape(Squared);
		Rotate = 45;

		switch (Setup.Function)
		{
		case Backspace:
			CIcon->SetMaterial(0, MatIconBackspace);
			CText->SetText(NSLOCTEXT("Button", "Backspace", "Löschen"));
			FaceMatColor = FColor::Red;
			break;
		case RunSequence:
			CIcon->SetMaterial(0, MatIconRunSequence);
			CText->SetText(NSLOCTEXT("Button", "RunSequence", "Ausführen"));
			FaceMatColor = FColor::Green;
			break;
		default: break;
		}
		
		FaceMatColorMultiplier = 2;
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

	if (FaceMatInstance)
	{
		FaceMatInstance->SetScalarParameterValue("Color Multiplier", FaceMatColorMultiplier);
		FaceMatInstance->SetVectorParameterValue("Color", FaceMatColor);	
	}
	
	Scale *= ScaleModifier;

	CText->SetWorldSize(10);
	CText->TextRenderColor = FColor::White;
	CText->HorizontalAlignment = EHTA_Center;
	CText->VerticalAlignment = EVRTA_TextBottom;
	CText->SetRelativeLocation(FVector(0, -7 * Scale, 20));
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
	
	CFrame->SetMobility(EComponentMobility::Movable);
	CFace->SetMobility(EComponentMobility::Movable);
	CIcon->SetMobility(EComponentMobility::Movable);
	CText->SetMobility(EComponentMobility::Movable);
}

void AControlButton::ConstructShape(const EControlButtonShape Shape)
{
	if (Shape == Round)
	{
		if (FrameMeshRound) CFrame->SetStaticMesh(FrameMeshRound);
		if (FaceMeshRound) CFace->SetStaticMesh(FaceMeshRound);
	} else {
		if (FrameMeshSquare) CFrame->SetStaticMesh(FrameMeshSquare);
		if (FaceMeshSquare) CFace->SetStaticMesh(FaceMeshSquare);
	}

	CFrame->SetRenderCustomDepth(true);
	CFace->SetRenderCustomDepth(true);
	CFrame->SetCustomDepthStencilValue(0);
	CFace->SetCustomDepthStencilValue(0);

	if (CFaceMaterial)
	{
		FaceMatInstance = UMaterialInstanceDynamic::Create(CFaceMaterial, CFace);
		CFace->SetMaterial(0, FaceMatInstance);
	}
}

void AControlButton::HandleInteractionCatcherUpdate() const
{
	const bool bIsActive = CatchersActive.Num() > 0;

	CText->SetVisibility(bIsActive);
	CFrame->SetCustomDepthStencilValue(bIsActive);
	CFace->SetCustomDepthStencilValue(bIsActive);

	if (FaceMatInstance)
		FaceMatInstance->SetScalarParameterValue("Hover", bIsActive);
}

void AControlButton::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	double Value;
	if (!PPressed.Get(Value))
	{
		
		if (Value >= 1 && !bIsToggleMode)
		{
			PPressed.Set(0);
		} else
		{
			SetActorTickEnabled(false);
		}
	}

	const auto Scale = CFace->GetRelativeScale3D();
	CFace->SetRelativeLocation(FVector(0,0, -10 * Value * Scale.X));

	if (FaceMatInstance)
		FaceMatInstance->SetScalarParameterValue("Pressed", Value);
}

void AControlButton::PlaySequences()
{
	TArray<AActor*> Screens;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AControlUnit::StaticClass(), Screens);
	for(const auto Actor: Screens)
	{
		if (const auto Screen = Cast<AControlUnit>(Actor))
			Screen->PlaySequence();
	}
}

