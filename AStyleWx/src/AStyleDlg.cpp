// AStyleDlg.cpp
// Copyright (c) 2018 by Jim Pattee <jimp03@email.com>.
// This code is licensed under the MIT License.
// License.md describes the conditions under which this software may be distributed.

//-----------------------------------------------------------------------------
// headers
//-----------------------------------------------------------------------------

#include <wx/dcclient.h>
#include <wx/msgdlg.h>

#include "AStyleDlg.h"

/* ----------------------------------------------------------------------------
TO ADD A NEW OPTION

Add the option to the GUI using wxFormBuilder.
Add the option to Get...Options.
Add the option to Set...Options.
Add the option to Display...Options in AStyleDisplay.cpp.

Update the AStyleIFace and Config classes.
---------------------------------------------------------------------------- */

//-----------------------------------------------------------------------------
// AStyleDlg class
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(AStyleDlg, AStyleDlgBase)
	// events are defined in AStyleDlgBase by wxFormBuilder
END_EVENT_TABLE()

AStyleDlg::AStyleDlg(ASFrame* frame, AStyleIFace* astyle, int page)
	: AStyleDlgBase{frame}, m_frame{frame}, m_astyle{astyle}, m_page{page}
// Set the GUI controls from the AStyleIFace object.
// The AStyleIFace object is updated when the OK button is pressed.
{
	m_display = new AStyleDisplay(m_useTabLength,
	                              m_attachClass,
	                              m_attachInline,
	                              m_indentSwitchBlocks,
	                              m_indentCaseBlocks,
	                              m_breakAfterLogical);
	// set static box sizer font to bold
	wxFont defaultFont = m_styleSizer->GetStaticBox()->GetFont();
	defaultFont.SetWeight(wxFONTWEIGHT_BOLD);
	InitializeStaticBoxes(defaultFont, m_styleSizer, m_stylePreviewSizer);
	InitializeStaticBoxes(defaultFont, m_tabSizer, m_tabPreviewSizer);
	InitializeStaticBoxes(defaultFont, m_modifySizer, nullptr);
	InitializeStaticBoxes(defaultFont, m_indentSizer, m_indentPreviewSizer);
	InitializeStaticBoxes(defaultFont, m_padSizer, m_padPreviewSizer);
	InitializeStaticBoxes(defaultFont, m_formatSizer, m_formatPreviewSizer);
	InitializeStaticBoxes(defaultFont, m_otherSizer, m_otherPreviewSizer);
	// set default settings for preview styled text control
	InitializeSTC(m_stylePreviewSTC);
	InitializeSTC(m_tabPreviewSTC);
	InitializeSTC(m_indentPreviewSTC);
	InitializeSTC(m_padPreviewSTC);
	InitializeSTC(m_formatPreviewSTC);
	InitializeSTC(m_otherPreviewSTC);
	// wxWidgets 3.0 needs this because Bind() doesn't work with all compilers
	// this not the case with wxWidgets 3.1
	// Connect() will work if the feature is needed
	// the definition is in wxWidgets include/wx/features.h
#ifdef wxHAS_EVENT_BIND
	// set radio buttons to display preview on left mouse up
	// must use bind because the event doesn't propagate
	m_styleNone->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleAllman->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleJava->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleKR->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleStroustrup->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleWhitesmith->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleVtk->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleRatliff->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleGnu->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleLinux->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleHorstmann->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_style1tbs->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleGoogle->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleMozilla->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleWebKit->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_stylePico->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_styleLisp->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnStyleMouseUp, this);
	m_spaceIndent->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnTabMouseUp, this);
	m_tabIndent->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnTabMouseUp, this);
	m_forceTabIndent->Bind(wxEVT_LEFT_UP, &AStyleDlg::OnTabMouseUp, this);
	// linux will set focus to the read-only textbox, which isn't wanted
	// must use bind because the event doesn't propagate
	m_stylePreviewSTC->Bind(wxEVT_SET_FOCUS, &AStyleDlg::OnPreviewFocus, this);
	m_tabPreviewSTC->Bind(wxEVT_SET_FOCUS, &AStyleDlg::OnPreviewFocus, this);
	m_indentPreviewSTC->Bind(wxEVT_SET_FOCUS, &AStyleDlg::OnPreviewFocus, this);
	m_padPreviewSTC->Bind(wxEVT_SET_FOCUS, &AStyleDlg::OnPreviewFocus, this);
	m_formatPreviewSTC->Bind(wxEVT_SET_FOCUS, &AStyleDlg::OnPreviewFocus, this);
	m_otherPreviewSTC->Bind(wxEVT_SET_FOCUS, &AStyleDlg::OnPreviewFocus, this);
#endif	// wxHAS_EVENT_BIND
	// set size of spin controls
	// NOTE: there does not seem to be a way to resize spin controls
	// NOTE: the following does not work
	wxSize size = m_indentLength->GetSize();
	size.x = size.x / 2;
	m_tabLength->SetSize(size);
	m_indentLength->SetSize(size);
	m_indentContinuationLength->SetSize(size);
	// set notebook page to the page previously open
	int pages = static_cast<int>(m_notebook->GetPageCount());
	if (m_page >= pages)
		m_page = 0;
	m_notebook->ChangeSelection(m_page);
	m_notebook->SetFocus();
	// update the notebook panels
	SetStyleOptions();
	SetModifierOptions();
	SetTabOptions();
	SetIndentOptions();
	SetPadOptions();
	SetFormatOptions();
	SetOtherOptions();
	// set the default sdb sizer button
	m_sdbSizer->GetAffirmativeButton()->SetDefault();

	// adjust for changes
	this->Layout();
	m_astyleDlgSizer->Fit(this);
	this->Centre(wxBOTH);
}

AStyleDlg::~AStyleDlg()
{
	// Remove the added tab without deleting the notebook page.
	if (m_notebook->GetPageText(3) == "Brace Modify")
		m_notebook->RemovePage(3);
	else
		m_notebook->RemovePage(2);
	delete m_display;
}

void AStyleDlg::AddAlignPointerData()
// Add selections to the alignPointer combobox.
{
	// This method is much faster than appending items one by one.
	wxArrayString alignPointerItem;
	if (alignPointerItem.GetCount() == 0)
	{
		alignPointerItem.Add(" Do not change");
		alignPointerItem.Add(" Attach to type");
		alignPointerItem.Add(" Align in middle");
		alignPointerItem.Add(" Attach to name");
	}
	void* alignPointerData[] =
	{
		reinterpret_cast<void*>(PTR_ALIGN_NONE),
		reinterpret_cast<void*>(PTR_ALIGN_TYPE),
		reinterpret_cast<void*>(PTR_ALIGN_MIDDLE),
		reinterpret_cast<void*>(PTR_ALIGN_NAME)
	};
	assert(alignPointerItem.GetCount() ==
	       sizeof(alignPointerData) / sizeof(alignPointerData[0]));
	m_alignPointer->Set(alignPointerItem, alignPointerData);
}

void AStyleDlg::AddAlignReferenceData()
// Add selections to the alignReference combobox.
{
	// This method is much faster than appending items one by one.
	wxArrayString alignReferenceItem;
	if (alignReferenceItem.GetCount() == 0)
	{
		alignReferenceItem.Add(" Same as pointer");
		alignReferenceItem.Add(" Do not change");
		alignReferenceItem.Add(" Attach to type");
		alignReferenceItem.Add(" Align in middle");
		alignReferenceItem.Add(" Attach to name");
	}
	void* alignReferenceData[] =
	{
		reinterpret_cast<void*>(REF_SAME_AS_PTR),
		reinterpret_cast<void*>(REF_ALIGN_NONE),
		reinterpret_cast<void*>(REF_ALIGN_TYPE),
		reinterpret_cast<void*>(REF_ALIGN_MIDDLE),
		reinterpret_cast<void*>(REF_ALIGN_NAME)
	};
	assert(alignReferenceItem.GetCount() ==
	       sizeof(alignReferenceData) / sizeof(alignReferenceData[0]));
	m_alignReference->Set(alignReferenceItem, alignReferenceData);
}

void AStyleDlg::AddMaxCodeLengthData()
// Add selections to the maxCodelength combobox.
{
	// This method is much faster than appending items one by one.
	wxArrayString maxCodeLengthItem;
	if (maxCodeLengthItem.GetCount() == 0)
	{
		maxCodeLengthItem.Add(" Do not change ");
		maxCodeLengthItem.Add("  50 characters");
		maxCodeLengthItem.Add("  60 characters");
		maxCodeLengthItem.Add("  70 characters");
		maxCodeLengthItem.Add("  80 characters");
		maxCodeLengthItem.Add("  90 characters");
		maxCodeLengthItem.Add(" 100 characters");
		maxCodeLengthItem.Add(" 110 characters");
		maxCodeLengthItem.Add(" 120 characters");
		maxCodeLengthItem.Add(" 130 characters");
		maxCodeLengthItem.Add(" 140 characters");
		maxCodeLengthItem.Add(" 150 characters");
		maxCodeLengthItem.Add(" 160 characters");
		maxCodeLengthItem.Add(" 170 characters");
		maxCodeLengthItem.Add(" 180 characters");
		maxCodeLengthItem.Add(" 190 characters");
		maxCodeLengthItem.Add(" 200 characters");
	}
	void* maxCodeLengthData[] =
	{
		reinterpret_cast<void*>(0),
		reinterpret_cast<void*>(50),
		reinterpret_cast<void*>(60),
		reinterpret_cast<void*>(70),
		reinterpret_cast<void*>(80),
		reinterpret_cast<void*>(90),
		reinterpret_cast<void*>(100),
		reinterpret_cast<void*>(110),
		reinterpret_cast<void*>(120),
		reinterpret_cast<void*>(130),
		reinterpret_cast<void*>(140),
		reinterpret_cast<void*>(150),
		reinterpret_cast<void*>(160),
		reinterpret_cast<void*>(170),
		reinterpret_cast<void*>(180),
		reinterpret_cast<void*>(190),
		reinterpret_cast<void*>(200),
	};
	assert(maxCodeLengthItem.GetCount() ==
	       sizeof(maxCodeLengthData) / sizeof(maxCodeLengthData[0]));
	m_maxCodeLength->Set(maxCodeLengthItem, maxCodeLengthData);
}

void AStyleDlg::AddMaxContinuationData()
// Add selections to the maxContinuation combobox.
{
	// This method is much faster than appending items one by one.
	wxArrayString maxContinuationItem;
	if (maxContinuationItem.GetCount() == 0)
	{
		maxContinuationItem.Add("  40 spaces");
		maxContinuationItem.Add("  50 spaces");
		maxContinuationItem.Add("  60 spaces");
		maxContinuationItem.Add("  70 spaces");
		maxContinuationItem.Add("  80 spaces");
		maxContinuationItem.Add("  90 spaces");
		maxContinuationItem.Add(" 100 spaces");
		maxContinuationItem.Add(" 110 spaces");
		maxContinuationItem.Add(" 120 spaces");
	}
	void* maxContinuationData[] =
	{
		reinterpret_cast<void*>(40),
		reinterpret_cast<void*>(50),
		reinterpret_cast<void*>(60),
		reinterpret_cast<void*>(70),
		reinterpret_cast<void*>(80),
		reinterpret_cast<void*>(90),
		reinterpret_cast<void*>(100),
		reinterpret_cast<void*>(110),
		reinterpret_cast<void*>(120),
	};
	assert(maxContinuationItem.GetCount() ==
	       sizeof(maxContinuationData) / sizeof(maxContinuationData[0]));
	m_maxContinuation->Set(maxContinuationItem, maxContinuationData);
}

void AStyleDlg::AddMinConditionalData()
// Add selections to the minConditional combobox.
{
	// This method is much faster than appending items one by one.
	wxArrayString minConditionalItem;
	if (minConditionalItem.GetCount() == 0)
	{
		minConditionalItem.Add(" Zero indents");
		minConditionalItem.Add(" One indent");
		minConditionalItem.Add(" Two indents");
		minConditionalItem.Add(" One-half indent");
	}
	void* minConditionalData[] =
	{
		reinterpret_cast<void*>(MINCOND_ZERO),
		reinterpret_cast<void*>(MINCOND_ONE),
		reinterpret_cast<void*>(MINCOND_TWO),
		reinterpret_cast<void*>(MINCOND_ONEHALF)
	};
	assert(minConditionalItem.GetCount() ==
	       sizeof(minConditionalData) / sizeof(minConditionalData[0]));
	m_minConditional->Set(minConditionalItem, minConditionalData);
}

void AStyleDlg::AddPadMethodColonData()
// Add selections to the padMethodColon combobox.
{
	// This method is much faster than appending items one by one.
	wxArrayString padMethodColonItem;
	if (padMethodColonItem.GetCount() == 0)
	{
		padMethodColonItem.Add(" Do not change");
		padMethodColonItem.Add(" Do not pad");
		padMethodColonItem.Add(" Before and after");
		padMethodColonItem.Add(" After only");
		padMethodColonItem.Add(" Before only");
	}
	void* padMethodColonData[] =
	{
		reinterpret_cast<void*>(COLON_PAD_NO_CHANGE),
		reinterpret_cast<void*>(COLON_PAD_NONE),
		reinterpret_cast<void*>(COLON_PAD_ALL),
		reinterpret_cast<void*>(COLON_PAD_AFTER),
		reinterpret_cast<void*>(COLON_PAD_BEFORE)
	};
	assert(padMethodColonItem.GetCount() ==
	       sizeof(padMethodColonData) / sizeof(padMethodColonData[0]));
	m_padMethodColon->Set(padMethodColonItem, padMethodColonData);
}

void AStyleDlg::EnableOrDisableTabLength()
// Enable m_useTabLength and m_tabLength if all the conditions are met.
{
	// set tab indent options only if indent=force-tab
	if (m_forceTabIndent->GetValue())
	{
		m_useTabLength->Enable();
		m_tabLength->Enable(m_useTabLength->IsChecked());
	}
	else
	{
		m_useTabLength->Disable();
		m_tabLength->Disable();
	}
}

void AStyleDlg::GetFormatOptions(AStyleIFace* astyle)
// get format control values and update astyle
// is used for both the config options and the current dialog settings
{
	astyle->SetBreakClosingBraces(m_breakClosingBraces->GetValue());
	astyle->SetBreakElseIfs(m_breakElseIf->GetValue());
	astyle->SetBreakOneLineHeaders(m_breakOneLineHeaders->GetValue());
	astyle->SetAddBraces(m_addBraces->GetValue());
	astyle->SetAddOneLineBraces(m_addOneLineBraces->GetValue());
	astyle->SetRemoveBraces(m_removeBraces->GetValue());
	astyle->SetBreakReturnType(m_breakReturnType->GetValue());
	astyle->SetBreakReturnTypeDecl(m_breakReturnTypeDecl->GetValue());
	astyle->SetAttachReturnType(m_attachReturnType->GetValue());
	astyle->SetAttachReturnTypeDecl(m_attachReturnTypeDecl->GetValue());
	astyle->SetBreakOneLineBlocks(!m_keepOneLineBlocks->GetValue());      // break == ! keep
	astyle->SetBreakOneLineStmts(!m_keepOneLineStatements->GetValue());   // break == ! keep
	astyle->SetConvertTabs(m_convertTabs->GetValue());
	astyle->SetCloseTemplates(m_closeTemplates->GetValue());
	astyle->SetRemoveCommentPrefix(m_removeCommentPrefix->GetValue());
	// get max code length option
	int maxIndex = m_maxCodeLength->GetCurrentSelection();
	size_t maxCodeLength = reinterpret_cast<size_t>(m_maxCodeLength->GetClientData(maxIndex));
	astyle->SetMaxCodeLength(static_cast<int>(maxCodeLength));
	if (astyle->GetMaxCodeLength() == 0)
		m_breakAfterLogical->SetValue(false);
	// end max code length
	astyle->SetBreakAfterLogical(m_breakAfterLogical->GetValue());
}

void AStyleDlg::GetIndentOptions(AStyleIFace* astyle)
// get indent control values and update astyle
// is used for both the config options and the current dialog settings
{
	astyle->SetClassIndent(m_indentClassBlocks->GetValue());
	astyle->SetModifierIndent(m_indentAccessModifiers->GetValue());
	astyle->SetSwitchIndent(m_indentSwitchBlocks->GetValue());
	astyle->SetCaseIndent(m_indentCaseBlocks->GetValue());
	astyle->SetNamespaceIndent(m_indentNamespaceBlocks->GetValue());
	astyle->SetAfterParenIndent(m_indentAfterParens->GetValue());
	if (m_indentContinuation->IsChecked()
	        && m_indentContinuationLength->GetValue() != astyle->GetDefaultContinuationIndent())
		astyle->SetContinuationIndent(m_indentContinuationLength->GetValue());
	else
		astyle->SetContinuationIndent(1);
	astyle->SetLabelIndent(m_indentGotoLabels->GetValue());
	astyle->SetPreprocBlockIndent(m_indentPreprocBlock->GetValue());
	astyle->SetPreprocDefineIndent(m_indentPreprocDefine->GetValue());
	astyle->SetPreprocCondIndent(m_indentPreprocConditional->GetValue());
	astyle->SetCol1CommentIndent(m_indentCol1comments->GetValue());
	// get min conditional option
	int minIndex = static_cast<int>(m_minConditional->GetCurrentSelection());
	size_t minOption = reinterpret_cast<size_t>(m_minConditional->GetClientData(minIndex));
	astyle->SetMinConditionalOption(static_cast<MinConditional>(minOption));
	// get max in-statement value
	int maxIndex = static_cast<int>(m_maxContinuation->GetCurrentSelection());
	size_t maxOption = reinterpret_cast<size_t>(m_maxContinuation->GetClientData(maxIndex));
	astyle->SetMaxContinuationIndent(static_cast<int>(maxOption));
}

void AStyleDlg::GetModifierOptions(AStyleIFace* astyle)
// get brace modifier control values and update astyle
// is used for both the config options and the current dialog settings
{
	astyle->SetAttachNamespace(m_attachNamespace->GetValue());
	astyle->SetAttachClass(m_attachClass->GetValue());
	astyle->SetAttachInline(m_attachInline->GetValue());
	astyle->SetAttachExternC(m_attachExternC->GetValue());
	astyle->SetAttachClosingWhile(m_attachClosingWhile->GetValue());
}

void AStyleDlg::GetOtherOptions(AStyleIFace* astyle)
// get other control values and update astyle
// is used for both the config options and the current dialog settings
{
	astyle->SetPadMethodPrefix(m_padMethodPrefix->GetValue());
	astyle->SetUnpadMethodPrefix(m_unpadMethodPrefix->GetValue());
	astyle->SetPadReturnType(m_padReturnType->GetValue());
	astyle->SetUnpadReturnType(m_unpadReturnType->GetValue());
	astyle->SetPadParamType(m_padParamType->GetValue());
	astyle->SetUnpadParamType(m_unpadParamType->GetValue());
	astyle->SetAlignMethodColon(m_alignMethodColon->GetValue());
	int padColonIndex = static_cast<int>(m_padMethodColon->GetCurrentSelection());
	size_t padMethodColon = reinterpret_cast<size_t>(
	                            m_padMethodColon->GetClientData(padColonIndex));
	astyle->SetPadMethodColon(static_cast<int>(padMethodColon));
}

void AStyleDlg::GetPadOptions(AStyleIFace* astyle)
// get pad control values and update astyle
// is used for both the config options and the current dialog settings
{
	astyle->SetBreakHeaderBlocks(m_breakHeaderBlocks->GetValue());
	astyle->SetBreakClosingBlocks(false);
	if (m_breakAllBlocks->GetValue())
	{
		astyle->SetBreakHeaderBlocks(true);
		astyle->SetBreakClosingBlocks(true);
	}
	astyle->SetPadOperator(m_padOperator->GetValue());
	astyle->SetPadComma(m_padComma->GetValue());
	astyle->SetPadParenOutside(m_padParenOutside->GetValue());
	astyle->SetPadParenInside(m_padParenInside->GetValue());
	astyle->SetPadFirstParenOut(m_padFirstParenOut->GetValue());
	astyle->SetPadHeader(m_padHeader->GetValue());
	astyle->SetUnpadParen(m_unpadParen->GetValue());
	astyle->SetDeleteEmptyLines(m_deleteEmptyLines->GetValue());
	astyle->SetFillEmptyLines(m_fillEmptyLines->GetValue());
	// get align pointer option
	int ptrIndex = static_cast<int>(m_alignPointer->GetCurrentSelection());
	size_t ptrOption = reinterpret_cast<size_t>(m_alignPointer->GetClientData(ptrIndex));
	astyle->SetAlignPointer(static_cast<PointerAlign>(ptrOption));
	// get align reference option
	int refIndex = static_cast<int>(m_alignReference->GetCurrentSelection());
	size_t refOption = reinterpret_cast<size_t>(m_alignReference->GetClientData(refIndex));
	astyle->SetAlignReference(static_cast<ReferenceAlign>(refOption));
}

int  AStyleDlg::GetPage()
{
	return m_page;
}

void AStyleDlg::GetStyleOptions(AStyleIFace* astyle)
// get brace style control values and update astyle
// is used for both the config options and the current dialog settings
{
	if (m_styleNone->GetValue())
		astyle->SetBraceStyle(STYLE_NONE);
	else if (m_styleAllman->GetValue())
		astyle->SetBraceStyle(STYLE_ALLMAN);
	else if (m_styleJava->GetValue())
		astyle->SetBraceStyle(STYLE_JAVA);
	else if (m_styleKR->GetValue())
		astyle->SetBraceStyle(STYLE_KR);
	else if (m_styleStroustrup->GetValue())
		astyle->SetBraceStyle(STYLE_STROUSTRUP);
	else if (m_styleWhitesmith->GetValue())
		astyle->SetBraceStyle(STYLE_WHITESMITH);
	else if (m_styleVtk->GetValue())
		astyle->SetBraceStyle(STYLE_VTK);
	else if (m_styleRatliff->GetValue())
		astyle->SetBraceStyle(STYLE_RATLIFF);
	else if (m_styleGnu->GetValue())
		astyle->SetBraceStyle(STYLE_GNU);
	else if (m_styleLinux->GetValue())
		astyle->SetBraceStyle(STYLE_LINUX);
	else if (m_styleHorstmann->GetValue())
		astyle->SetBraceStyle(STYLE_HORSTMANN);
	else if (m_style1tbs->GetValue())
		astyle->SetBraceStyle(STYLE_1TBS);
	else if (m_styleGoogle->GetValue())
		astyle->SetBraceStyle(STYLE_GOOGLE);
	else if (m_styleMozilla->GetValue())
		astyle->SetBraceStyle(STYLE_MOZILLA);
	else if (m_styleWebKit->GetValue())
		astyle->SetBraceStyle(STYLE_WEBKIT);
	else if (m_stylePico->GetValue())
		astyle->SetBraceStyle(STYLE_PICO);
	else if (m_styleLisp->GetValue())
		astyle->SetBraceStyle(STYLE_LISP);
}

void AStyleDlg::GetTabOptions(AStyleIFace* astyle)
// get brace indent values and update astyle
// is used for both the config options and the current dialog settings
{
	if (m_spaceIndent->GetValue())
		astyle->SetIndentType(INDENT_SPACES);
	else if (m_tabIndent->GetValue())
		astyle->SetIndentType(INDENT_TABS);
	else if (m_forceTabIndent->GetValue())
		astyle->SetIndentType(INDENT_FTABS);
	// set indent length value
	astyle->SetIndentLength(m_indentLength->GetValue());
	// set tab length value
	astyle->SetUseTabLength(m_useTabLength->GetValue());
	astyle->SetTabLength(m_tabLength->GetValue());
}

wxSize AStyleDlg::GetWxChoiceSize(const wxChoice* choice) const
// get the initial size of a wxChoice control
// the size is large enough to display the longest string
{
	// find the longest text
	size_t longLabel = 0;
	size_t longIndex = 0;
	for (size_t i = 0; i < choice->GetCount(); i++)
	{
		wxString currentItem = choice->GetString(static_cast<unsigned int>(i));
		if (currentItem.Length() > longLabel)
		{
			longLabel = currentItem.Length();
			longIndex = i;
		}
	}
	wxSize szText;
	// the extra characters are for KDE and macOS
	wxString longestText = choice->GetString(static_cast<unsigned int>(longIndex)) + "XX";
	choice->GetTextExtent(longestText, &szText.x, &szText.y);
	wxSize szReturn = choice->GetSizeFromTextSize(szText);
	return szReturn;
}

wxSize AStyleDlg::GetWxSpinCtrlSize(const wxSpinCtrl* spin) const
// get the initial size of a wxSpin control
{
	wxSize szText;
	spin->GetTextExtent("XXX", &szText.x, &szText.y);
	wxSize szReturn = spin->GetSizeFromTextSize(szText);
	return szReturn;
}

void AStyleDlg::InitializeStaticBoxes(wxFont& defaultFont,
                                      wxStaticBoxSizer* sizer,
                                      wxStaticBoxSizer* previewSizer)
{
#ifdef __WXOSX__
	// remove the text
	sizer->GetStaticBox()->SetFont(defaultFont);
	sizer->GetStaticBox()->SetLabel(wxEmptyString);
	if (previewSizer != nullptr)
		previewSizer->GetStaticBox()->SetLabel(wxEmptyString);
#else
	// set the new font
	sizer->GetStaticBox()->SetFont(defaultFont);
	if (previewSizer != nullptr)
		previewSizer->GetStaticBox()->SetFont(defaultFont);
	if (sizer == m_tabSizer)
		m_indentLengthText->SetFont(defaultFont);
#endif
}

void AStyleDlg::InitializeSTC(wxStyledTextCtrl* stc)
{
	stc->Disable();
	stc->SetSTCCursor(wxCURSOR_NONE);		// for OSX display
	// get system default GUI font
	wxFont stcFont(*wxNORMAL_FONT);
	int size = stcFont.GetPointSize();
	size++;
	stcFont.SetPointSize(size);
	// set font for the window
	stc->SetFont(stcFont);
	stc->StyleSetSize(wxSTC_STYLE_DEFAULT, size);
	stc->StyleClearAll();
	// set wxStyledTextCtrl options
	stc->SetMarginWidth(0, 0);
	stc->SetMarginWidth(1, 0);
	stc->SetMarginWidth(2, 0);
	stc->SetMarginWidth(3, 0);
	stc->SetMarginWidth(4, 0);
	if (stc->GetMarginType(0) != wxSTC_MARGIN_NUMBER)
		stc->SetMarginWidth(0, 16);
	else
		stc->SetMarginWidth(1, 16);
	stc->SetUseHorizontalScrollBar(false);
	stc->SetWrapMode(0);
	stc->SetIndentationGuides(0);
	// stc has a custom context menu
#if wxCHECK_VERSION(3, 1, 1)
	stc->UsePopUp(wxSTC_POPUP_NEVER);
#else
	stc->UsePopUp(false);
#endif
	stc->SetSelBackground(true, wxColour(*wxWHITE));
	stc->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour(*wxWHITE));
}

void AStyleDlg::OnFormatClick(wxCommandEvent& event)
// a format option has been clicked
{
	m_display->DisplayFormatOptions(event, m_formatPreviewSTC);
}

void AStyleDlg::OnIndentClick(wxCommandEvent& event)
// an indent option has been clicked
{
	if (m_indentContinuation->IsChecked())
		m_indentContinuationLength->Enable();
	else
		m_indentContinuationLength->Disable();
	m_display->DisplayIndentOptions(event, m_indentPreviewSTC);
}

void AStyleDlg::OnModifierClick(wxCommandEvent& event)
// a brace modifier option has been clicked
{
	m_display->DisplayModifierOptions(event, m_tabPreviewSTC);
}

void AStyleDlg::OnNotebookPageChanged(wxBOOKCTRL_EVENT&)
// A predefined book control event handler function.
// May be a Choicebook, Listbook, Notebook, or Treebook.
// Called when the page selection is changed.
{
	m_page = m_notebook->GetSelection();
}

void AStyleDlg::OnOkClick(wxCommandEvent&)
// update the astyle object and exit
{
	GetStyleOptions(m_astyle);
	GetModifierOptions(m_astyle);
	GetTabOptions(m_astyle);
	GetIndentOptions(m_astyle);
	GetPadOptions(m_astyle);
	GetFormatOptions(m_astyle);
	GetOtherOptions(m_astyle);
	EndModal(wxID_OK);
}

void AStyleDlg::OnOtherClick(wxCommandEvent& event)
// an other option has been clicked
{
	m_display->DisplayOtherOptions(event, m_otherPreviewSTC);
}

void AStyleDlg::OnPadClick(wxCommandEvent& event)
// a pad option has been clicked
{
	m_display->DisplayPadOptions(event, m_padPreviewSTC);
}

void AStyleDlg::OnPreviewFocus(wxFocusEvent& event)
// Linux focuses on the read-only textbox, which isn't wanted
{
	// do not use dynamic_cast with -no-rtti
	wxWindow* previewWindow = static_cast<wxWindow*>(event.GetEventObject());
	previewWindow->Navigate();
	// do NOT Skip
}

void AStyleDlg::OnResetAllClick(wxCommandEvent&)
// reset all controls to the default value
{
	int reply = wxMessageBox("Do you want to replace all current options with the default options?",
	                         "Set Default Options",
	                         wxYES_NO | wxICON_QUESTION);
	if (reply == wxNO)
		return;
	// create an object with the default values
	AStyleIFace astyleDefault;
	*m_astyle = astyleDefault;
	// radio buttons on Windows must be processed like this
	m_notebook->Freeze();
	int currPage = m_notebook->GetSelection();
	int stylePage = 0;
	int tabPage = 1;
	if (m_notebook->GetPageText(0).Contains("Artistic Style"))
	{
		// adjust page for wxTreebook
		++stylePage;
		++tabPage;
	}
	m_notebook->SetSelection(stylePage);
	assert(m_notebook->GetPageText(stylePage).Contains("Style"));
	m_styleNone->SetValue(true);
	m_notebook->SetSelection(tabPage);
	assert(m_notebook->GetPageText(tabPage).Contains("Tabs"));
	m_spaceIndent->SetValue(true);
	m_notebook->SetSelection(currPage);
	m_notebook->Thaw();
	// update the display
	SetStyleOptions();
	SetModifierOptions();
	SetTabOptions();
	SetIndentOptions();
	SetPadOptions();
	SetFormatOptions();
	SetOtherOptions();
	// clear the preview text
	m_stylePreviewSTC->ClearAll();
	m_tabPreviewSTC->ClearAll();
	m_indentPreviewSTC->ClearAll();
	m_padPreviewSTC->ClearAll();
	m_formatPreviewSTC->ClearAll();
	m_otherPreviewSTC->ClearAll();
}

void AStyleDlg::OnSettingsClick(wxCommandEvent&)
{
	// create a temporary object with the current dialog settings
	AStyleIFace astyleTemp;
	GetStyleOptions(&astyleTemp);
	GetModifierOptions(&astyleTemp);
	GetTabOptions(&astyleTemp);
	GetIndentOptions(&astyleTemp);
	GetPadOptions(&astyleTemp);
	GetFormatOptions(&astyleTemp);
	GetOtherOptions(&astyleTemp);
	// show the dialog
	SettingsDlg dialog(this, m_frame, &astyleTemp);
	dialog.SetTitle(" Settings");
	dialog.SetIcons(m_frame->GetIconBundle());
	dialog.Centre();
	dialog.ShowModal();
}

void AStyleDlg::OnStyleClick(wxCommandEvent& event)
// A style option has been clicked with the mouse or selected with the arrow key.
// This will display the preview.
{
	wxCommandEvent commandEvent(wxEVT_COMMAND_RADIOBUTTON_SELECTED, event.GetId());
	m_display->DisplayStyleOptions(commandEvent, m_stylePreviewSTC);
}

void AStyleDlg::OnStyleMouseUp(wxMouseEvent& mouseEvent)
// A style option has been clicked and the preview is empty.
// This will display the preview by clicking a button that is already selected.
{
	if (m_stylePreviewSTC->IsEmpty())
	{
		wxCommandEvent commandEvent(wxEVT_COMMAND_RADIOBUTTON_SELECTED, mouseEvent.GetId());
		m_display->DisplayStyleOptions(commandEvent, m_stylePreviewSTC);
	}
	mouseEvent.Skip();	// must have this
}

void AStyleDlg::OnTabClick(wxCommandEvent& event)
// A tab option has been clicked with the mouse or selected with the arrow key.
// This will display the preview.
{
	EnableOrDisableTabLength();
	m_display->DisplayTabOptions(event, m_tabPreviewSTC);
}

void AStyleDlg::OnTabMouseUp(wxMouseEvent& mouseEvent)
// A tab option has been clicked and the preview is empty.
// This will display the preview by clicking a button that is already selected.
{
	if (m_tabPreviewSTC->IsEmpty())
	{
		wxCommandEvent commandEvent(wxEVT_COMMAND_RADIOBUTTON_SELECTED, mouseEvent.GetId());
		m_display->DisplayTabOptions(commandEvent, m_tabPreviewSTC);
	}
	mouseEvent.Skip();	// must have this
}

void AStyleDlg::SetFormatOptions()
// set format controls from the current astyle values
{
	m_breakClosingBraces->SetValue(m_astyle->GetBreakClosingBraces());
	m_breakElseIf->SetValue(m_astyle->GetBreakElseIfs());
	m_breakOneLineHeaders->SetValue(m_astyle->GetBreakOneLineHeaders());
	m_addBraces->SetValue(m_astyle->GetAddBraces());
	m_addOneLineBraces->SetValue(m_astyle->GetAddOneLineBraces());
	m_removeBraces->SetValue(m_astyle->GetRemoveBraces());
	m_breakReturnType->SetValue(m_astyle->GetBreakReturnType());
	m_breakReturnTypeDecl->SetValue(m_astyle->GetBreakReturnTypeDecl());
	m_attachReturnType->SetValue(m_astyle->GetAttachReturnType());
	m_attachReturnTypeDecl->SetValue(m_astyle->GetAttachReturnTypeDecl());
	m_keepOneLineBlocks->SetValue(!m_astyle->GetBreakOneLineBlocks());		// keep == ! break
	m_keepOneLineStatements->SetValue(!m_astyle->GetBreakOneLineStmts());	// keep == ! break
	m_convertTabs->SetValue(m_astyle->GetConvertTabs());
	m_closeTemplates->SetValue(m_astyle->GetCloseTemplates());
	m_removeCommentPrefix->SetValue(m_astyle->GetRemoveCommentPrefix());
	// set max code length choice box
	if (m_maxCodeLength->GetCount() == 0)
		AddMaxCodeLengthData();
	// set max code length value
	size_t maxCodeLength = m_astyle->GetMaxCodeLength();
	size_t iMax;
	for (iMax = 0; iMax < m_maxCodeLength->GetCount(); iMax++)
	{
		void* clientDataAsPtr = m_maxCodeLength->GetClientData(static_cast<unsigned int>(iMax));
		size_t clientData = reinterpret_cast<size_t>(clientDataAsPtr);
		if (clientData == maxCodeLength)
			break;
	}
	m_maxCodeLength->SetSelection(static_cast<int>(iMax));
	// end max code length
	m_breakAfterLogical->SetValue(m_astyle->GetBreakAfterLogical());
	// set initial size of the wxChoice
	wxSize sz = GetWxChoiceSize(m_maxCodeLength);
	m_maxCodeLength->SetInitialSize(sz);
}

void AStyleDlg::SetIndentOptions()
// set indent controls from the current astyle values
{
	m_indentClassBlocks->SetValue(m_astyle->GetClassIndent());
	m_indentAccessModifiers->SetValue(m_astyle->GetModifierIndent());
	m_indentSwitchBlocks->SetValue(m_astyle->GetSwitchIndent());
	m_indentCaseBlocks->SetValue(m_astyle->GetCaseIndent());
	m_indentNamespaceBlocks->SetValue(m_astyle->GetNamespaceIndent());
	m_indentAfterParens->SetValue(m_astyle->GetAfterParenIndent());
	m_indentGotoLabels->SetValue(m_astyle->GetLabelIndent());
	m_indentPreprocBlock->SetValue(m_astyle->GetPreprocBlockIndent());
	m_indentPreprocDefine->SetValue(m_astyle->GetPreprocDefineIndent());
	m_indentPreprocConditional->SetValue(m_astyle->GetPreprocCondIndent());
	m_indentCol1comments->SetValue(m_astyle->GetCol1CommentIndent());
	// set indent continuation length
	m_indentContinuationLength->SetRange(0, 4);
	wxSize continueSize = GetWxSpinCtrlSize(m_indentContinuationLength);
	m_indentContinuationLength->SetInitialSize(continueSize);
	int continuationIndent = m_astyle->GetContinuationIndent();
	m_indentContinuationLength->SetValue(continuationIndent);
	if (continuationIndent == m_astyle->GetDefaultContinuationIndent())
	{
		m_indentContinuation->SetValue(false);
		m_indentContinuationLength->Disable();
	}
	else
	{
		m_indentContinuation->SetValue(true);
		m_indentContinuationLength->Enable();
	}
	// set min conditional choice box
	if (m_minConditional->GetCount() == 0)
		AddMinConditionalData();
	// set min conditional option
	int minConditionalOption = m_astyle->GetMinConditionalOption();
	size_t iMin;
	for (iMin = 0; iMin < m_minConditional->GetCount(); iMin++)
	{
		size_t clientData = reinterpret_cast<size_t>(
		                        m_minConditional->GetClientData(static_cast<unsigned int>(iMin)));
		if (static_cast<int>(clientData) == minConditionalOption)
			break;
	}
	m_minConditional->SetSelection(minConditionalOption);
	// set max in-statement choice box
	if (m_maxContinuation->GetCount() == 0)
		AddMaxContinuationData();
	// set max continuation indent
	int maxContinuationIndent = m_astyle->GetMaxContinuationIndent();
	size_t iMax;
	for (iMax = 0; iMax < m_maxContinuation->GetCount(); iMax++)
	{
		size_t clientData = reinterpret_cast<size_t>(
		                        m_maxContinuation->GetClientData(static_cast<unsigned int>(iMax)));
		if (static_cast<int>(clientData) == maxContinuationIndent)
			break;
	}
	m_maxContinuation->SetSelection(static_cast<int>(iMax));
	// set initial size of the wxChoice, make them the same size
	wxSize szMinCond = GetWxChoiceSize(m_minConditional);
	wxSize szMaxCont = GetWxChoiceSize(m_maxContinuation);
	if (szMinCond.GetWidth() > szMaxCont.GetWidth())
		szMaxCont = szMinCond;
	else
		szMinCond = szMaxCont;
	m_minConditional->SetInitialSize(szMinCond);
	m_maxContinuation->SetInitialSize(szMaxCont);
}

void AStyleDlg::SetModifierOptions()
// set brace modifier controls from the current astyle values
{
	m_attachNamespace->SetValue(m_astyle->GetAttachNamespace());
	m_attachClass->SetValue(m_astyle->GetAttachClass());
	m_attachInline->SetValue(m_astyle->GetAttachInline());
	m_attachExternC->SetValue(m_astyle->GetAttachExternC());
	m_attachClosingWhile->SetValue(m_astyle->GetAttachClosingWhile());
}

void AStyleDlg::SetOtherOptions()
// set other controls from the current astyle values
{
	m_padMethodPrefix->SetValue(m_astyle->GetPadMethodPrefix());
	m_unpadMethodPrefix->SetValue(m_astyle->GetUnpadMethodPrefix());
	m_padReturnType->SetValue(m_astyle->GetPadReturnType());
	m_unpadReturnType->SetValue(m_astyle->GetUnpadReturnType());
	m_padParamType->SetValue(m_astyle->GetPadParamType());
	m_unpadParamType->SetValue(m_astyle->GetUnpadParamType());
	m_alignMethodColon->SetValue(m_astyle->GetAlignMethodColon());
	// set pad method colon choice box
	if (m_padMethodColon->GetCount() == 0)
		AddPadMethodColonData();
	// set pad method colon value
	int padMethodColon = m_astyle->GetPadMethodColon();
	size_t iPadColon;
	for (iPadColon = 0; iPadColon < m_padMethodColon->GetCount(); iPadColon++)
	{
		size_t clientData = reinterpret_cast<size_t>(
		                        m_padMethodColon->GetClientData(static_cast<unsigned int>(iPadColon)));
		if (static_cast<int>(clientData) == padMethodColon)
			break;
	}
	m_padMethodColon->SetSelection(static_cast<int>(iPadColon));
	// set initial size of the wxChoice
	wxSize sz = GetWxChoiceSize(m_padMethodColon);
	m_padMethodColon->SetInitialSize(sz);
}

void AStyleDlg::SetPadOptions()
// set pad controls from the current astyle values
{
	m_breakHeaderBlocks->SetValue(m_astyle->GetBreakHeaderBlocks());
	m_breakAllBlocks->SetValue(m_astyle->GetBreakClosingBlocks());
	if (m_breakAllBlocks->IsChecked())
		m_breakHeaderBlocks->SetValue(false);
	m_padOperator->SetValue(m_astyle->GetPadOperator());
	m_padComma->SetValue(m_astyle->GetPadComma());
	m_padParenOutside->SetValue(m_astyle->GetPadParenOutside());
	m_padParenInside->SetValue(m_astyle->GetPadParenInside());
	m_padFirstParenOut->SetValue(m_astyle->GetPadFirstParenOut());
	m_padHeader->SetValue(m_astyle->GetPadHeader());
	m_unpadParen->SetValue(m_astyle->GetUnpadParen());
	m_deleteEmptyLines->SetValue(m_astyle->GetDeleteEmptyLines());
	m_fillEmptyLines->SetValue(m_astyle->GetFillEmptyLines());
	// build align pointer choice box
	if (m_alignPointer->GetCount() == 0)
		AddAlignPointerData();
	// set align pointer option
	int alignPointer = m_astyle->GetAlignPointer();
	size_t iPtr;
	for (iPtr = 0; iPtr < m_alignPointer->GetCount(); iPtr++)
	{
		size_t clientData = reinterpret_cast<size_t>(
		                        m_alignPointer->GetClientData(static_cast<unsigned int>(iPtr)));
		if (static_cast<int>(clientData) == alignPointer)
			break;
	}
	m_alignPointer->SetSelection(static_cast<int>(iPtr));
	// build align reference choice box
	if (m_alignReference->GetCount() == 0)
		AddAlignReferenceData();
	// set align reference option
	int alignReference = m_astyle->GetAlignReference();
	size_t iRef;
	for (iRef = 0; iRef < m_alignReference->GetCount(); iRef++)
	{
		size_t clientData = reinterpret_cast<size_t>(
		                        m_alignReference->GetClientData(static_cast<unsigned int>(iRef)));
		if (static_cast<int>(clientData) == alignReference)
			break;
	}
	m_alignReference->SetSelection(static_cast<int>(iRef));
	// set initial size of the wxChoice, make them the same size
	wxSize szPtr = GetWxChoiceSize(m_alignPointer);
	wxSize szRef = GetWxChoiceSize(m_alignReference);
	if (szPtr.GetWidth() > szRef.GetWidth())
		szRef = szPtr;
	else
		szPtr = szRef;
	m_alignPointer->SetInitialSize(szPtr);
	m_alignReference->SetInitialSize(szRef);
}

void AStyleDlg::SetStyleOptions()
// set brace controls from the current astyle values
{
	int braceType = m_astyle->GetBraceStyle();
	if (braceType == STYLE_NONE)
		m_styleNone->SetValue(true);
	else if (braceType == STYLE_ALLMAN)
		m_styleAllman->SetValue(true);
	else if (braceType == STYLE_JAVA)
		m_styleJava->SetValue(true);
	else if (braceType == STYLE_KR)
		m_styleKR->SetValue(true);
	else if (braceType == STYLE_STROUSTRUP)
		m_styleStroustrup->SetValue(true);
	else if (braceType == STYLE_WHITESMITH)
		m_styleWhitesmith->SetValue(true);
	else if (braceType == STYLE_VTK)
		m_styleVtk->SetValue(true);
	else if (braceType == STYLE_RATLIFF)
		m_styleRatliff->SetValue(true);
	else if (braceType == STYLE_GNU)
		m_styleGnu->SetValue(true);
	else if (braceType == STYLE_LINUX)
		m_styleLinux->SetValue(true);
	else if (braceType == STYLE_HORSTMANN)
		m_styleHorstmann->SetValue(true);
	else if (braceType == STYLE_1TBS)
		m_style1tbs->SetValue(true);
	else if (braceType == STYLE_GOOGLE)
		m_styleGoogle->SetValue(true);
	else if (braceType == STYLE_MOZILLA)
		m_styleMozilla->SetValue(true);
	else if (braceType == STYLE_WEBKIT)
		m_styleWebKit->SetValue(true);
	else if (braceType == STYLE_PICO)
		m_stylePico->SetValue(true);
	else if (braceType == STYLE_LISP)
		m_styleLisp->SetValue(true);
}

void AStyleDlg::SetTabOptions()
// set tab controls from the current astyle values
{
	IndentType indentType = static_cast<IndentType>(m_astyle->GetIndentType());
	if (indentType == INDENT_SPACES)
		m_spaceIndent->SetValue(true);
	else if (indentType == INDENT_TABS)
		m_tabIndent->SetValue(true);
	else if (indentType == INDENT_FTABS)
		m_forceTabIndent->SetValue(true);
	else
	{
		wxMessageBox("Invalid value in AStyleDlg::SetTabOptions()", "Error");
		exit(EXIT_FAILURE);
	}
	// set indent length spin control values 2 - 20
	m_indentLength->SetRange(2, 20);
	m_indentLength->SetValue(m_astyle->GetIndentLength());
	wxSize indentSize = GetWxSpinCtrlSize(m_indentLength);
	m_indentLength->SetInitialSize(indentSize);
	// set tab length spin control values 2 - 20
	m_tabLength->SetRange(2, 20);
	m_useTabLength->SetValue(m_astyle->GetUseTabLength());
	wxSize tabSize = GetWxSpinCtrlSize(m_tabLength);
	m_tabLength->SetInitialSize(tabSize);
	if (m_astyle->GetUseTabLength())
		m_tabLength->SetValue(m_astyle->GetTabLength());
	else
		m_tabLength->SetValue(m_astyle->GetIndentLength() * 2);
	EnableOrDisableTabLength();
}
