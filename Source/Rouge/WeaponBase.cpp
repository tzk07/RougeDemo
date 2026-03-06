// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Damage = 20.0f;
	Cooldown = 1.0f;
	AttackRange = 1000.0f;

	// 倍率默认是 1.0 (也就是 100%)
	DamageMultiplier = 1.0f;
	CooldownMultiplier = 1.0f;
	RangeMultiplier = 1.0f;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	BaseDamage = Damage;
	BaseCooldown = Cooldown;
	BaseAttackRange = AttackRange;
	// 游戏开始时启动循环攻击定时器
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &AWeaponBase::Attack, Cooldown, true);
}

// Called every frame
void AWeaponBase::Attack()
{
	// 基类不实现攻击逻辑, 留空
}
void AWeaponBase::UpgradeStats(float DamageBonus, float CooldownReduction, float RangeBonus)
{
	// 累加倍率（比如传入 0.1 代表增加 10%）
	DamageMultiplier += DamageBonus;
	RangeMultiplier += RangeBonus;

	// 冷却缩减通常是减法（比如传入 0.1 代表冷却时间缩短 10%）
	CooldownMultiplier -= CooldownReduction;

	// 设置冷却上限：防止减免超过100%导致冷却变负数，最多允许缩减到原本的 10%
	CooldownMultiplier = FMath::Max(0.1f, CooldownMultiplier);

	// 核心公式：永远使用 初始基数 * 最新倍率 来得出当前面板属性！
	Damage = BaseDamage * DamageMultiplier;
	AttackRange = BaseAttackRange * RangeMultiplier;
	Cooldown = BaseCooldown * CooldownMultiplier;

	// 重启定时器应用新的冷却频率
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &AWeaponBase::Attack, Cooldown, true);

	// 可选：在日志中打印数值变化，方便查错
	UE_LOG(LogTemp, Warning, TEXT("Weapon Upgraded! Dmg: %.1f, Range: %.1f, CD: %.2fs"), Damage, AttackRange, Cooldown);
}