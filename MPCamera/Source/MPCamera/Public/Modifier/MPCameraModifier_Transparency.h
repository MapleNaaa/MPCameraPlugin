// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MPCameraModifierBase.h"
#include "Camera/CameraModifier.h"
#include "MPCameraModifier_Transparency.generated.h"

/**
 * 
 */
UCLASS()
class MPCAMERA_API UMPCameraModifier_Transparency : public UMPCameraModifierBase
{
	GENERATED_BODY()

public:
    virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite,meta=(ToolTip = "Rate of change for transparency alpha"), Category = "MPCamera")
	float AlphaChangeRate = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,meta=(ToolTip = "Minimum transparency alpha value"), Category = "MPCamera")
    float MinTransparencyAlpha = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,meta=(ToolTip = "Detection sphere radius"), Category = "MPCamera")
    float DetectSphereRadius = 50.f;

	TMap<TWeakObjectPtr<AActor>, float> PrevCameraHitMap;
	TMap<TWeakObjectPtr<AActor>, float> CameraHitMap;
};
