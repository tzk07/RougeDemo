// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class ROUGE_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	// 接收强化数值的函数
	UFUNCTION(BlueprintCallable, Category = "Weapon Stats")
	void UpgradeStats(float DamageBonus, float CooldownReduction, float RangeBonus);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 武器通用属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	float AttackRange;

	// 存储初始基数
	float BaseDamage;
	float BaseCooldown;
	float BaseAttackRange;

	// 存储累加倍率
	float DamageMultiplier;
	float CooldownMultiplier;
	float RangeMultiplier;

	// 管理攻击频率的定时器
	FTimerHandle AttackTimerHandle;

	// 核心攻击函数
	UFUNCTION()
	virtual void Attack();
};
