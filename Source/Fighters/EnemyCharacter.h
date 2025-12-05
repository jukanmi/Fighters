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
	class UTextRenderComponent *HealthTextComp;

	// state UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UTextRenderComponent *StateTextComp;

	/** 오른손 주먹에 부착될 콜리전 스피어 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	class USphereComponent* RightFistCollisionSphere;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;

	// Update health text UI
	void UpdateHealthText();

	// Update state text UI
	void UpdateStateText();

	// speedMovement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MoveSpeed = 300.f;

	/** 공격 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackDamage = 10.f;

	/** 공격 반경 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRadius = 50.f;
	

	/** AnimNotify에서 호출되어 주먹 콜리전을 활성화합니다. */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ActivateFistCollision();

	/** AnimNotify에서 호출되어 주먹 콜리전을 비활성화합니다. */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void DeactivateFistCollision();

	// AI acceptance radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AcceptanceRadius = 300.f;

	// target actor
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI")
	AActor *TargetActor;

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
	void Knockback(const FVector &Direction, float Force);

private:
	void Die();

	/** 주먹 콜리전의 오버랩 이벤트가 발생했을 때 호출될 함수 */
	UFUNCTION()
	void OnFistOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** 한 번의 공격 스윙 동안 이미 데미지를 입은 액터들의 목록 */
	UPROPERTY()
	TArray<AActor*> HitActorsInSwing;
};