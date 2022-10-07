// Fill out your copyright notice in the Description page of Project Settings.


#include "TagDetails.h"
#include "PropertyEditor/Public/DetailBuilderTypes.h"
#include "PropertyEditor/Public/DetailLayoutBuilder.h"
#include "PropertyEditor/Public/DetailCategoryBuilder.h"
#include "PropertyEditor/Public/DetailWidgetRow.h"

#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "FTagDetails"

TSharedRef<IDetailCustomization> FTagDetails::MakeInstance()
{
	return MakeShareable(new FTagDetails);
}

void FTagDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& category = DetailBuilder.EditCategory("Settings", LOCTEXT("CatName", "MyCustomTag"), ECategoryPriority::Important);
	category.AddCustomRow(LOCTEXT("Keyword", "MyCustomTagTest"))
		.NameContent()
		[
			SNew(STextBlock).Text(LOCTEXT("NameText", "Test1")).Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		[
			SNew(STextBlock).Text(LOCTEXT("NameText", "MyTest2")).Font(IDetailLayoutBuilder::GetDetailFont())
		];
	
}

#undef LOCTEXT_NAMESPACE