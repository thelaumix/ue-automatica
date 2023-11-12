// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/ControlUnitIcon.h"


// Sets default values
AControlUnitIcon::AControlUnitIcon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("ControlUnitIcon"));
	RootComponent = Root;

	// Create mesh and attach to an empty parent root.
	// This is to encapsulate an offset icon.
	Icon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Icon"));
	Icon->SetupAttachment(RootComponent);

	Icon->SetRelativeScale3D(FVector::OneVector);

	MeshWidth = 30;
	Scale = 1;
	InnerExtent = 0;
	Visibility = FParamAnimator(this);
	InnerExtent_A = FParamAnimator(this);
	bAnimated = true;
}

void AControlUnitIcon::SetCommand(const ELogicControlType NewCommand)
{
	Command = NewCommand;
	SetCommand();
	SetterAftermath();
}

ELogicControlType AControlUnitIcon::GetCommand() const
{
	return Command;
}

void AControlUnitIcon::SetScale(const float NewScale)
{
	Scale = NewScale;
	SetterAftermath();
}

float AControlUnitIcon::GetScale() const
{
	return Scale;
}

void AControlUnitIcon::SetAnimated(const bool bNewAnimated)
{
	bAnimated = bNewAnimated;
	
	const double NewAniTime = bAnimated ? TransitionTime : 0;
	Visibility.SetTime(NewAniTime);
	InnerExtent_A.SetTime(NewAniTime);
}

bool AControlUnitIcon::GetAnimated() const
{
	return bAnimated;
}

float AControlUnitIcon::GetWidth() const
{
	return ((MeshWidth * Drawer.WidthPercentage / 100) + GetActualInnerExtent()) * Scale * Visibility.Get();
}

float AControlUnitIcon::GetLeftExtentPadding() const
{
	if (!Drawer.bUseInnerExtent) return 0;
	const float ActualWidth = MeshWidth * Drawer.WidthPercentage / 100 * Scale;
	return ActualWidth * Drawer.ExtentBorderLeftPercentage / 100;
}

void AControlUnitIcon::SetInnerExtent(const float NewInnerExtent)
{
	InnerExtent_A.Set(NewInnerExtent);
	SetActorTickEnabled(true);
	SetterAftermath();
}

bool AControlUnitIcon::SupportsNesting() const
{
	return Drawer.bUseInnerExtent;
}

void AControlUnitIcon::InitSelfDestruct()
{
	Destroy();
	// TODO: Implement ACTUAL destuct mechanism
}

void AControlUnitIcon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Icon->SetStaticMesh(IconMesh);
	SetCommand();
	Visibility.OverrideValue(1);
	InnerExtent_A.OverrideValue(InnerExtent);
	SetterAftermath();
}

void AControlUnitIcon::BeginPlay()
{
	Super::BeginPlay();

	SetAnimated(bAnimated);
	Visibility.SetInterpolator(TransitionCurve);
	InnerExtent_A.SetInterpolator(TransitionCurve);

	Visibility.OverrideValue(0);
	Visibility.Set(1);
	InnerExtent_A.OverrideValue(InnerExtent);

	IconMat = UMaterialInstanceDynamic::Create(Icon->GetMaterial(0), this);
	IconMat->SetScalarParameterValue("Mask", 0);
}

void AControlUnitIcon::TickProc_Visibility(uint8 &UnfinishedProcessors)
{
	double V_Visibility;
	if (!Visibility.Get(V_Visibility))
		return;
	
	UnfinishedProcessors++;
	IconMat->SetScalarParameterValue("Mask", V_Visibility);
}

void AControlUnitIcon::TickProc_InnerExtent(uint8 &UnfinishedProcessors)
{
	double V_InnerExtent;
	if (!InnerExtent_A.Get(V_InnerExtent))
		return;
	
	UnfinishedProcessors++;
	InnerExtent = V_InnerExtent;
}

void AControlUnitIcon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	uint8 UnfinishedProcessors = 0;
	TickProc_Visibility(UnfinishedProcessors);
	TickProc_InnerExtent(UnfinishedProcessors);

	if (UnfinishedProcessors > 0)
		SetterAftermath();
	else
		SetActorTickEnabled(false);
}

void AControlUnitIcon::Destroyed()
{
	Super::Destroyed();
}

void AControlUnitIcon::SetCommand()
{
	if (CommandMap && CommandMap->GetCommandSettingDrawer(Command, Drawer))
	{
		Icon->SetMaterial(0, Drawer.Material);
	} else
	{
		InnerExtent_A.Set(0);
		Icon->SetMaterial(0, nullptr);
	}
}

void AControlUnitIcon::SetterAftermath()
{
	const float ActualWidth = MeshWidth * Drawer.WidthPercentage / 100;
	float LeftOffset = (MeshWidth / 2) - (ActualWidth / 2);

	LeftOffset -= (1 - Visibility.Get()) * (ActualWidth / 2);
	
	Icon->SetRelativeLocation(FVector(-LeftOffset * Scale, 0.5, 0));
	
	Icon->SetRelativeScale3D(FVector(Scale + (GetActualInnerExtent() / ActualWidth), Scale, Scale));

	
#if WITH_EDITORONLY_DATA
	_ActualWidth = GetWidth();
	_LeftPadding = GetLeftExtentPadding();
#endif
}

float AControlUnitIcon::GetActualInnerExtent() const
{
	if (!Drawer.bUseInnerExtent) return 0;

	const float ActualWidth = MeshWidth * Drawer.WidthPercentage / 100 * Scale;
	const float ExtentAmount = ActualWidth * (Drawer.ExtentBorderLeftPercentage + Drawer.ExtentBorderRightPercentage) / 100;
	const float AvailableExtentSpace = ActualWidth - ExtentAmount;
	
	if (InnerExtent > AvailableExtentSpace)
		return InnerExtent - AvailableExtentSpace;
	return 0;
}

