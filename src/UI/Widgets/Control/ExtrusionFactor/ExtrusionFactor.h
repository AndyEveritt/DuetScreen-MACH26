/*
 * ExtrusionFactor.h
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#pragma once

#include "ExtrusionFactorPresenter.h"
#include "UI/Components/Input/NumberPad.h"
#include "UI/Components/Input/Slider.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/List/List.h"
#include "UI/Components/Modal/Modal.h"
#include "UI/Core/View.h"

namespace UI
{
	class ExtrusionFactor : public View<ExtrusionFactorPresenter>
	{
	  public:
		using focus_cb_t = std::function<void(bool focused, size_t index, Slider& slider)>;

		ExtrusionFactor(const std::string& name, LvObj& parent);

		void setExtruderCount(size_t count);
		size_t getExtruderCount() const { return m_extruders.getItemCount(); }
		void setExtruderLabel(size_t index, std::string_view label);
		void setExtruderValue(size_t index, uint32_t value);

		void setInputFocusCb(focus_cb_t cb) { m_inputFocusCb = std::move(cb); }
		void setNumberPad(NumberPad* numberPad) { m_numberPad = numberPad; }
		auto& getList() { return m_extruders; }

	  protected:
		void configureNumberPad(Slider* slider);

	  private:
		List<Slider> m_extruders{"e_factor_list", getRoot()};
		NumberPad* m_numberPad = nullptr;

		focus_cb_t m_inputFocusCb = nullptr;
	};

	class ModalExtrusionFactor : public Modal<View<ModalExtrusionFactorPresenter>>
	{
	  public:
		ModalExtrusionFactor(const std::string& name, LvObj& parent);

		auto& getExtrusionFactor() { return m_extrusionFactor; }
		auto& getSliders() { return m_extrusionFactor.getList(); }
		NumberPad& getNumberPad() { return m_numberPad; }

	  private:
		ExtrusionFactor m_extrusionFactor{"extrusion_factor", getRoot()};
		NumberPad m_numberPad{"number_pad", getRoot(), layout_t(0, 0, 50, 100)};
	};
} // namespace UI
