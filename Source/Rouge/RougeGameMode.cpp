// Copyright Epic Games, Inc. All Rights Reserved.

#include "RougeGameMode.h"
#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

ARougeGameMode::ARougeGameMode()
{
	// 初始化默认值
	SpawnInterval = 2.0f;     // 每2秒刷一只怪
	SpawnRadius = 1500.0f;    // 在距离玩家15米外生成（屏幕外）
	BossSpawnTime = 60.0f;    // 游戏开始60秒后出Boss（为了测试快一点，你之后可以改成180秒）
}

void ARougeGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 1. 开启普通怪物的循环生成定时器
	GetWorld()->GetTimerManager().SetTimer(EnemySpawnTimerHandle, this, &ARougeGameMode::SpawnEnemy, SpawnInterval, true);

	// 2. 开启Boss生成定时器 (注意最后一个参数是 false，代表只执行一次)
	GetWorld()->GetTimerManager().SetTimer(BossSpawnTimerHandle, this, &ARougeGameMode::SpawnBoss, BossSpawnTime, false);
}

void ARougeGameMode::SpawnEnemy()
{
	// 如果没有配置怪物类，直接返回
	if (!NormalEnemyClass) return;

	// 获取生成位置
	FVector SpawnLocation = GetRandomSpawnLocation();

	// 在世界中生成怪物
	GetWorld()->SpawnActor<AEnemyBase>(NormalEnemyClass, SpawnLocation, FRotator::ZeroRotator);
}

void ARougeGameMode::SpawnBoss()
{
	if (!BossEnemyClass) return;

	// 可以在这里打印一句警告，或者播放一个Boss警报音效
	UE_LOG(LogTemp, Warning, TEXT("WARNING: BOSS INCOMING!!!"));

	FVector SpawnLocation = GetRandomSpawnLocation();
	GetWorld()->SpawnActor<AEnemyBase>(BossEnemyClass, SpawnLocation, FRotator::ZeroRotator);
}

FVector ARougeGameMode::GetRandomSpawnLocation()
{
	// 找到玩家
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!PlayerCharacter) return FVector::ZeroVector;

	FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	// 生成一个 0 到 360 度的随机角度
	float RandomAngle = FMath::RandRange(0.0f, 360.0f);

	// 将角度转为弧度，并用三角函数计算出圆周上的 X 和 Y 坐标偏差
	float Radian = FMath::DegreesToRadians(RandomAngle);
	float OffsetX = FMath::Cos(Radian) * SpawnRadius;
	float OffsetY = FMath::Sin(Radian) * SpawnRadius;

	// 玩家位置加上偏差，就是最终的生成位置
	FVector FinalLocation = PlayerLocation + FVector(OffsetX, OffsetY, 0.0f);
	return FinalLocation;
}