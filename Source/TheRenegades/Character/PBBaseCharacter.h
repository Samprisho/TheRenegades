// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PBPlayerCharacter.h"
#include "PBBaseCharacter.generated.h"

class UCustomPBPlayerMovement;
/**
 * 
 */
UCLASS()
class THERENEGADES_API APBBaseCharacter : public APBPlayerCharacter
{
	GENERATED_BODY()
	
public:
	APBBaseCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	UCustomPBPlayerMovement* GetCustomCharacterMovement() const;
};
