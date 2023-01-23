// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/ElysiumUtilities.h"

#include <AssetRegistry/AssetRegistryModule.h>
#include <Components/SceneCaptureComponent2D.h>
#include <Engine/TextureRenderTarget2D.h>

UDataTable* ElysiumUtilities::FindDataTableChecked(const FString& path)
{
	UDataTable* DT = nullptr;
	FSoftObjectPath tablePath = FSoftObjectPath(path);
	DT = Cast<UDataTable>(tablePath.ResolveObject());
	if (DT)
	{
		return DT;
	}
	DT = Cast<UDataTable>(tablePath.TryLoad());

	if (DT)
	{
		return DT;
	}
	check(DT);
	UE_LOG(LogTemp, Warning, TEXT("Property Data Table not found !"));
	return DT;
}

void ElysiumUtilities::SaveTexture2D(UTexture2D* texture, FString filename)
{
	FString PackageName = TEXT("/Game/");
	PackageName += filename;
	UPackage* Package = CreatePackage(*PackageName);
	Package->FullyLoad();

	texture->UpdateResource();
	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(texture);

	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	bool bSaved = UPackage::SavePackage(Package, texture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);
	//Package->MarkPackageDirty();
	//bool bSaved = UPackage::SavePackage(Package, texture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);

	if (!bSaved)
	{
		UE_LOG(LogTemp, Warning, TEXT("Texture2D Not saved"));
	}
}

UTexture2D* ElysiumUtilities::CreateFromSceneCapture2D(USceneCaptureComponent2D* sceneCapture, UObject* Outer)
{
	UTextureRenderTarget2D* target = sceneCapture->TextureTarget;
	UTexture2D* newTexture = target->ConstructTexture2D(Outer, "RenderTexture_" + sceneCapture->GetName(), EObjectFlags::RF_Public, CTF_DeferCompression);
	newTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
	newTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	newTexture->SRGB = 0;
	newTexture->UpdateResource();
	return newTexture;
}
