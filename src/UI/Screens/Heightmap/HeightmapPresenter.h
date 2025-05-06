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

		PRESENTER_CONSTRUCTOR(HeightmapPresenter, HeightmapView)

		// Actions
		void setHeightmap(const std::shared_ptr<OM::Heightmap>& heightmap);
		void setRenderMode(HeightmapRenderMode mode);
		HeightmapRenderMode getRenderMode() const { return m_mode; }
		void render();
		void setActiveHeightmap(const size_t index);
		void toggleHeightmap(const size_t index);

		// Observers
		void newCompensationFile() override;
		void newDirectories() override;
		void newAxesData() override;
		void disconnected() override;

	  private:
		virtual void onActivate() override;

		void updateHeightmapList();

		std::shared_ptr<OM::Heightmap> m_heightmap;
		std::vector<OM::FileSystem::ItemPtr> m_heightmapFiles;
		HeightmapRenderMode m_mode = HeightmapRenderMode::Fixed;
	};
} // namespace UI
