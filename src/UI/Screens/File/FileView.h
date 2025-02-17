#pragma once

#include "FilePresenter.h"
#include "UI/Components/Button.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Core/View.h"

namespace UI
{
	class FileView;

	class FileItem : public BaseView
	{
	  public:
		FileItem(const size_t index, FileView* view, lv_obj_t* parent, layout_t layout);
		void setLabel(const char* name);
		void setDate(const char* date);
		void setSize(const char* size);
		void setThumbnail(lv_img_dsc_t* thumbnail);
		void setType(const bool isFolder);

		const char* getLabel() const;
		const char* getDate() const;
		const char* getSize() const;

	  private:
		FileView* getList() const { return m_list; }

		static void onClick(lv_event_t* e);

		size_t m_index;
		FileView* m_list;

		int32_t m_layoutColDsc[3];
		int32_t m_layoutRowDsc[4];

		lv_obj_t* m_label;
		lv_obj_t* m_date;
		lv_obj_t* m_size;
		lv_obj_t* m_thumbnail;
		lv_obj_t* m_type;

		bool m_isFolder;
	};

	class FileView : public View<FilePresenter>
	{
	  public:
		friend class FileItem;
		friend class FilePresenter;

		FileView(lv_obj_t* parent);

		const size_t getFileCount() const { return m_fileItems.size(); }
		void setFileCount(const size_t count);
		std::shared_ptr<FileItem> getFileItem(size_t index) const;

		void setFolder(const char* path);
		void confirmStartPrint(const char* filename, const char* date, const char* size);

		void onItemClicked(size_t index, bool isFolder);

		bool back() override;

	  private:
		virtual void onShow() override;
		virtual void onHide() override;

		int32_t m_layoutColDsc[3];
		int32_t m_layoutRowDsc[4];

		lv_obj_t* m_listHeader;
		lv_obj_t* m_listCont;
		std::vector<std::shared_ptr<FileItem>> m_fileItems;
		lv_obj_t* m_sideBar;
		Button m_refresh;
		lv_obj_t* m_footer;

		MessageBox m_startPrint;
	};
} // namespace UI