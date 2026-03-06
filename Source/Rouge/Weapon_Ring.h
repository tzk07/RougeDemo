// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Weapon_Ring.generated.h"

UCLASS()
class ROUGE_API AWeapon_Ring : public AWeaponBase
{
	GENERATED_BODY()

public:
	AWeapon_Ring();

protected:
	virtual void Attack() override;
};