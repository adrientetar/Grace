/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>

#include <wx/aboutdlg.h>
#include <wx/filename.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>

#include "main.h"

App::App()
{
    SetAppDisplayName(_T("Grace"));
}

bool App::OnInit()
{
    auto frame = new MainFrame(GetAppDisplayName());
    frame->Show();
    return true;
}

IMPLEMENT_APP(App);

MainFrame::MainFrame(const wxString& title)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1280, 800)),
          editor_(new Editor(this))
{
    auto fileMenu = new wxMenu;
    fileMenu->Append(wxID_NEW);
    fileMenu->Append(wxID_OPEN);
    fileMenu->Append(wxID_SAVE);
    fileMenu->Append(wxID_SAVEAS, _T("Save &As..."));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);

    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnNew, this, wxID_NEW);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnSave, this, wxID_SAVE);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnSaveAs, this, wxID_SAVEAS);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnExit, this, wxID_EXIT);

    auto helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT);

    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnAbout, this, wxID_ABOUT);

    auto menubar = new wxMenuBar;
    menubar->Append(fileMenu, wxGetStockLabel(wxID_FILE));
    menubar->Append(helpMenu, wxGetStockLabel(wxID_HELP));
    SetMenuBar(menubar);

    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);

    CreateStatusBar();

    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(editor_, 1, wxEXPAND);
    sizer->Add(new Sidebar(this), 0, wxEXPAND);
    SetSizer(sizer);

    editor_->SetFocus();

    editor_->Bind(wxEVT_STC_SAVEPOINTLEFT, [=](wxCommandEvent&) { UpdateTitle(); });
    editor_->Bind(wxEVT_STC_SAVEPOINTREACHED, [=](wxCommandEvent&) { UpdateTitle(); });
    Bind(STC_STATUS_CHANGED, &MainFrame::OnStatusChanged, this);

    UpdateTitle();
    Centre();
}

MainFrame::~MainFrame()
{
    delete editor_;
}

void MainFrame::OnStatusChanged(wxCommandEvent& event)
{
    SetStatusText(event.GetString());
}

bool MainFrame::DoSave(bool forceSaveAs/* = false */)
{
    auto path = path_.get();
    if (!forceSaveAs && path != wxEmptyString)
    {
        editor_->SaveFile(path);
        return true;
    }

    auto dialog = new wxFileDialog(
        this, _T("Save As"), wxEmptyString, wxEmptyString,
        _("G-code files (*.gcode)|*.gcode;*.txt|All files (*.*)|*"),
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

    bool ret = false;
    if (dialog->ShowModal() == wxID_OK)
    {
        editor_->SaveFile(dialog->GetPath());
        path_.set(this, dialog->GetPath());
        ret = true;
    }
    dialog->Destroy();
    return ret;
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    // TODO: if we can't veto, we should save changes to Documents folder
    if(event.CanVeto() && !QueryCanDiscard()) {
        event.Veto();
    }
    event.Skip();
}

void MainFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{
    if (!QueryCanDiscard()) return;

    editor_->ClearAll();
    editor_->EmptyUndoBuffer();
    path_.set(this, wxEmptyString);
}

void MainFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    if (!QueryCanDiscard()) return;

    auto dialog = new wxFileDialog(
        this, _T("Open"), wxEmptyString, wxEmptyString,
        _("G-code files (*.gcode)|*.gcode;*.txt|All files (*.*)|*"),
        wxFD_OPEN, wxDefaultPosition);

    if (dialog->ShowModal() == wxID_OK)
    {
        editor_->LoadFile(dialog->GetPath());
        path_.set(this, dialog->GetPath());
    }
    dialog->Destroy();
}

void MainFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    DoSave();
}

void MainFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
    DoSave(true);
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    // show warning dialog on the app close callback
    wxExit();
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxAboutDialogInfo info;
    //info.SetIcon();
    //info.SetVersion();
    info.SetDescription(_T(
        "Grace is an optimized G-code editor."));//.Format(wxGetApp().GetAppDisplayName()));
    info.SetCopyright(_T(
        "(C) 2017 The Grace Project Developers"));
    info.SetLicense(_T(
        "Licensed under the Mozilla Public License, Version 2.0.\n"
        "\n"
        "If a copy of the MPL was not distributed with this program,\n"
        "you can obtain one at <https://mozilla.org/MPL/2.0/>."));
    //info.SetWebSite();
    //info.SetDevelopers();
    wxAboutBox(info, this);
}

bool MainFrame::QueryCanDiscard()
{
    if (!editor_->GetModify()) {
        return true;
    }

    auto dialog = new wxMessageDialog(this, _T("Do you want to save your changes?"), wxEmptyString, wxYES_NO|wxCENTRE|wxCANCEL);
    dialog->SetTitle(wxGetApp().GetAppDisplayName());
    dialog->SetYesNoLabels(wxID_SAVE, _("&Don't save"));

    bool ret;
    auto dret = dialog->ShowModal();
    if (dret == wxID_YES)
    {
        ret = DoSave();
    }
    else if (dret == wxID_NO)
    {
        ret = true;
    }
    else
    {
        ret = false;
    }
    dialog->Destroy();
    return ret;
}

void MainFrame::UpdateTitle()
{
    auto path = path_.get();

    wxString basename;
    if (editor_->GetModify()) {
        basename << "*";
    }
    if (path != wxEmptyString) {
        basename << wxFileName(path).GetFullName();
    } else {
        basename << _T("Untitled");
    }
    SetTitle(basename << wxString::FromUTF8(" – ")
                      << wxGetApp().GetAppDisplayName());
}
