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

		using SortBy = OM::FileSystem::SortBy;

		// Actions
		void setBaseFolder(BaseFolder folder) { m_baseFolder = folder; }
		std::string_view getBaseFolderPath() const;
		void setFolder(std::string_view folder);
		void itemClicked(const size_t index);
		void deleteSelectedItem();
		void startPrint();
		void runMacro();
		void requestFiles();
		void refreshFiles();
		void setSort(SortBy by, bool descending = true);
		SortBy getSortBy() const { return m_sortBy; }
		bool getSortOrder() const { return m_sortOrder; }
		bool back();

		// Observers
		void newThumbnailData(const std::string& filename);

	  private:
		void onInit() override;
		void onActivate() override;
		void onConnect() override;
		void onDisconnect() override;

		void displayFiles();
		void sortFiles();

		BaseFolder m_baseFolder = BaseFolder::GCODES;
		std::string m_currentFolder;
		std::string m_gcodePath; // path to gcode file to print
		SortBy m_sortBy = SortBy::DATE;
		bool m_sortOrder = true;
		std::vector<OM::FileSystem::ItemPtr> m_items;
		std::string m_clickedItem;
	};
} // namespace UI
