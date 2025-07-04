// Copyright Epic Games, Inc. All Rights Reserved.

#include "PhysicsCharacter.h"
#include "PhysicsProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Components/StaticMeshComponent.h>
#include <PhysicsEngine/PhysicsHandleComponent.h>

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

APhysicsCharacter::APhysicsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	m_PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	
	m_SprintSpeedMultiplier = 1.5f;
	m_CurrentStamina = 100.0f;
	m_CurrentHealth = 100.0f;
}

void APhysicsCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APhysicsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// @TODO: Stamina update
	if (GetCharacterMovement()->MaxWalkSpeed > m_BaseWalkSpeed)
	{
		m_CurrentStamina -= DeltaSeconds * m_StaminaDepletionRate;
		m_CurrentStamina = FMath::Max(0, m_CurrentStamina);
	}
	else
	{
		m_CurrentStamina += DeltaSeconds * m_StaminaRecoveryRate; 
		m_CurrentStamina = FMath::Min(m_MaxStamina, m_CurrentStamina);
	}
	
	
	// @TODO: Physics objects highlight
	SetHighlightedMesh(nullptr);
	
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	
	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);
	
	//call GetWorld() from within an actor extending class
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,		//result
		this->GetFirstPersonCameraComponent()->GetComponentLocation(),	//start
		this->GetFirstPersonCameraComponent()->GetComponentLocation() + this->GetFirstPersonCameraComponent()->GetForwardVector()*m_MaxGrabDistance, //end
		ECC_Visibility, //collision channel
		RV_TraceParams
	);
	
	//FVector
	if(RV_Hit.bBlockingHit)
	{
		UPrimitiveComponent* HitComponent = RV_Hit.GetComponent();
		SetGrabbedHighlights(HitComponent);
	}	
	// @TODO: Grabbed object update
	if (IsValid(m_PhysicsHandle->GetGrabbedComponent()))
	{
		m_PhysicsHandle->SetTargetLocationAndRotation(this->GetFirstPersonCameraComponent()->GetComponentLocation() + this->GetFirstPersonCameraComponent()->GetForwardVector()*m_MaxGrabDistance, FRotator(0, 0, 0));
		SetGrabbedHighlights(m_PhysicsHandle->GetGrabbedComponent());
	}
}


void APhysicsCharacter::SetGrabbedHighlights(UPrimitiveComponent* HitComponent)
{
	if (HitComponent->Mobility == EComponentMobility::Movable)
	{
		if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(HitComponent))
		{
			SetHighlightedMesh(StaticMeshComp);
		}
		else if (USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(HitComponent))
		{
			SetHighlightedMesh(SkeletalMeshComp); 
		}
	}
	else
	{
		SetHighlightedMesh(nullptr);
	}
}

void APhysicsCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APhysicsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APhysicsCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APhysicsCharacter::Look);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APhysicsCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APhysicsCharacter::Sprint);
		EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Triggered, this, &APhysicsCharacter::GrabObject);
		EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Completed, this, &APhysicsCharacter::ReleaseObject);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APhysicsCharacter::SetIsSprinting(bool NewIsSprinting)
{
	// @TODO: Enable/disable sprinting use CharacterMovementComponent
	m_IsSprinting = NewIsSprinting;
	if (NewIsSprinting && m_CurrentStamina > 0.0f && GetCharacterMovement()->Velocity.Length() > 0.0f && !IsMovingAgainstWall())
	{
		GetCharacterMovement()->MaxWalkSpeed = m_SprintSpeedMultiplier * m_BaseWalkSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = m_BaseWalkSpeed;
	}
}

void APhysicsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void APhysicsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APhysicsCharacter::Sprint(const FInputActionValue& Value)
{
	SetIsSprinting(Value.Get<bool>());
}

void APhysicsCharacter::GrabObject(const FInputActionValue& Value)
{
	// @TODO: Grab objects using UPhysicsHandleComponent
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.bDebugQuery = true;
	
	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);
	
	//call GetWorld() from within an actor extending class
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,		//result
		this->GetFirstPersonCameraComponent()->GetComponentLocation(),	//start
		this->GetFirstPersonCameraComponent()->GetComponentLocation() + this->GetFirstPersonCameraComponent()->GetForwardVector()*m_MaxGrabDistance, //end
		ECC_Visibility, //collision channel
		RV_TraceParams
	);
	DrawDebugLine(
			GetWorld(),
			this->GetFirstPersonCameraComponent()->GetComponentLocation(),
			this->GetFirstPersonCameraComponent()->GetComponentLocation() + this->GetFirstPersonCameraComponent()->GetForwardVector()*m_MaxGrabDistance,
			FColor(255, 0, 0),
			false, -1, 0,
			0.1
		);
	
	 //FVector
	if(RV_Hit.bBlockingHit && RV_Hit.GetComponent()->Mobility == EComponentMobility::Movable){
		m_MaxGrabDistance = RV_Hit.Distance;
		m_PhysicsHandle->GrabComponentAtLocationWithRotation(RV_Hit.GetComponent(), RV_Hit.BoneName, RV_Hit.Location, FRotator(0,0,0));
		m_PhysicsHandle->SetInterpolationSpeed(m_BaseInterpolationSpeed/RV_Hit.GetComponent()->GetMass());
	}
}

void APhysicsCharacter::ReleaseObject(const FInputActionValue& Value)
{
	// @TODO: Release grabebd object using UPhysicsHandleComponent
	m_PhysicsHandle->ReleaseComponent();
	SetHighlightedMesh(nullptr);
	m_MaxGrabDistance = 1000.0f;
}

void APhysicsCharacter::SetHighlightedMesh(UMeshComponent* StaticMesh)
{
	if(m_HighlightedMesh)
	{
		m_HighlightedMesh->SetOverlayMaterial(nullptr);
	}
	m_HighlightedMesh = StaticMesh;
	if (m_HighlightedMesh)
	{
		m_HighlightedMesh->SetOverlayMaterial(m_HighlightMaterial);
	}
}


bool APhysicsCharacter::IsMovingAgainstWall() const
{
	if (GetCharacterMovement())
	{
		
		const FVector InputVector = GetCharacterMovement()->GetLastInputVector();
		const FVector Velocity = GetVelocity();

		const bool bIsTryingToMove = !InputVector.IsNearlyZero();
		const bool bIsMoving = Velocity.Size2D() > 85.0f;

		if (bIsTryingToMove && !bIsMoving)
		{
			UE_LOG(LogTemp, Warning, TEXT("You're pushing against a wall."));
			return true;
		}
	}
	return false;
}
