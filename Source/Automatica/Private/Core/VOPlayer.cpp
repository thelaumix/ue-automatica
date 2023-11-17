// 2023 Lukas A. Schopf // thelaumix productions


#include "Core/VOPlayer.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
AVOPlayer::AVOPlayer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Player = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("Player"));
	RootComponent = Player;
	Player->bEnableTimelineCallbacks = true;

	Player->OnTimelineMarker.AddDynamic(this, &AVOPlayer::Cb_Command);
	Player->SetAutoActivate(false);

	Player->OnEventStopped.AddDynamic(this, &AVOPlayer::Cb_Stopped);

	bIsRunning = false;
}

void AVOPlayer::Play()
{
	// Abort if no event assigned
	if (Event == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "No event assigned to VO Player '"+GetName()+"'");
		return;
	}
	// Also abort if already playing
	if (bIsRunning)
		return;

	Player->Activate();
	
	// Cancel all players
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVOPlayer::StaticClass(), Players);

	for(AActor* Found: Players)
	{
		AVOPlayer* Plr = Cast<AVOPlayer>(Found);
		if (Plr == nullptr || Plr == this)
			continue;
		Plr->Stop();
	}

	Player->Play();
	OnPlayerStarted.Broadcast();
	bIsRunning = true;
}

void AVOPlayer::Stop()
{
	if (!bIsRunning)
		return;

	Player->Stop();
	OnPlayerStopped.Broadcast();
	bIsRunning = false;
}

void AVOPlayer::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Player->SetAutoActivate(false);
	Player->SetEvent(Event);
	Player->bAutoDestroy = false;
	Player->bStopWhenOwnerDestroyed = true;
}

void AVOPlayer::BeginPlay()
{
	Super::BeginPlay();
	Player->Stop();
}

void AVOPlayer::Cb_Command(FString Command, int32 Index)
{
	if (bIsRunning)
		OnPlayerCommand.Broadcast(Command);
}

void AVOPlayer::Cb_Stopped()
{
	if (bIsRunning)
		OnPlayerStopped.Broadcast();
	bIsRunning = false;
}



