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
			m_view->clear();
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

	void HeightmapPresenter::setActiveHeightmap(const size_t index)
	{
		if (index >= m_heightmapFiles.size())
		{
			LOG_ERROR("Invalid heightmap index {:d}", index);
			return;
		}

		const std::string& name = m_heightmapFiles[index]->GetName();
		LOG_INFO("Loading heightmap {:s}", name);
		m_heightmap = OM::GetHeightmapData(name);
		m_heightmap->LoadFromDuet();
		render();
	}

	void HeightmapPresenter::toggleHeightmap(const size_t index)
	{
		if (index >= m_heightmapFiles.size())
		{
			LOG_ERROR("Invalid heightmap index {:d}", index);
			return;
		}

		const std::string& name = m_heightmapFiles[index]->GetName();
		LOG_INFO("Toggling heightmap {:s}", name);
		OM::ToggleHeightmap(name.c_str());
	}

	void HeightmapPresenter::newCompensationFile()
	{
		LOG_INFO("New compensation file");
		updateHeightmapList();
		OM::RequestHeightmapFiles([this]() { updateHeightmapList(); });
	}

	void HeightmapPresenter::newDirectories()
	{
		LOG_INFO("New directories");
		OM::RequestHeightmapFiles([this]() { updateHeightmapList(); });
	}

	void HeightmapPresenter::updateHeightmapList()
	{
		UI_LOCK();
		m_heightmapFiles = OM::GetHeightmapFiles();
		OM::FileSystem::SortFilesBy(m_heightmapFiles, OM::FileSystem::SortBy::NAME, false);
		m_view->setHeightmapCount(m_heightmapFiles.size());

		bool selected = false;
		for (size_t i = 0; i < m_heightmapFiles.size(); i++)
		{
			m_view->setHeightmapName(i, m_heightmapFiles[i]->GetName());
			if (m_heightmapFiles[i]->GetName() == OM::GetCurrentHeightmap())
			{
				selected = true;
				m_view->setSelectedHeightmap(i);
			}
		}
		if (!selected)
		{
			m_view->setSelectedHeightmap(-1);
		}
	}

	void HeightmapPresenter::onActivate()
	{
		OM::RequestHeightmapFiles([this]() { updateHeightmapList(); });
		std::shared_ptr<OM::Heightmap> map = OM::GetHeightmapData("heightmap.csv");
		if (!map->IsValid())
		{
			map->LoadFromDuet();
		}

		setHeightmap(map);
		render();
	}
} // namespace UI
