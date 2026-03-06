// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "HealthComponent.h"
#include "Components/SphereComponent.h"
#include "RougeCharacter.h"
#include "Kismet/GameplayStatics.h" // 包含获取玩家的工具函数
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h" // 旋转函数, 修改怪物的面朝方向
#include "ExperienceGem.h"


// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 实例化生命值组件
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	// 实例化球体碰撞箱并设置
	AttackCollision = CreateDefaultSubobject<USphereComponent>(TEXT("AttackCollision"));
	AttackCollision->SetupAttachment(RootComponent); // 挂载到跟组件
	AttackCollision->SetSphereRadius(50.0f); // 设置判定半径为 50
	// 怪物生成时自动分配一个基础的AI控制器
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// 初始化碰撞伤害
	CollisionDamage = 10.0f;
	// 设置默认CD为 0.5 秒
	DamageCooldown = 0.5f; 
	// 调整怪物的默认移动速度 (比玩家慢一点)
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	// 显式关闭bOrientRotationToMovement，确保手动控制朝向不被自动逻辑覆盖。
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	// 绑定血量归零事件！当 HealthComp 广播死亡时，执行 Die() 函数
	if (HealthComp)
	{
		HealthComp->OnHealthZero.AddDynamic(this, &AEnemyBase::Die);
	}
	// 绑定重叠开始事件
	AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnAttackOverlap);
	// 绑定重叠结束事件
	AttackCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::OnAttackOverlapEnd);
	// 在游戏开始时，找到场景中的第一个玩家，并把它存起来
	PlayerTarget = Cast<ARougeCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

// Called every frame
// 追踪玩家
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 如果找到玩家且玩家未死亡, 则向玩家移动
	if (PlayerTarget != nullptr) {
		// 计算从怪物指向玩家的方向向量
		FVector DirectionToPlayer = PlayerTarget->GetActorLocation() - this->GetActorLocation();
		// 消除Z轴（高度）的影响，防止怪物想飞上天或者钻地
		DirectionToPlayer.Z = 0.0f;
		// 只有当方向向量长度大于0时才处理（避免除以0）
		if (!DirectionToPlayer.IsNearlyZero())
		{
			// 归一化方向向量 (变成长度为1的纯方向)
			DirectionToPlayer.Normalize();
			// 让怪物朝着这个方向移动
			AddMovementInput(DirectionToPlayer, 1.0f);
			// 计算朝向玩家的旋转值
			FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(DirectionToPlayer);
			// 平滑旋转（可选，让转向更自然，不加的话会瞬间转向）
			TargetRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f);
			// 应用旋转到怪物身上
			SetActorRotation(TargetRotation);
		}
	}
}

// 怪物攻击范围与玩家重叠时持续扣血，离开范围则停止扣血
// 重叠开始逻辑
void AEnemyBase::OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor == PlayerTarget)
	{
		// 1. 碰到瞬间，立刻造成一次伤害
		DealDamage();
		// 2. 开启循环定时器：每隔 DamageCooldown 秒，调用一次 DealDamage 函数
		// GetWorldTimerManager: 是 AActor 的便捷封装，等价于: GetWorld()->GetTimerManager()
		// FTimerManager 是引擎全局的计时器管理类，负责创建、运行、停止所有计时器，而 FTimerHandle 是每个计时器的唯一标识（类似 “计时器 ID”），所有操作都需要通过这个句柄关联。
		// SetTimer（创建 / 启动计时器）
		GetWorldTimerManager().SetTimer(DamageTimerHandle, this, &AEnemyBase::DealDamage, DamageCooldown, true);
		/*
		void FTimerManager::SetTimer(
			FTimerHandle& InOutHandle,       // 计时器句柄（输出参数，绑定当前计时器）
			UserClass* InObj,                // 回调函数所属的对象（比如当前怪物this）
			typename FTimerDelegate::TUObjectMethodDelegate<UserClass>::FMethodPtr InTimerMethod, // 要执行的函数
			float InRate,                    // 执行间隔（延迟/循环周期）
			bool InbLoop,                    // 是否循环执行
			float InFirstDelay = -1.0f       // 第一次执行的延迟（默认=-1，等价于InRate）
		);
		*/
	}
}

// 结束重叠逻辑：
void AEnemyBase::OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 如果离开的是玩家
	if (OtherActor != nullptr && OtherActor == PlayerTarget)
	{
		// ClearTimer（停止 / 清除计时器）
		// 清除定时器，停止持续扣血
		GetWorldTimerManager().ClearTimer(DamageTimerHandle);
	}
}

// 执行扣血的逻辑：
void AEnemyBase::DealDamage()
{
	if (PlayerTarget != nullptr)
	{
		UHealthComponent* PlayerHealth = PlayerTarget->FindComponentByClass<UHealthComponent>();
		if (PlayerHealth)
		{
			PlayerHealth->TakeDamage(CollisionDamage);
		}
	}
}

// 实现死亡函数
void AEnemyBase::Die()
{
	// 如果我们在蓝图里配置了掉落物
	if (GemClassToDrop)
	{
		// 在怪物死亡的位置，生成一个经验球
		GetWorld()->SpawnActor<AExperienceGem>(GemClassToDrop, GetActorLocation(), FRotator::ZeroRotator);
	}

	// 销毁怪物
	Destroy();
}

