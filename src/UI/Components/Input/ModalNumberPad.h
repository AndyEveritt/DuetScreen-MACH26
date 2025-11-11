/*
 * ModalNumberPad.h
 *
 *  Created on: 2025-06-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Input/NumberPad.h"
#include "UI/Components/Modal/Modal.h"

namespace UI
{
	class ModalNumberPad : public Modal<NumberPad>
	{
	  public:
		ModalNumberPad(const std::string& name, LvObj& parent)
			: Modal<NumberPad>(name, parent, layout_t(0, 0, 50, 70))
		{
		}
	};
} // namespace UI
