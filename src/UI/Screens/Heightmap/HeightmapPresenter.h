#pragma once

#include "ObjectModel/Heightmap.h"
#include "UI/Core/Presenter.h"

namespace UI
{
	class HeightmapView;

	enum class HeightmapRenderMode
	{
		Fixed = 0,
		Auto,
	};

	class HeightmapPresenter : public Presenter<HeightmapView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(HeightmapPresenter, HeightmapView)

		// Actions
		void setHeightmap(const std::shared_ptr<OM::Heightmap>& heightmap);
		void setRenderMode(HeightmapRenderMode mode);
		void render();
		void setActiveHeightmap(const size_t index);
		void toggleHeightmap(const size_t index);

		// Observers
		void newCompensationFile() override;
		void newDirectories() override;

	  private:
		virtual void onActivate() override;

		void updateHeightmapList();

		std::shared_ptr<OM::Heightmap> m_heightmap;
		std::vector<OM::FileSystem::ItemPtr> m_heightmapFiles;
		HeightmapRenderMode m_mode = HeightmapRenderMode::Fixed;
	};
} // namespace UI
