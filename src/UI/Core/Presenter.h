#pragma once

#include "ModelListener.h"
#include "Subscribers/Subscribers.h"
#include <map>
#include <vector>

namespace UI
{
	class BaseView;

	class BasePresenter : public ModelListener
	{
	  public:
		void activate()
		{
			m_model.bind(this);
			onActivate();
		}
		void deactivate()
		{
			m_model.unbind(this);
			onDeactivate();
		}

	  protected:
		virtual void onActivate() {}
		virtual void onDeactivate() {}
	};

	template <class V>
	class Presenter : public BasePresenter
	{
	  public:
		Presenter()
			: m_view(nullptr)
		{
		}

		Presenter(BaseView* view) { m_view = static_cast<V*>(view); }

		Presenter(V* view)
			: m_view(view)
		{
		}

	  protected:
		V* m_view;
	};
} // namespace UI
