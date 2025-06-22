// Copyright (C) 2024-2025 by Haguk Kim


#include "Inventory/InvObject.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Templates/Tuple.h"

#include "Core/TRMacros.h"
#include "Core/ProjectTRGameModeBase.h"
#include "Items/ItemData.h"
#include "Items/BaseItem.h"
#include "Characters/GameCharacter.h"
#include "DungeonActors/IconStageActor.h"

UInvObject::UInvObject()
{
    static ConstructorHelpers::FObjectFinder<UMaterial> MatAsset(TEXT(ASSET_DEFAULT_ICON_MATERIAL));
    if (MatAsset.Succeeded())
    {
        BaseIconMaterial = MatAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UInvObject - Failed to load base material asset!"));
    }
}

void UInvObject::OnRep_Tier()
{
    // 필요 시 추가
    // 티어 설정으로 인한 액터 VFX 등의 처리는 InvObject가 아닌 아이템 액터 단에서 처리할 것
}

void UInvObject::Server_ChooseTierDuringRuntime(EItemTier NewTier)
{
    if (Tier != EItemTier::IT_TIER_UNSPECIFIED)
    {
        UE_LOG(LogTemp, Error, TEXT("UInvObject::Server_ChooseTierDuringRuntime - The tier has already been initialized before. Tier should not change after its chosen. Please check."))
    }
    Tier = NewTier;
}

void UInvObject::SetItemData(UItemData* Data)
{
    ItemData = Data;
}

void UInvObject::SetBaseItemClass(TSubclassOf<ABaseItem> Class)
{
    BaseItemClass = Class;
}

ABaseItem* UInvObject::GenerateAndSpawnItem(UObject* Outer, FVector Location, FRotator Rotation, FActorSpawnParameters Params, bool bRestoreUsingItemData, bool bUseTierVFX)
{
    UClass* ItemClass = BaseItemClass;
    ABaseItem* GeneratedItem = nullptr;
    UWorld* World = Outer->GetWorld();

    if (!IsValid(ItemClass))
    {
        UE_LOG(LogTemp, Warning, TEXT("GenerateAndSpawnItem - InvObject %s has no ItemDataClass set. Something went wrong during passing GetClass() from ABaseItem."), *GetName());
        return nullptr;
    }
    if (!IsValid(World))
    {
        UE_LOG(LogTemp, Warning, TEXT("GenerateAndSpawnItem - InvObject %s has no world."), *GetName());
        return nullptr;
    }

    AProjectTRGameModeBase* GameMode = Cast<AProjectTRGameModeBase>(World->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("GenerateAndSpawnItem - InvObject %s is unable to get appropriate GameMode."), *GetName());
        return nullptr;
    }

    if (bRestoreUsingItemData)
    {
        GeneratedItem = GameMode->RespawnItem(ItemClass, World, Location, Rotation, Params, this, bUseTierVFX);
    }
    else
    {
        GeneratedItem = GameMode->SpawnItem(ItemClass, World, Location, Rotation, Params, bUseTierVFX);
    }
    GeneratedItem->SetInvObject(this);
    return GeneratedItem;
}

void UInvObject::ChangeOuterRecursive(UObject* NewOuter, bool bAddToRootSet)
{
    if (bAddToRootSet && !IsRooted())
    {
        this->AddToRoot();
    }
    else if (!bAddToRootSet && IsRooted())
    {
        this->RemoveFromRoot();
    }

    if (NewOuter) Rename(nullptr, NewOuter);
    if (ItemData)
    {
        ItemData->ChangeOuterRecursive(NewOuter, bAddToRootSet);
    }
}

AGameCharacter* UInvObject::GetInvObjectOwner() const
{
    // 예외 발생을 줄이기 위해 GetOwner보다 GetOuter가 권장된다
    return Cast<AGameCharacter>(GetOuter());
}

FInvObjSize UInvObject::GetDimensions() const
{
    return { InvXSize, InvYSize };
}

UMaterialInstance* UInvObject::GetCurrIcon() const
{
    return GetIcon();
}

UMaterialInstance* UInvObject::GetIcon() const
{
    if (IconMat) return IconMat.Get();
    return nullptr;
}

void UInvObject::Local_InitIconStageActor()
{
    // 제대로 처리되기 위해 필요한 dependency들을 확인
    // 클라의 경우 아래 값들이 모두 레플리케이션 되어야 정상적으로 처리됨
    if (CurrIconStageActor.IsValid() && CurrIconStageActor->ReferencingInvObj && CurrIconStageActor->DisplayedActor)
    {
        // 카메라, 아이템 위치 및 회전 조정
        CurrIconStageActor->Local_SetupDisplayActor();
        
        // 렌더 텍스처 생성
        UTextureRenderTarget2D* TextureTarget = NewObject<UTextureRenderTarget2D>();
        TextureTarget->InitAutoFormat(GetDimensions().X * INV_GRID_PIXEL, GetDimensions().Y * INV_GRID_PIXEL);

        // 바인딩 후 캡처
        CurrIconStageActor->SetTextureTargetAs(TextureTarget);
        CurrIconStageActor->CaptureTarget();

        // 렌더타깃을 표기하기 위해서는 Dynamic material을 사용해야 한다
        // 이미 기존에 생성한 전력이 있는 경우 새로 만드는 대신 파라미터 값만 수정한다
        if (!bLocal_IsIconMatDynamic)
        {
            IconMat = UMaterialInstanceDynamic::Create(BaseIconMaterial, this);
            bLocal_IsIconMatDynamic = true;
        }
        UMaterialInstanceDynamic* DynamicMat = Cast<UMaterialInstanceDynamic>(IconMat);
        if (!DynamicMat)
        {
            UE_LOG(LogTemp, Error, TEXT("Local_InitIconStageActor - Something went wrong!"));
            return;
        }
        DynamicMat->SetTextureParameterValue(FName("Texture"), TextureTarget);
    }
    else
    {
        if (GetOwningActor() && GetOwningActor()->HasAuthority())
        {
            UE_LOG(LogTemp, Error, TEXT("Local_InitIconStageActor - Dependency check failed! This should never happen in the server!"));
        }
    }
}

void UInvObject::OnRep_IconStageActor()
{
    Local_InitIconStageActor();
}
