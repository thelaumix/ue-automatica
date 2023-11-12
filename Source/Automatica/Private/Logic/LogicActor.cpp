// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/LogicActor.h"


// Sets default values
ALogicActor::ALogicActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALogicActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALogicActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

