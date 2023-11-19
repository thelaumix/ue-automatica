// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/RasterField.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Logic/RasterCharacter.h"


// Sets default values
ARasterField::ARasterField()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Type = Normal;
	bLiftStartElevated = false;
	bFireActive = true;
	bUseChannelBusInput = true;
	bIsPowered = false;
	bIsDeadly = false;

	ZOffset = 0;
}

void ARasterField::LogicReset()
{
	Super::LogicReset();
	bIsPowered = false;
	ResetField();
}

// Called when the game starts or when spawned
void ARasterField::BeginPlay()
{
	Super::BeginPlay();

	bIsDeadly = false;
}

void ARasterField::OnReceiveCommand(ELogicControlType Command)
{
	if (Command == Toggle_On && !bIsPowered)
	{
		bIsPowered = true;
		OnPowerStateChange(true);
	} else if (Command == Toggle_Off && bIsPowered)
	{
		bIsPowered = false;
		OnPowerStateChange(false);
	} else if (Command == Toggle)
	{
		bIsPowered = !bIsPowered;
		OnPowerStateChange(bIsPowered);
	}
}

void ARasterField::SetDeadly(const bool bDeadly)
{
	bIsDeadly = bDeadly;

	if (bIsDeadly)
	{
		TArray<AActor*> Chars;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARasterCharacter::StaticClass(), Chars);

		for(const auto Actor: Chars)
		{
			const auto Char = Cast<ARasterCharacter>(Actor);
			if (Char && Char->GetCurrentField() == this)
				Char->Kill();
		}
	}
}

// Called every frame
void ARasterField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ARasterField::GetCharAnchor()
{
	return GetActorLocation() + FVector(0, 0, ZOffset);
}

void ARasterField::HandleCharEntering(ARasterCharacter* Character)
{
	SetCharacterRegisterd(Character, true);
	if (bIsDeadly)
		Character->Kill();
}

void ARasterField::HandleCharLeaving(ARasterCharacter* Character)
{
	SetCharacterRegisterd(Character, false);
}

bool ARasterField::IsGoalType() const
{
	return (Type == Beet);
}

void ARasterField::SetCharacterRegisterd(ARasterCharacter* Character, bool bRegistered)
{
	if (bRegistered && !HoldingChars.Contains(Character))
	{
		HoldingChars.Add(Character);
	} else if (!bRegistered && HoldingChars.Contains(Character))
	{
		HoldingChars.Remove(Character);
	}
}

void ARasterField::SetZOffset(const double Offset)
{
	ZOffset = Offset;
}

double ARasterField::GetZOffset() const
{
	return ZOffset;
}

