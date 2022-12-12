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

#define LOCTEXT_NAMESPACE "TagSelector"
/*
void FTagSelectorDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& categoryBuilder = DetailBuilder.EditCategory(FName("General"), FText::FromString("General2"), ECategoryPriority::Important);
	URandomPlacementBuilder* builder = nullptr;

	TArray<TWeakObjectPtr<UObject>> customizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(customizedObjects);

	for (auto Object : customizedObjects)
	{
		if (Object.IsValid())
		{
			builder = Cast<URandomPlacementBuilder>(Object);
			if (builder)
			{
				break;
			}
		}
	}
	check(builder);

	categoryBuilder.AddCustomRow(LOCTEXT("RowSearchName", "Selection"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Selection"))
		]
	.ValueContent()
		[
			SNew(SButton)
			.Text(FText::FromString("Button Test"))
		.ToolTipText(FText::FromString("This is a weird tooltip"))
		];
		


}
*/
void FTagSelectorDetails::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	//TODO: take this from the tutorial and adapt it
	/*
	// Get the property handler of the type property:
	TSharedPtr<IPropertyHandle> TypePropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMyStruct, Type));
	check(TypePropertyHandle.IsValid());

	// retrieve its value as a text to display
	FText Type;
	TypePropertyHandle->GetValueAsDisplayText(Type);

	// then change the HeaderRow to add some Slate widget
	// clang-format off
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()]
		.ValueContent()[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Font(FEditorStyle::GetFontStyle("PropertyWindow.NormalFont"))
				.Text(FText::Format(LOCTEXT("ValueType", "The value type is \"{0}\""), Type))
				]
		];
	// clang-format on
	*/
}

void FTagSelectorDetails::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> TypePropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTagSelector, tag));

	//ChildBuilder.AddCustomRow(LOCTEXT("TagSelectorRow", "TagSelector"))
	//[
	//	TypePropertyHandle->CreatePropertyValueWidget()
	//];

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
					.ListItemsSource(&Items) //The Items array is the source of this listview
					.OnGenerateRow(this, &FTagSelectorDetails::OnGenerateRowForList)
					.OnMouseButtonDoubleClick(this, &FTagSelectorDetails::OnClick)
					.ClearSelectionOnClick(true)
				]
		];
}



FReply FTagSelectorDetails::ButtonPressed()
{
	UDataTable* DT;
	FSoftObjectPath UnitDataTablePath = FSoftObjectPath(TAGS_SETTINGS);
	DT = Cast<UDataTable>(UnitDataTablePath.ResolveObject());
	if (DT)
	{
		PopulateItems(DT);
	}
	else
	{
		UE_LOG(LogShapeRenderer, Warning, TEXT("Property Data Table not found !"));
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
	Items.Reset();
	ListViewWidget->RequestListRefresh();

}

void FTagSelectorDetails::PopulateItems(UDataTable* table)
{
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
	//Items.Add(MakeShareable(new FString("Hello 1")));
}

#undef LOCTEXT_NAMESPACE