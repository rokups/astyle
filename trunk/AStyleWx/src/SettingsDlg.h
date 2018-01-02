// SettingsDlg.h
// Copyright (c) 2018 by Jim Pattee <jimp03@email.com>.
// This code is licensed under the MIT License.
// License.md describes the conditions under which this software may be distributed.

#ifndef _SETTINGSDLG_H_
#define _SETTINGSDLG_H_

//-----------------------------------------------------------------------------
// headers
//-----------------------------------------------------------------------------

#include "AStyleWx.h"
#include "SettingsDlgBase.h"

//-----------------------------------------------------------------------------
// SettingsDlg Class
// The inherited SettingsDlgBase class is generated by wsFormBuilder
//-----------------------------------------------------------------------------

class SettingsDlg: public SettingsDlgBase
{
public:
	SettingsDlg(wxWindow* parent, ASFrame* frame, AStyleIFace* astyle);
	~SettingsDlg();
};

#endif	// _SETTINGSDLG_H_
