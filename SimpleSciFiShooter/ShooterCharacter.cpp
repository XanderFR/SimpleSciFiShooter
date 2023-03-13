// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"

#include "DrawDebugHelpers.h"
#include "SimpleSciFiShooterGameModeBase.h"


// Sets default values
AShooterCharacter::AShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// when keyboard command activated, the object this cpp file is attached to runs the given function
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply); //Do the smallest amount of damage
	Health -= DamageToApply; //reduce health by the damage amount
	//UE_LOG(LogTemp, Warning, TEXT("Health left %f"), Health);

	if (Health <= 0.f) { //shooter character runs out fo health
		Die();
	}

	return DamageToApply; //traditional function return value

}

void AShooterCharacter::Die() { //when a shooter character runs out of health
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); //prepare a AnimInstance specific to the shooter character mesh
	if (AnimInstance && DeathMontage) {
		AnimInstance->Montage_Play(DeathMontage); //play the DeathMontage upon character death
	}

	ASimpleSciFiShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASimpleSciFiShooterGameModeBase>(); //represents game mode
	if (GameMode != nullptr) {
		GameMode->PawnKilled(this); //let the game mode know that this instance of a shooter character died
	}

	DetachFromControllerPendingDestroy(); //disconnect player or AI controller from shooter
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //disables collision on the dead character's capsule component, 
	//allowing player to pass through enemy standing location

}

void AShooterCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true; //pauses this component's animations
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

void AShooterCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Shoot()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound); // Play the gun shooting sound
	}

	const USkeletalMeshSocket *BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket"); // get the gun barrel socket
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh()); // get gun barrel direction and orientation
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform); // spawn muzzle flash in the direction of the gun barrel
		}	
	}
	AController *OwnerController = APawn::GetController();//returns controller for this actor <-new and old->UGameplayStatics::GetPlayerController(this, 0);

	FVector Location; //character camera location
	FRotator Rotation; //character camera rotation / direction
	OwnerController->GetPlayerViewPoint(Location, Rotation); //vector and rotator values filled based on character camera location and direction in the world

	FVector End = Location + Rotation.Vector() * MaxRange; //the end point to a laser firing out from the character camera center

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params); //bullet trajectory path, do i hit something?
	
	if (bSuccess) //if the invisible laser hits something in the world
	{
		FVector ShotDirection = -Rotation.Vector(); //Shotdirection = where the shot was coming from; the opposite direction of shooter camera
		if (ImpactEffect) { //bullet impact effect
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
		}
		if (ImpactSound) { //bullet impact sound
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);
		}

		AActor* HitActor = Hit.GetActor(); //the actor that got hit by bullet
		if (HitActor != nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *HitActor->GetName()); //new stuff, tests what bullet hits

			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr); //nature of damage
			HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this); //actor hit by bullet processes damage
		}

	}
	//shooting from the hip animation
	UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance(); //prepare a AnimInstance specific to the shooter character mesh
	if (AnimInstance && GunFireMontage) {
		// Plays the gun firing animation
		AnimInstance->Montage_Play(GunFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}