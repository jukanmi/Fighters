#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "MagicStruct.h"
#include "Engine/DamageEvents.h"
#include "MagicProjectile.generated.h"

class UNiagaraSystem;
class USoundBase;

UCLASS()
class FIGHTERS_API AMagicProjectile : public AActor
{
	GENERATED_BODY()

public:
	AMagicProjectile();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Visual)
	class UNiagaraComponent* EffectComp;


	//Magic spell data
	UPROPERTY(BlueprintReadOnly, Category = "Magic Stats")
	FFinalSpellData SpellData;


	// Initialize spell data
	UFUNCTION(BlueprintCallable, Category = "Magic Logic")
	void InitializeSpell(const FFinalSpellData& NewData);

	// Called when projectile hits something
	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};