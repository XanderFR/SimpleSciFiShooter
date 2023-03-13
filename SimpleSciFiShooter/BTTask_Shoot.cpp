// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Shoot.h"
#include "AIController.h"
#include "ShooterCharacter.h"

UBTTask_Shoot::UBTTask_Shoot() 
{
    NodeName = "Shoot";
}

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) 
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    //checks for AI controlled pawns in the game world
    if (OwnerComp.GetAIOwner() == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    AShooterCharacter* Character = Cast<AShooterCharacter>(OwnerComp.GetAIOwner()->GetPawn()); //represents the pawn controlled by AI
    if (Character == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    Character->Shoot();

    return EBTNodeResult::Succeeded;
}


