// Fill out your copyright notice in the Description page of Project Settings.
/*
	三个核心功能：
		生命组件
		伤害碰撞体：一个球形触发器，碰到玩家就造成伤害。
		AI移动逻辑：在每一帧（Tick）中获取玩家的位置，并向玩家移动。
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

// 前向声明
class UHealthComponent;
class USphereComponent;
class ARougeCharacter;
class AExperienceGem;

UCLASS()
class ROUGE_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ====== 核心组件 ====== 
	// 生命值组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeComponents")
	UHealthComponent* HealthComp;

	// 经验球组件
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RougeDrop")
	TSubclassOf<class AExperienceGem> GemClassToDrop;

	// 攻击碰撞范围: 碰撞伤害的触发范围
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeComponents")
	USphereComponent* AttackCollision;

	// 属性与引用
	// 碰撞伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RougeCombat")
	float CollisionDamage;

	// 缓存玩家的指针，避免每帧都去搜索玩家，节省性能
	UPROPERTY()
	ARougeCharacter* PlayerTarget;

	// 伤害触发的内置CD（冷却时间），默认比如 0.5 秒
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RougeCombat")
	float DamageCooldown;

	// 定时器句柄，用于管理伤害循环
	FTimerHandle DamageTimerHandle;

	// 伤害触发函数
	// 这是一个非常经典的UE碰撞事件函数签名，参数必须长这样才能绑定到碰撞体上
	// 开始重叠函数
	UFUNCTION()
	void OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 结束重叠函数 (名字可以自定义，但参数必须长这样)
	UFUNCTION()
	void OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 新增：真正用来扣血的函数
	void DealDamage();

	// 处理死亡的函数
	UFUNCTION()
	void Die();

public:	
	// Called every frame
	// 每一帧调用的函数，用来处理怪物的移动逻辑
	virtual void Tick(float DeltaTime) override;
};
