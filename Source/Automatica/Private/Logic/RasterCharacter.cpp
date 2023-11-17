// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/RasterCharacter.h"


// Sets default values
ARasterCharacter::ARasterCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

