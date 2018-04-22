/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <cmath>
#include <string>

#include <wx/wx.h>

#include <wx/button.h>

#include "main.h"
#include "sidebar.h"
#include "gproc/parser.h"
#include "gproc/types.h"


Sidebar::Sidebar(wxWindow* parent) : wxPanel(parent)
{
    mspeeds_.emplace_back(15, 18);
    mspeeds_.emplace_back(30, 38);
    mspeeds_.emplace_back(21, 40);
    mspeeds_.emplace_back(75, 105);

    materials_box_ = new wxComboBox(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, 0, NULL, wxCB_READONLY);
    materials_box_->Append("Steel (tough)");
    materials_box_->Append("Mild Steel");
    materials_box_->Append("Carbon Alloy Steel (C1008-1095)");
    materials_box_->Append("Aluminum");
    materials_box_->SetSelection(0);
    auto machineBox = new wxComboBox(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, 0, NULL, wxCB_READONLY);
    machineBox->Append("Milling");
    machineBox->Append("Lathe");
    machineBox->SetSelection(0);
    diameter_edit_ = new wxSpinCtrlDouble(this);
    diameter_edit_->SetValue(1.5);
    speed_list_ = new wxListView(this, wxID_ANY);
    speed_list_->AppendColumn("Value");
    speed_list_->AppendColumn("Line");
    speed_list_->AppendColumn("Status");
    auto button = new wxButton(this, wxID_ANY, "Calculate");
    button->Bind(wxEVT_BUTTON, &Sidebar::OnCalculateSpeeds, this);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    auto border = 16;
    sizer->Add(materials_box_, 0, wxALL|wxEXPAND, border);
    sizer->Add(machineBox, 0, wxLEFT|wxRIGHT|wxEXPAND, border);
    sizer->Add(diameter_edit_, 0, wxALL|wxEXPAND, border);
    sizer->Add(speed_list_, 0, wxALL|wxEXPAND, border);
    sizer->Add(button, 0, wxLEFT|wxRIGHT|wxEXPAND, border);
    sizer->AddStretchSpacer();
    SetSizerAndFit(sizer);
}

void Sidebar::OnCalculateSpeeds(wxCommandEvent& event)
{
    speed_list_->DeleteAllItems();

    auto text = ((MainFrame*) GetParent())->GetText();
    Program program;
    try {
        Parser parser(text.ToStdWstring());
        program = parser.parse();
    }
    catch (std::exception e) {
        return;
    }

    auto spr = mspeeds_[materials_box_->GetSelection()];

    auto visitor = SpeedVisitor(
        { spr.first, spr.second, (float)diameter_edit_->GetValue() });
    program.accept(&visitor);

    unsigned index = 0;
    for (SpeedVisitor::SpeedRecord rec : visitor.records()) {
        //std::cout << rec.value << rec.calculatedValueLo <<
        //    rec.calculatedValueHi << std::endl;
        speed_list_->InsertItem(index, std::to_string((int)rec.value));
        speed_list_->SetItem(index, 1, std::to_string(rec.line));
        wxString msg;
        msg << (int)rec.calculatedValueLo << wxString::FromUTF8("–")
            << (int)std::ceil(rec.calculatedValueHi);
        speed_list_->SetItem(index, 2, msg);
        ++index;
    }
}
