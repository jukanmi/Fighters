#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyAnimInst.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	HealthTextComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthText"));
	HealthTextComp->SetupAttachment(GetRootComponent());
	HealthTextComp->SetRelativeLocation(FVector(0.f, 0.f, 100.f)); // �Ӹ� ��
	HealthTextComp->SetHorizontalAlignment(EHTA_Center);
	HealthTextComp->SetTextRenderColor(FColor::Red);
	HealthTextComp->SetWorldSize(20.0f);

	StateTextComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("StateText"));
	StateTextComp->SetupAttachment(GetRootComponent());
	StateTextComp->SetRelativeLocation(FVector(0.f, 0.f, 80.f)); // 체력 텍스트 아래
	StateTextComp->SetHorizontalAlignment(EHTA_Center);
	StateTextComp->SetTextRenderColor(FColor::White);
	StateTextComp->SetWorldSize(15.0f);

	// movement speed
	MoveSpeed = 300.f;

	// Enable movement by CharacterMovement component
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

	// AI 초기 상태 설정
	CurrentState = EEnemyState::Idle;
	
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	UpdateHealthText();
	UpdateStateText();

	// CharacterMovement speed setup
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UEnemyAnimInst* AnimInstance = Cast<UEnemyAnimInst>(GetMesh()->GetAnimInstance());
	if (!AnimInstance) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	FVector ToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
	float DistanceToPlayer = ToPlayer.Size();

	switch (CurrentState)
	{
	case EEnemyState::Idle:
		// 플레이어가 시야 반경 내에 있는지 확인
		if (DistanceToPlayer <= SightRadius)
		{
			// 플레이어가 시야각 내에 있는지 확인
			FVector Forward = GetActorForwardVector();
			FVector ToPlayerNormal = ToPlayer.GetSafeNormal();
			float DotProduct = FVector::DotProduct(Forward, ToPlayerNormal);
			float Angle = FMath::Acos(DotProduct);

			if (Angle <= FMath::DegreesToRadians(FieldOfView / 2.0f))
			{
				// 플레이어와 적 사이에 장애물이 없는지 확인 (Line of Sight)
				FHitResult HitResult;
				FVector Start = GetActorLocation();
				FVector End = PlayerPawn->GetActorLocation();
				FCollisionQueryParams CollisionParams;
				CollisionParams.AddIgnoredActor(this);

				bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

				// 광선이 플레이어에 부딪혔거나, 아무것도 부딪히지 않았다면(장애물이 없음)
				if (!bHit || (bHit && HitResult.GetActor() == PlayerPawn))
				{
					// 플레이어 발견! 추적 상태로 변경
					TargetActor = PlayerPawn;
					CurrentState = EEnemyState::Chasing;
					UpdateStateText();
					UE_LOG(LogTemp, Log, TEXT("Player detected! Start chasing."));
				}

			}
		}
		AnimInstance->bIsAttacking = false; // 대기 상태에서는 공격 안함
		break;

	case EEnemyState::Chasing:
		if (TargetActor)
		{	
			if (AnimInstance->bIsAttacking)
			{
				break;
			}

			// 공격 범위에 도달했는지 확인
			if (DistanceToPlayer <= AcceptanceRadius)
			{
				CurrentState = EEnemyState::Attacking;
				UpdateStateText();
				GetCharacterMovement()->StopMovementImmediately(); // 공격 시 이동 멈춤
				UE_LOG(LogTemp, Log, TEXT("In attack range."));
			}
			// 플레이어가 시야 반경을 벗어났는지 확인
			else if (DistanceToPlayer > SightRadius)
			{
				CurrentState = EEnemyState::Idle;
				UpdateStateText();
				UE_LOG(LogTemp, Log, TEXT("Player lost. Stop chasing."));
			}
			else
			{
				// 타겟을 향해 이동
				FVector Dir = ToPlayer.GetSafeNormal();
				AddMovementInput(Dir, 1.0f);
			}
		}
		else
		{
			// 타겟을 잃으면 대기 상태로
			CurrentState = EEnemyState::Idle;
			UpdateStateText();
		}
		AnimInstance->bIsAttacking = false; // 추적 중에는 공격 안함
		break;

	case EEnemyState::Attacking:
		// 공격 범위를 벗어나면 다시 추적
		if (DistanceToPlayer > AcceptanceRadius)
		{
			CurrentState = EEnemyState::Chasing;
			UpdateStateText();
		}
		else if (AnimInstance && !AnimInstance->bIsAttacking)
		{
			// 공격 몽타주 재생
			AnimInstance->PlayAttackMontage();
		}
		break;
	}
}

void AEnemyCharacter::Die()
{
	CurrentState = EEnemyState::Dead;
	UpdateStateText();

	// Disable character functionalities
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
	SetActorEnableCollision(false);

	// Play death animation
	UEnemyAnimInst* AnimInstance = Cast<UEnemyAnimInst>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->PlayDieMontage();
	}
	UE_LOG(LogTemp, Log, TEXT("Enemy %s has died."), *GetName());
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		CurrentHealth -= ActualDamage;
		UpdateHealthText();
		if (CurrentHealth <= 0.f)
		{
			UE_LOG(LogTemp, Log, TEXT("Enemy %s health depleted."), *GetName());
			Die();
		}
	}
	return ActualDamage;
}

void AEnemyCharacter::UpdateHealthText()
{
	FString HealthStr = FString::Printf(TEXT("HP: %.0f / %.0f"), CurrentHealth, MaxHealth);
	HealthTextComp->SetText(FText::FromString(HealthStr));
}

void AEnemyCharacter::UpdateStateText()
{
	FString StateStr;
	switch (CurrentState)
	{
	case EEnemyState::Idle:
		StateStr = "Idle";
		break;
	case EEnemyState::Chasing:
		StateStr = "Chasing";
		break;
	case EEnemyState::Attacking:
		StateStr = "Attacking";
		break;
	case EEnemyState::Dead:
		StateStr = "Dead";
		break;
	}

	StateTextComp->SetText(FText::FromString(StateStr));
}

void AEnemyCharacter::Knockback(const FVector& Direction, float Force)
{
	if (GetCharacterMovement())
	{
		// Apply knockback force. We want to launch the character upwards slightly as well (XYOverride, ZOverride)
		GetCharacterMovement()->Launch(Direction * Force + FVector(0, 0, Force * 0.5f));
		UE_LOG(LogTemp, Log, TEXT("Enemy %s knocked back with force %f"), *GetName(), Force);
	}
}