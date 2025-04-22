#pragma once

#include "ModelListener.h"
#include "Subscribers/Subscribers.h"
#include <map>
#include <memory>
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
		void activate() { onActivate(); }
		void deactivate() { onDeactivate(); }

	  protected:
		virtual void onActivate() {}
		virtual void onDeactivate() {}
	};

	template <class V>
	class Presenter : public BasePresenter
	{
	  public:
		Presenter(BaseView* view)
			: m_view(static_cast<V*>(view))
		{
		}

		V* m_view;

	  protected:
	};
} // namespace UI
