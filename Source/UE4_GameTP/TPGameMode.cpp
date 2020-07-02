// Fill out your copyright notice in the Description page of Project Settings.


#include "TPGameMode.h"
#include "Engine//World.h"
#include "TimerManager.h"
#include "HealthComponent.h"
#include "TPCharacter.h"
#include "TPGameState.h"
#include "TPPlayerState.h"

ATPGameMode::ATPGameMode()
{
	TimeBetweenWaves = 2.0f;


	GameStateClass = ATPGameState::StaticClass();
	PlayerStateClass = ATPPlayerState::StaticClass();


	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.5f;


	PlayersLifes = 3;
}

void ATPGameMode::StartPlay()
{
	Super::StartPlay();

	//PrepareForNewWave();
	FTimerHandle SpawnRateHandle;
	GetWorldTimerManager().SetTimer(SpawnRateHandle, this, &ATPGameMode::RestartDeadPlayer, 5.0f, true, 5.0f);
}

void ATPGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds); //Hace falta si estamos usando un tick de 1.5?

	CheckWaveState();

	CheckPlayerAlive();
}

void ATPGameMode::StartWave()
{
	WaveCount++;

	if (WaveCount < 2)
	{

		EnemiesToSpawn = WaveCount * 3;

		GetWorldTimerManager().SetTimer(EnemySpawnerHandler, this, &ATPGameMode::SpawnEnemyTimer, 3.0f, true, 0.0f);
	}
	SetWaveState(EWaveState::WaveInProgress);
}

void ATPGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(EnemySpawnerHandler);
	GetWorldTimerManager().ClearTimer(NextWaveStartHandle);
	SetWaveState(EWaveState::WaveComplete);
}

void ATPGameMode::PrepareForNewWave()
{

	GetWorldTimerManager().SetTimer(NextWaveStartHandle, this, &ATPGameMode::StartWave, TimeBetweenWaves, false);
	SetWaveState(EWaveState::WaitingToStart);
}

void ATPGameMode::CheckWaveState()
{

	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(NextWaveStartHandle);

	if (EnemiesToSpawn > 0 || bIsPreparingForWave)
	{
		return;
	}

	bool bIsEnemyAlive = false;


	for (FConstPawnIterator Iterator = GetWorld()->GetPawnIterator(); Iterator; Iterator++)
	{
		APawn* CurrentPawn = Iterator->Get();
		if (CurrentPawn == nullptr || CurrentPawn->IsPlayerControlled())
		{
			continue;

		}

		UHealthComponent* HealthComp = Cast<UHealthComponent>(CurrentPawn->GetComponentByClass(UHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bIsEnemyAlive = true;
			break;
		}
	}

	if (!bIsEnemyAlive)
	{
		SetWaveState(EWaveState::WaveComplete);
		PrepareForNewWave();
	}

}

void ATPGameMode::CheckPlayerAlive()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; Iterator++)
	{
		APlayerController* PC = Cast<APlayerController>(Iterator->Get());
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			UHealthComponent* HealthComp = Cast<UHealthComponent>(MyPawn->GetComponentByClass(UHealthComponent::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)// si no pasa tira warning
			{
				//Player ta vivo
				return;
			}
			else
			{
				GameOver(MyPawn);
			}
		}
	}
}

void ATPGameMode::SpawnEnemyTimer()
{
	SpawnNewEnemy();
	EnemiesToSpawn--;

	if (EnemiesToSpawn <= 0)
	{
		EndWave();
	}

}

void ATPGameMode::GameOver(APawn* PawnToShowHud)
{
	if (PawnToShowHud)
	{
		EndWave();

		UE_LOG(LogTemp, Log, TEXT("GAme Over F"));

		SetWaveState(EWaveState::GameOver);

		//Terminar match, game over con puntaje

		GameOverImplementation(PawnToShowHud);
	}


	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; Iterator++)
	{
		APlayerController* PC = Cast<APlayerController>(Iterator->Get());
		if (PC && PC->GetPawn())
		{
			GameOverImplementation(PC->GetPawn());
		}
	}

}



void ATPGameMode::SetWaveState(EWaveState NewState)
{
	ATPGameState* GS = GetGameState<ATPGameState>();
	if (ensureAlways(GS)) //si no pasa el if avisa 
	{
		GS->SetWaveState(NewState);
	}
}

void ATPGameMode::RestartDeadPlayer()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; Iterator++)
	{
		APlayerController* PC = Cast<APlayerController>(Iterator->Get());
		if (PC && PC->GetPawn() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("RessedPLayer"));
			PlayersLifes--;
			if (PlayersLifes > 0) //Usar las vidas de cada player?
			{
				RestartPlayer(PC);
			}
			else
			{
				GameOver(PC->GetPawn());
			}
		}
	}

}
