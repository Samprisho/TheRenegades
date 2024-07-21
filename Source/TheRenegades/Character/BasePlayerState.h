// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "Attributes/BaseAttributes.h"
#include "BasePlayerState.generated.h"

class UBaseAttributes;
class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class THERENEGADES_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ABasePlayerState();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBaseAttributes* BaseAttributes;

public:

	virtual void BeginPlay() override;
	
};
