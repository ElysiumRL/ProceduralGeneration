// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * Some utility functions
 */
class PROCEDURALGENERATIONTOOL_API ElysiumUtilities
{
public:

	// Finds a data table from the content browser and will warn you if path is invalid
	UE_NODISCARD static UDataTable* FindDataTableChecked(const FString& path);

	// NOT WORKING YET : Saves a texture to the content browser
	static void SaveTexture2D(UTexture2D* texture, FString filename);

	UE_NODISCARD static UTexture2D* CreateFromSceneCapture2D(class USceneCaptureComponent2D* sceneCapture, UObject* Outer);

};
