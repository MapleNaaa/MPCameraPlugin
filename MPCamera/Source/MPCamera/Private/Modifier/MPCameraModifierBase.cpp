// Fill out your copyright notice in the Description page of Project Settings.


#include "Modifier/MPCameraModifierBase.h"

void UMPCameraModifierBase::AddedToCamera(APlayerCameraManager* Camera)
{
	Super::AddedToCamera(Camera);
	OnAddedToCamera_Lua(Camera);
}

void UMPCameraModifierBase::RemovedFromCamera(APlayerCameraManager* Camera)
{
	OnRemovedFromCamera_Lua(Camera);
}

void UMPCameraModifierBase::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV,
	FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCamera(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);

	ModifyCamera_Lua(DeltaTime, ViewLocation, ViewRotation, FOV);
	
}

