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
    this->bIsDie = false;

}


void UEnemyAnimInst::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    ACharacter *OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
    if (OwnerCharacter)
    {
        FVector Velocity = OwnerCharacter->GetVelocity();
        Velocity.Z = 0.f;
        Speed = Velocity.Size();

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
    if (Montage == AttackMontage)
    {
        bIsAttacking = false;
        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(TryGetPawnOwner());
        if (Enemy)
        {
            Enemy->CurrentState = EEnemyState::Chasing;
        }
    }
    else if (Montage == DieMontage)
    {
        AActor* OwnerActor = TryGetPawnOwner();
        if(OwnerActor) OwnerActor->Destroy();
    }
}