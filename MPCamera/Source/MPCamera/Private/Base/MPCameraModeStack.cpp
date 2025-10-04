// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/MPCameraModeStack.h"

#include "Base/MPCameraDefine.h"
#include "Mode/MPCameraModeBase.h"


UMPCameraModeStack::UMPCameraModeStack()
{
	bIsActive = true;
}

void UMPCameraModeStack::ActivateStack()
{
	if (!bIsActive)
	{
		bIsActive = true;

		// Notify camera modes that they are being activated.
        for (const auto CamMode : CameraModeStack)
		{
            if(!CamMode)
            {
	            UE_LOG(LogMPCamera, Error, TEXT("ActiveCamera.CameraMode is nullptr on ActivateStack"));
            	continue;
            }
            CamMode->OnBecomeActive();
		}
	}
}

void UMPCameraModeStack::DeactivateStack()
{
	if (bIsActive)
	{
		bIsActive = false;

		for (const auto CamMode : CameraModeStack)
		{
			if(!CamMode)
			{
				UE_LOG(LogMPCamera, Error, TEXT("ActiveCamera.CameraMode is nullptr on DeactivateStack"));
				continue;
            }
            int32 CameraModeIndex = FindOrCreateCameraModeInstance(CamMode->GetClass());
			check(CameraModeInstances.IsValidIndex(CameraModeIndex));
            CamMode->OnDeactivated();
		}
	}
}

/**
 * Pushes a new camera mode onto the stack
 * @param CameraModeClass The class of the camera mode to push
 * @return true if the camera mode was successfully pushed, false otherwise
 *
 * 将camera mode压入栈中，如果已经存在，则将已经存在的camera mode设置为out，新的camera mode设置为in
 * 同时选择是否需要blend，如果需要blend，则计算blend weight
 */
bool UMPCameraModeStack::PushCameraMode(TSubclassOf<UMPCameraModeBase> CameraModeClass)
{
	if (!CameraModeClass) return false;

    int32 CameraModeIndex = FindOrCreateCameraModeInstance(CameraModeClass);
    check(CameraModeInstances.IsValidIndex(CameraModeIndex));
	
	UMPCameraModeBase* CameraMode = CameraModeInstances[CameraModeIndex].Get();

	int32 StackSize = CameraModeStack.Num();
    if (StackSize > 0 && CameraModeStack[0] == CameraMode) return true;

	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackWeight = 1.f;

	for (int32 Index = 0; Index < StackSize; Index++)
	{
		UMPCameraModeBase* CamMode = CameraModeStack[Index].Get();
		if (CamMode == CameraMode)
		{
			CamMode->SetBlendWay(true);
			ExistingStackIndex = Index;
            ExistingStackWeight *= CamMode->GetBlendWeight();

			// 将剩余的camera mode设置为out
			for (int32 InnerIndex = Index + 1; InnerIndex < StackSize; InnerIndex++)
			{
				CameraModeStack[InnerIndex]->SetBlendWay(false);
			}
			break;
		}
		else
		{
			CamMode->SetBlendWay(false);
            ExistingStackWeight *= (1 - CamMode->GetBlendWeight());
		}
	}
	
	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
        ExistingStackWeight = 0.f;
	}

	const bool bShouldBlend = (CameraMode->GetBlendTime() > 0.f && StackSize > 0);
	const float BlendWeight = (bShouldBlend ? ExistingStackWeight : 1.f);

	CameraMode->SetBlendWeight(BlendWeight);
    CameraModeStack.Insert(CameraMode, 0);
	CameraModeStack.Last()->SetBlendWeight(1.f);

	if (ExistingStackIndex == INDEX_NONE)
	{
		CameraMode->OnBecomeActive();
	}
    return true;
}

void UMPCameraModeStack::PreEvaluateStack(float DeltaTime)
{
	// Remove any invalid camera modes.
    for (int32 Idx = CameraModeInstances.Num() - 1; Idx >= 0; --Idx)
    {
        if (!CameraModeInstances[Idx])
        {
        	UE_LOG(LogTemp, Warning, TEXT("Removing invalid camera mode at index %d"), Idx);
        	UE_LOG(LogTemp, Warning, TEXT("Object pointer: %p"), CameraModeStack[Idx].Get());
        	UE_LOG(LogTemp, Warning, TEXT("Is nullptr: %s"), CameraModeStack[Idx].Get() ? TEXT("No") : TEXT("Yes"));
        
            CameraModeInstances.RemoveAt(Idx);
        }
	}

	// Remove any invalid camera modes from the stack.
	for (int32 Idx = CameraModeStack.Num() - 1; Idx >= 0; --Idx)
	{
		if (!CameraModeStack[Idx])
		{
			CameraModeStack.RemoveAt(Idx);
		}
	}
}

bool UMPCameraModeStack::EvaluateStack(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	if (!bIsActive) return false;
	
	PreEvaluateStack(DeltaTime);
	
    UpdateStack(DeltaTime, OutResult);
    BlendStack(DeltaTime, OutResult);

	return true;
}

FMinimalViewInfo UMPCameraModeStack::POVBlend(const FMinimalViewInfo A, const FMinimalViewInfo B, float Alpha)
{
	// Alpha = 0, return A , Alpha = 1, return B
    if (Alpha <= 0.f) return A;
    if (Alpha >= 1.f) return B;
	
	FMinimalViewInfo Result;
    Result.Location = FMath::Lerp(A.Location, B.Location, Alpha);

	const FRotator DeltaRotation = (B.Rotation - A.Rotation).GetNormalized();
	Result.Rotation = A.Rotation + DeltaRotation * Alpha;
	
    Result.FOV = FMath::Lerp(A.FOV, B.FOV, Alpha);
    return Result;
}

int32 UMPCameraModeStack::FindOrCreateCameraModeInstance(TSubclassOf<UMPCameraModeBase> CameraModeClass)
{
    if (!CameraModeClass) return INDEX_NONE;

    for (int32 Index = 0; Index < CameraModeInstances.Num(); Index++)
    {
        if (CameraModeInstances[Index].Get()->StaticClass() == CameraModeClass)
        {
            return Index;
		}
	}

	// Not found, create a new instance.
    UMPCameraModeBase* CameraMode = NewObject<UMPCameraModeBase>(this, CameraModeClass);
	check(CameraMode);
    CameraModeInstances.Add(CameraMode);
    return CameraModeInstances.Num() - 1;
}

void UMPCameraModeStack::UpdateStack(float DeltaTime, struct FMinimalViewInfo InPOV)
{
	if (CameraModeStack.Num() == 0) return;

	int32 RemoveIdx = 0;
	int32 RemoveCount = 0;
	for (int32 Idx = 0; Idx < CameraModeStack.Num(); ++Idx)
	{
        UMPCameraModeBase* CamMode = CameraModeStack[Idx].Get();
	    check(CamMode);

		CamMode->UpdateCamera(DeltaTime, InPOV);
		if (CamMode->GetBlendWeight() > 1.0f)
		{
			RemoveIdx = Idx + 1;
			RemoveCount = CameraModeStack.Num() - RemoveIdx;
			break;
		}
	}

	if (RemoveCount > 0)
	{
		for (int32 Idx = RemoveIdx; Idx < CameraModeStack.Num(); ++Idx)
		{
			UMPCameraModeBase* CamMode = CameraModeStack[Idx].Get();
			check(CamMode);

			CamMode->OnDeactivated();
		}
		CameraModeStack.RemoveAt(RemoveIdx, RemoveCount);
	}
	
}

void UMPCameraModeStack::BlendStack(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	const int32 StackNum = CameraModeStack.Num();
	if (StackNum <= 0) return;

	const UMPCameraModeBase* CamMode = CameraModeStack.Last().Get();
	check(CamMode);

	OutResult = CamMode->GetPOV();

	for (int32 Idx = StackNum - 2; Idx >= 0; --Idx)
	{
		CamMode = CameraModeStack[Idx].Get();
		check(CamMode);

        OutResult = POVBlend(OutResult, CamMode->GetPOV(), CamMode->GetBlendWeight());
	}
}
