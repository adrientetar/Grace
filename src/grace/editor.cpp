/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>

#include "editor.h"
#include "gproc/lexer.h"

#define LEX_COMMENT       9
#define LEX_NUMOP_1      10  // G M
#define LEX_NUMOP_2      11  // X Y Z
#define LEX_NUMOP_3      12  // T R H P
#define LEX_NUMOP_4      13  // F S
#define LEX_NUMOP_5      15  // N O
#define LEX_NUMOP_6      14  // I J

#define STC_FOLDMARGIN    2


Editor::Editor(wxWindow* parent)
        : wxStyledTextCtrl(parent)
{
    SetLexer(wxSTC_LEX_CONTAINER);

    SetMarginWidth(0, 30);
    SetMarginWidth(2, 14);
    SetMarginMask(2, wxSTC_MASK_FOLDERS);
    SetFoldMarginColour(true, "white");
    SetFoldMarginHiColour(true, wxColour(233, 233, 233));

    MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED);
    MarkerSetForeground(wxSTC_MARKNUM_FOLDEREND, wxColor(243,243,243));
    MarkerSetBackground(wxSTC_MARKNUM_FOLDEREND, wxColor(128,128,128));
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED);
    MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPENMID, wxColor(243,243,243));
    MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPENMID, wxColor(128,128,128));
    MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER);
    MarkerSetForeground(wxSTC_MARKNUM_FOLDERMIDTAIL, wxColor(243,243,243));
    MarkerSetBackground(wxSTC_MARKNUM_FOLDERMIDTAIL, wxColor(128,128,128));
    MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER);
    MarkerSetForeground(wxSTC_MARKNUM_FOLDERTAIL, wxColor(243,243,243));
    MarkerSetBackground(wxSTC_MARKNUM_FOLDERTAIL, wxColor(128,128,128));
    MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE);
    MarkerSetForeground(wxSTC_MARKNUM_FOLDERSUB, wxColor(243,243,243));
    MarkerSetBackground(wxSTC_MARKNUM_FOLDERSUB, wxColor(128,128,128));
    MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS);
    MarkerSetForeground(wxSTC_MARKNUM_FOLDER, wxColor(243,243,243));
    MarkerSetBackground(wxSTC_MARKNUM_FOLDER, wxColor(128,128,128));
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
    MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPEN, wxColor(243,243,243));
    MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPEN, wxColor(128,128,128));

    MarkerEnableHighlight(true);

    SetMarginSensitive(STC_FOLDMARGIN, true);
    Bind(wxEVT_STC_MARGINCLICK, &OnMarginClick, this);
    Bind(wxEVT_STC_STYLENEEDED, &OnStyleNeeded, this);

    SetScrollWidth(1);
    SetScrollWidthTracking(true);

    StyleSetForeground(LEX_COMMENT, wxColour(106, 115, 125));
    StyleSetForeground(LEX_NUMOP_1, wxColour(215, 58, 73));
    StyleSetForeground(LEX_NUMOP_2, wxColour(0, 132, 176));
    StyleSetForeground(LEX_NUMOP_3, wxColour(227, 98, 9));
    StyleSetForeground(LEX_NUMOP_4, wxColour(120, 91, 160));
    StyleSetForeground(LEX_NUMOP_5, wxColour(180, 189, 108));
    StyleSetForeground(LEX_NUMOP_6, wxColour(120, 91, 160));

    StyleSetFaceName(wxSTC_STYLE_DEFAULT, "Consolas");
    StyleSetForeground(wxSTC_STYLE_DEFAULT, wxColour(35, 36, 38));
    StyleSetSize(wxSTC_STYLE_DEFAULT, 12);

    StyleSetForeground(wxSTC_STYLE_LINENUMBER, "grey");
    StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour(228, 228, 228));
}

void Editor::DoSetFoldLevels(unsigned fromPos, int startLevel, wxString& text) {
    // TODO
}

void Editor::DoSetStyling(unsigned fromPos, unsigned toPos, wxString &text) {
    StartStyling(fromPos);
    SetStyling(toPos - fromPos, 0);

    Lexer lexer(text.ToStdWstring());
    int numop;
    unsigned start, length;
    for (Token t = lexer.next();
         t.type != TokenType_::EndOfFile;
         t = lexer.next())
    {
        //std::cout << t << std::flush;
        //continue;

        start = t.start;
        length = t.length;

        numop = 0;
        if (t.type == TokenType_::G ||
            t.type == TokenType_::M)
        {
            numop = LEX_NUMOP_1;
        }
        else if (t.type == TokenType_::X ||
                 t.type == TokenType_::Y ||
                 t.type == TokenType_::Z)
        {
            numop = LEX_NUMOP_2;
        }
        else if (t.type == TokenType_::T ||
                 t.type == TokenType_::R ||
                 t.type == TokenType_::H ||
                 t.type == TokenType_::P)
        {
            numop = LEX_NUMOP_3;
        }
        else if (t.type == TokenType_::F ||
                 t.type == TokenType_::S)
        {
            numop = LEX_NUMOP_4;
        }
        else if (t.type == TokenType_::N ||
                 t.type == TokenType_::O)
        {
            numop = LEX_NUMOP_5;
        }
        else if (t.type == TokenType_::I ||
                 t.type == TokenType_::J)
        {
            numop = LEX_NUMOP_6;
        }

        if (numop)
        {
            t = lexer.next();
            if (t.type == TokenType_::Number)
            {
                length += t.length;

                StartStyling(fromPos + start);
                SetStyling(length, numop);
            }
        }
        else if (t.type == TokenType_::Comment)
        {
            StartStyling(fromPos + start);
            SetStyling(length, LEX_COMMENT);
        }
    }
}

void Editor::OnMarginClick(wxStyledTextEvent& event) {
    int margin = event.GetMargin();
    int line = LineFromPosition(event.GetPosition());

    int foldLevel = GetFoldLevel(line);
    bool headerFlag = (foldLevel & wxSTC_FOLDLEVELHEADERFLAG)!=0;

    if (margin == STC_FOLDMARGIN && headerFlag) {
        ToggleFold(line);
    }
 }

void Editor::OnStyleNeeded(wxStyledTextEvent& event) {
    // restyle the whole modified line otherwise we'll be lacking context
    unsigned startLine = LineFromPosition(GetEndStyled());
    unsigned startPos = PositionFromLine(startLine);
    unsigned endPos = event.GetPosition();

    int startLvl = GetFoldLevel(startLine);
    // mask out the flags and only use the fold level
    startLvl &= wxSTC_FOLDFLAG_LEVELNUMBERS;

    wxString text = GetTextRange(startPos, endPos);
    DoSetStyling(startPos, endPos, text);
    //DoSetFoldLevels(startPos, startLvl, text);
}
