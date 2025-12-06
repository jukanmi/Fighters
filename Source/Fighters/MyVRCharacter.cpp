// Fill out your copyright notice in the Description page of Project Settings.


#include "MyVRCharacter.h"
#include "PlayerAnimInst.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SpellComponent.h"
#include "MagicProjectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"


static bool IsValidSpawnTransform(const FVector& Loc, const FRotator& Rot)
{
	const float MaxAbs = 1e7f;
	bool bLocOk = FMath::IsFinite(Loc.X) && FMath::IsFinite(Loc.Y) && FMath::IsFinite(Loc.Z)
		&& FMath::Abs(Loc.X) <= MaxAbs && FMath::Abs(Loc.Y) <= MaxAbs && FMath::Abs(Loc.Z) <= MaxAbs;
	bool bRotOk = FMath::IsFinite(Rot.Roll) && FMath::IsFinite(Rot.Pitch) && FMath::IsFinite(Rot.Yaw);
	return bLocOk && bRotOk;
}

// Sets default values
AMyVRCharacter::AMyVRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);


	//camera setup
	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FPSCamera->SetupAttachment(GetMesh(), FName("Head"));
	FPSCamera->SetRelativeLocation(FVector(10.0f, 0.0f, 0.0f));
	FPSCamera->bUsePawnControlRotation = true;

	SpellComponent = CreateDefaultSubobject<USpellComponent>(TEXT("SpellComponent"));

	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

// Called when the game starts or when spawned
void AMyVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		CurrentHealth = MaxHealth;
		CurrentMana = MaxMana;

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AMyVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentMana < MaxMana)
	{
		CurrentMana += ManaRegenRate * DeltaTime;
		if (CurrentMana > MaxMana)
		{
			CurrentMana = MaxMana;
		}
	}
}

void AMyVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyVRCharacter::Move);
		if (LookAction)    EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyVRCharacter::Look);
		if (CastSpellAction) EnhancedInputComponent->BindAction(CastSpellAction, ETriggerEvent::Started, this, &AMyVRCharacter::Projectile);
		if (ToggleUIAction) EnhancedInputComponent->BindAction(ToggleUIAction, ETriggerEvent::Started, this, &AMyVRCharacter::ToggleMagicUI);

		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyVRCharacter::JumpStart);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyVRCharacter::JumpStop);
		}
	}
}


void AMyVRCharacter::Move(const FInputActionValue& Value)
{
	UPlayerAnimInst* AnimInstance = Cast<UPlayerAnimInst>(GetMesh()->GetAnimInstance());
	if (AnimInstance && AnimInstance->bIsAttacking)
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}


void AMyVRCharacter::Look(const FInputActionValue& Value)
{
	UPlayerAnimInst* AnimInstance = Cast<UPlayerAnimInst>(GetMesh()->GetAnimInstance());
	if (AnimInstance && AnimInstance->bIsAttacking)
	{
		return;
	}

	if (MagicWidgetInstance && MagicWidgetInstance->IsInViewport())
	{
		return;
	}

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void AMyVRCharacter::Projectile()
{
	if (bIsCasting) return;
	
	if (!SpellComponent) return;

	if (SpellBook.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpellBook Empty! Adding Test Spell."));
		AddSpellToBook(FName("Fireball"));
		CurrentSpellSlotIndex = 0;
	}

	if (!SpellBook.IsValidIndex(CurrentSpellSlotIndex)) return;

	FCustomizedSpell SelectedSpell = SpellBook[CurrentSpellSlotIndex];
	FFinalSpellData FinalStats = SpellComponent->CalculateFinalStats(SelectedSpell);

	// 마나 체크
	if (!UseMana(FinalStats.FinalManaCost))
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough mana to cast spell. Required: %f, Current: %f"), FinalStats.FinalManaCost, CurrentMana);
		return;
	}
	// PlayerAnimInst를 가져와서 공격 몽타주 재생을 요청합니다.	
	UPlayerAnimInst* AnimInstance = Cast<UPlayerAnimInst>(GetMesh()->GetAnimInstance());
	if (AnimInstance && !AnimInstance->bIsAttacking && !GetCharacterMovement()->IsFalling())
	{
		AnimInstance->PlayAttackMontage();
	}
	else
	{
		return;
	}

	// Cast Time Logic
	if (FinalStats.FinalCastTime > 0.f)
	{
		bIsCasting = true;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("SpawnProjectileDelayed"), FinalStats);
		GetWorld()->GetTimerManager().SetTimer(CastTimerHandle, TimerDelegate, FinalStats.FinalCastTime, false);
		UE_LOG(LogTemp, Log, TEXT("Started Casting. Time: %f"), FinalStats.FinalCastTime);
	}
	else
	{
		SpawnProjectileDelayed(FinalStats);
	}
}

void AMyVRCharacter::SpawnProjectileDelayed(FFinalSpellData FinalStats)
{
	bIsCasting = false;
	
	FVector SpawnLoc = FPSCamera->GetComponentLocation() + (FPSCamera->GetForwardVector() * 50.0f);
	FRotator SpawnRot = FPSCamera->GetComponentRotation();

	if (!IsValidSpawnTransform(SpawnLoc, SpawnRot))
	{
		UE_LOG(LogTemp, Error, TEXT("CastMagicCheck: Invalid spawn transform. Aborting spawn. Loc=%s Rot=%s"), *SpawnLoc.ToString(), *SpawnRot.ToString());
		return;
	}


	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	UClass* ClassToSpawn = FinalStats.ProjectileActorClass;

	if (!ClassToSpawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("CastMagicCheck: ProjectileActorClass is null; aborting spawn."));
		return;
	}

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnLoc, SpawnRot, SpawnParams);
	if (!SpawnedActor)
	{
		UE_LOG(LogTemp, Error, TEXT("CastMagicCheck: SpawnActor failed for class %s"), *ClassToSpawn->GetName());
		return;
	}

	if (AMagicProjectile* MagicProj = Cast<AMagicProjectile>(SpawnedActor))
	{
		MagicProj->InitializeSpell(FinalStats);
		UE_LOG(LogTemp, Log, TEXT("PC Fire! Damage: %f"), FinalStats.FinalDamage);
	}
}

float AMyVRCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		CurrentHealth -= ActualDamage;
		UE_LOG(LogTemp, Log, TEXT("Player took %f damage. Current health: %f"), ActualDamage, CurrentHealth);
		if (CurrentHealth <= 0.f)
		{
			Die();
		}
	}
	return ActualDamage;
}

bool AMyVRCharacter::UseMana(float ManaCost)
{
	if (CurrentMana >= ManaCost)
	{
		CurrentMana -= ManaCost;
		return true;
	}
	return false;
}

void AMyVRCharacter::Die()
{
	UE_LOG(LogTemp, Log, TEXT("Player has died."));	

	// Disable input
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		DisableInput(PC);
	}

	// Enable ragdoll physics
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FCustomizedSpell AMyVRCharacter::GetCurrentCustomizedSpell()
{
	if (SpellBook.IsValidIndex(CurrentSpellSlotIndex))
	{
		return SpellBook[CurrentSpellSlotIndex];
	}
	return FCustomizedSpell(); // Return empty/default struct if not valid
}

void AMyVRCharacter::UpdateCurrentCustomizedSpell(const FCustomizedSpell& NewSpellData)
{
	if (SpellBook.IsValidIndex(CurrentSpellSlotIndex))
	{
		SpellBook[CurrentSpellSlotIndex] = NewSpellData;
	}
}




void AMyVRCharacter::AddSpellToBook(FName BaseRowName)
{
	if (SpellComponent)
	{
		FCustomizedSpell NewSpell = SpellComponent->CreateDefaultCustomSpell(BaseRowName);
		SpellBook.Add(NewSpell);
	}
}

// --- Jump handlers ---
void AMyVRCharacter::JumpStart(const FInputActionValue& Value)
{
	UPlayerAnimInst* AnimInstance = Cast<UPlayerAnimInst>(GetMesh()->GetAnimInstance());
	if (AnimInstance && AnimInstance->bIsAttacking)
	{
		return;
	}
	Super::Jump();
}

void AMyVRCharacter::JumpStop(const FInputActionValue& Value)
{
	Super::StopJumping();
}

void AMyVRCharacter::ToggleMagicUI(const FInputActionValue& Value)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (MagicWidgetInstance && MagicWidgetInstance->IsInViewport())
		{
			// Close UI
			MagicWidgetInstance->RemoveFromParent();
			PC->SetShowMouseCursor(false);
			PC->SetInputMode(FInputModeGameOnly());
		}
		else
		{
			// Open UI
			if (!MagicWidgetInstance && MagicWidgetClass)
			{
				MagicWidgetInstance = CreateWidget<UUserWidget>(PC, MagicWidgetClass);
			}

			if (MagicWidgetInstance)
			{
				MagicWidgetInstance->AddToViewport();
				PC->SetShowMouseCursor(true);
				
				FInputModeGameAndUI InputMode;
				InputMode.SetWidgetToFocus(MagicWidgetInstance->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PC->SetInputMode(InputMode);
			}
		}
	}
}
