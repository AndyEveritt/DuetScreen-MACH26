#pragma once

#include "model.h"

namespace UI
{
	class ModelListener
	{
	  public:
		ModelListener() : m_model(0) {}

		virtual ~ModelListener() {}

		void bind(Model* m) { m_model = m; }
		Model* getModel() { return m_model; }

		virtual void newData(float data) {}

	  protected:
		Model* m_model;
	};
} // namespace UI
