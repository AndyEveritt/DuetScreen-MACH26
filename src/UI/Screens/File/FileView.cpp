#include "FileView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/Home/HomeView.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "lvgl/lvgl_private.h"
#include "utils/StorageHelper.h"

namespace UI
{
	FileView::FileItem::FileItem(const size_t index, FileView* view, lv_obj_t* parent, layout_t layout)
		: BaseView(utils::format("file_item_%u", index), parent, layout)
		, m_index(index)
		, m_list(view)
		, m_layoutColDsc{LV_GRID_FR(4), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_layoutRowDsc{LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_label(lv_label_create(getCont()))
		, m_date(lv_label_create(getCont()))
		, m_size(lv_label_create(getCont()))
		, m_thumbnail(lv_image_create(getCont()))
		, m_type(lv_label_create(getCont()))
	{
		Lock lock;
		// Layout
		constexpr lv_coord_t pad = 2;
		lv_obj_set_height(getCont(), LV_SIZE_CONTENT);
		lv_obj_set_style_pad_all(getCont(), pad, 0);
		lv_obj_set_style_pad_column(getCont(), pad, 0);

		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		lv_obj_set_grid_dsc_array(getCont(), m_layoutColDsc, m_layoutRowDsc);
		lv_obj_set_grid_cell(m_label, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 0, 1);
		lv_obj_set_grid_cell(m_date, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_END, 1, 1);
		lv_obj_set_grid_cell(m_size, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 2, 1);
		lv_obj_set_grid_cell(m_thumbnail, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 2);
		lv_obj_set_grid_cell(m_type, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 2, 1);

		lv_obj_set_height(m_label, LV_SIZE_CONTENT);

		lv_image_set_inner_align(m_thumbnail, LV_IMAGE_ALIGN_AUTO_SCALE);

		// Callbacks
		lv_obj_add_event_cb(getCont(), onClick, LV_EVENT_CLICKED, this);

		// Styles
		lv_obj_set_style_bg_color(getCont(), lv_color_hex(0xcb7912), LV_STATE_CHECKED);
	}

	void FileView::FileItem::setLabel(const char* name)
	{
		Lock lock;
		lv_label_set_text(m_label, name);
	}

	void FileView::FileItem::setDate(const char* date)
	{
		Lock lock;
		lv_label_set_text(m_date, date);
	}

	void FileView::FileItem::setSize(const char* size)
	{
		Lock lock;
		lv_label_set_text(m_size, size);
	}

	void FileView::FileItem::setThumbnail(const char* thumbnail)
	{
		Lock lock;
		lv_image_set_src(m_thumbnail, thumbnail);
	}

	void FileView::FileItem::setType(const bool isFolder)
	{
		Lock lock;
		m_isFolder = isFolder;
		lv_label_set_text(m_type, isFolder ? _("folder") : _("file"));
		lv_obj_set_state(getCont(), LV_STATE_CHECKED, isFolder);
	}

	const char* FileView::FileItem::getLabel() const
	{
		Lock lock;
		return lv_label_get_text(m_label);
	}

	const char* FileView::FileItem::getDate() const
	{
		Lock lock;
		return lv_label_get_text(m_date);
	}

	const char* FileView::FileItem::getSize() const
	{
		Lock lock;
		return lv_label_get_text(m_size);
	}

	void FileView::FileItem::onClick(lv_event_t* e)
	{
		Lock lock;
		FileItem* item = static_cast<FileItem*>(lv_event_get_user_data(e));
		item->getList()->onItemClicked(item->m_index, item->m_isFolder);
	}

	FileView::FileView(lv_obj_t* parent)
		: View("file_view", parent, layout_t(0, 0, 100, 100))
		, m_layoutColDsc{LV_GRID_FR(1), 100, LV_GRID_TEMPLATE_LAST}
		, m_layoutRowDsc{LV_GRID_CONTENT, LV_GRID_FR(4), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST}
		, m_listHeader(lv_label_create(getCont()))
		, m_listCont(lv_obj_create(getCont()))
		, m_sideBar(lv_obj_create(getCont()))
		, m_refresh("file_refresh", m_sideBar, _("refresh"), layout_t(0, 0, 100, LV_SIZE_CONTENT))
		, m_footer(lv_label_create(getCont()))
		, m_startPrint("file_messageBox", getCont(), layout_t(0, 0, 70, LV_SIZE_CONTENT))
	{
		Lock lock;

		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		lv_obj_set_grid_dsc_array(getCont(), m_layoutColDsc, m_layoutRowDsc);
		lv_obj_set_grid_cell(
			m_listHeader, LV_GRID_ALIGN_STRETCH, 0, ARRAY_SIZE(m_layoutColDsc) - 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_listCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		lv_obj_set_grid_cell(
			m_sideBar, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, ARRAY_SIZE(m_layoutRowDsc) - 2);
		lv_obj_set_grid_cell(m_footer, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);

		// Header
		lv_label_set_text(m_listHeader, utils::format(_("file_header"), "").c_str());

		// List
		lv_obj_set_flex_flow(m_listCont, LV_FLEX_FLOW_ROW_WRAP);
		lv_obj_set_flex_align(m_listCont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
		lv_obj_set_style_pad_all(m_listCont, 10, 0);
		lv_obj_set_style_pad_column(m_listCont, 10, 0);

		// Sidebar
		lv_obj_set_flex_flow(m_sideBar, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(m_sideBar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_style_pad_all(m_sideBar, 2, 0);

		// Footer
		constexpr lv_coord_t footerPad = 2;
		lv_obj_set_style_pad_all(m_footer, footerPad, 0);

		// Start Print
		lv_obj_add_flag(m_startPrint.getCont(), LV_OBJ_FLAG_FLOATING);
		lv_obj_set_align(m_startPrint.getCont(), LV_ALIGN_CENTER);
		lv_obj_set_style_max_height(m_startPrint.getCont(), LV_PCT(70), 0);
		m_startPrint.setMode(OM::Alert::Mode::ConfirmCancel);
		m_startPrint.setImageSize(200, 200);

		// Callbacks
		m_refresh.setCallback(onRefreshClicked, LV_EVENT_CLICKED, this);
	}

	void FileView::setFileCount(const size_t count)
	{
		Lock lock;
		if (count == getFileCount())
		{
			return;
		}
		if (count < getFileCount())
		{
			m_fileItems.resize(count);
			return;
		}

		m_fileItems.reserve(count);
		for (size_t i = getFileCount(); i < count; ++i)
		{
			m_fileItems.emplace_back(
				std::make_unique<FileItem>(i, this, m_listCont, layout_t(0, 0, 49, LV_SIZE_CONTENT)));
		}
	}

	std::shared_ptr<FileView::FileItem> FileView::getFileItem(size_t index) const
	{
		Lock lock;
		if (index < m_fileItems.size())
		{
			return m_fileItems[index];
		}
		return nullptr;
	}

	void FileView::setFolder(const char* folder)
	{
		Lock lock;
		lv_label_set_text(m_listHeader, utils::format(_("file_header"), folder).c_str());
	}

	void FileView::onItemClicked(size_t index, bool isFolder)
	{
		Lock lock;
		if (index >= m_fileItems.size())
		{
			return;
		}

		m_presenter.itemClicked(index);
	}

	bool FileView::cancelStartPrint()
	{
		Lock lock;
		if (m_startPrint.isVisible())
		{
			m_startPrint.cancel();
			return true;
		}
		return false;
	}

	void FileView::confirmStartPrint(const char* filename, const char* date, const char* size, const char* thumbnail)
	{
		Lock lock;
		m_startPrint.setTitle(_("file_start_print_title"));
		m_startPrint.setText(utils::format(_("file_start_print_message"), filename, date, size));
		m_startPrint.setOkCallback(
			[this]()
			{
				m_presenter.startPrint();
				openScreen(&HomeView::instance().getStatusView());
			});
		m_startPrint.setImage(IsThumbnailCached(thumbnail) ? thumbnail : nullptr);
		m_startPrint.show();
	}

	void FileView::confirmRunMacro(const char* filename)
	{
		Lock lock;
		m_startPrint.setTitle(_("file_run_macro_title"));
		m_startPrint.setText(utils::format(_("file_run_macro_message"), filename));
		m_startPrint.setOkCallback([this]() { m_presenter.runMacro(); });
		m_startPrint.show();
	}

	void FileView::onRefreshClicked(lv_event_t* e)
	{
		Lock lock;
		FileView* view = static_cast<FileView*>(lv_event_get_user_data(e));
		view->cancelStartPrint();
		view->m_presenter.refreshFiles();
	}

	bool FileView::back()
	{
		Lock lock;
		if (cancelStartPrint())
		{
			return true;
		}
		return m_presenter.back();
	}

	void FileView::onShow()
	{
		Lock lock;
		cancelStartPrint();
	}

	void FileView::onHide() {}
} // namespace UI
