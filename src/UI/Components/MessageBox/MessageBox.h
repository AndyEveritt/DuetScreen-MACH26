/*
 * MessageBox.h
 *
 *  Created on: 2025-01-29
 *      Author: Andy Everitt
 */

#pragma once

#include "ObjectModel/Alert.h"
#include "ObjectModel/Axis.h"
#include "UI/Components/Button.h"
#include "UI/Core/View.h"
#include <functional>
#include <map>
#include <memory>

namespace UI
{

	class MessageBox : public BaseView
	{
	  public:
		enum class VerticalPosition
		{
			top = 0,
			center,
			bottom,
		};

		enum class HorizontalPosition
		{
			left = 0,
			center,
			right,
		};

		class AxisJog : public BaseView
		{
		  public:
			AxisJog(const size_t index, lv_obj_t* parent, MessageBox& msgBox);
			void setAxisLetter(char letter);
			const char* getAxisLetter() const { return m_axisLetter; }
			void setPosition(float position);
			void setEnabled(bool enabled);

		  private:
			static void onRelMoveEvent(lv_event_t* e);

			const size_t m_index;
			MessageBox& m_msgBox;

			char m_axisLetter[2];
			float m_position;

			lv_obj_t* m_label;
			Button m_relMove[6];
		};

		MessageBox(const std::string& name, lv_obj_t* parent, layout_t layout);

		void open();
		void open(std::function<void(void)> okCb);
		void open(std::function<void(void)> okCb, std::function<void(void)> cancelCb);
		void ok();
		void cancel(bool close = true);
		void setPosition(const VerticalPosition& vertical, const HorizontalPosition& horizontal);
		void setTitle(const std::string& title);
		void setText(const std::string& text);
		void setOkBtnText(const std::string& text);
		void setCancelBtnText(const std::string& text);
		void setImage(const char* imagePath);
		void showImage(bool show);
		void setProgress(int percent);
		void setMode(OM::Alert::Mode mode);
		void preventClosing(bool prevent);
		void close();

		void setMinTextf(const char* format, ...);
		void setMaxTextf(const char* format, ...);
		void setWarningTextf(const char* format, ...);
		void setInput(int32_t val);
		void setInput(float val);
		void setInput(const char* text);

		void okVisible(bool visible);
		void cancelVisible(bool visible);
		void selectionVisible(bool visible);
		void numberInputVisible(bool visible);
		void textInputVisible(bool visible);
		void warningTextVisible(bool visible);
		void minTextVisible(bool visible);
		void maxTextVisible(bool visible);
		void axisJogVisible(bool visible);

		// Axis Jog
		size_t getJogAxisCount() const;
		void setJogAxisCount(size_t count);
		const char* getJogAxisLetter(int index) const;
		void setJogAxisLetter(size_t index, char letter);

		// Choices
		size_t getChoiceCount() const;
		void setChoiceCount(size_t count);
		void setChoice(size_t index, const std::string& text);

		const char* getInput() const;

		void cancelTimeout();
		void setTimeout(uint32_t timeout);
		const uint32_t getTimeout() const;

		bool isOpen() const;
		bool isBlocking() const;
		bool isResponse() const;
		void clear();

		bool validateIntegerInput(const char* text);
		bool validateFloatInput(const char* text);
		bool validateTextInput(const char* text);

		const OM::Alert::Mode getMode() const { return m_mode; }

	  private:
		static void onChoiceEvent(lv_event_t* e);

		void init();
		bool validateIntegerInputInner(const char* text);
		bool validateFloatInputInner(const char* text);
		bool validateTextInputInner(const char* text);

		int32_t m_layoutColDsc[4];
		int32_t m_layoutRowDsc[3];

		lv_obj_t* m_msgBox;
		lv_obj_t* m_title;

		lv_obj_t* m_topCont;
		lv_obj_t* m_centralCont;
		lv_obj_t* m_bottomCont;

		// Top container
		lv_obj_t* m_text;
		lv_obj_t* m_image;

		// Central Container
		lv_obj_t* m_inputCont;
		lv_obj_t* m_axisJogCont;

		lv_obj_t* m_choicesList;

		// Input Container
		lv_obj_t* m_warningText;
		lv_obj_t* m_minText;
		lv_obj_t* m_maxText;
		lv_obj_t* m_input;

		// Axis Jog Container
		std::vector<std::shared_ptr<AxisJog>> m_axisJogList;

		// Choices
		std::vector<std::shared_ptr<Button>> m_choices;

		// Bottom Container
		Button m_cancelBtn;
		Button m_okBtn;
		lv_obj_t* m_progress;

		std::function<void(void)> m_okCb;
		std::function<void(void)> m_cancelCb;
		OM::Alert::Mode m_mode;
		OM::Move::Axis* m_axes[MAX_TOTAL_AXES];
		uint32_t m_timeout;
		int selectedAxis = 0;
	};
} // namespace UI
