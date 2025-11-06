/*
 * ThemePreview.cpp
 *
 *  Created on: 2025-08-06
 *      Author: Andy Everitt
 */

#include "ThemePreview.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"
#include "UI/Styles/Themes/CustomTheme.h"
#include "i18n/i18n.h"

namespace UI
{
	class Swatch : public LvContainer
	{
	  public:
		Swatch(const std::string& name, LvObj& parent)
			: LvContainer(name, parent)
			, m_colorBox("color_box", getRoot())
			, m_label("label", getRoot())
		{
			m_colorBox.addStyle(Themes::getLvglStyles().border);

			setFlexFlow(LV_FLEX_FLOW_COLUMN);
			setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

			m_colorBox.setWidth(LV_PCT(100));
			m_colorBox.setFlexGrow(1);
			m_colorBox.setMinHeight(20);
			m_colorBox.setStyleBgOpa(LV_OPA_COVER);
			m_label.setSize(LV_PCT(100), LV_SIZE_CONTENT);
			m_label.setMinWidth(LV_SIZE_CONTENT);
			m_label.setStyleTextAlign(LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
		}

		void setSwatchColor(lv_color_t color) { m_colorBox.setStyleBgColor(color, LV_PART_MAIN); }

		void setLabel(std::string_view text) { m_label.setText(text); }

	  private:
		LvContainer m_colorBox;
		LvLabel m_label;
		lv_style_t* m_style = nullptr;
	};

	struct LabelledStyle
	{
		std::string_view name;
		const Themes::Style& style;
		lv_style_prop_t color_prop;
	};

	ThemePreview::ThemePreview(const std::string& name, LvObj& parent)
		: LvContainer(name, parent)
		, m_swatches("swatches", getRoot())
		, m_primaryHueSlider("primary_hue_slider", getRoot())
		, m_secondaryHueSlider("secondary_hue_slider", getRoot())
		, m_chromaSlider("chroma_slider", getRoot())
		, m_darkMode("dark_mode", getRoot())
	{
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

		m_swatches.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_swatches.setListFlow(LV_FLEX_FLOW_ROW_WRAP);
		m_swatches.getListContainer().setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

		m_primaryHueSlider.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_primaryHueSlider.setRange(0, 360);
		m_primaryHueSlider.setValue(0);
		m_primaryHueSlider.setLabel(_("theme.primary_hue"));
		m_primaryHueSlider.setValueChangedCallback([this](int32_t) { updateThemeColors(); });

		m_secondaryHueSlider.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_secondaryHueSlider.setRange(0, 360);
		m_secondaryHueSlider.setValue(0);
		m_secondaryHueSlider.setLabel(_("theme.secondary_hue"));
		m_secondaryHueSlider.setValueChangedCallback([this](int32_t) { updateThemeColors(); });

		m_chromaSlider.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_chromaSlider.setRange(0, 0.4);
		m_chromaSlider.setIncrementValue(0.01f);
		m_chromaSlider.setValue(0.2);
		m_chromaSlider.setLabel(_("theme.chroma"));
		m_chromaSlider.setValueChangedCallback([this](int32_t) { updateThemeColors(); });

		m_primaryHueSlider.setFocusedCallback(
			[this](bool focused)
			{
				if (m_keyboard == nullptr)
					return;
				m_keyboard->setTextArea(&m_primaryHueSlider.getInput());
				m_keyboard->setVisible(focused);
				m_keyboard->setMode(LV_KEYBOARD_MODE_NUMBER);
			});

		m_secondaryHueSlider.setFocusedCallback(
			[this](bool focused)
			{
				if (m_keyboard == nullptr)
					return;
				m_keyboard->setTextArea(&m_secondaryHueSlider.getInput());
				m_keyboard->setVisible(focused);
				m_keyboard->setMode(LV_KEYBOARD_MODE_NUMBER);
			});

		m_chromaSlider.setFocusedCallback(
			[this](bool focused)
			{
				if (m_keyboard == nullptr)
					return;
				m_keyboard->setTextArea(&m_chromaSlider.getInput());
				m_keyboard->setVisible(focused);
				m_keyboard->setMode(LV_KEYBOARD_MODE_NUMBER);
			});

		m_darkMode.setText(_("theme.dark_mode"));
		m_darkMode.setChecked(true);
		m_darkMode.setCheckedCallback([this](bool checked) { updateThemeColors(); });

		updateSwatches();
	}

	ThemePreview::~ThemePreview() = default;

	void ThemePreview::updateSwatches()
	{
		const auto& styles = Themes::getLvglStyles();
		const LabelledStyle swatch_styles[] = {
			{.name = "bg_dark", .style = styles.bg_dark, .color_prop = LV_STYLE_BG_COLOR},
			{.name = "bg", .style = styles.bg, .color_prop = LV_STYLE_BG_COLOR},
			{.name = "bg_light", .style = styles.bg_light, .color_prop = LV_STYLE_BG_COLOR},
			{.name = "bg_color_primary", .style = styles.bg_color_primary, .color_prop = LV_STYLE_BG_COLOR},
			{.name = "bg_color_primary_muted", .style = styles.bg_color_primary_muted, .color_prop = LV_STYLE_BG_COLOR},
			{.name = "bg_color_secondary", .style = styles.bg_color_secondary, .color_prop = LV_STYLE_BG_COLOR},
			{.name = "bg_color_secondary_muted",
			 .style = styles.bg_color_secondary_muted,
			 .color_prop = LV_STYLE_BG_COLOR},
#if 0
			{.name="bg_color_header", .style=styles.bg_color_header, .color_prop=LV_STYLE_BG_COLOR},
			{.name="bg_color_list_item", .style=styles.bg_color_list_item, .color_prop=LV_STYLE_BG_COLOR},
#endif
			{.name = "text", .style = styles.text, .color_prop = LV_STYLE_TEXT_COLOR},
			{.name = "text_muted", .style = styles.text_muted, .color_prop = LV_STYLE_TEXT_COLOR},
			{.name = "text_header", .style = styles.text_header, .color_prop = LV_STYLE_TEXT_COLOR},
			{.name = "border", .style = styles.border, .color_prop = LV_STYLE_BORDER_COLOR},
			{.name = "border_muted", .style = styles.border_muted, .color_prop = LV_STYLE_BORDER_COLOR},
			{.name = "border_highlight", .style = styles.border_highlight, .color_prop = LV_STYLE_BORDER_COLOR},
			{.name = "success", .style = styles.bg_color_success, .color_prop = LV_STYLE_BG_COLOR},
			{.name = "warning", .style = styles.bg_color_warning, .color_prop = LV_STYLE_BG_COLOR},
			{.name = "error", .style = styles.bg_color_error, .color_prop = LV_STYLE_BG_COLOR},
#if 0
			{.name="style_border_color_primary", .style=styles.border_color_primary,
			.color_prop=LV_STYLE_BORDER_COLOR},
			{.name="style_border_color_secondary", .style=styles.border_color_secondary,
			.color_prop=LV_STYLE_BORDER_COLOR},
#endif
		};

		m_swatches.clear();
		m_swatches.setItemCount(std::size(swatch_styles),
								[this, &swatch_styles](size_t index, LvObj& parent)
								{
									auto swatch = std::make_unique<Swatch>(fmt::format("{:d}", index), parent);
									swatch->setSize(180, LV_SIZE_CONTENT);
									swatch->setMaxWidth(LV_PCT(100));
									auto& style = swatch_styles[index];
									swatch->setLabel(_(fmt::format("settings.style.{:s}", style.name)));

									lv_style_value_t value;
									if (lv_style_get_prop(style.style, style.color_prop, &value) == LV_STYLE_RES_FOUND)
									{
										swatch->setSwatchColor(value.color);
									}
									else
									{
										LOG_ERROR("Failed to get color for style: {}", style.name);
									}

									// swatch->addSwatchStyle(style.style);

									return swatch;
								});
	}

	void ThemePreview::updateSliders(size_t primaryHue, size_t secondaryHue, float chroma, bool darkMode)
	{
		m_primaryHueSlider.setSendMode(Slider::SendMode::DISABLED);
		m_secondaryHueSlider.setSendMode(Slider::SendMode::DISABLED);
		m_chromaSlider.setSendMode(Slider::SendMode::DISABLED);

		m_primaryHueSlider.setValue(static_cast<float>(primaryHue));
		m_secondaryHueSlider.setValue(static_cast<float>(secondaryHue));
		m_chromaSlider.setValue(chroma);
		m_darkMode.setChecked(darkMode); // this will call the updateThemeColors() callback

		m_primaryHueSlider.setSendMode(Slider::SendMode::VALUE_CONFIRMED);
		m_secondaryHueSlider.setSendMode(Slider::SendMode::VALUE_CONFIRMED);
		m_chromaSlider.setSendMode(Slider::SendMode::VALUE_CONFIRMED);
	}

	void ThemePreview::setPrimaryHue(size_t hue)
	{
		m_primaryHueSlider.setValue(static_cast<float>(hue));
	}

	void ThemePreview::setSecondaryHue(size_t hue)
	{
		m_secondaryHueSlider.setValue(static_cast<float>(hue));
	}

	void ThemePreview::setChroma(float chroma)
	{
		m_chromaSlider.setValue(chroma);
	}

	void ThemePreview::setDarkMode(bool enable)
	{
		m_darkMode.setChecked(enable);
	}

	void ThemePreview::showControls(bool show)
	{
		m_primaryHueSlider.setVisible(show);
		m_secondaryHueSlider.setVisible(show);
		m_chromaSlider.setVisible(show);
		m_darkMode.setVisible(show);
	}

	void ThemePreview::setKeyboard(LvKeyboard* keyboard)
	{
		m_keyboard = keyboard;

		m_primaryHueSlider.setKeyboard(keyboard);
		m_secondaryHueSlider.setKeyboard(keyboard);
		m_chromaSlider.setKeyboard(keyboard);
	}

	void ThemePreview::updateThemeColors()
	{
		UI::Themes::Theme* theme = Themes::getCurrentTheme();

		if (!theme)
		{
			LOG_ERROR("No current theme set");
			return;
		}

		auto customTheme = dynamic_cast<UI::Themes::CustomTheme*>(theme);
		if (!customTheme)
		{
			LOG_ERROR("Current theme does not inherit from CustomTheme");
			return;
		}

		customTheme->setColors(m_primaryHueSlider.getValue(),
							   m_secondaryHueSlider.getValue(),
							   m_chromaSlider.getValue(),
							   m_darkMode.getChecked());

		UI::Themes::refreshCurrentTheme();
		updateSwatches();
	}

} // namespace UI
