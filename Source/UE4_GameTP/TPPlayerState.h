// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TPPlayerState.generated.h"

/**
 *
 */
UCLASS()
class UE4_GAMETP_API ATPPlayerState : public APlayerState
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable)
		void AddScore(float ScoreDelta);
};
