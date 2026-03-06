// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RougeGameMode.generated.h"

// 前向声明怪物基类
class AEnemyBase;

/**
 *  Simple Game Mode for a top-down perspective game
 *  Sets the default gameplay framework classes
 *  Check the Blueprint derived class for the set values
 */
UCLASS(abstract)
class ARougeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/** Constructor */
	ARougeGameMode();

protected:
	virtual void BeginPlay() override;

	// ====== 刷怪配置 ======

	// 要生成的普通怪物蓝图类 (可以在蓝图里配置)
	UPROPERTY(EditDefaultsOnly, Category = "RougeSpawning")
	TSubclassOf<AEnemyBase> NormalEnemyClass;

	// 要生成的Boss蓝图类
	UPROPERTY(EditDefaultsOnly, Category = "RougeSpawning")
	TSubclassOf<AEnemyBase> BossEnemyClass;

	// 每次刷怪的间隔时间 (秒)
	UPROPERTY(EditDefaultsOnly, Category = "RougeSpawning")
	float SpawnInterval;

	// 距离玩家多远生成怪物 (保证生成在屏幕外)
	UPROPERTY(EditDefaultsOnly, Category = "RougeSpawning")
	float SpawnRadius;

	// Boss生成的时间 (秒)
	UPROPERTY(EditDefaultsOnly, Category = "RougeSpawning")
	float BossSpawnTime;

	// ====== 计时器与逻辑 ======

	FTimerHandle EnemySpawnTimerHandle;
	FTimerHandle BossSpawnTimerHandle;

	// 生成普通怪物
	UFUNCTION()
	void SpawnEnemy();

	// 生成Boss
	UFUNCTION()
	void SpawnBoss();

	// 获取玩家周围的随机生成点
	FVector GetRandomSpawnLocation();
};



