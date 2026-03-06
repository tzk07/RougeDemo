// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Beam.h"
#include "EnemyBase.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h" // 提供射线检测和画线功能

AWeapon_Beam::AWeapon_Beam()
{
	// 设置光束武器的默认属性
	Damage = 30.0f;
	Cooldown = 1.2f;
	AttackRange = 1200.0f; // 索敌和光束的最远距离
	BeamWidth = 40.0f;     // 光束的半径为40
}

void AWeapon_Beam::Attack()
{
	// 索敌逻辑：和法球一样，找到范围内的最近敌人作为发射方向
	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyBase::StaticClass(), AllEnemies);

	AActor* TargetEnemy = nullptr;
	float ClosestDistance = AttackRange; // 限制在攻击范围内

	for (AActor* Enemy : AllEnemies)
	{
		float DistanceToEnemy = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
		if (DistanceToEnemy < ClosestDistance)
		{
			ClosestDistance = DistanceToEnemy;
			TargetEnemy = Enemy;
		}
	}

	// 如果找到了目标，就发射光束
	if (TargetEnemy != nullptr)
	{
		// 计算射击的起点和方向
		FVector StartLocation = GetActorLocation();
		FVector Direction = (TargetEnemy->GetActorLocation() - StartLocation).GetSafeNormal();
		Direction.Z = 0.0f; // 保持在水平面上

		// 光束的终点 = 起点 + (方向 * 射程)
		FVector EndLocation = StartLocation + (Direction * AttackRange);

		// 准备射线检测的参数 (相当于蓝图里的 SphereTraceMultiForObjects)
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		// 检测目标为 Pawn（角色物理类型）
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetOwner()); // 忽略玩家自己

		// 用于存放射线打中的所有结果
		TArray<FHitResult> HitResults;

		// 执行多重球体检测（打穿一条线）
		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
			this,
			StartLocation,
			EndLocation,
			BeamWidth,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration, // 核心！画出蓝红相间的调试光束，持续显示一小段时间
			HitResults,
			true,
			FLinearColor::Blue, // 光束颜色
			FLinearColor::Red,  // 打中东西时的颜色
			0.5f // 调试光束在屏幕上残留 0.5 秒
		);

		// 如果打中了东西，遍历所有命中的目标造成伤害
		if (bHit)
		{
			// 创建一个数组记录已经受到伤害的怪物，防止同一个怪物因为模型复杂被射线重复击中多次
			TArray<AActor*> DamagedEnemies;

			for (const FHitResult& Hit : HitResults)
			{
				AActor* HitActor = Hit.GetActor();

				// 如果打中了Actor，并且这个Actor还没有被我们算过伤害
				if (HitActor && !DamagedEnemies.Contains(HitActor))
				{
					if (AEnemyBase* Enemy = Cast<AEnemyBase>(HitActor))
					{
						if (UHealthComponent* EnemyHealth = Enemy->FindComponentByClass<UHealthComponent>())
						{
							EnemyHealth->TakeDamage(Damage);
							DamagedEnemies.Add(HitActor);
						}
					}
				}
			}
		}
	}
}