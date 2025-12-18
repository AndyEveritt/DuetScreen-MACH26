#pragma once

#include "View.h"

namespace UI
{
	void back();
	void home();

	// Internal helpers used by template APIs; not part of public modal type contract
	namespace detail
	{
		void openModalImpl(LvObj* view);
		bool closeModalImpl(LvObj* view);
		void addHomeScreenImpl(LvObj* view);
		void removeHomeScreenImpl(LvObj* view, bool close);
		void openScreenImpl(LvObj* view, bool closePrevious);
		bool closeScreenImpl(LvObj* view, bool returnable);
	} // namespace detail

	template <typename T>
	concept NotModalType = !requires { typename T::modal_base_marker; };

	template <typename T>
	concept ModalType = requires { typename T::modal_base_marker; };

	template <NotModalType T>
	void addHomeScreen(T* view)
	{
		detail::addHomeScreenImpl(static_cast<LvObj*>(view));
	}

	template <NotModalType T>
	void removeHomeScreen(T* view, bool close = true)
	{
		detail::removeHomeScreenImpl(static_cast<LvObj*>(view), close);
	}

	void clearHomeScreens();

	template <NotModalType T>
	void openScreen(T* view, bool closePrevious = true)
	{
		detail::openScreenImpl(static_cast<LvObj*>(view), closePrevious);
	}

	template <NotModalType T>
	bool closeScreen(T* view, bool returnable = true)
	{
		return detail::closeScreenImpl(static_cast<LvObj*>(view), returnable);
	}

	LvObj* getCurrentScreen();
	bool closeLastScreen();

	/* Modal-only APIs: only accept types deriving from Modal<...> */

	template <ModalType T>
	void openModal(T* view)
	{
		detail::openModalImpl(static_cast<LvObj*>(view));
	}

	template <ModalType T>
	bool closeModal(T* view)
	{
		return detail::closeModalImpl(static_cast<LvObj*>(view));
	}

	void closeAllModals();
	bool closeLastModal();
} // namespace UI
