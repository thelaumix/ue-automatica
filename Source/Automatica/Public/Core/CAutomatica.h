// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "GameProgress.h"
#include "Engine/GameInstance.h"
#include "Logic/LogicActor.h"
#include "CAutomatica.generated.h"

#define SAVE_NAME "aprog"

DECLARE_LOG_CATEGORY_EXTERN(AutomaticaCore, Log, All);

/**
 * Automatica game instance class. This beast handles the overall game.
 * Yes, this could be departed in LevelInstances, GameStates or GameModes, but there's no time for that now.
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

	/** Register a logic actor to the current instance network.
	 * This shall ONLY be done by the ALogicActor class. */
	static bool BindLogicActor(ALogicActor* LogicActor);

	/** Release a logic actor from the current instance network.
	 * This shall ONLY be done by the ALogicActor class. */
	static bool ReleaseLogicActor(ALogicActor* LogicActor);

	/** Fetches all currently registered logic actors */
	static bool SendLogicCommand(ALogicActor* Sender, int32 Channel, ELogicControlType Command);

	template<typename LogicActorType>
	static TArray<LogicActorType*> GetLogicActorsOfType(ALogicActor* Requester);

	/** Fetch the array containing all currently registered logic actors */
	static TArray<ALogicActor*> GetRegisteredLogicActors(const ALogicActor* Requester);

	/** Retreives game instance */
	static UAutomatica* Get(const UObject* Outer);
	
	/** Retreives game instance pointer driven */
	static bool Get(const UObject* Outer, UAutomatica*& Instance);
	
private:
	UPROPERTY() UGameProgress* Saved;
	UPROPERTY() TArray<ALogicActor*> RegisteredLogicNetworkActors;
};
