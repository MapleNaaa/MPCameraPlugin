// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mode/MPCameraModeBase.h"
#include "UObject/Interface.h"
#include "MPCameraModeProviderInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UMPCameraModeProviderInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MPCAMERA_API IMPCameraModeProviderInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UMPCameraModeBase> GetCameraMode(uint8& CameraType) const;
    virtual TSubclassOf<UMPCameraModeBase> GetCameraModeImpl(uint8& CameraType) const = 0;
};
