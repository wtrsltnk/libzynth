/*
  ZynAddSubFX - a software synthesizer

  Dump.cpp - It dumps the notes to a text file

  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2 or later) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/
#include <stdlib.h>
#include <time.h>
#include "Util.h"
#include "Dump.h"

Dump dump;

Dump::Dump()
{
    tick = 0;
    k    = 0;
    keyspressed = 0;
}

Dump::~Dump()
{
    if(file.is_open()) {
        int duration = int(tick * synth->buffersize_f / synth->samplerate_f);
        file << std::endl << "# statistics: duration = " << duration << " seconds; keyspressed = " << keyspressed << std::endl << std::endl << std::endl;
        this->file.close();
    }
}

void Dump::startnow()
{
    if(file.is_open())
        return;            //the file is already open

    if(config.cfg.DumpNotesToFile != 0) {
        if(config.cfg.DumpAppend != 0)
            file.open(config.cfg.DumpFile.c_str(), std::ios::app);
        else
            file.open(config.cfg.DumpFile.c_str());
        if(file == NULL)
            return;
        if(config.cfg.DumpAppend != 0)
            file << "#************************************" << std::endl;

        time_t tm = time(NULL);

        file << "#date/time = " << ctime(&tm) << std::endl;
        file << "#1 tick = " << synth->buffersize_f * 1000.0f / synth->samplerate_f << " milliseconds" << std::endl;
        file << "SAMPLERATE = " << synth->samplerate << std::endl;
        file << "TICKSIZE = " << synth->buffersize << " #samples" << std::endl;
        file << std::endl << std::endl << "START" << std::endl;
    }
}

void Dump::inctick()
{
    tick++;
}


void Dump::dumpnote(char chan, char note, char vel)
{
    if(file == NULL)
        return;
    if(note == 0)
        return;
    if(vel == 0)
        file << "n " << tick << " -> " << chan << " " << note << std::endl;
    else
        file << "N " << tick << " -> " << chan << " " << note << " " << vel << std::endl;

    if(vel != 0)
        keyspressed++;
#ifndef JACKAUDIOOUT
    if(k++ > 25) {
        file.flush();
        k = 0;
    }
#endif
}

void Dump::dumpcontroller(char chan, unsigned int type, int par)
{
    if(file == NULL)
        return;
    switch(type) {
        case C_pitchwheel:
            file << "P " << tick << " -> " << chan << " " << par << std::endl;
            break;
        default:
        file << "C " << tick << " -> " << chan << " " << type << " " << par << std::endl;
            break;
    }
#ifndef JACKAUDIOOUT
    if(k++ > 25) {
        file.flush();
        k = 0;
    }
#endif
}
