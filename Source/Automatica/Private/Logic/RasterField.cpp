// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/RasterField.h"


// Sets default values
ARasterField::ARasterField()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Type = Normal;
	bLiftStartElevated = false;
	bFireActive = true;
}

// Called when the game starts or when spawned
void ARasterField::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARasterField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

