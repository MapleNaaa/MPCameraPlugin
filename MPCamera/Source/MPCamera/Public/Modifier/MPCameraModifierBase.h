// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "MPCameraModifierBase.generated.h"

/**
 * 
 */
UCLASS()
class MPCAMERA_API UMPCameraModifierBase : public UCameraModifier
{
	GENERATED_BODY()

public:
	virtual void AddedToCamera(APlayerCameraManager* Camera) override;
    virtual void RemovedFromCamera(APlayerCameraManager* Camera);
	virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;


protected:

	UFUNCTION(BlueprintImplementableEvent)
    void OnAddedToCamera_Lua(APlayerCameraManager* Camera);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnRemovedFromCamera_Lua(APlayerCameraManager* Camera);

	UFUNCTION(BlueprintImplementableEvent)
    void ModifyCamera_Lua(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV);
};
