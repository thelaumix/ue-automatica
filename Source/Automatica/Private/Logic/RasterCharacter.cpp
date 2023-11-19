// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/RasterCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Logic/ControlUnit.h"


const FVector TraceOffset = FVector(0, 0, 50);

// Sets default values
ARasterCharacter::ARasterCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseChannelBusInput = true;
	CanNextDieIn = 0;
}

void ARasterCharacter::LogicReset()
{
	if (StartField != CurrentField && CurrentField != nullptr)
		CurrentField->SetCharacterRegisterd(this, false);
	
	CurrentField = StartField;
	CurrentRotation = StartRotation;
	bIsDead = false;

	if (StartField)
	{
		StartField->SetCharacterRegisterd(this, true);
		SetActorLocation(StartField->GetCharAnchor());
	}
	SetActorRotation(FRotator(0, StartRotation, 0));

	CanNextDieIn = UGameplayStatics::GetTimeSeconds(this) + .5;

	ResetCharacter();
}

// Called when the game starts or when spawned
void ARasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	StartRotation = GetActorRotation().Yaw;
	StartField = SweepRasterField(GetActorLocation());
	if (StartField == nullptr)
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "No starting field on character " + GetActorNameOrLabel());
	LogicReset();
}

void ARasterCharacter::OnReceiveCommand(ELogicControlType Command)
{
	if (bIsDead)
		return;
	
	FVector DirectionPointer;
	float FromRotation = CurrentRotation;
	bool bDoMove = false;
	bool bDoRotate = false;

	switch(Command)
	{
	default: break;
	case Move_Forward:
		DirectionPointer = GetActorForwardVector(); bDoMove = true; break;
	case Move_Backward:
		DirectionPointer = -GetActorForwardVector(); bDoMove = true; break;
	case Move_Left:
		DirectionPointer = -GetActorRightVector(); bDoMove = true; break;
	case Move_Right:
		DirectionPointer = GetActorRightVector(); bDoMove = true; break;
	case Turn_Right:
		CurrentRotation += 90; bDoRotate = true; break;
	case Turn_Left:
		CurrentRotation -= 90; bDoRotate = true; break;
	case Turn_Back:
		CurrentRotation += 180; bDoRotate = true; break;
	}

	if (bDoMove)
	{
		FVector StartLocation;
		FVector TargetLocation;
		if (CurrentField)
			StartLocation = CurrentField->GetCharAnchor();
		else
			StartLocation = GetActorLocation();

		TargetLocation = StartLocation + DirectionPointer * 100;

		auto TargetField = SweepRasterField(TargetLocation);
		bool bFoundTarget = false;

		if (TargetField)
		{
			// Validate clear line of sight
			FHitResult VisHit;
			FCollisionQueryParams QueryParams;

			QueryParams.AddIgnoredActor(this);
			if (CurrentField)
				QueryParams.AddIgnoredActor(CurrentField);
			GetWorld()->LineTraceSingleByChannel(VisHit, StartLocation + TraceOffset, TargetLocation + TraceOffset, ECC_Visibility, QueryParams);

			if (!VisHit.bStartPenetrating && !VisHit.bBlockingHit)
			{
				bFoundTarget = true;
			}
		}

		if (bFoundTarget)
		{
			// Actually okay, so move
			if (CurrentField != TargetField && CurrentField != nullptr)
				CurrentField->HandleCharLeaving(this);
			CurrentField = TargetField;
			CurrentField->HandleCharEntering(this);
			SetActorTickEnabled(false);
			AnimateMovement(StartLocation, TargetLocation);

			//TODO: Handle field type handling (Damage, Win etc.)
		} else
		{
			// Otherwise, ignore
		}
	}
	else if (bDoRotate)
	{
		FRotator From = FRotator(0,FromRotation, 0);
		FRotator To = FRotator(0,CurrentRotation, 0);
		AnimateRotation(From, To);
	}
	
}

void ARasterCharacter::AnimationEnded()
{
	SetActorTickEnabled(true);
}

ARasterField* ARasterCharacter::GetCurrentRasterField()
{
	return CurrentField;
}

// Called every frame
void ARasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentField)
		SetActorLocation(CurrentField->GetCharAnchor());
}

void ARasterCharacter::Kill()
{
	if (bIsDead)
		return;

	if (UGameplayStatics::GetTimeSeconds(this) < CanNextDieIn)
		return;
	
	bIsDead = true;
	
	// Check if any character survived
	TArray<AActor*> Chars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), StaticClass(), Chars);
	int AliveChars = 0;
	for(const auto Actor: Chars)
	{
		const auto Char = Cast<ARasterCharacter>(Actor);
		if (Char == nullptr || Char == this) continue;

		if (!Char->IsDead()) AliveChars++;
	}

	AnimateDeath();

	if (AliveChars == 0)
	{
		TArray<AActor*> Units;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AControlUnit::StaticClass(), Units);
		
		for(const auto Actor: Units)
		{
			if (const auto Unit = Cast<AControlUnit>(Actor))
				Unit->AbortSequence();
		}
	}
}

bool ARasterCharacter::IsDead() const
{
	return bIsDead;
}

bool ARasterCharacter::IsInGoal() const
{
	return (CurrentField != nullptr && CurrentField->IsGoalType());
}

ARasterField* ARasterCharacter::GetCurrentField() const
{
	return CurrentField;
}


ARasterField* ARasterCharacter::SweepRasterField(const FVector& Position) const
{
	FHitResult Hit;
	GetWorld()->SweepSingleByChannel(Hit, Position + TraceOffset, Position + TraceOffset, FQuat::Identity, ECC_GameTraceChannel4, FCollisionShape::MakeSphere(30));

	if (Hit.bBlockingHit)
	{
		if (Hit.GetActor()->IsA(ARasterField::StaticClass()))
		{
			return Cast<ARasterField>(Hit.GetActor());
		} else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Field cast failed on " + GetActorNameOrLabel());
		}
	}
	return nullptr;
}

