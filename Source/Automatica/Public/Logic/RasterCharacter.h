// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "LogicActor.h"
#include "RasterCharacter.generated.h"

UCLASS()
class AUTOMATICA_API ARasterCharacter : public ALogicActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARasterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
