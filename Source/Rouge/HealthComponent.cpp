// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	// 生命组件不需要每帧 Tick, 可以关闭节省性能
	PrimaryComponentTick.bCanEverTick = false;
	// 初始化默认血量
	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;
	bIsDead = false;
	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// 确保游戏开始时满血
	CurrentHealth = MaxHealth;
	bIsDead = false;
}


// 扣血逻辑实现
void UHealthComponent::TakeDamage(float DamageAmount) {
	// 已经死亡直接 return
	if (bIsDead || DamageAmount <= 0.0f) {
		return;
	}
	// 扣除血量, 使用 FMath:Clamp 确保 0 <= CurrentHealth <= MaxHealth
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	// 广播血量改变事件(通知绑定的 UI 或 特效)
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	// 检查是否死亡, 死亡则广播死亡事件
	if (CurrentHealth <= 0.0f) {
		bIsDead = true;
		OnHealthZero.Broadcast();
	}
}
