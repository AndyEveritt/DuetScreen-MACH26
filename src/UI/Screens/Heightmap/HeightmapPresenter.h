#pragma once

#include "ObjectModel/Heightmap.h"
#include "UI/Core/Presenter.h"

namespace UI
{
	class HeightmapView;

	class HeightmapPresenter : public Presenter<HeightmapView>
	{
	  public:
		enum class HeightmapRenderMode
		{
			Fixed = 0,
			Auto,
		};

		struct AxisRange
		{
			float min = 0.0f;
			float max = 0.0f;

			bool operator==(const AxisRange& other) const { return min == other.min && max == other.max; }
		};

		PRESENTER_CONSTRUCTOR(HeightmapPresenter, HeightmapView)

		// Actions
		void setHeightmap(const std::shared_ptr<OM::Heightmap>& heightmap);
		void setRenderMode(HeightmapRenderMode mode);
		HeightmapRenderMode getRenderMode() const { return m_mode; }
		void render();
		void setActiveHeightmap(const size_t index);
		void toggleHeightmap(const size_t index);

		// Observers
		void newCompensationFile();
		void newDirectories();
		void newAxesData();
		
		private:
		void onActivate() override;
		void onInit() override
		{
			registerEventListener<EventType::CompensationFile>(this, &HeightmapPresenter::newCompensationFile);
			registerEventListener<EventType::Directories>(this, &HeightmapPresenter::newDirectories);
			registerEventListener<EventType::AxesData>(this, &HeightmapPresenter::newAxesData);
		}
		void onConnect() override;
		void onDisconnect() override;

		void updateHeightmapList();
		bool checkMode();

		std::shared_ptr<OM::Heightmap> m_heightmap;
		std::vector<OM::FileSystem::ItemPtr> m_heightmapFiles;
		HeightmapRenderMode m_mode = HeightmapRenderMode::Fixed;
		AxisRange m_axis0Range;
		AxisRange m_axis1Range;
	};
} // namespace UI
