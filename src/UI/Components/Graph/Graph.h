/*
 * Graph.h
 *
 *  Created on: 2025-01-20
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/View.h"

namespace UI
{
	class Graph : public BaseView
	{
	  public:
		Graph(const std::string& name, lv_obj_t* parent, layout_t layout);

	  private:
		lv_obj_t* m_chart;
		lv_obj_t* m_vCont;
		lv_obj_t* m_hCont;
		lv_obj_t* m_vScale;
		lv_obj_t* m_hScale;
	};
} // namespace UI
