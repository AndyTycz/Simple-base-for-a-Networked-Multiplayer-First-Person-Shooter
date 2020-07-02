// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TPGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,
	WaveInProgress,
	WaitingForComplete,
	WaveComplete,
	GameOver,
};



/**
 *
 */
UCLASS()
class UE4_GAMETP_API ATPGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	
	void SetWaveState(EWaveState NewState);

protected:
	UFUNCTION()
		void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent)
		void WaveStateChanged(EWaveState NewState, EWaveState OldState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState)
		EWaveState WaveState;


};
