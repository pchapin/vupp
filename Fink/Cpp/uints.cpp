/****************************************************************************
FILE      : uints.cpp
SUBJECT   : Implementation of various unsigned integer types.
PROGRAMMER: (C) Copyright 2003 by Peter Chapin

Please send comments or bug reports to

     Peter Chapin
     Vermont Technical College
     Williston, VT 05495
     PChapin@vtc.vsc.edu
****************************************************************************/

#include "uints.hpp"

//
// Here are the various friend functions. Since they all have the same format, I put them all
// together.
//

Byte operator+(const Byte &Left, const Byte &Right)
{
    Byte Temp(Left);
    Temp += Right;
    return Temp;
}

Byte operator-(const Byte &Left, const Byte &Right)
{
    Byte Temp(Left);
    Temp -= Right;
    return Temp;
}

Word operator+(const Word &Left, const Word &Right)
{
    Word Temp(Left);
    Temp += Right;
    return Temp;
}

Word operator-(const Word &Left, const Word &Right)
{
    Word Temp(Left);
    Temp -= Right;
    return Temp;
}

DoubleWord operator+(const DoubleWord &Left, const DoubleWord &Right)
{
    DoubleWord Temp(Left);
    Temp += Right;
    return Temp;
}

DoubleWord operator-(const DoubleWord &Left, const DoubleWord &Right)
{
    DoubleWord Temp(Left);
    Temp -= Right;
    return Temp;
}

QuadWord operator+(const QuadWord &Left, const QuadWord &Right)
{
    QuadWord Temp(Left);
    Temp += Right;
    return Temp;
}

QuadWord operator-(const QuadWord &Left, const QuadWord &Right)
{
    QuadWord Temp(Left);
    Temp -= Right;
    return Temp;
}


//
// class Byte
//

Byte::Byte(long Initial)
{
    if (Initial < -128L || Initial > 255L) 
        throw uint_Error(uint_Error::RANGE_ERROR, uint_Error::BYTE);

    Number = (unsigned char)Initial;
}

Byte::operator long()
{
    return (long)Number;
}

void Byte::operator+=(const Byte &Other)
{
    Number += Other.Number;
}

void Byte::operator-=(const Byte &Other)
{
    Number -= Other.Number;
}


//
// class Word
//

Word::Word(long Initial)
{
    if (Initial < -32768L || Initial > 65535L) 
        throw uint_Error(uint_Error::RANGE_ERROR, uint_Error::WORD);

    Number = (unsigned int)Initial;
}

Word::operator long()
{
    return (long)Number;
}

void Word::operator+=(const Word &Other)
{
    Number += Other.Number;
}

void Word::operator-=(const Word &Other)
{
    Number -= Other.Number;
}


//
// class DoubleWord
//

DoubleWord::DoubleWord(long Initial)
{
    // Anything coming through in 'Initial' will fit.
    Number = (unsigned long)Initial;
}

DoubleWord::operator long()
{
    return (long)Number;
}

void DoubleWord::operator+=(const DoubleWord &Other)
{
    Number += Other.Number;
}

void DoubleWord::operator-=(const DoubleWord &Other)
{
    Number -= Other.Number;
}


//
// class QuadWord
//

QuadWord::QuadWord(long Initial)
{
    // Anything coming through in 'Initial' will fit.
    Number[0] = (unsigned)  (0x0000FFFFUL & (unsigned long)Initial);
    Number[1] = (unsigned) ((0xFFFF0000UL & (unsigned long)Initial) >> 16);

    // We should sign extend negative values.
    if (Initial < 0) {
        Number[2] = 0xFFFFU;
        Number[3] = 0xFFFFU;
    }
    else {
        Number[2] = 0x0000U;
        Number[3] = 0x0000U;
    }
}

void QuadWord::operator+=(const QuadWord &Other)
{
    long Carry = 0;

    for (int i = 0; i < 4; i++) {
        long Sum  = Number[i] + Other.Number[i] + Carry;
        Number[i] = (unsigned) (0x0000FFFFUL & Sum);
        Carry     = (0xFFFF0000UL & Sum) >> 16;
    }
}

void QuadWord::operator-=(const QuadWord &Other)
{
    long Borrow = 0;

    for (int i = 0; i < 4; i++) {
        long Difference = Number[i] - Other.Number[i] - Borrow;
        Number[i] = (unsigned) (0x0000FFFFUL & Difference);
        Borrow    = (0xFFFF0000L  & Difference) >> 16;
    }
}

