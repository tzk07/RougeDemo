// Fill out your copyright notice in the Description page of Project Settings.
// 弹道武器派生类
#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Weapon_Projectile.generated.h"

class AMagicOrb;

UCLASS()
class ROUGE_API AWeapon_Projectile : public AWeaponBase
{
	GENERATED_BODY()

public:
	AWeapon_Projectile();

protected:
	// 重写基类的攻击逻辑
	virtual void Attack() override;

	// 在蓝图中配置发射什么类型的法球
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Config")
	TSubclassOf<AMagicOrb> OrbClassToSpawn;
};
