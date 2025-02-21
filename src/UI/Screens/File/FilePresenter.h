#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class FileView;

	class FilePresenter : public Presenter<FileView>
	{
	  public:
		using Presenter::Presenter;
		enum class BaseFolder
		{
			GCODES,
			MACROS
		};

		// Actions
		void setBaseFolder(BaseFolder folder) { m_baseFolder = folder; }
		const char* getBaseFolderPath() const;
		void setFolder(const char* folder);
		void itemClicked(const size_t index);
		void startPrint();
		void runMacro();
		void requestFiles();
		bool back();

		// Observers

	  private:
		void onActivate() override;

		BaseFolder m_baseFolder = BaseFolder::GCODES;
		std::string m_currentFolder;
		std::string m_gcodePath; // path to gcode file to print
	};
} // namespace UI
