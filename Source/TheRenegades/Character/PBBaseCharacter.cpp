// Fill out your copyright notice in the Description page of Project Settings.


#include "PBBaseCharacter.h"


APBBaseCharacter::APBBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomPBPlayerMovement>(APBPlayerCharacter::CharacterMovementComponentName))
{

}

UCustomPBPlayerMovement* APBBaseCharacter::GetCustomCharacterMovement() const
{
	return GetCharacterMovement<UCustomPBPlayerMovement>();
}

FCollisionQueryParams APBBaseCharacter::GetIgnoreCharacterParams() const
{

	FCollisionQueryParams Params;

	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);

	return Params;
}
