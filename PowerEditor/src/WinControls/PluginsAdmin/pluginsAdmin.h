// This file is part of Notepad++ project
// Copyright (C)2017 Don HO <don.h@free.fr>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// Note that the GPL places important restrictions on "derived works", yet
// it does not provide a detailed definition of that term.  To avoid      
// misunderstandings, we consider an application to constitute a          
// "derivative work" for the purpose of this license if it does any of the
// following:                                                             
// 1. Integrates source code from Notepad++.
// 2. Integrates/includes/aggregates Notepad++ into a proprietary executable
//    installer, such as those produced by InstallShield.
// 3. Links to a library or executes a program that does any of the above.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#pragma once

#include "StaticDialog.h"
#include "pluginsAdminRes.h"
#include "TabBar.h"
#include "ListView.h"
#include "tinyxml.h"

class PluginsManager;

struct Version
{
	unsigned long _major = 0;
	unsigned long _minor = 0;
	unsigned long _patch = 0;
	unsigned long _build = 0;
	void setVersionFrom(generic_string filePath);
	generic_string toString();
};

struct PluginUpdateInfo
{
	generic_string _fullFilePath; // only for the installed Plugin

	generic_string _folderName;   // plugin folder name - should be the same name with plugin and should be uniq among the plugins
	generic_string _displayName;  // plugin description name
	Version _version;
	generic_string _homepage;
	generic_string _sourceUrl;
	generic_string _description;
	generic_string _author;
	generic_string _md5;
	generic_string _id;
	generic_string _repository;

	generic_string describe();
	PluginUpdateInfo() {};
	PluginUpdateInfo(const generic_string& fullFilePath, const generic_string& fileName);
};

struct NppCurrentStatus
{
	bool _isAdminMode;              // can launch gitup en Admin mode directly

	bool _isInProgramFiles;         // true: install/update/remove on "Program files" (ADMIN MODE)
									// false: install/update/remove on NPP_INST or install on %APPDATA%, update/remove on %APPDATA% & NPP_INST (NORMAL MODE)
									
	bool _isAppDataPluginsAllowed;  // true: install on %APPDATA%, update / remove on %APPDATA% & "Program files" or NPP_INST

	generic_string _nppInstallPath;
	generic_string _appdataPath;

	// it should determinate :
	// 1. deployment location : %ProgramFile%   %appdata%   %other%
	// 2. gitup launch mode:    ADM             ADM         NOMAL
	bool shouldLaunchInAdmMode() { return _isInProgramFiles; };
};

class PluginViewList
{
public:
	PluginViewList() {};
	~PluginViewList() {
		_ui.destroy();
		for (auto i : _list)
		{
			delete i;
		}
	};

	void pushBack(PluginUpdateInfo* pi);
	HWND getViewHwnd() { return _ui.getHSelf(); };
	void displayView(bool doShow) const { _ui.display(doShow); };
	std::vector<size_t> getCheckedIndexes() const { return _ui.getCheckedIndexes(); };
	long getSelectedIndex() const { return _ui.getSelectedIndex(); };
	void setSelection(int index) const { _ui.setSelection(index); };
	void initView(HINSTANCE hInst, HWND parent) { _ui.init(hInst, parent); };
	void addColumn(const columnInfo & column2Add) { _ui.addColumn(column2Add); };
	void reSizeView(RECT & rc) { _ui.reSizeTo(rc); }
	void setViewStyleOption(int32_t extraStyle) { _ui.setStyleOption(extraStyle); };
	size_t nbItem() const { return _ui.nbItem(); };
	PluginUpdateInfo* getPluginInfoFromIndex(int index) const { return reinterpret_cast<PluginUpdateInfo*>(_ui.getLParamFromIndex(index)); };
	PluginUpdateInfo* findPluginInfoFromFolderName(const generic_string& folderName, int& index) const;
	bool removeFromIndex(size_t index2remove);

private:
	std::vector<PluginUpdateInfo*> _list;
	ListView _ui;
};

class PluginsAdminDlg final : public StaticDialog
{
public :
	PluginsAdminDlg() {};
	~PluginsAdminDlg() {}
    void init(HINSTANCE hInst, HWND parent)	{
        Window::init(hInst, parent);
	};

	virtual void create(int dialogID, bool isRTL = false, bool msgDestParent = true);

    void doDialog(bool isRTL = false) {
    	if (!isCreated())
		{
			create(IDD_PLUGINSADMIN_DLG, isRTL);
		}

		if (!::IsWindowVisible(_hSelf))
		{

		}
	    display();
    };

	void switchDialog(int indexToSwitch);
	void setPluginsManager(PluginsManager *pluginsManager) { _pPluginsManager = pluginsManager; };

	bool updateListAndLoadFromJson();
	void setAdminMode(bool isAdm) { _nppCurrentStatus._isAdminMode = isAdm; };

	bool installPlugins();
	bool updatePlugins();
	bool removePlugins();

protected:
	virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

private :
	TabBar _tab;

	PluginViewList _availableList; // All plugins (pluginList.json) - installed plugins 
	PluginViewList _updateList;    // A list returned by gitup.exe
	PluginViewList _installedList; // for each installed plugin, check its json file

	PluginsManager *_pPluginsManager = nullptr;
	NppCurrentStatus _nppCurrentStatus;

	void collectNppCurrentStatusInfos();
	bool searchInPlugins(bool isNextMode) const;
	const bool _inNames = true;
	const bool _inDescs = false;
	bool isFoundInAvailableListFromIndex(int index, generic_string str2search, bool inWhichPart) const;
	long searchFromCurrentSel(generic_string str2search, bool inWhichPart, bool isNextMode) const;
	long searchInNamesFromCurrentSel(generic_string str2search, bool isNextMode) const {
		return searchFromCurrentSel(str2search, _inNames, isNextMode);
	};

	long searchInDescsFromCurrentSel(generic_string str2search, bool isNextMode) const {
		return searchFromCurrentSel(str2search, _inDescs, isNextMode);
	};

	bool loadFromPluginInfos();
	bool checkUpdates();
};

