// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MPCameraControllerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UMPCameraControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MPCAMERA_API IMPCameraControllerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual bool HasLastRotationInput() = 0;
	virtual void SetCurrentCameraType(uint8 CameraType) = 0;
	virtual uint8 GetCurrentBaseCameraType() = 0;
};
