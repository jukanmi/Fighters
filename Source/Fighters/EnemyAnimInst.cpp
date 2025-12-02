// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAnimInst.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UEnemyAnimInst::UEnemyAnimInst()
{
    this->Speed = 0.f;
    this->bIsFalling = false;
    this->bIsAttacking = false;

}

void UEnemyAnimInst::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // 애니메이션 인스턴스의 오너(캐릭터)를 가져옵니다.
    ACharacter *OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
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
        }
    }
}

void UEnemyAnimInst::PlayAttackMontage()
{
    if (!bIsAttacking && AttackMontage)
    {
        Montage_Play(AttackMontage);
        bIsAttacking = true;
    }
}

void UEnemyAnimInst::PlayDieMontage()
{
    if (DieMontage)
    {
        Montage_Play(DieMontage);
    }
}

void UEnemyAnimInst::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // 공격 몽타주가 끝났을 때 bIsAttacking을 false로 설정
    if (Montage == AttackMontage)
    {
        UE_LOG(LogTemp, Log, TEXT("Enemy Attack Montage Ended"));
        bIsAttacking = false;
        // 공격이 끝나면 다시 Chasing 상태로 전환하여 다음 행동을 결정하도록 함
        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(TryGetPawnOwner());
        if (Enemy)
        {
            Enemy->CurrentState = EEnemyState::Chasing;
        }
    }
    // 죽음 몽타주가 끝났을 때 오너 액터를 제거
    else if (Montage == DieMontage)
    {
        AActor* OwnerActor = TryGetPawnOwner();
        if(OwnerActor) OwnerActor->Destroy();
    }
}