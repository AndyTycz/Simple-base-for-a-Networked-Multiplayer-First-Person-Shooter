// Fill out your copyright notice in the Description page of Project Settings.


#include "TPGameMode.h"
#include "Engine//World.h"
#include "TimerManager.h"


ATPGameMode::ATPGameMode()
{
	TimeBetweenWaves = 2.0f;
}

void ATPGameMode::StartWave()
{
	WaveCount++;

	if (WaveCount < 5)
	{

		EnemiesToSpawn = WaveCount * 2;
	}
	GetWorldTimerManager().SetTimer(EnemySpawnerHandler, this, &ATPGameMode::SpawnNewEnemy, 15.0f, true, 0.0f);
}

void ATPGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(EnemySpawnerHandler);
}

void ATPGameMode::PrepareForNewWave()
{
	FTimerHandle NextWaveStartHandle;
	GetWorldTimerManager().SetTimer(NextWaveStartHandle, this, &ATPGameMode::StartWave, TimeBetweenWaves, false);
}

void ATPGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNewWave();
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