// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Weapon_Beam.generated.h"

/**
 * 
 */
UCLASS()
class ROUGE_API AWeapon_Beam : public AWeaponBase
{
	GENERATED_BODY()

public:
	AWeapon_Beam();

protected:
	virtual void Attack() override;

	// ¹âÊøµÄ¿í¶È£¨´ÖÏ¸°ë¾¶£©
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	float BeamWidth;
};