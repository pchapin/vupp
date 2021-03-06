/****************************************************************************
FILE          : str.h
LAST REVISED  : 2003-05-18
SUBJECT       : Interface to a Rexx-like string class.
PROGRAMMER    : (C) Copyright 2003 by Peter Chapin

Everyone has their own string class. This is mine. These strings mimic
the features of strings that exist in the Rexx language.


REVISION HISTORY

+ 2003-05-18: Fixed severe bugs in the multithread handling. See str.cpp
  for more information. Added doxygen style documentation.

+ 1999-09-22: Mostly cosmetic updates. I did add a full set of
  relational operators as inline functions in this file.

+ 1999-02-02: First official release.


LICENSE

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANT-
ABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Please send comments or bug reports pertaining to this file to

     Peter Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@ecet.vtc.edu
****************************************************************************/

#ifndef STR_H
#define STR_H

#include "environ.hpp"
#include <iosfwd>
#include <limits.h>

namespace pcc {

  //! String class supporting Rexx-like operations.
  class String {

    //! Insert a string into an output stream.
    friend std::ostream &operator<<(std::ostream &, const String &);

    //! Extract a string from an input stream.
    friend std::istream &operator>>(std::istream &, String &);

    //! Compare two strings for equality.
    friend bool operator==(const String &, const String &);

    //! Compare two strings.
    friend bool operator< (const String &, const String &);

  private:

    // The text of a string is found through a string_node. There might
    // be many String objects pointing to any particular string_node.
    // Strings share their representations when possible. Copying is
    // done on demand.
    //
    struct string_node {
      int   count;
      char *workspace;

      string_node() : count(1), workspace(0) { }
    };

    string_node *rep;

  public:

   
    //! Construct an empty string.
    String();

    //! Construct a string that is a copy of the given string.
    String(const String &);

    //! Construct a string that is a copy of the given string.
    String(const char *);

    //! Construct a string from a single character.
    String(char);

    //! Assign the given string to this string.
    String &operator=(const String &);

    //! Assign the given string to this string.
    String &operator=(const char *);

    //! Destroy this string.
   ~String();

    //! Convert this string to a C-style string.
    /*! This method returns a pointer to this string's internal
        representation. That pointer will be invalidated by any mutating
        operation.
    */
    operator const char *() const { return rep->workspace; }

    //! Return the length of this string.
    int length() const;

    //! Return the length of this string.
    /*! \sa length */
    int size() const { return length(); }

    //! Append the given string to the end of this string.
    String &append(const String &);

    //! Append the given string to the end of this string.
    String &append(const char *);

    //! Append the given character to the end of this string.
    String &append(char);

    //! Erase this string, making it empty.
    void erase();

    //! Return the rightmost characters of this string.
    String right(int length, char pad = ' ') const;

    //! Return the leftmost characters of this string.
    String left(int length, char pad = ' ') const;

    //! Return this string centered between runs of pad characters.
    String center(int length, char pad = ' ') const;

    //! Copy this string.
    String copy(int count) const;

    //! Erase a substring of this string.
    String erase(int offset, int count = INT_MAX) const;

    //! Insert a string into this string.
    String insert(
      const String &incoming, int offset = 1, int length = INT_MAX) const;

    //! Search this string forward for a character.
    int pos(char needle, int offset = 1) const;

    //! Search this string forward for a string.
    int pos(const char *needle, int offset = 1) const;

    //! Search this string backward for a character.
    int last_pos(char needle, int offset = INT_MAX) const;

    //! Strip leading or trailing instances of kill_char from this string.
    String strip(char mode = 'B', char kill_char = ' ') const;

    //! Locate a substring of this string.
    String substr(int offset, int count = INT_MAX) const;

    //! Locate a substring of this string consisting of the specified
    //! number of words.
    String subword(
      int offset, int count = INT_MAX, const char *white = 0) const;

    //! Return a specific word from this string.
    /*! \param offset The index of the word of interest.

        \param white Pointer to a string of word delimiter characters.

        For this method indicies are word counts. The first word in the
        string is word number 1. \sa subword.
     */
    String word(int offset, const char *white = 0) const
      { return subword(offset, 1, white); }

    //! Return the number of words in this string.
    int words(const char *white = 0) const;
  };

  // +++++
  // These relational operators are defined in terms of the two friends.
  // +++++

  //! Compare two strings for inequality.
  /*! This funtion returns true if the strings are different. The
      comparison is done in a case sensitive manner. */
  inline bool operator!=(const String &left, const String &right)
    { return !(left == right); }
    
  //! Compare two strings.
  /*! This funtion returns true if the first string is the same as the
      second or if it comes after the second. The comparison is done in a
      case sensitive manner. */
  inline bool operator>=(const String &left, const String &right)
    { return !(left < right); }

  //! Compare two strings.
  /*! This funtion returns true if the first strings comes after the
      second. */
  inline bool operator>(const String &left, const String &right)
    { return right < left; }

  //! Compare two strings.
  /*! This funtion returns true if the first string is the same as the
      second or if it comes before the second. The comparison is done in a
      case sensitive manner. */
  inline bool operator<=(const String &left, const String &right)
    { return right >= left; }

  // +++++
  // Infix binary concatenation is too useful to pass up.
  // +++++

  //! Concatenate two strings.
  String operator+(const String &left, const String &right);

  //! Concatenate two strings.
  String operator+(const String &left, const char *right);

  //! Concatenate two strings.
  String operator+(const char *left, const String &right);

  //! Concatenate a character and a string.
  String operator+(const String &left, char right);

  //! Concatenate a character and a string.
  String operator+(char left, const String &right);

}

#endif
