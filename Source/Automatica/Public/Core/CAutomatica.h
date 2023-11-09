// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "GameProgress.h"
#include "Engine/GameInstance.h"
#include "CAutomatica.generated.h"

#define SAVE_NAME "aprog"

DECLARE_LOG_CATEGORY_EXTERN(AutomaticaCore, Log, All);

/**
 * 
 */
UCLASS()
class AUTOMATICA_API UAutomatica : public UGameInstance
{
	GENERATED_BODY()
	virtual void Init() override;

public:
	/** Retreives the current savegame instance */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Automatica", meta=(WorldContext="Outer"))
	static UGameProgress* GetProgress(UObject* Outer, bool& Valid);

	/** Writes the save buffer to disk */
	UFUNCTION(BlueprintCallable, Category="Automatica", meta=(WorldContext="Outer"))
	static UPARAM(DisplayName="Success") bool WriteProgress(UObject* Outer);

	/** Retreives game instance */
	static UAutomatica* Get(const UObject* Outer);
	
private:
	UPROPERTY() UGameProgress* Saved;
};
