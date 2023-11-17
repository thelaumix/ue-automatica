// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ControlCommandSettings.generated.h"

UENUM(BlueprintType, Category=Logic)
enum ELogicControlType: uint8
{
	Unknown			UMETA(Hidden, DisplayName = "---"),
	Wrap_Loop,
	Wrap_Async,

	Move_Forward,
	Move_Backward,
	Move_Left,
	Move_Right,
	Turn_Left,
	Turn_Right,
	Turn_Back,
	Pick_Up,
	Pick_Drop,
	Toggle_On,
	Toggle_Off
};

USTRUCT()
struct AUTOMATICA_API FCommandSettingDrawer
{
	GENERATED_BODY()

	/** Material instance to use for the icon*/
	UPROPERTY(EditAnywhere, Category=General)
	UMaterialInstance* Material;

	/** The display name used for this command instance */
	UPROPERTY(EditAnywhere, Category=General)
	FText DisplayName;

	/** The detailed description for help */
	UPROPERTY(EditAnywhere, Category=General)
	FText Description;

	/** Width amount for the material sprite to take off */
	UPROPERTY(EditAnywhere, Category=Display, meta=(ClampMin = 0, UIMin = 0, ClampMax = 100, UIMax = 100))
	float WidthPercentage = 100;

	/** Whether the command can contain inner elements */
	UPROPERTY(EditAnywhere, Category=Display)
	bool bUseInnerExtent = false;
	
	/** The relative extent borders inner objects will be respecting left and right */
	UPROPERTY(EditAnywhere, Category=Display, meta=(EditConditionHides = true, EditCondition = "bUseInnerExtent"), DisplayName="Left Extent Percentage")
	float ExtentBorderLeftPercentage = 20;
	
	/** The relative extent borders inner objects will be respecting left and right */
	UPROPERTY(EditAnywhere, Category=Display, meta=(EditConditionHides = true, EditCondition = "bUseInnerExtent"), DisplayName="Right Extent Percentage")
	float ExtentBorderRightPercentage = 20;
	
};

/**
 * 
 */
UCLASS()
class AUTOMATICA_API UControlCommandSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Retrieves the setting drawer for a specified control type (command) if existing.
	 * Should exist, but you never know!
	 * @param Type The type to look up
	 * @param Drawer Will be populated with a pointer to the found drawer
	 * @return Whether a drawer could be retrieved
	 */
	bool GetCommandSettingDrawer(ELogicControlType Type, FCommandSettingDrawer& Drawer);
	
protected:
	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<ELogicControlType>, FCommandSettingDrawer> ControlMap;
};
