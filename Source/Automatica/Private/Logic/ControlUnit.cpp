// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/ControlUnit.h"


// Sets default values
AControlUnit::AControlUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Control Unit"));

	Screen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
	Screen->SetupAttachment(RootComponent);
}

void AControlUnit::AddCommand(const ELogicControlType Command)
{
	FControlUnitCommandInstance Instance;
	
	Instance.Icon = Cast<AControlUnitIcon>(GetWorld()->SpawnActor(IconClass));
	Instance.Icon->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	//Instance.Icon = NewObject<AControlUnitIcon>(RootComponent, IconClass);
	Instance.Icon->SetCommand(Command);
	Instance.Layer = CurrentLayer;
	Commands.Add(Instance);

	if (Instance.Icon->SupportsNesting())
		CurrentLayer++;
}

void AControlUnit::Backspace()
{
	int Amount = Commands.Num();
	if (Amount <= 0) return;

	const auto ToDelete = Commands[Amount-1];
	ToDelete.Icon->InitSelfDestruct();
	Commands.RemoveAt(--Amount);

	// In case any commands are left, apply current layer of latest one
	if (Amount > 0)
	{
		auto Latest = Commands.Last();
		CurrentLayer = Latest.Layer;
	} else
	{
		CurrentLayer = 0;
	}
}

void AControlUnit::CloseNesting()
{
	if (CurrentLayer > 0)
		CurrentLayer--;
}

void AControlUnit::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ScreenMesh)
	{
		Screen->SetStaticMesh(ScreenMesh);
		Screen->SetWorldScale3D(FVector(ScreenWidth/ScreenMeshWidth, 1, ScreenHeight/ScreenMeshWidth));
	} else
	{
		Screen->SetStaticMesh(nullptr);
	}
}

// Called when the game starts or when spawned
void AControlUnit::BeginPlay()
{
	Super::BeginPlay();
	CurrentLayer = 0;
}

// Called every frame
void AControlUnit::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}



#if WITH_EDITORONLY_DATA
void AControlUnit::_D_AddCommand()
{
	AddCommand(_D_CommandToAdd);
}

void AControlUnit::_D_Backspace()
{
	Backspace();
}

void AControlUnit::_D_EndNested()
{
	CloseNesting();
}
#endif
