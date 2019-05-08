/****************************************************************************
FILE      : uints.hpp
SUBJECT   : Unsigned integer classes
PROGRAMMER: (C) Copyright 2003 by Peter Chapin

Please send comments or bug reports to

     Peter Chapin
     Vermont Technical College
     Williston, VT 05495
     PChapin@vtc.vsc.edu
****************************************************************************/

#ifndef UINTS_H
#define UINTS_H

//
// This error class is used by all the classes below. This is because they all have similar
// error needs.
//
class uint_Error {
public:
    enum Error_Mode { RANGE_ERROR };
    enum Size       { BYTE, WORD, DOUBLEWORD, QUADWORD };

    Error_Mode What;
    Size       Who;

    uint_Error(Error_Mode X, Size  Y) : What(X), Who(Y) { }
};


//
// 8 bit unsigned integers.
//
class Byte {
private:
    unsigned char Number;

public:
    Byte(long Initial = 0L);
    operator long();

    void operator+=(const Byte &);
    void operator-=(const Byte &);
};

Byte operator+(const Byte &, const Byte &);
Byte operator-(const Byte &, const Byte &);


//
// 16 bit unsigned integers.
//
class Word {
private:
    unsigned short Number;

public:
    Word(long Initial = 0L);
    operator long();

    void operator+=(const Word &);
    void operator-=(const Word &);
};

Word operator+(const Word &, const Word &);
Word operator-(const Word &, const Word &);


//
// 32 bit unsigned integers.
//
class DoubleWord {
private:
    unsigned long Number;

public:
    DoubleWord(long Initial = 0L);
    operator long();

    void operator+=(const DoubleWord &);
    void operator-=(const DoubleWord &);
};

DoubleWord operator+(const DoubleWord &, const DoubleWord &);
DoubleWord operator-(const DoubleWord &, const DoubleWord &);

//
// 64 bit unsigned integers.
//
class QuadWord {
private:
    unsigned int Number[4];

public:
    QuadWord(long Initial = 0L);
    operator long();

    void operator+=(const QuadWord &);
    void operator-=(const QuadWord &);
};

QuadWord operator+(const QuadWord &, const QuadWord &);
QuadWord operator-(const QuadWord &, const QuadWord &);

#endif

