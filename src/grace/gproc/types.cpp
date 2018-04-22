/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.h"

void Word::accept(Visitor* v) { v->visit(this); };
void Header::accept(Visitor* v) { v->visit(this); };
void BlockNumber::accept(Visitor* v) { v->visit(this); };
void Block::accept(Visitor* v) { v->visit(this); };
void Program::accept(Visitor* v) { v->visit(this); };

void Visitor::visit(Word* w)
{
}

void Visitor::visit(Header* h)
{
}

void Visitor::visit(BlockNumber* bn)
{
}

void Visitor::visit(Block* b)
{
    if (b->number != std::nullopt)
    {
        b->number->accept(this);
    }
    for (Word w : b->data_words)
    {
        w.accept(this);
    }
}

void Visitor::visit(Program* p)
{
    for (Block b : p->blocks) { b.accept(this); }
}

/* */

float SpeedVisitor::calcSpindleSpeed(float cs)
{
    float value;
    if (speed_kind_ == SpeedVisitor::ConstantSurfaceSpeed) // G96
    {
        value = cs;
    }
    else
    {
        value = 1000. * cs / (PI_F * ref_data_.toolDiameter);
    }
    if (units_ == SpeedVisitor::Imperial) { // G70
        value /= 39.37f;
    }
    return value;
}

void SpeedVisitor::visit(Word* w)
{
    if (w->kind == Token::G)
    {
        if (w->value == 70)
        {
            units_ = SpeedVisitor::Imperial;
        }
        else if (w->value == 71)
        {
            units_ = SpeedVisitor::Metrics;
        }
        else if (w->value == 96)
        {
            speed_kind_ = SpeedVisitor::ConstantSurfaceSpeed;
        }
        else if (w->value == 97)
        {
            speed_kind_ = SpeedVisitor::RevPerMinute;
        }
    }
    if (w->kind == Token::F)
    {
        // TODO
    }
    else if (w->kind == Token::S)
    {
        // XXX add current line
        speed_records_.emplace_back(SpeedVisitor::SpeedRecord {
            0, w->value,
            calcSpindleSpeed(ref_data_.cuttingSpeedLo),
            calcSpindleSpeed(ref_data_.cuttingSpeedHi)
        });
    }
}
