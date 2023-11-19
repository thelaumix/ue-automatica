// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/ControlUnit.h"

#include "FMODBlueprintStatics.h"
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
	CommandLimit = 10;
	BaseIconScale = 1;
}

bool AControlUnit::AddCommand(const ELogicControlType Command)
{
	if (Commands.Num() >= CommandLimit)
		return false;

	
	auto Icon = Cast<AControlUnitIcon>(GetWorld()->SpawnActor(IconClass));
	Icon->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	//Instance.Icon = NewObject<AControlUnitIcon>(RootComponent, IconClass);
	Icon->SetCommand(Command);
	Icon->Layer = CurrentLayer;
	Commands.Add(Icon);
	CommandChildBuffer.Add(Icon);

	Icon->SetColor(UAutomatica::GetChannelColor(ChannelOut));

	

	const double Scale = BaseIconScale * FMath::Pow(LayerScaleFactor, CurrentLayer);
	Icon->SetScale(Scale);
	CommandIndexPtr++;

	if (Icon->SupportsNesting())
	{
		ContainerIndexStack.Add(Commands.Num() - 1);
		CurrentLayer++;
	}

	UpdateLimitDisplay();

	return true;
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

	UpdateLimitDisplay();
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
	
	for(const auto Cmd: Commands)
		Cmd->SetVisibility(0.1);
	
	SequencePointer = -1;
	SequenceLastTime = UGameplayStatics::GetTimeSeconds(this);
	bIsPlaying = true;
}

void AControlUnit::AbortSequence()
{
	if (!bIsPlaying) return;

	SequenceLastTime = UGameplayStatics::GetTimeSeconds(this) + 1;
	SequencePointer = Commands.Num()-1;
}

void AControlUnit::ClearUnit()
{
	if (bIsPlaying) return;
	while(CommandIndexPtr >= 0)
	{
		Backspace();
	}
}

TArray<TEnumAsByte<ELogicControlType>> AControlUnit::GetComandSequence()
{
	TArray<TEnumAsByte<ELogicControlType>> Sequence;

	for(const auto Icon: Commands)
	{
		Sequence.Add(Icon->GetCommand());
	}

	return Sequence;
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
	
	UpdateLimitDisplay();
}

void AControlUnit::BeginPlay()
{
	Super::BeginPlay();
	CurrentLayer = 0;
	CommandIndexPtr = -1;

	UpdateLimitDisplay();
}

void AControlUnit::BeginDestroy()
{
	Super::BeginDestroy();

	for(auto Icon: CommandChildBuffer)
	{
		if (IsValid(Icon))
			Icon->Destroy();
	}
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
				UE_LOGFMT(LogTemp, Warning, " SET UNIT NON_PLAYING");
				UAutomatica::SetControlUnitPlaying(this, false);
				bIsPlaying = false;
			} else
			{
				if (SequencePointer > 0)
					Commands[SequencePointer-1]->SetVisibility(0.3);
				
				const auto Cmd = Commands[SequencePointer];
				Cmd->SetVisibility(1);
				UAutomatica::GSendLogicCommand(this, Cmd->GetCommand(), ChannelOut);
				//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Yellow, UEnum::GetValueAsString(Commands[SequencePointer]->GetCommand()));
				UFMODBlueprintStatics::PlayEvent2D(this, StepSound, true);
				
				if (SequencePointer >= (CmdNum - 1))
					SequenceLastTime = Time + 1;
				else
					SequenceLastTime = Time;
			}
		}
	}
}

void AControlUnit::LogicReset()
{
	Super::LogicReset();

	for(const auto Cmd: Commands)
		Cmd->SetVisibility(1);
}

void AControlUnit::UpdateLimitDisplay() const
{
	OnSequenceUpdate.Broadcast(Commands.Num());
	if (!LimitDisplay) return;

	LimitDisplay->SetValue(CommandLimit - Commands.Num());
}
