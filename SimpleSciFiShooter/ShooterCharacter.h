// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SIMPLESCIFISHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Die();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

public:	
	UFUNCTION(BlueprintPure)
	bool IsDead() const; //whether character is dead
	
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;

	void Shoot();
	
private:
	//Movement
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	//Looking Around
	void LookUpRate(float AxisValue);
	void LookRightRate(float AxisValue);

	UPROPERTY(EditAnywhere)
	float RotationRate = 10;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;

	//Gunshot Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	//Gun Muzzle Flash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	//Montage for firing the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* GunFireMontage;

	//Montage for character death
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* DeathMontage;

	//IMPACT
	UPROPERTY(EditAnywhere)
	UParticleSystem *ImpactEffect;

	UPROPERTY(EditAnywhere)
	USoundBase *ImpactSound;

	//GUN STATISTICS
	UPROPERTY(EditAnywhere)
	float MaxRange = 1000;

	UPROPERTY(EditAnywhere)
	float Damage = 10;
	
};
