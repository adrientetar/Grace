/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <wx/wx.h>

#include <wx/stc/stc.h>

wxDECLARE_EVENT(STC_STATUS_CHANGED, wxCommandEvent);

class Editor : public wxStyledTextCtrl {
public:
    Editor(wxWindow* parent);

private:
    void DoSetFoldLevels(unsigned fromPos, int startLevel, wxString& text);
    void DoSetStyling(unsigned fromPos, unsigned toPos, wxString &text);
    void OnMarginClick(wxStyledTextEvent& event);
    void OnModified(wxStyledTextEvent& event);
    void OnStyleNeeded(wxStyledTextEvent& event);

    bool modified_;
};
