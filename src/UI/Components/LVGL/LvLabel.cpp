/*
 * LvLabel.cpp
 *
 *  Created on: 2025-06-10
 *      Author: Andy Everitt
 */

#include "LvLabel.h"
#include "Debug.h"

namespace UI
{
	void LvLabel::setText(std::string_view text)
	{
		UI_LOCK();
		if (text == getText())
		{
			return;
		}
		if (text.data() == nullptr)
		{
			lv_label_set_text(getRootPtr(), nullptr);
			return;
		}
		lv_label_set_text_fmt(getRootPtr(), "%.*s", (int)text.length(), text.data());
	}
} // namespace UI
