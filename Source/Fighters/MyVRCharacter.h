// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MagicStruct.h"
#include "MyVRCharacter.generated.h"

class UCameraComponent;
class USpellComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class FIGHTERS_API AMyVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FPSCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpellComponent* SpellComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	TArray<FCustomizedSpell> SpellBook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	int32 CurrentSpellSlotIndex = 0;

	/** Adds a new spell to the spellbook based on a data table row. */
	UFUNCTION(BlueprintCallable, Category = "Magic")
	void AddSpellToBook(FName BaseRowName);

	/** 
	 * Gets a copy of the currently selected customized spell data.
	 * Use this to display spell stats in the UI.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Magic")
	FCustomizedSpell GetCurrentCustomizedSpell();

	/**
	 * Updates the currently selected spell with new customized data.
	 * Call this when UI values are changed by the player.
	 */
	UFUNCTION(BlueprintCallable, Category = "Magic")
	void UpdateCurrentCustomizedSpell(const FCustomizedSpell& NewSpellData);

	// --- Health and Mana ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxMana = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentMana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float ManaRegenRate = 10.f;



	/** Applies damage to the character and returns the actual damage taken after calculations. */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	/** Consumes mana if enough is available. Returns true if mana was consumed, false otherwise. */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool UseMana(float ManaCost);

	/** Handles character death logic. */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void Die();

	

protected:
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* CastSpellAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ToggleUIAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> MagicWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UUserWidget* MagicWidgetInstance;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void ToggleMagicUI(const FInputActionValue& Value);

	/** Called for casting spell */
	void Projectile();
	
	UFUNCTION()
	void SpawnProjectileDelayed(FFinalSpellData FinalStats);
	
	FTimerHandle CastTimerHandle;
	bool bIsCasting = false;

	/** Called for jumping input */
	void JumpStart(const FInputActionValue& Value);
	void JumpStop(const FInputActionValue& Value);

};
