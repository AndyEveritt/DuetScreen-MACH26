#pragma once

#include "FilePresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Icon/Icon.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvImage.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/List/LazyList.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Components/Modal/Modal.h"
#include "UI/Core/View.h"
#include <memory>

namespace UI
{
#define SHOW_FILE_ITEM_SIZE 0

	class FileView : public View<FilePresenter>
	{
	  public:
		class FileItem : public LvContainer
		{
		  public:
			FileItem(const std::string& name, LvObj& parent);
			void setFileLabel(std::string_view name);
			void setFileDate(std::string_view date);
#if SHOW_FILE_ITEM_SIZE
			void setFileSize(std::string_view size);
#endif
			void setThumbnail(const char* thumbnail);
			void setType(const bool isFolder);

			std::string_view getLabel() const;
			std::string_view getDate() const;
#if SHOW_FILE_ITEM_SIZE
			std::string_view getSize() const;
#endif

			void setFileView(FileView* fileView) { m_fileView = fileView; }
			void setIndex(size_t index) { m_index = index; }

		  private:
			int32_t m_layoutColDsc[3];
			int32_t m_layoutRowDsc[5];

			LvLabel m_label{"label", getRoot()};
#if SHOW_FILE_ITEM_SIZE
			LvLabel m_size{"size", getRoot()};
#endif
			LvLabel m_date{"date", getRoot()};
			LvImage m_thumbnail{"thumb", getRoot()};
			Icon m_folderIcon{"folder_icon", getRoot()};

			FileView* m_fileView = nullptr;
			size_t m_index = 0;
			bool m_isFolder = false;
		};

		class LazyFileItem : public LazyObj<FileItem>
		{
		  public:
			LazyFileItem(FileView& fileView);

			lv_coord_t getWidth() const override;
			lv_coord_t getHeight() const override;
			void update(size_t index, FileItem& obj) override;

			std::string_view m_filename;
			std::string_view m_date;
#if SHOW_FILE_ITEM_SIZE
			std::string m_size;
#endif
			std::string m_thumbnail;

			bool m_isFolder;
			FileView& m_fileView;
		};

		class StartPrintModal : public Modal<MessageBox>
		{
		  public:
			StartPrintModal(const std::string& name, LvObj& parent);

			void setFile(std::string_view value);
			void setFileDate(std::string_view value);
			void setFileSize(std::string_view value);
			void setGeneratedBy(std::string_view value);
			void setPrintTime(std::string_view value);
			void setHeight(float value);
			void setLayerHeight(float value);
			void setDeleteCallback(std::function<void()> cb) { m_deleteCb = std::move(cb); }
			void deleteVisible(bool visible) { m_deleteBtn.setVisible(visible); }

			auto& getFileInfo() { return m_fileInfoCont; }

		  private:
			LvContainer m_fileInfoCont{"file_info_cont", getBodyTextCont()};
			LvLabel m_fileDateLabel{"file_date_label", m_fileInfoCont};
			LvLabel m_fileDateValue{"file_date_value", m_fileInfoCont};

			LvLabel m_fileSizeLabel{"file_size_label", m_fileInfoCont};
			LvLabel m_fileSizeValue{"file_size_value", m_fileInfoCont};

			LvLabel m_generatedByLabel{"generated_by_label", m_fileInfoCont};
			LvLabel m_generatedByValue{"generated_by_value", m_fileInfoCont};

			LvLabel m_printTimeLabel{"print_time_label", m_fileInfoCont};
			LvLabel m_printTimeValue{"print_time_value", m_fileInfoCont};

			LvLabel m_heightLabel{"height_label", m_fileInfoCont};
			LvLabel m_heightValue{"height_value", m_fileInfoCont};

			LvLabel m_layerHeightLabel{"layer_height_label", m_fileInfoCont};
			LvLabel m_layerHeightValue{"layer_height_value", m_fileInfoCont};
			Button m_deleteBtn{"delete", getFooter(), LV_SYMBOL_TRASH};
			std::function<void()> m_deleteCb;
		};

		FileView(const std::string& name, LvObj& parent);

		auto& getList() { return m_fileList; }
		size_t getFileCount() const { return m_fileList.getLazyItemCount(); }
		void setFileCount(const size_t count);
		auto getFileItem(size_t index) const { return m_fileList.getLazyItem(index); }

		void setFolder(const std::string& path);
		bool cancelStartPrint();
		void confirmStartPrint(std::string_view filename, const std::filesystem::path& thumbnail);
		void confirmRunMacro(std::string_view filename);
		void confirmDelete(std::string_view filename);
		auto& getConfirmModal() { return m_startPrint; }

		void showSort(FilePresenter::SortBy by, bool descending);

		void onItemClicked(size_t index, bool isFolder);

		bool back() override;

	  private:
		static void onRefreshClicked(lv_event_t* e);
		static void onSortClicked(lv_event_t* e);
		static void onBreadcrumbClicked(lv_event_t* e);

		void onInit() override;
		void onShow() override;
		void onHide() override;

		LvContainer m_sideBar{"sidebar", getRoot()};
		LazyList<LazyFileItem> m_fileList{"list", getRoot()}; // manages header (breadcrumbs) + items container
		Button m_sortName{"sort_name", m_sideBar};
		Button m_sortDate{"sort_date", m_sideBar};
		Button m_sortSize{"sort_size", m_sideBar};
		LvContainer m_pad{"pad", m_sideBar};
		Button m_refresh{"refresh", m_sideBar};
		LvLabel m_footer{"footer", getRoot()};

		LvLabel m_breadcrumbPrefix{"breadcrumb_prefix", m_fileList.getHeader()};
		LvContainer m_breadcrumbCont{"breadcrumb_container", m_fileList.getHeader()};
		std::vector<std::string> m_breadcrumbPaths; // relative paths for each breadcrumb index
		std::vector<std::unique_ptr<Button>> m_breadcrumbButtons;
		std::vector<std::unique_ptr<LvLabel>> m_breadcrumbLabels;

		StartPrintModal m_startPrint{"messageBox", getRoot()};
	};
} // namespace UI