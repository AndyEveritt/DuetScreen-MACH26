#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class FileView;

	class FilePresenter : public Presenter<FileView>
	{
	  public:
		using Presenter::Presenter;

		// Actions
		void setFolder(const char* folder);
		void itemClicked(const size_t index);
		void requestFiles();
		bool back();

		// Observers

	  private:
		void onActivate() override;

		std::string m_currentFolder;
	};
} // namespace UI
