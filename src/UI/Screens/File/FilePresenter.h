#pragma once

#include "ObjectModel/Files.h"
#include "UI/Core/Presenter.h"

namespace UI
{
	class FileView;

	class FilePresenter : public Presenter<FileView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(FilePresenter, FileView)
		enum class BaseFolder
		{
			GCODES,
			MACROS
		};

		enum class SortBy
		{
			NAME,
			DATE,
			SIZE
		};

		// Actions
		void setBaseFolder(BaseFolder folder) { m_baseFolder = folder; }
		const char* getBaseFolderPath() const;
		void setFolder(const char* folder);
		void itemClicked(const size_t index);
		void startPrint();
		void runMacro();
		void requestFiles();
		void refreshFiles();
		void setSort(SortBy by, bool descending = true);
		SortBy getSortBy() const { return m_sortBy; }
		bool getSortOrder() const { return m_sortOrder; }
		bool back();

		// Observers
		virtual void newThumbnailData(const char* filename) override;

	  private:
		void onActivate() override;

		void displayFiles();
		void sortFiles();

		BaseFolder m_baseFolder = BaseFolder::GCODES;
		std::string m_currentFolder;
		std::string m_gcodePath; // path to gcode file to print
		SortBy m_sortBy = SortBy::DATE;
		bool m_sortOrder = true;
		std::vector<std::shared_ptr<OM::FileSystem::FileSystemItem>> m_items;
	};
} // namespace UI
