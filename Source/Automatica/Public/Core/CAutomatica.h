// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "GameProgress.h"
#include "Engine/GameInstance.h"
#include "Logic/ControlUnit.h"
#include "Logic/LogicActor.h"
#include "CAutomatica.generated.h"

#define SAVE_NAME "aprog"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FElevatorMovementEvent_d);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSequenceReset_d, bool, bInGoal);

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

	UFUNCTION(BlueprintCallable, Category = "Automatica")
	static FLinearColor GetChannelColor(UPARAM(meta=(Bitmask, BitmaskEnum = "/Script/Automatica.EControlChannel")) int32 Channel);

	/** Register a logic actor to the current instance network.
	 * This shall ONLY be done by the ALogicActor class. */
	static bool BindLogicActor(ALogicActor* LogicActor);

	/** Release a logic actor from the current instance network.
	 * This shall ONLY be done by the ALogicActor class. */
	static bool ReleaseLogicActor(ALogicActor* LogicActor);

	/** Send a command instance to all registered logic actors matching the given channel mask */
	UFUNCTION(BlueprintCallable, Category="Automatica", meta=(WorldContext="Outer"), DisplayName="Send Logic Command")
	static bool GSendLogicCommand(const UObject* Outer, const ELogicControlType Command, UPARAM(meta=(Bitmask, BitmaskEnum = "/Script/Automatica.EControlChannel")) const int32 Channel = 7);

	/** Send a command instance to all registered logic actor matching the given channel mask */
	UFUNCTION(Exec)
	bool SendLogicCommand(const ELogicControlType Command, UPARAM(meta=(Bitmask, BitmaskEnum = "/Script/Automatica.EControlChannel")) const int32 Channel = 7);

	template<typename LogicActorType>
	static TArray<LogicActorType*> GetLogicActorsOfType(ALogicActor* Requester);

	/** Fetch the array containing all currently registered logic actors */
	static TArray<ALogicActor*> GetRegisteredLogicActors(const ALogicActor* Requester);

	/** Retreives game instance */
	static UAutomatica* Get(const UObject* Outer);
	
	/** Retreives game instance pointer driven */
	static bool Get(const UObject* Outer, UAutomatica*& Instance);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Automatica", DisplayName="Automatica", meta=(WorldContext="Outer"))
	static UAutomatica* BP_GetAutomaticaInstance(const UObject* Outer);

	UPROPERTY(BlueprintAssignable, Category = "Automatica|Elevator")
	FElevatorMovementEvent_d OnElevatorStop;
	
	UPROPERTY(BlueprintAssignable, Category = "Automatica|Elevator")
	FElevatorMovementEvent_d OnElevatorStart;
	
	UPROPERTY(BlueprintAssignable, Category = "Automatica")
	FSequenceReset_d OnSequenceEnd;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Automatica", meta=(WorldContext="Outer"))
	static bool IsInteractionEnabled(const UObject* Outer);

	UFUNCTION(BlueprintCallable, Category="Automatica", meta=(WorldContext="Outer"))
	static void SetInteractionEnabled(const UObject* Outer, bool bEnabled);

	static void SetControlUnitPlaying(AControlUnit* Unit, bool bPlaying);

	UFUNCTION(Exec, DisplayName = "Elevator.TravelTo")
	void Elev_TravelToFloor(int Floor);

	UFUNCTION(Exec, DisplayName = "Elevator.Approach")
	void Elev_ApproachFloor(int Floor);
	
private:
	static void PlayEnableSound(UAutomatica* A, const bool bWasReset = false);
	UPROPERTY() UGameProgress* Saved;
	UPROPERTY() TArray<ALogicActor*> RegisteredLogicNetworkActors;
	UPROPERTY() TArray<AControlUnit*> RunningControlUnits;

	UPROPERTY() bool bInteractionEnabled;
};
