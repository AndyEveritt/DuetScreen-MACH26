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
	PresenterType(BaseView* view)                                                                                      \
		: Presenter<ViewType>(view)                                                                                    \
	{                                                                                                                  \
		init();                                                                                                        \
	}

	class BaseView;

	class BasePresenter : public ModelListener
	{
	  public:
		virtual void init() {}
		void activate()
		{
			onActivate();
			m_active = true;
		}
		void deactivate()
		{
			onDeactivate();
			m_active = false;
		}

		virtual const std::string& getName() const
		{
			static std::string name = "BasePresenter";
			return name;
		}

		bool isActive() const { return m_active; }

	  protected:
		virtual void onActivate() {}
		virtual void onDeactivate() {}

		volatile bool m_active = false;
	};

	template <class V>
	class Presenter : public BasePresenter
	{
	  public:
		Presenter(BaseView* view)
			: m_view(static_cast<V*>(view))
		{
		}

		const std::string& getName() const final
		{
			if (m_view == nullptr)
			{
				static std::string empty = "";
				return empty;
			}

			return m_view->getName();
		}

		void setView(BaseView* view)
		{
			if (view == nullptr)
			{
				m_view = nullptr;
				return;
			}
			m_view = static_cast<V*>(view);
		}

		V* getView() const { return m_view; }

	  protected:
		V* m_view;
	};
} // namespace UI
