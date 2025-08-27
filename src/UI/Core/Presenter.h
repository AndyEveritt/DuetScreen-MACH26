#pragma once

#include "ModelListener.h"
#include "Subscribers/Subscribers.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace UI
{
#define PRESENTER_CONSTRUCTOR(PresenterType, ViewType)                                                                 \
	PresenterType(LvObj* view)                                                                                         \
		: Presenter<ViewType>(view)                                                                                    \
	{                                                                                                                  \
	}

	class LvObj;

	class BasePresenter : public ModelListener
	{
	  public:
		void init();
		virtual void activate();
		virtual void deactivate();

		virtual std::string_view getName() const;

		bool isActive() const { return m_active; }

		void connected();
		void disconnected();

	  protected:
		virtual void onInit() {}
		virtual void onActivate() {}
		virtual void onDeactivate() {}

		virtual void onConnect() {}
		virtual void onDisconnect() {}

		volatile bool m_active = false;
	};

	template <class V>
	class Presenter : public BasePresenter
	{
	  public:
		Presenter(LvObj* view)
			: m_view(static_cast<V*>(view))
		{
		}

		void init() { BasePresenter::init(); }

		virtual void activate() final
		{
			if (m_view != nullptr)
			{
				BasePresenter::activate();
				Model::get().bind(m_view->getPresenter());
			}
		}

		virtual void deactivate() final
		{
			if (m_view != nullptr)
			{
				BasePresenter::deactivate();
				Model::get().unbind(m_view->getPresenter());
			}
		}

		std::string_view getName() const final
		{
			if (m_view == nullptr)
			{
				static std::string empty = "";
				return empty;
			}

			return m_view->getName();
		}

		void setView(V* view) { m_view = view; }

		V* getView() const { return m_view; }

	  protected:
		V* m_view;
	};
} // namespace UI
