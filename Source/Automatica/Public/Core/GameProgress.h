// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameProgress.generated.h"

/**
 * 
 */
UCLASS()
class AUTOMATICA_API UGameProgress : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Automatica|Save")
	int CurrentFloor = 0;
};
