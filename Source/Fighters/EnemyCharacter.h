#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

// AI state
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
Idle,
Chasing,
Attacking,
Dead
};


UCLASS()
class FIGHTERS_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()



public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float CurrentHealth;


	// health UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UTextRenderComponent* HealthTextComp;

	// state UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UTextRenderComponent* StateTextComp;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// Update health text UI
	void UpdateHealthText();

	// Update state text UI
	void UpdateStateText();

	// speedMovement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MoveSpeed = 300.f;

	// damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Damage = 10.f;

	// AI acceptance radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AcceptanceRadius = 300.f;

	// target actor
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI")
	AActor* TargetActor;

	// AI State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EEnemyState CurrentState;

	// AI sight radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float SightRadius = 2000.f;

	// AI field of view (degrees)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float FieldOfView = 90.f;

	// Apply knockback to enemy
	void Knockback(const FVector& Direction, float Force);

private:
	void Die();
};