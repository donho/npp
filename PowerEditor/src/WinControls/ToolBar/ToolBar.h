// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


#pragma once

#include "Common.h"
#include "Window.h"
#include "Notepad_plus_msgs.h"
#include "ImageListSet.h"
#include "dpiManagerV2.h"

#define REBAR_BAR_TOOLBAR		0
#define REBAR_BAR_SEARCH		1

#define REBAR_BAR_EXTERNAL		10
#ifndef _WIN32_IE
#define _WIN32_IE	0x0600
#endif //_WIN32_IE

enum toolBarStatusType {TB_SMALL, TB_LARGE, TB_SMALL2, TB_LARGE2, TB_STANDARD};

constexpr int SEPARATOR_CMD_ID = 0;

struct iconLocator {
	size_t _listIndex = 0;
	size_t _iconIndex = 0;
	std::wstring _iconLocation;

	iconLocator(size_t iList, size_t iIcon, const std::wstring& iconLoc)
		: _listIndex(iList), _iconIndex(iIcon), _iconLocation(iconLoc){};
};

struct CommandInfo {
	int _cmdID = -1;
	std::wstring _commandName;
	std::wstring _pluginName;
	bool _hasToolbarButton = false;
	bool _hideToolbarButton = false;
	bool _isFromHiddenPlugin = false;
};

struct ToolbarButtonConfig {
	std::vector<CommandInfo> _commandInfos;
	std::vector<std::wstring> _hiddenPlugins;
	bool _useAllDefaults = true; // Allow us to skip checking button visibility altogether
};

struct ToolbarButtonConfigLoadResult {
	bool _success = false;
	ToolbarButtonConfig _config;
};

struct TBConfigConsts
{
	inline static const std::wstring _notepadPlus = L"NotepadPlus";
	inline static const std::wstring _toolbarButtons = L"ToolbarButtons";
	inline static const std::wstring _plugin = L"Plugin";
	inline static const std::wstring _button = L"Button";
	inline static const std::wstring _name = L"Name";
	inline static const std::wstring _hide = L"Hide";
	inline static const std::wstring _hideAll = L"HideAll";
	inline static const std::wstring _builtInName = L"Built-In";
	inline static const wchar_t _trueStr[] = L"yes";
	inline static const wchar_t _falseStr[] = L"no";
};

class ReBar;
class TiXmlDocument;
class TiXmlNode;

class ToolBar : public Window
{
public :
	ToolBar() = default;
	~ToolBar() = default;

    void initTheme(TiXmlDocument *toolIconsDocRoot);
	virtual bool init(HINSTANCE hInst, HWND hPere, toolBarStatusType type, 
		ToolBarButtonUnit *buttonUnitArray, int arraySize, const ToolbarButtonConfig& toolbarButtonConfig);

	virtual void destroy();
	void enable(int cmdID, bool doEnable) const {
		::SendMessage(_hSelf, TB_ENABLEBUTTON, cmdID, static_cast<LPARAM>(doEnable));
	};

	int getWidth() const;
	int getHeight() const;

	void reduce();
	void enlarge();
	void reduceToSet2();
	void enlargeToSet2();
	void setToBmpIcons();

	bool getCheckState(int ID2Check) const {
		return bool(::SendMessage(_hSelf, TB_GETSTATE, ID2Check, 0) & TBSTATE_CHECKED);
	};

	void setCheck(int ID2Check, bool willBeChecked) const {
		::SendMessage(_hSelf, TB_CHECKBUTTON, ID2Check, MAKELONG(willBeChecked, 0));
	};

	toolBarStatusType getState() const {
		return _state;
	};

    bool change2CustomIconsIfAny() {    
	    if (!_toolIcons) return false;

	    for (size_t i = 0, len = _customIconVect.size(); i < len; ++i)
		    changeIcons(_customIconVect[i]._listIndex, _customIconVect[i]._iconIndex, (_customIconVect[i]._iconLocation).c_str());
        return true;
    };

	bool changeIcons(size_t whichLst, size_t iconIndex, const wchar_t *iconLocation){
		return _toolBarIcons.replaceIcon(whichLst, iconIndex, iconLocation);
	};

	void registerDynBtn(UINT message, toolbarIcons* iconHandles, HICON absentIco);
	void registerDynBtnDM(UINT message, toolbarIconsWithDarkMode* iconHandles);

	void doPopop(POINT chevPoint);	//show the popup if buttons are hidden

	void addToRebar(ReBar * rebar);

	void resizeIconsDpi(UINT dpi);

	ToolbarButtonConfig initToolbarButtonVisibilityConfig(ToolBarButtonUnit standardCommandToolbarIcons[], ToolbarButtonConfigLoadResult toolbarButtonXmlResult, int toolbarIconsArrayCount);
	static ToolbarButtonConfigLoadResult loadToolbarVisibilityXML(std::wstring xmlFilePath);
	static void writeToolbarButtonsExampleConfig(std::wstring exampleXmlPath, const ToolbarButtonConfig& config);

private :
	TBBUTTON *_pTBB = nullptr;
	ToolBarIcons _toolBarIcons;
	toolBarStatusType _state = TB_SMALL;
	std::vector<DynamicCmdIcoBmp> _vDynBtnReg;
	size_t _nbButtons = 0;
	size_t _nbDynButtons = 0;
	size_t _nbTotalButtons = 0;
	size_t _nbCurrentButtons = 0;
	ReBar * _pRebar = nullptr;
	REBARBANDINFO _rbBand = {};
    std::vector<iconLocator> _customIconVect;

    TiXmlNode *_toolIcons = nullptr;

	DPIManagerV2 _dpiManager;

	void setDefaultImageList() {
		::SendMessage(_hSelf, TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(_toolBarIcons.getDefaultLst()));
	};

	void setDisableImageList() {
		::SendMessage(_hSelf, TB_SETDISABLEDIMAGELIST, 0, reinterpret_cast<LPARAM>(_toolBarIcons.getDisableLst()));
	};

	void setDefaultImageList2() {
		::SendMessage(_hSelf, TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(_toolBarIcons.getDefaultLstSet2()));
	};

	void setDisableImageList2() {
		::SendMessage(_hSelf, TB_SETDISABLEDIMAGELIST, 0, reinterpret_cast<LPARAM>(_toolBarIcons.getDisableLstSet2()));
	};

	void setDefaultImageListDM() {
		::SendMessage(_hSelf, TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(_toolBarIcons.getDefaultLstDM()));
	};

	void setDisableImageListDM() {
		::SendMessage(_hSelf, TB_SETDISABLEDIMAGELIST, 0, reinterpret_cast<LPARAM>(_toolBarIcons.getDisableLstDM()));
	};

	void setDefaultImageListDM2() {
		::SendMessage(_hSelf, TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(_toolBarIcons.getDefaultLstSetDM2()));
	};

	void setDisableImageListDM2() {
		::SendMessage(_hSelf, TB_SETDISABLEDIMAGELIST, 0, reinterpret_cast<LPARAM>(_toolBarIcons.getDisableLstSetDM2()));
	};

	void reset(bool create = false);
	void setState(toolBarStatusType state) {
		_state = state;
	}
	
};

class ReBar : public Window
{
public :
	ReBar():Window() { usedIDs.clear(); };

	virtual void destroy() {
		::DestroyWindow(_hSelf);
		_hSelf = NULL;
		usedIDs.clear();
	};

	void init(HINSTANCE hInst, HWND hPere);
	bool addBand(REBARBANDINFO * rBand, bool useID);	//useID true if ID from info should be used (false for plugins). wID in bandinfo will be set to used ID
	void reNew(int id, REBARBANDINFO * rBand);					//wID from bandinfo is used for update
	void removeBand(int id);

	void setIDVisible(int id, bool show);
	bool getIDVisible(int id);
	void setGrayBackground(int id);

private:
	std::vector<int> usedIDs;

	int getNewID();
	void releaseID(int id);
	bool isIDTaken(int id);
};
