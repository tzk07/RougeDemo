// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


// 声明两个动态多播委托 (用于给蓝图绑定事件)
// 1. 当血量改变时触发，传递当前血量和最大血量
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
// 2. 当血量归零时触发
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthZeroSignature);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ROUGE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

protected:
	virtual void BeginPlay() override;

	// 最大生命值 (EditAnywhere 允许在蓝图属性面板配置)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	// 当前生命值 (VisibleAnywhere 表示只能看不能直接改，必须通过函数改)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	// 标记是否已经死亡，防止重复触发死亡逻辑
	bool bIsDead;

public:
	// 核心扣血函数 (BlueprintCallable 允许在蓝图里调用)
	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float DamageAmount);

	// 实例化上面声明的委托，开放给蓝图绑定
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthZeroSignature OnHealthZero;
};