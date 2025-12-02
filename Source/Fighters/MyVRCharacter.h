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

	/** Toggles the magic customization UI on or off. */
	
	

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

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for casting spell */
	void Projectile();

	/** Called for jumping input */
	void JumpStart(const FInputActionValue& Value);
	void JumpStop(const FInputActionValue& Value);

};
