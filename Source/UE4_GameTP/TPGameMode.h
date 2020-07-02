// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPGameMode.generated.h"


enum class EWaveState : uint8;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, DeadActor, AController*, KillerController, float, DamageMultiplier); //K
/**
 *
 */
UCLASS()
class UE4_GAMETP_API ATPGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:

	FTimerHandle EnemySpawnerHandler;

	FTimerHandle NextWaveStartHandle;

	int32 EnemiesToSpawn;

	int32 WaveCount;

	int32 PlayersLifes;

	UPROPERTY(EditDefaultsOnly)
		float TimeBetweenWaves;

protected:

	UFUNCTION(BlueprintImplementableEvent)
		void SpawnNewEnemy();

	UFUNCTION(BlueprintImplementableEvent)
		void GameOverImplementation(APawn* PawnToShowHud);

	void GameOver(APawn* PawnToShowHud);

	void SpawnEnemyTimer();

	void StartWave();

	void EndWave();

	void PrepareForNewWave();


	void CheckWaveState();

	void CheckPlayerAlive();


	void SetWaveState(EWaveState NewState);

	void RestartDeadPlayer();

public:

	ATPGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable)
		FOnActorKilled OnActorKilled;
};
