// Fill out your copyright notice in the Description page of Project Settings.


#include "TagSelectorDetails.h"
#include <DetailLayoutBuilder.h>
#include "PropBuilderComponentBase.h"
#include <Widgets/Text/STextBlock.h>
#include <DetailCategoryBuilder.h>
#include <DetailWidgetRow.h>
#include "RandomPlacementBuilder.h"
#include <IDetailChildrenBuilder.h>
#include <Widgets/Layout/SWrapBox.h>
#include <Widgets/Layout/SScrollBox.h>
#include <Widgets/Input/SButton.h>

#define LOCTEXT_NAMESPACE "TagSelector"

void FTagSelectorDetails::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{	
	TSharedPtr<IPropertyHandle> TypePropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTagSelector, tag));
	
	check(TypePropertyHandle.IsValid());

	TypePropertyHandle->GetValueAsDisplayText(ChosenTypeText);
	
	OnTypeChanged(TypePropertyHandle);

	TypePropertyHandle->SetOnPropertyValueChanged(
		FSimpleDelegate::CreateSP(this, &FTagSelectorDetails::OnTypeChanged, TypePropertyHandle));

	HeaderRow.NameContent()[PropertyHandle->CreatePropertyNameWidget()]
		.ValueContent()[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth().VAlign(EVerticalAlignment::VAlign_Center).HAlign(EHorizontalAlignment::HAlign_Fill)
				[
					SNew(STextBlock)
					.Text(MakeAttributeLambda([=] { return FText::Format(LOCTEXT("ValueType", "{0}"), ChosenTypeText); }))
				.Justification(ETextJustify::Right).ColorAndOpacity(MakeAttributeLambda([=] { return FSlateColor(FColor::Orange); }))
				]
		];
	
}

void FTagSelectorDetails::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> TypePropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTagSelector, tag));
	tagHandle = TypePropertyHandle;

	ChildBuilder.AddCustomRow(LOCTEXT("TagSelectorRow", "TagSelector"))
		[
			SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					SNew(SButton)
						.Text(FText::FromString("Add From Tag List..."))
						.OnClicked(this, &FTagSelectorDetails::ButtonPressed)
					
				]
				+ SScrollBox::Slot()
				[
					SAssignNew(ListViewWidget, SListView<TSharedPtr<FString>>)
					.ItemHeight(24)
					.ListItemsSource(&Items)
					.OnGenerateRow(this, &FTagSelectorDetails::OnGenerateRowForList)
					.OnMouseButtonDoubleClick(this, &FTagSelectorDetails::OnClick)
					.ClearSelectionOnClick(true)
				]
		];
}



FReply FTagSelectorDetails::ButtonPressed()
{
	Items.Reset();
	UDataTable* DT;
	FSoftObjectPath UnitDataTablePath = FSoftObjectPath(TAGS_SETTINGS);
	DT = Cast<UDataTable>(UnitDataTablePath.ResolveObject());
	if (DT)
	{
		PopulateItems(DT);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Property Data Table not found !"));
	}

	ListViewWidget->RequestListRefresh();

	return FReply::Handled();
}


TSharedRef<ITableRow> FTagSelectorDetails::OnGenerateRowForList(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
		SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		.Padding(2.0f)
		[
			SNew(STextBlock).Text(FText::FromString(*Item.Get()))
		];
	
}


void FTagSelectorDetails::OnClick(TSharedPtr<FString> argument)
{
	UE_LOG(LogTemp, Warning, L"%s", *(*argument.Get()));
	tagHandle->SetValue(*(*argument.Get()));
	Items.Reset();
	ListViewWidget->RequestListRefresh();

}

void FTagSelectorDetails::PopulateItems(UDataTable* table)
{
	Items.Add(MakeShareable(new FString("None")));

	FTableTags* tagsAsTable = table->FindRow< FTableTags>("Settings", "", true);
	
	if (tagsAsTable)
	{
		for (int i = 0; i < tagsAsTable->tags.Num(); i++)
		{
			Items.Add(MakeShareable(new FString(tagsAsTable->tags[i].tag.ToString())));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, L"Tag Table Row not found");
	}
}

void FTagSelectorDetails::OnTypeChanged(TSharedPtr<IPropertyHandle> TypePropertyHandle)
{
	if (TypePropertyHandle.IsValid() && TypePropertyHandle->IsValidHandle())
	{
		TypePropertyHandle->GetValueAsDisplayText(ChosenTypeText);
	}
}

#undef LOCTEXT_NAMESPACE