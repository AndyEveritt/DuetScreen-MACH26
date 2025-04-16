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
		using Presenter::Presenter;

		// Actions
		void setHeightmap(const std::shared_ptr<OM::Heightmap>& heightmap);
		void setRenderMode(HeightmapRenderMode mode);
		void render();

		// Observers

	  private:
		virtual void onActivate() override;

		std::shared_ptr<OM::Heightmap> m_heightmap;
		HeightmapRenderMode m_mode;
	};
} // namespace UI
