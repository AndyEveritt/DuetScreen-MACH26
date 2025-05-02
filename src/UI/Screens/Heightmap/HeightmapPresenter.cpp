#include "HeightmapPresenter.h"
#include "Debug.h"
#include "HeightmapView.h"
#include "lv_i18n/lv_i18n.h"
#include <cmath>

namespace UI
{
	void HeightmapPresenter::setRenderMode(HeightmapRenderMode mode)
	{
		m_mode = mode;
		render();
	}

	void HeightmapPresenter::setHeightmap(const std::shared_ptr<OM::Heightmap>& heightmap)
	{
		m_heightmap = heightmap;
	}

	void HeightmapPresenter::render()
	{
		if (m_heightmap == nullptr || !m_heightmap->IsValid())
		{
			LOG_WARN("Heightmap is not valid");
			return;
		}

		auto axis0 = m_heightmap->meta.GetAxis(0);
		auto axis1 = m_heightmap->meta.GetAxis(1);

		if (axis0 == nullptr || axis1 == nullptr)
		{
			LOG_WARN("Heightmap axes are not valid");
			return;
		}

		m_view->setXRange({static_cast<int32_t>(axis0->minPosition), static_cast<int32_t>(axis0->maxPosition)});
		m_view->setYRange({static_cast<int32_t>(axis1->minPosition), static_cast<int32_t>(axis1->maxPosition)});

		switch (m_mode)
		{
		case HeightmapRenderMode::Fixed:
			m_view->setValueRange(-0.25f, 0.25f);
			break;
		case HeightmapRenderMode::Auto:
			m_view->setValueRange(static_cast<float>(m_heightmap->GetMinError()),
								  static_cast<float>(m_heightmap->GetMaxError()));
			break;
		}

		uint32_t width, height;
		m_view->getResolution(width, height);

		for (uint32_t px = 0; px < width; px++)
		{
			for (uint32_t py = 0; py < height; py++)
			{
				float x, y;
				if (m_view->pxToPos(px, py, x, y))
				{
					double value = m_heightmap->GetInterpolatedPoint(x, y);
					if (std::isnan(value))
					{
						continue;
					}
					m_view->setPx(px, height - py - 1, value);
				}
			}
		}
		m_view->renderColorBar();
	}

	void HeightmapPresenter::onActivate()
	{
		auto map = OM::GetHeightmapData("heightmap.csv");
		setHeightmap(map);
		render();
	}
} // namespace UI
