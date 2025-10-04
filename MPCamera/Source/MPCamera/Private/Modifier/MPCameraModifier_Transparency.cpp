// Fill out your copyright notice in the Description page of Project Settings.


#include "Modifier/MPCameraModifier_Transparency.h"

void UMPCameraModifier_Transparency::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation,
	float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCamera(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);

	
}
