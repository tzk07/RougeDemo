// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Projectile.h"
#include "MagicOrb.h"
#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"

AWeapon_Projectile::AWeapon_Projectile()
{
	// 覆盖武器的默认数值
	Cooldown = 0.8f;
	Damage = 50.0f;
	AttackRange = 800.0f;
}

void AWeapon_Projectile::Attack()
{
	if (!OrbClassToSpawn) return;

	// 在发射前寻找距离自己最近，且在攻击范围内的敌人
	// 获取所有怪物
	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyBase::StaticClass(), AllEnemies);

	AActor* TargetEnemy = nullptr;
	float ClosestDistance = AttackRange; // 关键：初始最短距离设为我们的攻击范围！

	// 遍历所有怪物，找到范围内最近的那一个
	for (AActor* Enemy : AllEnemies)
	{
		float DistanceToEnemy = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());

		// 如果这个怪物比当前记录的最近距离还要近（并且自然就小于 AttackRange）
		if (DistanceToEnemy < ClosestDistance)
		{
			ClosestDistance = DistanceToEnemy;
			TargetEnemy = Enemy;
		}
	}

	// 只有当找到了范围内的目标时，才发射法球
	if (TargetEnemy != nullptr)
	{
		FVector DirectionToEnemy = TargetEnemy->GetActorLocation() - GetActorLocation();
		DirectionToEnemy.Z = 0.0f;
		FRotator ShootRotation = DirectionToEnemy.Rotation();

		FActorSpawnParameters SpawnParams;
		// 创建一个FActorSpawnParameters类型的对象，这个对象是 UE 专门用来配置 Actor 生成规则的 “参数容器”
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		// 这两行是强制让法球 Actor “无视碰撞” 生成，解决 “生成位置有障碍物时法球无法生成” 的问题

		AMagicOrb* SpawnedOrb = GetWorld()->SpawnActor<AMagicOrb>(OrbClassToSpawn, GetActorLocation(), ShootRotation, SpawnParams);

		if (SpawnedOrb)
		{
			// 将基类的伤害和范围，通过初始化函数传递给法球
			SpawnedOrb->InitializeOrb(this->Damage, this->AttackRange);
		}
	}
}