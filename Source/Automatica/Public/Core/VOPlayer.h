// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "FMODAudioComponent.h"
#include "FMODEvent.h"
#include "GameFramework/Actor.h"
#include "VOPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerState_d);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerMarkerCallback_d, FString, Command);

UCLASS()
class AUTOMATICA_API AVOPlayer : public AActor
{
	GENERATED_BODY()

public:
	AVOPlayer();

	UFUNCTION(BlueprintCallable, Category="VO Player")
	void Play();

	UFUNCTION(BlueprintCallable, Category="VO Player")
	void Stop();

	UPROPERTY(BlueprintAssignable, Category="VO Player")
	FPlayerState_d OnPlayerStarted;

	UPROPERTY(BlueprintAssignable, Category="VO Player")
	FPlayerState_d OnPlayerStopped;

	UPROPERTY(BlueprintAssignable, Category="VO Player")
	FPlayerMarkerCallback_d OnPlayerCommand;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category="VO Player") UFMODEvent* Event;


private:

	UPROPERTY() UFMODAudioComponent* Player;

	UFUNCTION()
	void Cb_Command(FString Command, int32 Index);
	UFUNCTION()
	void Cb_Stopped();

	UPROPERTY()
	bool bIsRunning;
};
