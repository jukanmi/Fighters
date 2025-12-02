#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MagicStruct.generated.h"

class UNiagaraSystem;
class USoundBase;

USTRUCT(BlueprintType)
struct FSpellBaseData : public FTableRowBase
{
	GENERATED_BODY() 

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Stats")
	float BaseDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Stats")
	float BaseKnockbackForce = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Stats")
	float BaseAOERange = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Stats")
	float BaseDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Stats")
	float BaseSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Cost")
	float BaseManaCost = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Cost")
	float BaseCastTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Cost")
	float BaseCooldownTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ProjectileActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UObject *ProjectileEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	UNiagaraSystem* ImpactFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	USoundBase* ImpactSound;
};

USTRUCT(BlueprintType)
struct FCustomizedSpell
{
	GENERATED_BODY() 

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BaseSpellRowName;
	// --- Merit (Positive Effects) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ratio | Merit", meta = (ClampMin = "1.0"))
	float DamageRatio = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ratio | Merit", meta = (ClampMin = "1.0"))
	float KnockbackForceRatio = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ratio | Merit", meta = (ClampMin = "1.0"))
	float AOERangeRatio = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ratio | Merit", meta = (ClampMin = "1.0"))
	float DurationRatio = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ratio | Merit", meta = (ClampMin = "1.0"))
	float SpeedRatio = 1.0f;

	// --- Demerit (Negative Effects) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ratio | Demerit")
	float ManaCostRatio = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ratio | Demerit")
	float CastTimeRatio = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ratio | Demerit")
	float CooldownTimeRatio = 1.0f;
};

USTRUCT(BlueprintType)
struct FFinalSpellData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Final Stats")
	float FinalDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Final Stats")
	float FinalKnockbackForce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Final Stats")
	float FinalAOERange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Final Stats")
	float FinalDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Final Stats")
	float FinalSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Final Stats")
	float FinalManaCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Final Stats")
	float FinalCastTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Final Stats")
	float FinalCooldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ProjectileActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UObject *ProjectileEffect = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	UNiagaraSystem* ImpactFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	USoundBase* ImpactSound;

	FFinalSpellData()
		: FinalDamage(0), FinalKnockbackForce(0), FinalAOERange(0),
		  FinalManaCost(0), FinalCastTime(0), FinalCooldownTime(0)
	{
	}
};