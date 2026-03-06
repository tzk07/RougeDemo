// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Ring.h"
#include "EnemyBase.h"
#include "HealthComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h" // 引入画调试线的库

AWeapon_Ring::AWeapon_Ring()
{
	// 环形武器的特点：伤害低，频率极快，范围适中
	Damage = 15.0f;
	Cooldown = 0.5f; // 每 0.5 秒判定一次伤害
	AttackRange = 300.0f; // 保护圈的半径（3米）
}

void AWeapon_Ring::Attack()
{
	FVector CenterLocation = GetActorLocation();

	// 准备范围检测参数
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this); // 忽略武器自身
	ActorsToIgnore.Add(UGameplayStatics::GetPlayerPawn(this, 0)); // 忽略玩家

	// 用于存放检测到的目标
	TArray<AActor*> OutActors;

	// 执行球体范围重叠检测 (相当于蓝图里的 Sphere Overlap Actors)
	// 注意这里我们直接把 AEnemyBase::StaticClass() 传进去作为过滤条件，进一步保证安全！
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		this,
		CenterLocation,
		AttackRange,
		ObjectTypes,
		AEnemyBase::StaticClass(), // 只把继承自 AEnemyBase 的目标放进 OutActors 数组
		ActorsToIgnore,
		OutActors
	);

	// 遍历在圆圈内的所有怪物，并造成伤害
	if (bHit)
	{
		for (AActor* HitActor : OutActors)
		{
			// 因为前面已经用 AEnemyBase::StaticClass() 过滤了，这里 Cast 必定成功，但写上是好习惯
			if (AEnemyBase* Enemy = Cast<AEnemyBase>(HitActor))
			{
				if (UHealthComponent* EnemyHealth = Enemy->FindComponentByClass<UHealthComponent>())
				{
					EnemyHealth->TakeDamage(Damage);
				}
			}
		}
	}

	// 画一个绿色的圆球，让你在测试时能看见伤害范围！
	// 参数：世界，中心点，半径，圆滑度(线段数)，颜色，是否永久显示，持续时间
	DrawDebugSphere(GetWorld(), CenterLocation, AttackRange, 32, FColor::Green, false, 0.2f);
}

