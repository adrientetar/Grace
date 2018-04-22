/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <wx/wx.h>

#include <wx/combobox.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/spinctrl.h>
#include <wx/window.h>

class Sidebar : public wxPanel {
    friend class MainFrame;
public:
    Sidebar(wxWindow* parent);
    void OnCalculateSpeeds(wxCommandEvent& event);
private:
    //const static wxString materials_[] = {
    //  wxT("a"), wxT("b"), wxT("c"), wxT("d")};

    wxComboBox* materials_box_;
    wxSpinCtrlDouble* diameter_edit_;
    wxListView* speed_list_;

    std::vector<std::pair<float, float>> mspeeds_;
};
