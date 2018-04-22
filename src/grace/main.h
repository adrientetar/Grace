/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <wx/wx.h>

#include <wx/app.h>

#include "editor.h"
#include "property.h"
#include "sidebar.h"

class App : public wxApp
{
public:
    App();
    virtual bool OnInit();
    //virtual int OnExit();
};

DECLARE_APP(App);

class MainFrame : public wxFrame {
    friend class App;
public:
    MainFrame(const wxString& title);
    ~MainFrame();
    bool DoSave(bool forceSaveAs = false);
    void OnClose(wxCloseEvent& event);
    void OnNew(wxCommandEvent& WXUNUSED(event));
    void OnOpen(wxCommandEvent& WXUNUSED(event));
    void OnSave(wxCommandEvent& WXUNUSED(event));
    void OnSaveAs(wxCommandEvent& WXUNUSED(event));
    void OnStatusChanged(wxCommandEvent& event);
    void OnExit(wxCommandEvent& WXUNUSED(event));
    void OnAbout(wxCommandEvent& WXUNUSED(event));
    bool QueryCanDiscard();
    void UpdateTitle();

    wxString GetText() { return editor_->GetText(); }

private:
    Editor* editor_;

    property(wxString) {
        wxString get() {
            return value;
        }
        void set(MainFrame* self, wxString path) {
            value = path;
            self->UpdateTitle();
        }
    } path_;
};
