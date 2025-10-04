// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/MPPlayerCameraManager.h"
#include "Base/MPCameraActor.h"

AMPPlayerCameraManager::AMPPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
{
}

void AMPPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
	if (bUseMPCameraLogic)
    {
        SpawnMPCameraActor();
    }
}

void AMPPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	if (bUseMPCameraLogic)
	{
		UpdateMPViewTarget(OutVT, DeltaTime);
	}
	else
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
	}
}

void AMPPlayerCameraManager::UpdateMPViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// Note: In MPCamera, we don't use ViewTarget, ViewTarget is always same.
	// And the ViewTarget is always the same as PendingViewTarget.
	// ViewTarget is the SubjectClass of the ACameraActor.
	
	// Don't update outgoing viewtarget during an interpolation 
	if ((PendingViewTarget.Target != NULL) && BlendParams.bLockOutgoing && OutVT.Equal(ViewTarget))
	{
		return;
	}

	// Store previous POV, in case we need it later
	FMinimalViewInfo OrigPOV = OutVT.POV;

	// Reset the view target POV fully
	static const FMinimalViewInfo DefaultViewInfo;
	OutVT.POV = DefaultViewInfo;
	OutVT.POV.FOV = DefaultFOV;
	OutVT.POV.OrthoWidth = DefaultOrthoWidth;
	OutVT.POV.AspectRatio = DefaultAspectRatio;
	OutVT.POV.bConstrainAspectRatio = bDefaultConstrainAspectRatio;
	OutVT.POV.ProjectionMode = bIsOrthographic ? ECameraProjectionMode::Orthographic : ECameraProjectionMode::Perspective;
	OutVT.POV.PostProcessBlendWeight = 1.0f;
	OutVT.POV.bAutoCalculateOrthoPlanes = bAutoCalculateOrthoPlanes;
	OutVT.POV.AutoPlaneShift = AutoPlaneShift;
	OutVT.POV.bUpdateOrthoPlanes = bUpdateOrthoPlanes;
	OutVT.POV.bUseCameraHeightAsViewTarget = bUseCameraHeightAsViewTarget;

	bool bDoNotApplyModifiers = false;

	if (ACameraActor* CamActor = Cast<ACameraActor>(OutVT.Target))
	{
		// Viewing through a camera actor.
		// CamActor->GetCameraComponent()->GetCameraView(DeltaTime, OutVT.POV);
		CamActor->CalcCamera(DeltaTime, OutVT.POV);
	}
	else
	{
		UE_LOG(LogMPCamera, Error, TEXT("OutVT.Target is not ACameraActor, Please check it. If you want to use MPCamera, please use AMPCameraActor."));
	}

	if (!bDoNotApplyModifiers || bAlwaysApplyModifiers)
	{
		// Apply camera modifiers at the end (view shakes for example)
		ApplyCameraModifiers(DeltaTime, OutVT.POV);
	}

	// Synchronize the actor with the view target results
	SetActorLocationAndRotation(OutVT.POV.Location, OutVT.POV.Rotation, false);
	if (bAutoCalculateOrthoPlanes && OutVT.Target)
	{
		OutVT.POV.SetCameraToViewTarget(OutVT.Target->GetActorLocation());
	}

	UpdateCameraLensEffects(OutVT);
}

void AMPPlayerCameraManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AMPPlayerCameraManager::ApplyCameraModifiers(float DeltaTime, FMinimalViewInfo& OutViewInfo)
{
	Super::ApplyCameraModifiers(DeltaTime, OutViewInfo);
}

void AMPPlayerCameraManager::UpdateCameraLensEffects(const FTViewTarget& OutVT)
{
	Super::UpdateCameraLensEffects(OutVT);
}

void AMPPlayerCameraManager::SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	Super::SetViewTarget(NewViewTarget, TransitionParams);
}

void AMPPlayerCameraManager::SpawnMPCameraActor()
{
    // Spawn MPCameraActor
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    
    MPCameraActor = GetWorld()->SpawnActor<AMPCameraActor>(AMPCameraActor::StaticClass(), SpawnParams);
	if (!IsValid(MPCameraActor))
	{
		UE_LOG(LogMPCamera, Error, TEXT("Spawn MPCameraActor failed."));
		return;
	}

	SetViewTarget(MPCameraActor);
	SetMPCameraMode(DefaultCameraModeClass);
}

void AMPPlayerCameraManager::SetMPCameraMode(TSubclassOf<UMPCameraModeBase> InCameraModeClass)
{
	if (!InCameraModeClass) return;
	if (!MPCameraActor) return;

	MPCameraActor->SetCameraMode(InCameraModeClass);
}
