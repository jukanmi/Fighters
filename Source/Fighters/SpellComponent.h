// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MagicStruct.h"
#include "SpellComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FIGHTERS_API USpellComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell Database")
	class UDataTable* SpellDataTable;

	UFUNCTION(BlueprintCallable, Category = "Spell Logic")
	FFinalSpellData CalculateFinalStats(const FCustomizedSpell& CustomData);

	UFUNCTION(BlueprintCallable, Category = "Spell Logic")
	FCustomizedSpell CreateDefaultCustomSpell(FName RowName);
};