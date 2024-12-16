#pragma once

#include "model.h"

namespace UI
{
	class ModelListener
	{
	  public:
		ModelListener()
			: m_model(Model::getInstance())
		{
		}

		virtual ~ModelListener() {}

		Model& getModel() { return m_model; }

	  protected:
		Model& m_model;
	};
} // namespace UI
