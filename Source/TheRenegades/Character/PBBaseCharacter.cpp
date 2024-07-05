// Fill out your copyright notice in the Description page of Project Settings.


#include "PBBaseCharacter.h"
#include "../Character Movement/CustomPBPlayerMovement.h"

APBBaseCharacter::APBBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomPBPlayerMovement>(ACharacter::CharacterMovementComponentName))
{

}

UCustomPBPlayerMovement* APBBaseCharacter::GetCustomCharacterMovement() const
{
	return GetCharacterMovement<UCustomPBPlayerMovement>();
}
