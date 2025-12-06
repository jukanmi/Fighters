// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInst.generated.h"

/**
 *
 */
UCLASS()
class FIGHTERS_API UEnemyAnimInst : public UAnimInstance
{
	GENERATED_BODY()
protected:

public:
	// 생성자
	UEnemyAnimInst();



	// 매 프레임 호출되는 업데이트 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 적의 현재 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float Speed;

	// 적이 공중에 떠 있는지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsFalling;

	// 적이 공격 중인지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsAttacking;

	// 적이 죽음 중인지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsDie;

	// 공격 몽타주를 재생하는 함수
	void PlayAttackMontage();

	// 죽음 몽타주를 재생하는 함수
	void PlayDieMontage();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* DieMontage;



	// attack montage is ended callback
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
