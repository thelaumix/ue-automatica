// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/ControlButton.h"

#include "Core/CAutomatica.h"
#include "Logic/ControlUnit.h"


AControlButton::AControlButton()
{
	PrimaryActorTick.bCanEverTick = true;
	//bUseChannelBusOutput = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Button Holder"));

	CFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Frame"));
	CFace = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Face"));

	CFrame->SetupAttachment(RootComponent);
	CFace->SetupAttachment(RootComponent);

	ScaleModifier = 0.4;
	IconScaleModifier = 3;
}

void AControlButton::Press()
{
	for(const auto Actor: UAutomatica::GetRegisteredLogicActors(this))
	{
		if (const auto Unit = Cast<AControlUnit>(Actor))
			Unit->AddCommand(Setup.Command);
	}
}

void AControlButton::SetButtonSetup(const FControlButtonSetup ButtonSetup)
{
	Setup = ButtonSetup;
	OnConstruction(GetTransform());
}

void AControlButton::BeginPlay()
{
	Super::BeginPlay();
}

void AControlButton::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ConstructIcon(Setup.Type == Command);

	float Scale = 1;
	switch(Setup.Type)
	{
	case Command:
		if (CIcon)
		{
			CIcon->SetCommand(Setup.Command);
			if (CIcon->SupportsNesting())
				ConstructShape(Round);
			else
				ConstructShape(Squared);
		}
		break;
	case Backspace:
		Scale = 0.8;
		ConstructShape(Squared);
		break;
	case CounterModifier:
		Scale = 0.3;
		ConstructShape(Round);
		break;
	default:
		Scale = 1;
		ConstructShape(Squared);
		break;
	}

	if (CIcon)
	{
		CIcon->SetActorRelativeLocation(FVector(0, 0, 125 * ScaleModifier));
		CIcon->SetActorRelativeRotation(FRotator(0, 0, -90));
		CIcon->SetScale(Scale * IconScaleModifier);
	}
	
	Scale *= ScaleModifier;

	const FVector Scale3D(Scale);
	CFrame->SetRelativeScale3D(Scale3D);
	CFace->SetRelativeScale3D(Scale3D);
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
}

void AControlButton::ConstructIcon(const bool bMakeIcon)
{
	if (bMakeIcon && CIcon == nullptr && IconClass)
	{
		CIcon = Cast<AControlUnitIcon>(GetWorld()->SpawnActor(IconClass));
		CIcon->AttachToComponent(CFace, FAttachmentTransformRules::SnapToTargetIncludingScale);
		CIcon->SetAnimated(false);
		CIcon->SetCentered(true);
	} else if (!bMakeIcon && CIcon != nullptr)
	{
		CIcon->Destroy();
		CIcon = nullptr;
	}
}

void AControlButton::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

