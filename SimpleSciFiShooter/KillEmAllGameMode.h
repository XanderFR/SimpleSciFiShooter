// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleSciFiShooterGameModeBase.h"
#include "KillEmAllGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESCIFISHOOTER_API AKillEmAllGameMode : public ASimpleSciFiShooterGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void PawnKilled(APawn* PawnKilled) override;

private:
	void EndGame(bool bIsPlayerWinner);

};
