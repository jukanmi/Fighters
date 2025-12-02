// Fill out your copyright notice in the Description page of Project Settings.
#include "SpellComponent.h"

USpellComponent::USpellComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USpellComponent::BeginPlay()
{
    Super::BeginPlay();
}

FFinalSpellData USpellComponent::CalculateFinalStats(const FCustomizedSpell& CustomData)
{
    FFinalSpellData ResultData;

    if (SpellDataTable == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("SpellComponent: DataTable is NOT set!"));
        return ResultData;
    }

    if (CustomData.BaseSpellRowName.IsNone())
    {
        return ResultData;
    }

    static const FString ContextString(TEXT("Spell Calculation"));
    FSpellBaseData* BaseData = SpellDataTable->FindRow<FSpellBaseData>(CustomData.BaseSpellRowName, ContextString);

    if (BaseData)
    {
        // Merit
        ResultData.FinalDamage = BaseData->BaseDamage * CustomData.DamageRatio;
        ResultData.FinalKnockbackForce = BaseData->BaseKnockbackForce * CustomData.KnockbackForceRatio;
        ResultData.FinalAOERange = BaseData->BaseAOERange * CustomData.AOERangeRatio;

        ResultData.FinalDuration = BaseData->BaseDuration * CustomData.DurationRatio;
        ResultData.FinalSpeed = BaseData->BaseSpeed * CustomData.SpeedRatio;

        // Demerit
        ResultData.FinalManaCost = BaseData->BaseManaCost * CustomData.ManaCostRatio;
        ResultData.FinalCastTime = BaseData->BaseCastTime * CustomData.CastTimeRatio;
        ResultData.FinalCooldownTime = BaseData->BaseCooldownTime * CustomData.CooldownTimeRatio;

        // Others
         ResultData.ProjectileEffect = BaseData->ProjectileEffect;
         ResultData.ProjectileActorClass = BaseData->ProjectileActorClass;
         ResultData.ImpactFX = BaseData->ImpactFX;
         ResultData.ImpactSound = BaseData->ImpactSound;

    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SpellComponent: Cannot find row '%s' in DataTable"), *CustomData.BaseSpellRowName.ToString());
    }

    return ResultData;
}

FCustomizedSpell USpellComponent::CreateDefaultCustomSpell(FName RowName)
{
    FCustomizedSpell NewSpell;
    NewSpell.BaseSpellRowName = RowName;
    return NewSpell;
}