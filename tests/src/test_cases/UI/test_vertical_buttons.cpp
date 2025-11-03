/*
 * test_vertical_buttons.cpp
 *
 *  Created on: 2025-11-02
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Components/Button/VerticalButtonPanel.h"
#include "UI/Widgets/BabyStep/BabyStep.h"
#include "test_utils/UiTestSuite.h"
#include <gtest/gtest.h>

using namespace UI;

class TestVerticalButtons : public UiTestSuite
{
  public:
	TestVerticalButtons() {}
};

TEST_F(TestVerticalButtons, BasicDisplay)
{
	VerticalButtonPanel panel{"panel", screen};
	panel.setSize(135, 240);
	panel.setIncrementIcon("babystep_increment.png");
	panel.setDecrementIcon("babystep_decrement.png");
	// panel.setIncrementLabel("Inc");
	// panel.setDecrementLabel("Dec");
	panel.setResetLabel("Reset");
	panel.setValueLabelFmt("{:g} mm");
	panel.setIncrementValues({0.01f, 0.05f});

	panel.setSelectedValueIndex(-1);

	VerticalButtonPanel panel2{"panel2", screen};
	panel2.setPos(150, 0);
	panel2.setSize(135, LV_PCT(100));
	panel2.setIncrementIcon("babystep_increment.png");
	panel2.setDecrementIcon("babystep_decrement.png");
	// panel2.setIncrementLabel("Inc");
	// panel2.setDecrementLabel("Dec");
	panel2.setResetLabel("Reset");
	panel2.setValueLabelFmt("{:g} mm");
	panel2.setIncrementValues({0.01f, 0.05f});

	EXPECT_EQUAL_SCREENSHOT("vertical_buttons/basic_display.png");
}

#if 0
/**
* These tests exist to try to diagnose a suspected lvgl layout bug observed with the babystepping increment value buttons.
*/

TEST_F(TestVerticalButtons, BabyStep)
{
	BabyStep babystep{"babystep", screen};
	babystep.setSize(135, 250);

	EXPECT_EQUAL_SCREENSHOT("vertical_buttons/babystep.png");
}

TEST_F(TestVerticalButtons, EncapsulatedPanel)
{
	LvContainer cont("cont", screen);

	LvLabel header("header", cont);

	VerticalButtonPanel panel("panel", cont);

	cont.setHeight(LV_PCT(100));
	cont.setSize(135, 260);
	cont.setFlexFlow(LV_FLEX_FLOW_COLUMN);

	header.setText("Encapsulated Panel");

	panel.setWidth(LV_PCT(100));
	panel.setFlexGrow(1);
	panel.setIncrementIcon("babystep_increment.png");
	panel.setDecrementIcon("babystep_decrement.png");
	panel.setResetLabel("Reset");
	panel.setValueLabelFmt("{:g} mm");
	panel.setIncrementValues({0.01f, 0.05f});

	EXPECT_EQUAL_SCREENSHOT("vertical_buttons/encapsulated_panel.png");
}
#endif