/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>

#include "editor.h"
#include "gproc/lexer.h"
#include "gproc/parser.h"

#define LEX_COMMENT       9  // ( ) - comments
#define LEX_NUMOP_1      10  // G - g-words
#define LEX_NUMOP_2      11  // X Y Z U V W P Q R A B C - dimension words
#define LEX_NUMOP_3      12  // I J K - interpolation lead words
#define LEX_NUMOP_4      13  // E F - feed rate
#define LEX_NUMOP_5      14  // S - spindle speed
#define LEX_NUMOP_6      15  // D T - tool function
#define LEX_NUMOP_7      16  // M - misc function
#define LEX_PNUMBER      17  // N - pgm. number

#define STC_FOLDMARGIN    2

#define USE_LEXER         1
#define USE_PARSER        1

wxDEFINE_EVENT(STC_STATUS_CHANGED, wxCommandEvent);


Editor::Editor(wxWindow* parent)
        : wxStyledTextCtrl(parent), modified_(false)
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
    Bind(wxEVT_STC_MARGINCLICK, &Editor::OnMarginClick, this);
    Bind(wxEVT_STC_MODIFIED, &Editor::OnModified, this);
    Bind(wxEVT_STC_STYLENEEDED, &Editor::OnStyleNeeded, this);

    SetScrollWidth(1);
    SetScrollWidthTracking(true);

    StyleSetForeground(LEX_COMMENT, wxColour(106, 115, 125));
    StyleSetForeground(LEX_NUMOP_1, wxColour(215, 58, 73));
    StyleSetForeground(LEX_NUMOP_2, wxColour(0, 132, 176));
    StyleSetForeground(LEX_NUMOP_3, wxColour(120, 91, 160));
    StyleSetForeground(LEX_NUMOP_4, wxColour(120, 91, 160));
    StyleSetForeground(LEX_NUMOP_5, wxColour(180, 189, 108));
    StyleSetForeground(LEX_NUMOP_6, wxColour(227, 98, 9));
    StyleSetForeground(LEX_NUMOP_7, wxColour(215, 58, 73));
    StyleSetForeground(LEX_PNUMBER, wxColour(227, 98, 9));

    StyleSetFaceName(wxSTC_STYLE_DEFAULT, "Consolas");
    StyleSetForeground(wxSTC_STYLE_DEFAULT, wxColour(35, 36, 38));
    StyleSetSize(wxSTC_STYLE_DEFAULT, 12);

    StyleSetForeground(wxSTC_STYLE_LINENUMBER, "grey");
    StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour(228, 228, 228));
}

void Editor::DoSetFoldLevels(unsigned fromPos, int startLevel, wxString& text) {
    // TODO if multiple programs are allowed in a file we could fold acc. to %
}

void Editor::DoSetStyling(unsigned fromPos, unsigned toPos, wxString &text) {
    StartStyling(fromPos);
    SetStyling(toPos - fromPos, 0);

#if USE_LEXER
    Lexer lexer(text.ToStdWstring());
    int numop;
    unsigned start, length;
    for (Token::Token t = lexer.next();
         t.type != Token::EndOfFile;
         t = lexer.next())
    {
        //std::cout << t << std::flush;
        //continue;

        start = t.start;
        length = t.length;

        numop = 0;
        if (t.type == Token::G)
        {
            numop = LEX_NUMOP_1;
        }
        else if (t.type == Token::X ||
                 t.type == Token::Y ||
                 t.type == Token::Z ||
                 t.type == Token::U ||
                 t.type == Token::V ||
                 t.type == Token::W ||
                 t.type == Token::P ||
                 t.type == Token::Q ||
                 t.type == Token::R ||
                 t.type == Token::A ||
                 t.type == Token::B ||
                 t.type == Token::C)
        {
            numop = LEX_NUMOP_2;
        }
        else if (t.type == Token::I ||
                 t.type == Token::J ||
                 t.type == Token::K)
        {
            numop = LEX_NUMOP_3;
        }
        else if (t.type == Token::E ||
                 t.type == Token::F)
        {
            numop = LEX_NUMOP_4;
        }
        else if (t.type == Token::S)
        {
            numop = LEX_NUMOP_5;
        }
        else if (t.type == Token::D ||
                 t.type == Token::T)
        {
            numop = LEX_NUMOP_6;
        }
        else if (t.type == Token::M)
        {
            numop = LEX_NUMOP_7;
        }
        else if (t.type == Token::N)
        {
            numop = LEX_PNUMBER;
        }

        if (numop)
        {
            t = lexer.next();
            if (t.type == Token::Number)
            {
                length += t.length;

                StartStyling(fromPos + start);
                SetStyling(length, numop);
            }
        }
        else if (t.type == Token::Comment)
        {
            StartStyling(fromPos + start);
            SetStyling(length, LEX_COMMENT);
        }
    }
#endif

#if USE_PARSER
    if (!modified_) return;

    wxCommandEvent event(STC_STATUS_CHANGED);
    try {
        /* we need to keep this ref alive for the duration of the scope */
        auto text = GetText();
        Parser parser(text.ToStdWstring());
        parser.parse();
        event.SetString("Compiles fine");
    }
    catch (ParserException e)
    {
        auto line = LineFromPosition(e.position());
        auto column = e.position() - PositionFromLine(line);
        event.SetString(std::to_string(line+1) + ":" + std::to_string(column) + ": " + e.what());
    }
    wxPostEvent(GetParent(), event);
    modified_ = false;
#endif
}

void Editor::OnMarginClick(wxStyledTextEvent& event) {
    int margin = event.GetMargin();
    int line = LineFromPosition(event.GetPosition());

    int foldLevel = GetFoldLevel(line);
    bool headerFlag = (foldLevel & wxSTC_FOLDLEVELHEADERFLAG) != 0;

    if (margin == STC_FOLDMARGIN && headerFlag) {
        ToggleFold(line);
    }
}

void Editor::OnModified(wxStyledTextEvent& event) {
    int type = event.GetModificationType();
    if (type & (wxSTC_MOD_INSERTTEXT || wxSTC_MOD_DELETETEXT ||
                wxSTC_PERFORMED_UNDO || wxSTC_PERFORMED_REDO))
    {
        modified_ = true;
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
