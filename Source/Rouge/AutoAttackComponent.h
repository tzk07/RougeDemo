// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AutoAttackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROUGE_API UAutoAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAutoAttackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// ====== 攻击属性配置 ======
	// 攻击范围 (半径)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RougeAttack")
	float AttackRadius;
	// 每次攻击造成的伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RougeAttack")
	float AttackDamage;
	// 攻击间隔 (冷却时间)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RougeAttack")
	float AttackCooldown;
	// 自动攻击的定时器句柄
	FTimerHandle AttackTimerHandle;

	// ====== 核心攻击函数 ======

	// 执行一次AOE范围攻击
	UFUNCTION()
	void ExecuteAOEAttack();	
};
