/*
 * ModalMessageBox.h
 *
 *  Created on: 2026-01-08
 *      Author: Andy Everitt
 */

#pragma once

#include "MessageBox.h"
#include "UI/Components/Modal/Modal.h"

namespace UI
{
	class ModalMessageBox : public Modal<MessageBox>
	{
	  public:
		template <typename... Args>
			requires(std::is_constructible_v<MessageBox, const std::string&, LvObj&, Args...>)
		ModalMessageBox(const std::string& name, LvObj& parent, Args&&... args)
			: Modal<MessageBox>(name, parent, std::forward<Args>(args)...)
		{
		}

		virtual ~ModalMessageBox() = default;

	  private:
		void onHide() override;
	};
} // namespace UI