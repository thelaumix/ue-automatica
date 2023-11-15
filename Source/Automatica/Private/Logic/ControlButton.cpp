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
	CIcon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Icon"));

	CFrame->SetupAttachment(RootComponent);
	CFace->SetupAttachment(RootComponent);
	CIcon->SetupAttachment(CFace);

	CFace->SetCollisionProfileName("Interactable");

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
}

void AControlButton::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (PlaneMesh)
		CIcon->SetStaticMesh(PlaneMesh);

	float Scale = 1;
	float IconScale = 1;
	float Rotate = 0;
	switch(Setup.Type)
	{
	case Command:
		if (FCommandSettingDrawer Drawer; CommandMap && CommandMap->GetCommandSettingDrawer(Setup.Command, Drawer))
		{
			CIcon->SetMaterial(0, Drawer.Material);
			if (Drawer.bUseInnerExtent)
			{
				ConstructShape(Round);
			} else
			{
				ConstructShape(Squared);
				IconScale = 1.35;
			}
		}
		break;
	case Backspace:
		Scale = 0.7;
		CIcon->SetMaterial(0, MatIconBackspace);
		ConstructShape(Squared);
		Rotate = 45;
		break;
	case CounterModifier:
		Scale = 0.3;
		ConstructShape(Round);
		CIcon->SetMaterial(0, Setup.Modifier == Subtract ? MatIconMinus : MatIconPlus);
		break;
	default:
		Scale = 1;
		ConstructShape(Squared);
		break;
	}

	
	CIcon->SetRelativeLocation(FVector(0, 0, 53));
	CIcon->SetRelativeRotation(FRotator(0, -Rotate, 0));
	CIcon->SetRelativeScale3D(FVector(IconScaleModifier * IconScale));
	
	Scale *= ScaleModifier;

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
}

void AControlButton::HandleInteractionCatcherUpdate()
{
	bool bIsActive = CatchersActive.Num() > 0;
}

void AControlButton::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

