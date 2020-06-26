// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPGameMode.generated.h"

/**
 *
 */
UCLASS()
class UE4_GAMETP_API ATPGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:

	FTimerHandle EnemySpawnerHandler;

	int32 EnemiesToSpawn;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly)
		float TimeBetweenWaves;
protected:

	UFUNCTION(BlueprintImplementableEvent)
		void SpawnNewEnemy();

	void SpawnEnemyTimer();

	void StartWave();

	void EndWave();

	void PrepareForNewWave();

public:

	ATPGameMode();

	virtual void StartPlay() override;
};
