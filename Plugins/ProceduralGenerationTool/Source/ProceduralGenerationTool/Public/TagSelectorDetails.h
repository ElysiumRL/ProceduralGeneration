// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <IDetailCustomization.h>
#include <IPropertyTypeCustomization.h>


class FTagSelectorDetails : public IPropertyTypeCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FTagSelectorDetails());
	}

	FText ChosenTypeText;

	void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	FReply ButtonPressed();

	/* Adds a new textbox with the string to the list */
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable);

	/* The list of strings */
	TArray<TSharedPtr<FString>> Items;

	/* The actual UI list */
	TSharedPtr<SListView<TSharedPtr<FString>>> ListViewWidget;

	void OnClick(TSharedPtr<FString> argument);
	
	void PopulateItems(UDataTable* table);
	
	TSharedPtr<IPropertyHandle> tagHandle;

	void OnTypeChanged(TSharedPtr<IPropertyHandle> TypePropertyHandle);

};