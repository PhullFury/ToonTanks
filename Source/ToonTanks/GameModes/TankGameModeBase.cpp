// Fill out your copyright notice in the Description page of Project Settings.


#include "Kismet/GameplayStatics.h"
#include "TankGameModeBase.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include "ToonTanks/Pawns/PawnTurret.h"
#include "ToonTanks/PlayerControllers/PlayerControllerBase.h"

#define OUT

void ATankGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	HandleGameStart();
}

void ATankGameModeBase::ActorDied(AActor* DeadActor)
{
	if (DeadActor == PlayerTank)
	{
		PlayerTank->HandleDestruction();
		HandleGameOver(false);

		if (PlayerControllerRef)
		{
			PlayerControllerRef->SetPlayerEnabledState(false);
		}
	}
	else if (APawnTurret* DestroyedTurret = Cast<APawnTurret>(DeadActor))
	{
		DestroyedTurret->HandleDestruction();
		TargetTurrets = TargetTurrets - 1;

		if (TargetTurrets == 0)
		{
			HandleGameOver(true);
		}
	}
}

void ATankGameModeBase::HandleGameStart()
{
	GameStart();

	PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
	PlayerControllerRef = Cast< APlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 0));
	TargetTurrets = GetTargetTurretCount();

	if (PlayerControllerRef)
	{
		PlayerControllerRef->SetPlayerEnabledState(false);

		FTimerHandle PlayerEnableHandle;
		FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerControllerRef, &APlayerControllerBase::SetPlayerEnabledState, true);

		GetWorld()->GetTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, StartDelay, false);
	}
}

void ATankGameModeBase::HandleGameOver(bool PlayerWon)
{
	GameOver(PlayerWon);
}

int32 ATankGameModeBase::GetTargetTurretCount()
{
	TArray <AActor*> TurretActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawnTurret::StaticClass(), OUT TurretActors);
	return TurretActors.Num();
}