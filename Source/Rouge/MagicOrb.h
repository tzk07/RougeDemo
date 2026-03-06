// Fill out your copyright notice in the Description page of Project Settings.
// 法球实体
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicOrb.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class ROUGE_API AMagicOrb : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMagicOrb();

	// 开放给武器赋值的变量: 
	// 这颗法球能造成多少伤害
	float OrbDamage;
	// 法球的最大飞行距离
	float MaxRange;

	void InitializeOrb(float InDamage, float InRange);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 碰撞球 —— 处理交互
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeComponents")
	USphereComponent* CollisionComp;

	// 法球模型 —— 视觉模型
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeComponents")
	UStaticMeshComponent* MeshComp;

	// 抛体运动组件 —— 控制运动
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeComponents")
	UProjectileMovementComponent* ProjectileMovement;

	// 记录法球发射时的初始位置
	FVector StartLocation;

	// 碰到怪物的处理逻辑
	UFUNCTION()
	void OnOrbOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};