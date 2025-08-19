/*
 * test_theme.cpp
 *
 *  Created on: 2025-08-19
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Components/Canvas/Canvas.h"
#include "UI/Components/Graph/Graph.h"
#include "UI/Components/LVGL/LvArc.h"
#include "UI/Components/LVGL/LvArcLabel.h"
#include "UI/Components/LVGL/LvBar.h"
#include "UI/Components/LVGL/LvButtonMatrix.h"
#include "UI/Components/LVGL/LvCheckbox.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvDropdown.h"
#include "UI/Components/LVGL/LvImage.h"
#include "UI/Components/LVGL/LvKeyboard.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/LVGL/LvList.h"
#include "UI/Components/LVGL/LvSlider.h"
#include "UI/Components/LVGL/LvTextArea.h"
#include "UI/Components/Theme/ThemePreview.h"
#include "UI/Styles/Styles.h"
#include "UI/Styles/Themes/DefaultTheme.h"
#include "test_utils/UiTestSuite.h"
#include "utils/StorageHelper.h"
#include <gtest/gtest.h>

using namespace UI;

class TestTheme : public UiTestSuite
{
  public:
	TestTheme() {}

	~TestTheme()
	{
		const Themes::Theme* default_theme = Themes::getTheme(StorageHelper::getData<int>(ID_THEME, 0));
		if (default_theme == nullptr)
		{
			return;
		}
		default_theme->setThemeActive();
	}
};

TEST_F(TestTheme, DefaultTheme)
{
	constexpr uint16_t primaryHue = 210;
	constexpr uint16_t secondaryHue = 50;
	constexpr float chroma = 0.02f;
	const lv_font_t* font = LV_FONT_DEFAULT;
	constexpr bool darkMode = true;

	Themes::ThemeColors colors = Themes::createThemeColors(primaryHue, secondaryHue, chroma, darkMode);
	Themes::DefaultTheme theme("test_theme", colors, font, darkMode, [](Themes::Theme* theme) {});
	theme.init();
	theme.setThemeActive();

	ThemePreview themePreview("theme_preview", lv_screen_active());
	themePreview.setSize(LV_PCT(100), LV_SIZE_CONTENT);

	EXPECT_TRUE(Themes::getCurrentTheme() == &theme);
	themePreview.setPrimaryHue(primaryHue);
	themePreview.setSecondaryHue(secondaryHue);
	themePreview.setChroma(chroma);
	themePreview.setDarkMode(darkMode);
	EXPECT_EQUAL_SCREENSHOT("theme_1.png");

	themePreview.setPrimaryHue(25);
	themePreview.setSecondaryHue(150);
	themePreview.setChroma(0.5f);
	themePreview.setDarkMode(false);
	EXPECT_EQUAL_SCREENSHOT("theme_2.png");

	themePreview.setPrimaryHue(300);
	themePreview.setSecondaryHue(50);
	themePreview.setChroma(0.5f);
	themePreview.setDarkMode(true);
	EXPECT_EQUAL_SCREENSHOT("theme_3.png");
}

static std::unique_ptr<LvLabel> createLabel(const std::string& text, lv_obj_t* parent)
{
	auto label = std::make_unique<LvLabel>("label", parent);
	label->setText(text);
	label->setAlign(LV_ALIGN_CENTER, 0, 0);
	label->setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	label->setMaxWidth(LV_PCT(100));
	return label;
}

TEST_F(TestTheme, Widgets)
{
	lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);

	ThemePreview preview("theme_preview", lv_screen_active());
	preview.setSize(LV_PCT(100), LV_SIZE_CONTENT);
	preview.showControls(false);

	LvContainer cont("container", lv_screen_active());
	cont.setWidth(LV_PCT(100));
	cont.setFlexGrow(1);
	cont.setFlexFlow(LV_FLEX_FLOW_COLUMN_WRAP);
	cont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

	constexpr size_t col_width = LV_PCT(20);

	/* LvBar */
	LvBar bar("bar", cont);
	bar.setWidth(col_width);
	bar.setValue(40);

	/* LvButtonMatrix */
	LvButtonMatrix button_matrix("button_matrix", cont);
	static const char* btnm_map[] = {"Default", "Checked"};
	static const lv_buttonmatrix_ctrl_t btnm_ctrl_map[] = {
		LV_BUTTONMATRIX_CTRL_WIDTH_1,
		LV_BUTTONMATRIX_CTRL_CHECKED,
	};
	button_matrix.setMap(btnm_map);
	button_matrix.setCtrlMap(btnm_ctrl_map);
	button_matrix.setSize(col_width, 100);

	/* LvCheckbox */
	LvCheckbox checkbox("checkbox", cont);
	LvCheckbox checkbox2("checkbox2", cont);
	checkbox2.setChecked(true);

	/* LvImage */
	LvImage image("image", cont);

	LvLabel label("label", cont);
	label.setText("Label");

	/* LvSlider */
	LvSlider slider("slider", cont);
	slider.setWidth(col_width);
	slider.setValue(50);

	/* LvDropdown */
	LvDropdown dropdown("dropdown", cont);
	dropdown.open();

	/* LvTextArea */
	LvTextArea text_area("text_area", cont);
	text_area.setFlag(LV_OBJ_FLAG_FLEX_IN_NEW_TRACK, true);
	text_area.setSize(col_width, LV_SIZE_CONTENT);
	text_area.setText("This is a text area. You can type here.");

	/* LvKeyboard */
	LvKeyboard keyboard("keyboard", cont);
	keyboard.setWidth(250);

	/* LvArc */
	LvContainer arc_cont("arc_cont", cont);
	arc_cont.setSize(col_width, 100);
	// arc_cont.setFlexGrow(1);
	arc_cont.setFlexFlow(LV_FLEX_FLOW_ROW);

	LvArc arc("arc", arc_cont);
	arc.setSize(LV_PCT(40), LV_PCT(100));

	LvArcLabel arc_label("arc_label", arc_cont);
	arc_label.setRadius(50);
	arc_label.setSize(LV_PCT(40), LV_PCT(100));
	arc_label.setText("Arc label");

	/* LvList */
	LvList list("list", cont);
	list.setSize(col_width, LV_SIZE_CONTENT);
	list.addText("List text");
	list.addButton(nullptr, "List button");
	list.addButton(nullptr, "List button 2");

	/* Button */
	LvContainer btn_cont("btn_cont", cont);
	btn_cont.setSize(col_width, LV_SIZE_CONTENT);
	btn_cont.setFlexFlow(LV_FLEX_FLOW_ROW_WRAP);

	Button button("button", btn_cont, "Button");

	Button button_checked("button_checked", btn_cont, "Checked");
	button_checked.setChecked(true);

	Button long_press("long_press", btn_cont, "Long Press");
	long_press.addStyle(Themes::getLvglStyles().long_press);

	Button action("action", btn_cont, "Action");
	action.addStyle(Themes::getLvglStyles().actionBtn);

	Button draggable("draggable", btn_cont, "Draggable");
	draggable.addStyle(Themes::getLvglStyles().draggable);

	/* Card */
	Card card("card", cont);
	card.setSize(col_width, LV_SIZE_CONTENT);
	auto card_label = createLabel("Card", card);

	/* Canvas */
	Canvas canvas("canvas", cont);
	canvas.setSize(col_width, LV_SIZE_CONTENT);
	canvas.setTitle("Canvas");
	canvas.setResolution(100, 100);

	/* Graph */
	Graph graph("graph", cont);
	graph.setWidth(250);
	graph.showLegend(true);
	srand(0);
	for (size_t i = 0; i < 3; ++i)
	{
		graph.createSeries(lv_palette_main((lv_palette_t)graph.getSeriesCount()), fmt::format("Series {}", i));
		for (size_t j = 0; j < 100; ++j)
		{
			graph.addData(i, rand() % 100);
		}
	}
	graph.getSeries(0)->legendObj->setText("Hidden");
	graph.showSeries(0, false);

	/* lv_table */
	lv_obj_t* table = lv_table_create(cont);
	lv_obj_set_size(table, col_width, LV_SIZE_CONTENT);
	const size_t table_cols = 3;
	const size_t table_rows = 4;
	lv_table_set_column_count(table, table_cols);
	lv_table_set_row_count(table, table_rows);
	lv_table_set_selected_cell(table, 1, 1);
	for (size_t i = 0; i < table_cols * table_rows; i++)
	{
		lv_table_set_cell_value(table, i % table_cols, i / table_cols, fmt::format("Cell {}", i).c_str());
	}

	for (size_t i = 0; i < Themes::getThemeCount(); ++i)
	{
		const Themes::Theme* theme = Themes::getTheme(i);
		if (theme == nullptr)
		{
			continue;
		}
		LOG_INFO("Testing theme: {}", theme->getName());
		theme->setThemeActive();
		preview.updateSwatches();
		canvas.clear();
		canvas.drawLine({0, 0}, {99, 99}, lv_palette_main(LV_PALETTE_RED), LV_OPA_COVER);
		canvas.drawLabelPx({75, 20}, "Label", lv_palette_main(LV_PALETTE_BLUE), LV_OPA_COVER);

		EXPECT_EQUAL_SCREENSHOT(fmt::format("theme_widgets_{}.png", theme->getName()).c_str());
	}
}
