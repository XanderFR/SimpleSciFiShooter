// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void AShooterPlayerController::BeginPlay()
{
    Super::BeginPlay();

    HUD = CreateWidget(this, HUDClass);
    if (HUD != nullptr)
    {
        HUD->AddToViewport();
    }
}

void AShooterPlayerController::GameHasEnded(class AActor *EndGameFocus, bool bIsWinner)
{
    Super::GameHasEnded(EndGameFocus, bIsWinner);

    HUD->RemoveFromViewport();

    if (bIsWinner)
    {
        FString CurrentLevel = GetWorld()->GetMapName();
        CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

        if (CurrentLevel.Equals(TEXT("Level_3")))
        {
            UUserWidget *BeatGameScreen = CreateWidget(this, BeatGameScreenClass);
            if (BeatGameScreen != nullptr)
            {
                BeatGameScreen->AddToViewport();
                //GetWorldTimerManager().SetTimer(LevelTimer, this, &AShooterPlayerController::SwitchLevel, TimeDelay);
            }
        }
        else
        {
            UUserWidget *WinScreen = CreateWidget(this, WinScreenClass);
            if (WinScreen != nullptr)
            {
                WinScreen->AddToViewport();
                //GetWorldTimerManager().SetTimer(LevelTimer, this, &AShooterPlayerController::SwitchLevel, TimeDelay);
            }
        }
        GetWorldTimerManager().SetTimer(LevelTimer, this, &AShooterPlayerController::SwitchLevel, TimeDelay);
    }
    else
    {
        UUserWidget *LoseScreen = CreateWidget(this, LoseScreenClass);
        if (LoseScreen != nullptr)
        {
            LoseScreen->AddToViewport();
            GetWorldTimerManager().SetTimer(LevelTimer, this, &APlayerController::RestartLevel, TimeDelay);
        }
    }
}

void AShooterPlayerController::SwitchLevel()
{
    FString CurrentLevel = GetWorld()->GetMapName();
    CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
    FString NextLevel;
    if (CurrentLevel.Equals(TEXT("Level_1")))
    {
        NextLevel = "Level_2";
    }
    else if (CurrentLevel.Equals(TEXT("Level_2")))
    {
        NextLevel = "Level_3";
    }
    else 
    {
        NextLevel = "Level_1";
    }
    UGameplayStatics::OpenLevel(GetWorld(), FName(NextLevel));
}
