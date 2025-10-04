// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/MPCameraComponent.h"

#include "Base/MPCameraActor.h"

UMPCameraComponent::UMPCameraComponent(const FObjectInitializer& ObjectInitializer)
{
	CameraModeStack = nullptr;
}

AActor* UMPCameraComponent::GetAttachedTarget() const
{
    return Cast<AMPCameraActor>(GetOwner())->GetAttachedActor();
}

void UMPCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	CameraModeStack->EvaluateStack(DeltaTime, DesiredView);
}

void UMPCameraComponent::SetCameraMode(TSubclassOf<UMPCameraModeBase> CameraModeClass)
{
	CameraModeStack->PushCameraMode(CameraModeClass);
}

AActor* UMPCameraComponent::GetAttachedActor() const
{
	return Cast<AMPCameraActor>(GetOwner())->GetAttachedActor();
}

void UMPCameraComponent::OnRegister()
{
	Super::OnRegister();

	if (!CameraModeStack)
	{
		CameraModeStack = NewObject<UMPCameraModeStack>(this);
		check(CameraModeStack);
	}
	
}
