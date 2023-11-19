// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "ControlCommandSettings.h"
#include "GameFramework/Actor.h"
#include "LogicActor.generated.h"

/** Control channel for command logic */
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum EControlChannel: uint8
{
	None	   = 0,
	All		   = 0b111,
	Red		   = 1 << 0,
	Green	   = 1 << 1,
	Blue	   = 1 << 2,
};
ENUM_CLASS_FLAGS(EControlChannel);

/** Global logic actor */
UCLASS()
class AUTOMATICA_API ALogicActor : public AActor
{
	GENERATED_BODY()

public:

	/** Whether the logic actor can receive logic commands */
	UPROPERTY(EditDefaultsOnly, Category = "Logic", BlueprintReadWrite, DisplayName = "Can receive commands")
	bool bUseChannelBusInput = false;

	/** The channel to send into */
	UPROPERTY(EditInstanceOnly, Category = "Logic", BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/Automatica.EControlChannel", EditCondition="bUseChannelBusInput", EditConditionHides=true), DisplayName = "Input Channel")
	int32 ChannelIn = All;

	/** Whether the logic actor can send logic commands */
	UPROPERTY(EditDefaultsOnly, Category = "Logic", BlueprintReadWrite, DisplayName = "Can send commands")
	bool bUseChannelBusOutput = false;

	/** The channel to send into */
	UPROPERTY(EditInstanceOnly, Category = "Logic", BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/Automatica.EControlChannel", EditCondition="bUseChannelBusOutput", EditConditionHides=true), DisplayName = "Output Channel")
	int32 ChannelOut = All;

	void PushCommandToReceive(ELogicControlType Command);

	/** Called once the stage resets */
	virtual void LogicReset() {}

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Send a command from this actor to the specified output channel */
	void SendCommand(ELogicControlType Command);

	/** Called once the actor receives a command */
	virtual void OnReceiveCommand(ELogicControlType Command) {}

	/** Called once the actor receives a command */
	UFUNCTION(BlueprintImplementableEvent, Category = "Logic|Button", DisplayName = "On Receive Command")
	void Bp_OnReceiveCommand(ELogicControlType Command);
};
