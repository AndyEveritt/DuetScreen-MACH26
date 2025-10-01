#include "HeightmapPresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "HeightmapView.h"
#include "lv_i18n/lv_i18n.h"
#include <cmath>

#define RENDER_MEASUREMENT_POINTS 0

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
		if (m_heightmap == nullptr)
		{
			m_view->clear();
		}
	}

	void HeightmapPresenter::render()
	{
		if (!checkMode())
		{
			return;
		}

		if (m_heightmap == nullptr)
		{
			m_view->clear();
			return;
		}

		if (!m_heightmap->IsValid())
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

		LOG_DBG("Rendering heightmap {:s}", m_heightmap->GetFileName());

		m_view->setShownHeightmapName(m_heightmap->GetFileName());
		m_view->setXRange({static_cast<int32_t>(axis0->minPosition), static_cast<int32_t>(axis0->maxPosition)});
		m_view->setYRange({static_cast<int32_t>(axis1->minPosition), static_cast<int32_t>(axis1->maxPosition)});

		m_axis0Range = {axis0->minPosition, axis0->maxPosition};
		m_axis1Range = {axis1->minPosition, axis1->maxPosition};

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

		m_view->drawGrid();

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

#if RENDER_MEASUREMENT_POINTS
		const auto& measurements = m_heightmap->GetPoints();
		for (size_t i = 0; i < measurements.size(); i++)
		{
			const auto& point = measurements[i];
			m_view->addMeasurementPoint(point.x, point.y);
		}
#endif

		m_view->setStatistics(m_heightmap->GetPointCount(),
							  m_heightmap->GetArea() / 100,
							  m_heightmap->GetMinError(),
							  m_heightmap->GetMaxError(),
							  m_heightmap->GetMeanError(),
							  m_heightmap->GetStdDev());
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
		m_heightmap->LoadFromDuet([this](OM::Heightmap& heightmap) { render(); });
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
		OM::RequestHeightmapFiles(
			[this](const OM::FileSystem::ItemList& files)
			{
				m_heightmapFiles = OM::GetHeightmapFiles(files);
				updateHeightmapList();
			});
	}

	void HeightmapPresenter::newDirectories()
	{
		LOG_DBG("New directories");
		OM::RequestHeightmapFiles(
			[this](const OM::FileSystem::ItemList& files)
			{
				m_heightmapFiles = OM::GetHeightmapFiles(files);
				updateHeightmapList();
			});
	}

	void HeightmapPresenter::newAxesData()
	{
		LOG_DBG("New axes data");
		if (m_heightmap == nullptr)
		{
			LOG_DBG("Heightmap is not set, skipping axis range update");
			return;
		}

		auto axis0 = m_heightmap->meta.GetAxis(0);
		auto axis1 = m_heightmap->meta.GetAxis(1);

		if (axis0 == nullptr || axis1 == nullptr)
		{
			LOG_WARN("Heightmap axes are not valid");
			return;
		}

		if (m_axis0Range == AxisRange(axis0->minPosition, axis0->maxPosition) &&
			m_axis1Range == AxisRange(axis1->minPosition, axis1->maxPosition))
		{
			LOG_VERBOSE("No change in axis range");
			return;
		}

		if (m_heightmap != nullptr && m_heightmap->IsValid())
		{
			render();
		}
	}

	void HeightmapPresenter::updateHeightmapList()
	{
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

	bool HeightmapPresenter::checkMode()
	{
#if 0
		if (Comm::DUET.GetCommunicationType() != Comm::CommunicationType::network)
		{
			LOG_WARN("Heightmap not supported in this mode");
			uint32_t width, height;
			m_view->getResolution(width, height);
			m_view->clear();
			m_view->drawLabel(width / 2,
							  height / 2,
							  utils::format(_("heightmap_not_supported"), _(Comm::DUET.GetCommunicationTypeName())),
							  lv_palette_main(LV_PALETTE_RED),
							  LV_OPA_100);
			return false;
		}
#endif

		return true;
	}

	void HeightmapPresenter::onActivate()
	{
		LOG_DBG("activate");
		OM::RequestHeightmapFiles(
			[this](const OM::FileSystem::ItemList& files)
			{
				m_heightmapFiles = OM::GetHeightmapFiles(files);
				updateHeightmapList();
			});
		std::string_view currentHeightmap = OM::GetCurrentHeightmap();
		std::shared_ptr<OM::Heightmap> map =
			currentHeightmap.empty() ? nullptr : OM::GetHeightmapData(currentHeightmap);
		if (map)
		{
			map->LoadFromDuet([this](OM::Heightmap& heightmap) { render(); });
		}
		setHeightmap(map);
	}

	void HeightmapPresenter::onConnect()
	{
		LOG_DBG("Connected");
		checkMode();
	}

	void HeightmapPresenter::onDisconnect()
	{
		LOG_DBG("Disconnect");
		if (m_heightmap != nullptr)
		{
			m_heightmap = nullptr;
			m_view->clear();
			m_view->setHeightmapCount(0);
		}
	}
} // namespace UI
