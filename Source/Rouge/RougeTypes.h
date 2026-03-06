// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"  // 补充UTexture2D的头文件
#include "WeaponBase.h"
#include "RougeTypes.generated.h"

/**
 * 
 */
 // 强化的类型枚举
UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
	StatBonus UMETA(DisplayName = "StatBonus"),
	NewWeapon UMETA(DisplayName = "New Weapon")
};

// 继承自 FTableRowBase 才能在数据表中使用
USTRUCT(BlueprintType)
struct FRougeUpgradeOption : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RougeUpgrade")
	FText UpgradeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RougeUpgrade")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RougeUpgrade")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RougeUpgrade")
	EUpgradeType Type;

	// === 数值强化部分 ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UpgradeStats")
	float DamageBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UpgradeStats")
	float CooldownReduction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UpgradeStats")
	float RangeBonus = 0.0f;

	// === 新武器部分 ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UpgradeWeapon")
	TSubclassOf<AWeaponBase> WeaponClassToSpawn;
};