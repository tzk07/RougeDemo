// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoAttackComponent.h"
#include "Kismet/KismetSystemLibrary.h" // 提供蓝图常用的系统函数 (如范围检测和画线调试)
#include "EnemyBase.h"                  // 包含怪物基类，用于过滤目标
#include "HealthComponent.h"            // 包含生命组件，用于扣血

// Sets default values for this component's properties
UAutoAttackComponent::UAutoAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// 设置默认的攻击属性
	AttackRadius = 300.0f; // 范围300厘米
	AttackDamage = 30.0f;  // 每次伤害30
	AttackCooldown = 1.0f; // 每1秒攻击一次
}


// Called when the game starts
void UAutoAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	// 游戏开始时，启动循环定时器。每隔 AttackCooldown 秒，自动调用一次 ExecuteAOEAttack 函数
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &UAutoAttackComponent::ExecuteAOEAttack, AttackCooldown, true);
}

void UAutoAttackComponent::ExecuteAOEAttack()
{
	// 1. 获取组件拥有者(玩家)的位置，作为攻击的圆心
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;
	FVector CenterLocation = OwnerActor->GetActorLocation();

	// 2. 准备检测参数 (对应蓝图节点的引脚)
	// ObjectTypes: 检测哪种物理类型的物体 (Pawn代表角色)
	// 核心作用：限定 “检测的物体类型”，只找 “Pawn 类物体”（UE5 中 Pawn 是可操控的角色类，比如玩家、怪物都属于 Pawn）。
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// EObjectTypeQuery 是 UE5 用于 “碰撞检测筛选类型” 的枚举，TEnumAsByte 是 UE 对枚举的封装，兼容蓝图和 C++
	// ECollisionChannel::ECC_Pawn：碰撞通道（可以理解为 “物体的碰撞标签”），Pawn 通道专门给角色用；
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	// 最终效果：检测时只看 “属于 Pawn 碰撞通道” 的物体，忽略场景道具、地形等。

	// ClassFilter: 只检测 AEnemyBase (怪物类)，忽略其他东西
	// 核心作用：进一步缩小检测范围，即使是 Pawn 类型，也只找 “怪物（AEnemyBase）”，排除玩家、友方角色等其他 Pawn。
	TSubclassOf<AActor> ClassFilter = AEnemyBase::StaticClass();

	// ActorsToIgnore: 忽略玩家自己，防止自己打自己
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerActor);

	// OutActors: 用来存放检测到的所有怪物的数组
	TArray<AActor*> OutActors;

	// 3. 执行球形重叠检测！(和蓝图 SphereOverlapActors 节点逻辑完全一致)
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		CenterLocation,
		AttackRadius,
		ObjectTypes,
		ClassFilter,
		ActorsToIgnore,
		OutActors
	);

	// 4. (非常重要) 画一个红色的调试球体，持续 0.2 秒，让你在游戏里能用肉眼看到攻击范围！
	UKismetSystemLibrary::DrawDebugSphere(this, CenterLocation, AttackRadius, 12, FLinearColor::Red, 0.2f, 2.0f);

	// 5. 遍历所有扫到的怪物，并让它们扣血
	for (AActor* OverlappedActor : OutActors)
	{
		// 尝试获取怪物身上的生命组件
		if (UHealthComponent* EnemyHealth = OverlappedActor->FindComponentByClass<UHealthComponent>())
		{
			EnemyHealth->TakeDamage(AttackDamage);
		}
	}
}

