// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInst.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"

UPlayerAnimInst::UPlayerAnimInst()
{
	Speed = 0.f;
	bIsFalling = false;
	bIsMovingBackward = false;
	bIsAttacking = false;
	AttackMontage = nullptr;
}

void UPlayerAnimInst::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 애니메이션 인스턴스의 오너(캐릭터)를 가져옵니다.
	ACharacter* OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		// 캐릭터의 속력을 계산합니다. (수평 속도만)
		FVector Velocity = OwnerCharacter->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size();

		// 캐릭터의 이동 컴포넌트에서 공중 상태인지 확인합니다.
		if (OwnerCharacter->GetCharacterMovement())
		{
			bIsFalling = OwnerCharacter->GetCharacterMovement()->IsFalling();

			float DotProduct = FVector::DotProduct(Velocity.GetSafeNormal(), OwnerCharacter->GetActorForwardVector());
			bIsMovingBackward = Speed > 0.f && DotProduct < -0.7f;
		}
	}
}

void UPlayerAnimInst::PlayAttackMontage()
{
	if (!bIsAttacking && AttackMontage)
	{
		Montage_Play(AttackMontage);
		bIsAttacking = true;
	}
}

void UPlayerAnimInst::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
	{
		bIsAttacking = false;
	}
}