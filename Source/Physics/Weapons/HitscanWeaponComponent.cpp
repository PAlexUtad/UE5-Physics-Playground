// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/HitscanWeaponComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "PhysicsCharacter.h"
#include "PhysicsWeaponComponent.h"
#include <Camera/CameraComponent.h>
#include <Components/SphereComponent.h>

void UHitscanWeaponComponent::Fire()
{
	Super::Fire();

	// @TODO: Add firing functionality
	FHitResult Hit;
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector LineStart = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

	FVector LineEnd   =  PlayerController->PlayerCameraManager->GetActorForwardVector() * m_fRange + LineStart;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);
	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Red, true, 5);
	GetWorld()->LineTraceSingleByChannel(Hit, LineStart, LineEnd, ECC_Visibility, Params);

	if (Hit.bBlockingHit && Hit.GetActor() && Hit.GetComponent()->Mobility == EComponentMobility::Movable)
	{
		Hit.GetComponent()->AddImpulse(Hit.Normal * 100000 * -1);
		onHitscanImpact.Broadcast(Hit.GetActor(), Hit.ImpactPoint, GetOwner()->GetActorForwardVector());
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_ImpactEffect, Hit.Location, FRotator::ZeroRotator, true);
	}
}
