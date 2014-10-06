// BluetoothInterface.cpp
/*

Copyright (C) 2014 Bricolabs - http://bricolabs.cc

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
USA.

See LICENSE.txt for details

*/

#include "BluetoothInterface.h"
#include <string.h>

#define NL '\n'

const static char* SI_PROGRAM = "PRG:";

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

BluetoothInterface::BluetoothInterface(const Config* cfg)
{
    _config = cfg;
    _command_idx = 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void BluetoothInterface::init()
{
    _config->serial->begin(_config->bauds, SERIAL_8N1);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

ButtonSet::BUTTON BluetoothInterface::scanButtons()
{
    while (_readLine())
    {
        // one-char commands
        if (strlen(_command) == 1)
        {
            char* found = strchr(BUTTONS_RELEASED, _command[0]);
            if (found)
            {
                return (BUTTON)((found - BUTTONS_RELEASED) + 1);
            }
        }
    }

    return BUTTON_NONE;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool BluetoothInterface::_readLine()
{
    while (_config->serial->available())
    {
        char key = _config->serial->read();

        if (key == NL)
        {
            _command[_command_idx] = '\0';
            _command_idx = 0;
            return true;
        }

        if (_command_idx < COMMAND_MAX_LENGTH)
        {
            _command[_command_idx++] = key;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void BluetoothInterface::SiMoveExecuting(MOVE move)
{
    _config->serial->print(BUTTONS_PRESSED[move - 1]);
    _config->serial->print(NL);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void BluetoothInterface::SiMoveExecuted(MOVE move)
{
    _config->serial->print(BUTTONS_RELEASED[move - 1]);
    _config->serial->print(NL);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void BluetoothInterface::SiMoveAdded(MOVE move)
{
    _config->serial->print(SI_PROGRAM);
    _config->serial->print(move);
    _config->serial->print(' ');
    _config->serial->print(BUTTONS_RELEASED[move - 1]);
    _config->serial->print(NL);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void BluetoothInterface::SiProgramStarted(uint8_t total_moves)
{
    _config->serial->print(SI_PROGRAM);
    _config->serial->print("GO ");
    _config->serial->print(total_moves);
    _config->serial->print(NL);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void BluetoothInterface::SiProgramFinished()
{
    _config->serial->print(SI_PROGRAM);
    _config->serial->print("END");
    _config->serial->print(NL);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void BluetoothInterface::SiProgramReset()
{
    _config->serial->print(SI_PROGRAM);
    _config->serial->print("RESET");
    _config->serial->print(NL);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void BluetoothInterface::SiProgramAborted(uint8_t executed, uint8_t total)
{
    _config->serial->print(SI_PROGRAM);
    _config->serial->print("ABORTED ");
    _config->serial->print(executed);
    _config->serial->print("/");
    _config->serial->print(total);
    _config->serial->print(NL);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


// EOF