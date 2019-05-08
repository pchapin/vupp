/****************************************************************************
FILE      : fink.cpp
SUBJECT   : VuPP linker. This program links OJ files to HEX files.
PROGRAMMER: (C) Copyright 2003 by Peter Chapin

Please send comments or bug reports to

     Peter Chapin
     Vermont Technical College
     Williston, VT 05495
     PChapin@vtc.vsc.edu
****************************************************************************/

#include <cstdlib>
#include <iostream>
#include <list>

#include "str.hpp"
#include "uints.hpp"

using namespace std;

//+++++++++++++++++++++++++++++++++
//           Global Data
//+++++++++++++++++++++++++++++++++
DoubleWord        starting_address;
int               databus_size = 0;
pcc::String       base_name;
list<pcc::String> OJ_names;

//+++++++++++++++++++++++++++++++++++++++++++++++++
//           Internally Linked Functions
//+++++++++++++++++++++++++++++++++++++++++++++++++

//
// Process_CommmandLine
//
static bool process_commandline(int argc, char **argv)
{
    // Step down the command line. Skip over the first item.
    while (*++argv) {

        // If this is a switch...
        if (**argv == '-' || **argv == '/') {
            switch (*++*argv) {
            case 's':
            case 'S':
                break;

            case 'l':
            case 'L':
                if (*++argv == 0) {
                    cerr << "Missing parameter given to the -l switch!" << endl;
                    return false;
                }
                databus_size = atoi(*argv);
                if (databus_size !=  8 &&
                    databus_size != 16 &&
                    databus_size != 32 &&
                    databus_size != 64) {
                    cerr << "Invalid size given to the -l switch. Use 8, 16, 32, or 64." << endl;
                    return false;
                }
                break;

            case 'n':
            case 'N':
                break;

            default:
                cerr << "Unknown switch on the command line: " << **argv << endl;
                return false;
            }
        }
    
        // It's not a switch. Handle it as something else.
        else {

            // Is this a response file?
            if (**argv == '@') {
                cerr << "Sorry, response files are not implemented." << endl;
                return false;
            }

            // Nope. Add this file name to our list.
            else {
                pcc::String name(*argv);
                int         length;
        
                // Put an extension on this name if there isn't one already.
                if (name.pos('.') == -1) name.append(".oj");

                // If we don't have a base name yet, set it up.
                if (base_name.length() == 0) {
                    base_name = name;
                    if ((length = base_name.last_pos('.')) != -1)
                        base_name.substr(0, length);
                }
                OJ_names.push_back(name);
            }
        }
    }
    return true;
}

//++++++++++++++++++++++++++++++++++++++
//           Public Functions
//++++++++++++++++++++++++++++++++++++++

//
// main
//
int main(int argc, char **argv)
{
    if (process_commandline(argc, argv) == false) {
        cerr << "FINK process aborted." << endl;
        return 1;
    }

    cout << "base_name = " << base_name << endl;
    cout << "File names are: " << endl;

    list<pcc::String>::iterator stepper = OJ_names.begin();

    while (stepper != OJ_names.end()) {
        cout << *stepper << endl;
        stepper++;
    }
    return 0;
}
