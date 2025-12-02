// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInst.generated.h"
/**
 * 
 */
UCLASS()
class FIGHTERS_API UPlayerAnimInst : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// 생성자
	UPlayerAnimInst();

	// 매 프레임 호출되는 업데이트 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 공격 몽타주를 재생하는 함수
	void PlayAttackMontage();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsAttacking;

protected:
	// 캐릭터의 현재 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float Speed;

	// 캐릭터가 공중에 떠 있는지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsFalling;

	// 캐릭터가 뒤로 이동하는지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsMovingBackward;


	// 공격 몽타주 (블루프린트에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* AttackMontage;

	// attack montage is ended callback
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
