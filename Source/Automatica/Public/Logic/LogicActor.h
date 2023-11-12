// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LogicActor.generated.h"

UCLASS()
class AUTOMATICA_API ALogicActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALogicActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
