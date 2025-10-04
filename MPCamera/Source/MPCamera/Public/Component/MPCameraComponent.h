// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/MPCameraModeStack.h"
#include "Camera/CameraComponent.h"
#include "UObject/Object.h"
#include "MPCameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class MPCAMERA_API UMPCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UMPCameraComponent(const FObjectInitializer& ObjectInitializer);
	virtual AActor* GetAttachedTarget() const;

    virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
    virtual void SetCameraMode(TSubclassOf<UMPCameraModeBase> CameraModeClass);
	virtual AActor* GetAttachedActor() const;

protected:
	virtual void OnRegister() override;
	

protected:


	UPROPERTY()
    TObjectPtr<UMPCameraModeStack> CameraModeStack;


	
	
};
