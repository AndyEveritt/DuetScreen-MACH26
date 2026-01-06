/*
 * SpeedFactor.h
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#pragma once

#include "SpeedFactorPresenter.h"
#include "UI/Components/Input/NumberPad.h"
#include "UI/Components/Input/Slider.h"
#include "UI/Core/View.h"

namespace UI
{
	class SpeedFactor : public View<SpeedFactorPresenter>
	{
	  public:
		SpeedFactor(const std::string& name, LvObj& parent);

		void setSpeedValue(uint32_t value);
		void setNumberPad(NumberPad* numberPad) { m_numberPad = numberPad; }

	  private:
		Slider m_speed{"slider", getRoot()};
		NumberPad* m_numberPad = nullptr;
	};
} // namespace UI
