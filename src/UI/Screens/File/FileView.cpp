#include "FileView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/Home/HomeView.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"
#include "lvgl/lvgl_private.h"
#include "utils/StorageHelper.h"

#define ENABLE_BREADCRUMB_SEPARATOR 1

namespace UI
{
	FileView::FileItem::FileItem(const std::string& name, LvObj& parent)
		: LvContainer(name, parent)
		, m_layoutColDsc{LV_GRID_FR(4), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_layoutRowDsc{LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
	{
		UI_LOCK();

		setWidth(LV_PCT(100));
		// setMaxWidth(400);

		setLayoutStyle(LV_LAYOUT_GRID);
		setGridDsc(m_layoutColDsc, m_layoutRowDsc);
		setGridCell(m_label, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 0, 1);
#if SHOW_FILE_ITEM_SIZE
		setGridCell(m_size, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_END, 1, 1);
#endif
		setGridCell(m_date, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_END, 2, 1);
		setGridCell(m_thumbnail, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 3);
		setGridCell(m_folderIcon, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 3);

		m_label.setHeight(LV_SIZE_CONTENT);
		m_thumbnail.setInnerAlign(LV_IMAGE_ALIGN_CONTAIN);
		m_folderIcon.setIcon("folder.png");

		addEventCallback(
			[this](lv_event_t*)
			{
				if (!m_fileView)
					return;
				m_fileView->onItemClicked(m_index, m_isFolder);
			},
			LV_EVENT_CLICKED);

		// Styles
		addStyle(Themes::getComponentStyles().file);
		addStyle(Themes::getComponentStyles().folder, LV_STATE_CHECKED);
	}

	void FileView::FileItem::setFileLabel(std::string_view name)
	{
		UI_LOCK();
		m_label.setText(name);
	}

	void FileView::FileItem::setFileDate(std::string_view date)
	{
		UI_LOCK();
		m_date.setText(date);
	}

#if SHOW_FILE_ITEM_SIZE
	void FileView::FileItem::setFileSize(std::string_view size)
	{
		UI_LOCK();
		m_size.setText(size);
	}
#endif

	void FileView::FileItem::setThumbnail(const char* thumbnail)
	{
		UI_LOCK();
		m_thumbnail.setSrc(thumbnail);
	}

	void FileView::FileItem::setType(const bool isFolder)
	{
		UI_LOCK();
		m_isFolder = isFolder;
#if SHOW_FILE_ITEM_SIZE
		m_size.setVisible(!isFolder);
#endif
		setState(LV_STATE_CHECKED, isFolder);
		m_folderIcon.setVisible(isFolder);
		m_thumbnail.setVisible(!isFolder);
	}

	std::string_view FileView::FileItem::getLabel() const
	{
		UI_LOCK();
		return m_label.getText();
	}

	std::string_view FileView::FileItem::getDate() const
	{
		UI_LOCK();
		return m_date.getText();
	}

#if SHOW_FILE_ITEM_SIZE
	std::string_view FileView::FileItem::getSize() const
	{
		UI_LOCK();
		return m_size.getText();
	}
#endif

	FileView::LazyFileItem::LazyFileItem(FileView& fileView)
		: m_fileView(fileView)
	{
	}

	lv_coord_t FileView::LazyFileItem::getSize() const
	{
		UI_LOCK();
		// Return a fixed size for now
		return 80;
	}

	void FileView::LazyFileItem::update(size_t index, FileItem& obj)
	{
		UI_LOCK();

		obj.setFileView(&m_fileView);
		obj.setIndex(index);
		obj.setFileLabel(m_filename);
		std::string date(m_date);
		std::replace(date.begin(), date.end(), 'T', ' ');
		obj.setFileDate(date);
#if SHOW_FILE_ITEM_SIZE
		obj.setFileSize(m_size);
#endif
		obj.setThumbnail(m_thumbnail.empty() ? nullptr : m_thumbnail.c_str());
		obj.setType(m_isFolder);
	}

	FileView::FileView(const std::string& name, LvObj& parent, LvObj* msgBoxParent)
		: View(name, parent, layout_t(0, 0, 100, 100))
		, m_startPrint("messageBox", msgBoxParent ? *msgBoxParent : getRoot(), layout_t(0, 0, 70, LV_SIZE_CONTENT))
	{
		UI_LOCK();

		addStyle(Themes::getLvglStyles().bg_dark);

		// Layout
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		// Sidebar
		m_sortDate.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_sortName.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_sortSize.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_pad.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_pad.setFlexGrow(1);
		m_refresh.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		m_sortDate.setText(_("file.sort_by_name"));
		m_sortName.setText(_("file.sort_by_date"));
		m_sortSize.setText(_("file.sort_by_size"));
		m_refresh.setText(_("file.refresh"));

		// Header (from List) becomes breadcrumb container
		m_fileList.getHeader().setFlexFlow(LV_FLEX_FLOW_ROW);
		m_fileList.getHeader().setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		m_fileList.showHeader(true);
		m_breadcrumbPrefix.setText(_("file.header_prefix"));
		m_breadcrumbPrefix.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_breadcrumbCont.setHeight(LV_SIZE_CONTENT);
		m_breadcrumbCont.setFlexFlow(LV_FLEX_FLOW_ROW_WRAP);
		m_breadcrumbCont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
		m_breadcrumbCont.setFlexGrow(1);
		m_breadcrumbCont.addStyle(Themes::getLvglStyles().pad_tiny);
		setFolder("");

		// List container styling
		m_fileList.setWidth(LV_PCT(100));
		m_fileList.setFlexGrow(1);
		// m_fileList.setListFlow(LV_FLEX_FLOW_ROW_WRAP);
		// m_fileList.getListContainer().setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

		// Sidebar
		m_sideBar.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_sideBar.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_sideBar.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		m_sortName.setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(FilePresenter::SortBy::NAME)));
		m_sortDate.setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(FilePresenter::SortBy::DATE)));
		m_sortSize.setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(FilePresenter::SortBy::SIZE)));
		m_sortName.setCheckable(true);
		m_sortDate.setCheckable(true);
		m_sortSize.setCheckable(true);

		// Footer
		m_footer.hide();

		// Start Print
		m_startPrint.setFlag(LV_OBJ_FLAG_FLOATING, true);
		m_startPrint.setAlign(LV_ALIGN_CENTER, 0, 0);
		m_startPrint.setMaxHeight(LV_PCT(70));
		m_startPrint.okVisible(true);
		m_startPrint.cancelVisible(true);
		m_startPrint.setImageSize(LV_PCT(50), 200);
		m_startPrint.setCloseCallback([this]() { closeModal(&m_startPrint); });
		m_startPrint.hide();

		// Callbacks
		m_refresh.addClickedCallback(onRefreshClicked, this);
		m_sortName.addClickedCallback(onSortClicked, this);
		m_sortDate.addClickedCallback(onSortClicked, this);
		m_sortSize.addClickedCallback(onSortClicked, this);
	}

	void FileView::setFileCount(const size_t count)
	{
		UI_LOCK();
		m_fileList.setItemCount(count,
								[this](size_t)
								{
									auto item = std::make_unique<LazyFileItem>(*this);
									return item;
								});
		// m_fileList.setItemCount(count, *this);
	}

	void FileView::setFolder(const std::string& folder)
	{
		UI_LOCK();
		// Clear previous breadcrumb elements
		m_breadcrumbButtons.clear();
		m_breadcrumbLabels.clear();
		m_fileList.showHeader(true); // ensure visible

		m_breadcrumbPaths.clear();
		std::string_view basePath = m_presenter->getBaseFolderPath();

		// Remove base path prefix from full path to get relative path
		std::string_view relPath = folder;
		std::vector<std::string_view> segments;
		if (!basePath.empty() && relPath.starts_with(basePath))
		{
			segments.emplace_back(basePath);
			relPath = relPath.substr(basePath.size());
			if (!relPath.empty() && relPath.front() == '/')
			{
				relPath.remove_prefix(1);
			}
		}

		if (!relPath.empty())
		{
			size_t start = 0;
			while (start < relPath.size())
			{
				size_t end = relPath.find('/', start);
				if (end == std::string_view::npos)
					end = relPath.size();
				if (end > start)
				{
#if ENABLE_BREADCRUMB_SEPARATOR
					segments.emplace_back(relPath.substr(start, end - start));
#else
					segments.emplace_back(relPath.substr(start, end - start + 1));
#endif
				}
				start = end + 1;
			}
		}

		std::string accum;
		m_breadcrumbButtons.reserve(segments.size());
		for (size_t i = 0; i < segments.size(); ++i)
		{
			if (!accum.empty())
			{
				accum += '/';
			}
			accum += segments[i];
			m_breadcrumbPaths.push_back(accum); // relative path accumulation
		}

		m_breadcrumbLabels.reserve(segments.size());
		m_breadcrumbButtons.reserve(segments.size());
		for (size_t i = 0; i < segments.size(); ++i)
		{
#if ENABLE_BREADCRUMB_SEPARATOR
			if (i > 0)
			{
				auto sep = std::make_unique<LvLabel>(fmt::format("crumb_sep_{}", i), m_breadcrumbCont);
				sep->setText(_("file.crumb_separator"));
				m_breadcrumbLabels.push_back(std::move(sep));
			}
#endif // ENABLE_BREADCRUMB_SEPARATOR
			if (i == segments.size() - 1)
			{
				// Last segment is not clickable
				auto label = std::make_unique<LvLabel>(fmt::format("crumb_label_{}", i), m_breadcrumbCont);
				label->setText(segments[i]);
				m_breadcrumbLabels.push_back(std::move(label));
				continue;
			}

			auto btn = std::make_unique<Button>(fmt::format("crumb_btn_{}", i), m_breadcrumbCont, segments[i]);
			btn->setUserData(reinterpret_cast<void*>(i));
			btn->addClickedCallback(onBreadcrumbClicked, this);
			m_breadcrumbButtons.push_back(std::move(btn));
		}
	}

	void FileView::onBreadcrumbClicked(lv_event_t* e)
	{
		UI_LOCK();
		FileView* view = static_cast<FileView*>(lv_event_get_user_data(e));
		LvObj* btn = LvObj::fromPtr(lv_event_get_target_obj(e));
		size_t index = reinterpret_cast<size_t>(btn->getUserData());
		if (index >= view->m_breadcrumbPaths.size())
		{
			return;
		}
		// Set folder relative path using presenter->setFolder
		view->m_presenter->setFolder(view->m_breadcrumbPaths[index]);
	}

	void FileView::onItemClicked(size_t index, bool /* isFolder */)
	{
		UI_LOCK();
		if (index >= m_fileList.getLazyItemCount())
		{
			return;
		}

		m_presenter->itemClicked(index);
	}

	bool FileView::cancelStartPrint()
	{
		UI_LOCK();
		if (m_startPrint.isVisible())
		{
			m_startPrint.cancel();
			return true;
		}
		return false;
	}

	void FileView::confirmStartPrint(std::string_view filename,
									 std::string_view date,
									 std::string_view size,
									 std::string_view thumbnail)
	{
		UI_LOCK();
		m_startPrint.setTitle(_("file.start_print_title"));
		m_startPrint.setText(_("file.start_print_message", filename, date, size));
		m_startPrint.setOkCallback([this]() { m_presenter->startPrint(); });
		m_startPrint.setImage(IsThumbnailCached(thumbnail) ? thumbnail.data() : nullptr);
		openModal(&m_startPrint);
	}

	void FileView::confirmRunMacro(std::string_view filename)
	{
		UI_LOCK();
		m_startPrint.setTitle(_("file.run_macro_title"));
		m_startPrint.setText(_("file.run_macro_message", filename));
		m_startPrint.setOkCallback([this]() { m_presenter->runMacro(); });
		m_startPrint.setImage(nullptr);
		openModal(&m_startPrint);
	}

	void FileView::showSort(FilePresenter::SortBy by, bool descending)
	{
		m_sortName.setChecked(false);
		m_sortDate.setChecked(false);
		m_sortSize.setChecked(false);

		m_sortName.setText(_("file.sort_by_name"));
		m_sortDate.setText(_("file.sort_by_date"));
		m_sortSize.setText(_("file.sort_by_size"));

		switch (by)
		{
		case FilePresenter::SortBy::NAME:
			m_sortName.setChecked(true);
			m_sortName.setText(descending ? fmt::format(LV_SYMBOL_DOWN " {:s}", _("file.sort_by_name"))
										  : fmt::format(LV_SYMBOL_UP " {:s}", _("file.sort_by_name")));
			break;
		case FilePresenter::SortBy::DATE:
			m_sortDate.setChecked(true);
			m_sortDate.setText(descending ? fmt::format(LV_SYMBOL_DOWN " {:s}", _("file.sort_by_date"))
										  : fmt::format(LV_SYMBOL_UP " {:s}", _("file.sort_by_date")));
			break;
		case FilePresenter::SortBy::SIZE:
			m_sortSize.setChecked(true);
			m_sortSize.setText(descending ? fmt::format(LV_SYMBOL_DOWN " {:s}", _("file.sort_by_size"))
										  : fmt::format(LV_SYMBOL_UP " {:s}", _("file.sort_by_size")));
			break;
		}
	}

	void FileView::onRefreshClicked(lv_event_t* e)
	{
		UI_LOCK();
		FileView* view = static_cast<FileView*>(lv_event_get_user_data(e));
		view->cancelStartPrint();
		view->m_presenter->refreshFiles();
	}

	void FileView::onSortClicked(lv_event_t* e)
	{
		UI_LOCK();
		FileView* view = static_cast<FileView*>(lv_event_get_user_data(e));
		LvObj* btn = LvObj::fromPtr(lv_event_get_target_obj(e));
		FilePresenter::SortBy sort =
			static_cast<FilePresenter::SortBy>(reinterpret_cast<uintptr_t>(btn->getUserData()));

		bool forward = view->m_presenter->getSortOrder();
		if (view->m_presenter->getSortBy() == sort)
		{
			forward = !forward;
		}
		view->m_presenter->setSort(sort, forward);
	}

	bool FileView::back()
	{
		UI_LOCK();
		if (cancelStartPrint())
		{
			return true;
		}
		return m_presenter->back();
	}

	void FileView::onShow()
	{
		UI_LOCK();
		cancelStartPrint();
	}

	void FileView::onHide() {}
} // namespace UI
