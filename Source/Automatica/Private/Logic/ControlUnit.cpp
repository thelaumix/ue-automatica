// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/ControlUnit.h"

#include "Core/CAutomatica.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"


AControlUnit::AControlUnit()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseChannelBusOutput = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Control Unit"));

	Screen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
	Screen->SetupAttachment(RootComponent);

	bIsPlaying = false;
	SequencePointer = 0;
	SequenceLastTime = 0;
}

void AControlUnit::AddCommand(const ELogicControlType Command)
{

	auto Icon = Cast<AControlUnitIcon>(GetWorld()->SpawnActor(IconClass));
	Icon->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	//Instance.Icon = NewObject<AControlUnitIcon>(RootComponent, IconClass);
	Icon->SetCommand(Command);
	Icon->Layer = CurrentLayer;
	Commands.Add(Icon);
	CommandChildBuffer.Add(Icon);

	const double Scale = 1 * FMath::Pow(LayerScaleFactor, CurrentLayer);
	Icon->SetScale(Scale);
	CommandIndexPtr++;

	if (Icon->SupportsNesting())
	{
		ContainerIndexStack.Add(Commands.Num() - 1);
		CurrentLayer++;
	}
}

void AControlUnit::Backspace()
{
	if (CommandIndexPtr < 0) return;

	const auto Icon = Commands[CommandIndexPtr];
	if (Icon->SupportsNesting())
	{
		ContainerIndexStack.Remove(CommandIndexPtr);
	}
	Icon->InitSelfDestruct();
	Commands.RemoveAt(CommandIndexPtr--);

	// In case any commands are left, apply current layer of latest one
	if (CommandIndexPtr >= 0)
	{
		const auto Last = Commands.Last();
		CurrentLayer = Last->Layer;
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

void AControlUnit::PlaySequence()
{
	if (Commands.Num() == 0) return;

	UAutomatica::SetControlUnitPlaying(this, true);
	
	SequencePointer = -1;
	SequenceLastTime = UGameplayStatics::GetTimeSeconds(this);
	bIsPlaying = true;
}

void AControlUnit::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ScreenMesh)
	{
		Screen->SetStaticMesh(ScreenMesh);
		Screen->SetWorldScale3D(FVector(ScreenWidth/ScreenMeshWidth, 1, ScreenHeight/ScreenMeshWidth));
		if (ScreenMaterial)
			Screen->SetMaterial(0, ScreenMaterial);
	} else
	{
		Screen->SetStaticMesh(nullptr);
	}
}

void AControlUnit::BeginPlay()
{
	Super::BeginPlay();
	CurrentLayer = 0;
	CommandIndexPtr = -1;
}

void AControlUnit::HandleLayer(const uint8 BatchLayer, int& Index, double LeftOffset, AControlUnitIcon* Container)
{

	// In case there is no container but handler shall be a nested layer, abort
	if (BatchLayer > 0 && Container == nullptr)
		return;

	double WidthSum = 0;
	
	while(Index < CommandChildBuffer.Num())
	{
		const auto Icon = CommandChildBuffer[Index++];

		if (Icon == nullptr || !IsValid(Icon))
		{
			CommandChildBuffer.Remove(Icon);
			Index--;
			continue;
		}

		// If the instance is a higher layer, abort
		if (Icon->Layer < BatchLayer)
		{
			// Decrease index again
			Index--;
			break;
		}

		if (WidthSum > 0)
		{
			WidthSum += GridGap;
			LeftOffset += GridGap;
		}
		
		Icon->SetActorRelativeLocation(FVector(LeftOffset, 0, 0));

		if (Icon->SupportsNesting())
		{
			HandleLayer(Icon->Layer+1, Index, LeftOffset + Icon->GetLeftExtentPadding(), Icon);
		}
		WidthSum += Icon->GetWidth();
		LeftOffset += Icon->GetWidth();
	}

	if (Container)
		Container->SetInnerExtent(WidthSum);
}

// Called every frame
void AControlUnit::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	int Index = 0;
	HandleLayer(0, Index, ScreenPadding);
	
	if (bIsPlaying)
	{
		double Time = UGameplayStatics::GetTimeSeconds(this);
		if (Time >= SequenceLastTime + SequenceTickTime)
		{
			++SequencePointer;

			int CmdNum = Commands.Num();

			if (SequencePointer >= CmdNum)
			{
				UAutomatica::SetControlUnitPlaying(this, false);
				bIsPlaying = false;
			} else
			{
				UAutomatica::SendLogicCommand(this, ChannelOut, Commands[SequencePointer]->GetCommand());
				GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Yellow, UEnum::GetValueAsString(Commands[SequencePointer]->GetCommand()));

				if (SequencePointer >= (CmdNum - 1))
					SequenceLastTime = Time + 2;
				else
					SequenceLastTime = Time;
			}
		}
	}
}