// 2023 Lukas A. Schopf // thelaumix productions


#include "../../../Public/Actors/General/Elevator.h"


// Sets default values
AElevator::AElevator(): StartingZPosition(0), TargetZPosition(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	AccelerationRegion = 90;
	FloorHeight = 900;
	MinTravelSpeed = 50;
	MaxTravelSpeed = 200;
	ApproachingOffset = -310;
}

void AElevator::TravelUp()
{
	TravelTo(TargetZPosition + FloorHeight);
}

void AElevator::TravelDown()
{
	TravelTo(TargetZPosition - FloorHeight);
}

void AElevator::TravelTo(const float ZPosition)
{
	StartingZPosition = GetActorLocation().Z;
	TargetZPosition = ZPosition;
	SetActorTickEnabled(true);
}

void AElevator::TravelToFloor(const int FloorIndex)
{
	TravelTo(FloorHeight * FloorIndex);
}

void AElevator::ApproachFloor(const int FloorIndex)
{
	const float TargetingHeight = FloorHeight * FloorIndex;
	const float StartingHeight = TargetingHeight + ApproachingOffset;
	FVector Location = GetActorLocation();

	Location.Z = StartingHeight;
	StartingZPosition = StartingHeight;
	SetActorLocation(Location);

	TravelTo(TargetingHeight);
}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
	Super::BeginPlay();

	StartingZPosition = GetActorLocation().Z;
	TargetZPosition = GetActorLocation().Z;
}

// Called every frame
void AElevator::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentPosition = GetActorLocation();
	float DirectionMultiplier = FMath::Sign(TargetZPosition - CurrentPosition.Z);

	// Get the total acceleration speed
	const float OffsetEnd = FMath::Abs(TargetZPosition - CurrentPosition.Z);
	const float OffsetStart = FMath::Abs(StartingZPosition - CurrentPosition.Z);

	const float AmountEnd = FMath::Clamp(OffsetEnd / AccelerationRegion, 0, 1);
	const float AmountStart = FMath::Clamp(OffsetStart / AccelerationRegion, 0, 1);

	const float AmountFinal = FMath::Min(AmountEnd, AmountStart);

	const float Speed = FMath::Lerp(MinTravelSpeed, MaxTravelSpeed, AmountFinal);

	float NextActualZDistance = DeltaTime * Speed;
	// If the current frame's moving offset is greater than the total offset, clamp it to fit
	if (NextActualZDistance > OffsetEnd)
		NextActualZDistance = OffsetEnd;

	CurrentPosition.Z += DirectionMultiplier * NextActualZDistance;
	SetActorLocation(CurrentPosition);

	// If the target has arrived, close the case and shut down the systems.
	// Or simply stop the elevator tick...
	if (CurrentPosition.Z == TargetZPosition)
		SetActorTickEnabled(false);
}

