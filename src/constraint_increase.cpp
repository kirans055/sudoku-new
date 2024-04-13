#include "constraint_increase.hpp"

#include <stdio.h>

#include "list.tcc"

ConstraintIncrease::ConstraintIncrease()
{
}

ConstraintIncrease::ConstraintIncrease(ConstraintIncrease *src, int diff)
{
    for(int i = 0; i < src->direction.size(); i++) {
        auto &item = src->direction[i];
        add(item.index, item.diff - diff);
    }
}

void ConstraintIncrease::add(int index, int diff)
{
    IndexDiff item = {index, diff};
    direction.push(item);
}

void ConstraintIncrease::apply(Sudoku *dst)
{
    for(int i = 0; i < direction.size(); i++) {
        if(direction[i].diff == 0) {
            dst->addConstraint(direction[i].index, this);
        } else {
            dst->addConstraint(direction[i].index, new ConstraintIncrease(this, direction[i].diff));
        }
    }
}

void ConstraintIncrease::print()
{
    for(int i = 0; i < direction.size(); i++) {
        int index = direction[i].index;
    }
}

void ConstraintIncrease::onAdded(Sudoku *sudoku, int index)
{
    int above = 0;
    int bellow = 0;
    for(int i = 0; i < direction.size(); i++) {
        auto &item = direction[i];
        if(item.diff < 0) bellow++;
        if(item.diff > 0) above++;
    }
    int bit = SudokuNoteAll >> above & SudokuNoteAll << bellow;
    sudoku->note[index] &= bit;
}

SudokuResult ConstraintIncrease::onCollapsed(Sudoku *sudoku, int index, int value, SudokuNote note)
{
    SudokuResult fresult = SudokuResultUnchanged;
    for(int i = 0; i < direction.size(); i++) {
        auto &item = direction[i];
        int oldValue = sudoku->note[item.index];
        if(oldValue & SudokuNoteOk) continue;

        int bit;
        if(item.diff < 0) bit = SudokuNoteAll >> (8 - value - item.diff);
        else if(item.diff > 0) bit = SudokuNoteAll << (value + item.diff);
        else continue;

        int newValue = (oldValue & ~SudokuNoteAll) | (oldValue & bit & SudokuNoteAll);

        if(oldValue != newValue) {
            fresult = SudokuResultOk;
        }
        if((newValue & SudokuNoteAll) == 0) {
            return SudokuResultError;
        }
        sudoku->note[item.index] = newValue;
    }
    return fresult;
}