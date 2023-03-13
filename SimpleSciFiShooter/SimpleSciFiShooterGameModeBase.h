// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SimpleSciFiShooterGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESCIFISHOOTER_API ASimpleSciFiShooterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void PawnKilled(APawn* PawnKilled);

};
