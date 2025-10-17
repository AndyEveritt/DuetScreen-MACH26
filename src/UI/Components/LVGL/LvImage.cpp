#include "LvImage.h"
#include "Debug.h"

namespace UI
{
	LvImage::LvImage(const std::string& name, LvObj& parent)
		: LvObj(lv_image_create, name, parent)
	{
		setInnerAlign(LV_IMAGE_ALIGN_CONTAIN);
	}

	void LvImage::setSrc(const void* src)
	{
		UI_LOCK();
		lv_image_set_src(getRootPtr(), src);
	}
	void LvImage::setOffsetX(int32_t x)
	{
		UI_LOCK();
		lv_image_set_offset_x(getRootPtr(), x);
	}
	void LvImage::setOffsetY(int32_t y)
	{
		UI_LOCK();
		lv_image_set_offset_y(getRootPtr(), y);
	}
	void LvImage::setRotation(int32_t angle)
	{
		UI_LOCK();
		lv_image_set_rotation(getRootPtr(), angle);
	}
	void LvImage::setPivot(int32_t x, int32_t y)
	{
		UI_LOCK();
		lv_image_set_pivot(getRootPtr(), x, y);
	}
	void LvImage::setPivotX(int32_t x)
	{
		UI_LOCK();
		lv_image_set_pivot_x(getRootPtr(), x);
	}
	void LvImage::setPivotY(int32_t y)
	{
		UI_LOCK();
		lv_image_set_pivot_y(getRootPtr(), y);
	}
	void LvImage::setScale(uint32_t zoom)
	{
		UI_LOCK();
		lv_image_set_scale(getRootPtr(), zoom);
	}
	void LvImage::setScaleX(uint32_t zoom)
	{
		UI_LOCK();
		lv_image_set_scale_x(getRootPtr(), zoom);
	}
	void LvImage::setScaleY(uint32_t zoom)
	{
		UI_LOCK();
		lv_image_set_scale_y(getRootPtr(), zoom);
	}
	void LvImage::setBlendMode(lv_blend_mode_t mode)
	{
		UI_LOCK();
		lv_image_set_blend_mode(getRootPtr(), mode);
	}
	void LvImage::setAntialias(bool enable)
	{
		UI_LOCK();
		lv_image_set_antialias(getRootPtr(), enable);
	}
	void LvImage::setInnerAlign(lv_image_align_t align)
	{
		UI_LOCK();
		lv_image_set_inner_align(getRootPtr(), align);
	}
	void LvImage::setBitmapMapSrc(const lv_image_dsc_t* src)
	{
		UI_LOCK();
		lv_image_set_bitmap_map_src(getRootPtr(), src);
	}

	const void* LvImage::getSrc() const
	{
		UI_LOCK();
		return lv_image_get_src(getRootPtr());
	}
	int32_t LvImage::getOffsetX() const
	{
		UI_LOCK();
		return lv_image_get_offset_x(getRootPtr());
	}
	int32_t LvImage::getOffsetY() const
	{
		UI_LOCK();
		return lv_image_get_offset_y(getRootPtr());
	}
	int32_t LvImage::getRotation() const
	{
		UI_LOCK();
		return lv_image_get_rotation(getRootPtr());
	}
	void LvImage::getPivot(lv_point_t* pivot) const
	{
		UI_LOCK();
		lv_image_get_pivot(getRootPtr(), pivot);
	}
	int32_t LvImage::getScale() const
	{
		UI_LOCK();
		return lv_image_get_scale(getRootPtr());
	}
	int32_t LvImage::getScaleX() const
	{
		UI_LOCK();
		return lv_image_get_scale_x(getRootPtr());
	}
	int32_t LvImage::getScaleY() const
	{
		UI_LOCK();
		return lv_image_get_scale_y(getRootPtr());
	}
	int32_t LvImage::getSrcWidth() const
	{
		UI_LOCK();
		return lv_image_get_src_width(getRootPtr());
	}
	int32_t LvImage::getSrcHeight() const
	{
		UI_LOCK();
		return lv_image_get_src_height(getRootPtr());
	}
	int32_t LvImage::getTransformedWidth() const
	{
		UI_LOCK();
		return lv_image_get_transformed_width(getRootPtr());
	}
	int32_t LvImage::getTransformedHeight() const
	{
		UI_LOCK();
		return lv_image_get_transformed_height(getRootPtr());
	}
	lv_blend_mode_t LvImage::getBlendMode() const
	{
		UI_LOCK();
		return lv_image_get_blend_mode(getRootPtr());
	}
	bool LvImage::getAntialias() const
	{
		UI_LOCK();
		return lv_image_get_antialias(getRootPtr());
	}
	lv_image_align_t LvImage::getInnerAlign() const
	{
		UI_LOCK();
		return lv_image_get_inner_align(getRootPtr());
	}
	const lv_image_dsc_t* LvImage::getBitmapMapSrc() const
	{
		UI_LOCK();
		return lv_image_get_bitmap_map_src(getRootPtr());
	}
} // namespace UI
