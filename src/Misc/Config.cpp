/*
  ZynAddSubFX - a software synthesizer

  Config.cpp - Configuration file functions
  Copyright (C) 2003-2005 Nasca Octavian Paul
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
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <direct.h>
#endif

#include "Config.h"
#include "XMLwrapper.h"

using namespace std;

Config::Config()
{}
void Config::Init()
{
    //defaults
    cfg.SampleRate      = 44100;
    cfg.SoundBufferSize = 256;
    cfg.OscilSize  = 1024;
    cfg.SwapStereo = 0;

    cfg.Interpolation = 0;
    cfg.CheckPADsynth = 1;

//get the midi input devices name
    cfg.currentBankDir = "./testbnk";

    this->ReadConfig(this->GetConfigFileName());

    if(cfg.bankRootDirList[0].empty()) {
        //banks
        cfg.bankRootDirList[0] = "~/banks";
        cfg.bankRootDirList[1] = "./";
        cfg.bankRootDirList[2] = "/usr/share/zynaddsubfx/banks";
        cfg.bankRootDirList[3] = "/usr/local/share/zynaddsubfx/banks";
        cfg.bankRootDirList[4] = "../banks";
        cfg.bankRootDirList[5] = "banks";
    }

    if(cfg.presetsDirList[0].empty()) {
        //presets
        cfg.presetsDirList[0] = "./";
        cfg.presetsDirList[1] = "../presets";
        cfg.presetsDirList[2] = "presets";
        cfg.presetsDirList[3] = "/usr/share/zynaddsubfx/presets";
        cfg.presetsDirList[4] = "/usr/local/share/zynaddsubfx/presets";
    }
}

Config::~Config()
{ }


void Config::Save()
{
    this->SaveConfig(this->GetConfigFileName());
}

void Config::ClearBankRootDirList()
{
    for(int i = 0; i < MAX_BANK_ROOT_DIRS; ++i)
        cfg.bankRootDirList[i].clear();
}

void Config::ClearPresetsDirList()
{
    for(int i = 0; i < MAX_BANK_ROOT_DIRS; ++i)
        cfg.presetsDirList[i].clear();
}

void Config::ReadConfig(const std::string& filename)
{
    XMLwrapper xmlcfg;
    if(xmlcfg.loadXMLfile(filename) < 0)
        return;
    if(xmlcfg.enterbranch("CONFIGURATION"))
    {
        cfg.SampleRate = xmlcfg.getpar("sample_rate",
                                       cfg.SampleRate,
                                       4000,
                                       1024000);
        cfg.SoundBufferSize = xmlcfg.getpar("sound_buffer_size",
                                            cfg.SoundBufferSize,
                                            16,
                                            8192);
        cfg.OscilSize = xmlcfg.getpar("oscil_size",
                                      cfg.OscilSize,
                                      MAX_AD_HARMONICS * 2,
                                      131072);
        cfg.SwapStereo = xmlcfg.getpar("swap_stereo",
                                       cfg.SwapStereo,
                                       0,
                                       1);

        cfg.currentBankDir = xmlcfg.getparstr("bank_current", "");
        cfg.Interpolation  = xmlcfg.getpar("interpolation",
                                           cfg.Interpolation,
                                           0,
                                           1);

        cfg.CheckPADsynth = xmlcfg.getpar("check_pad_synth",
                                          cfg.CheckPADsynth,
                                          0,
                                          1);

        //get bankroot dirs
        for(int i = 0; i < MAX_BANK_ROOT_DIRS; ++i)
            if(xmlcfg.enterbranch("BANKROOT", i)) {
                cfg.bankRootDirList[i] = xmlcfg.getparstr("bank_root", "");
                xmlcfg.exitbranch();
            }

        //get preset root dirs
        for(int i = 0; i < MAX_BANK_ROOT_DIRS; ++i)
            if(xmlcfg.enterbranch("PRESETSROOT", i)) {
                cfg.presetsDirList[i] = xmlcfg.getparstr("presets_root", "");
                xmlcfg.exitbranch();
            }

        xmlcfg.exitbranch();
    }

    cfg.OscilSize = (int) powf(2, ceil(logf(cfg.OscilSize - 1.0f) / logf(2.0f)));
}

void Config::SaveConfig(const std::string& filename)
{
    XMLwrapper *xmlcfg = new XMLwrapper();

    xmlcfg->beginbranch("CONFIGURATION");

    xmlcfg->addpar("sample_rate", cfg.SampleRate);
    xmlcfg->addpar("sound_buffer_size", cfg.SoundBufferSize);
    xmlcfg->addpar("oscil_size", cfg.OscilSize);
    xmlcfg->addpar("swap_stereo", cfg.SwapStereo);

    xmlcfg->addpar("check_pad_synth", cfg.CheckPADsynth);

    xmlcfg->addparstr("bank_current", cfg.currentBankDir);


    for(int i = 0; i < MAX_BANK_ROOT_DIRS; ++i)
        if(!cfg.bankRootDirList[i].empty()) {
            xmlcfg->beginbranch("BANKROOT", i);
            xmlcfg->addparstr("bank_root", cfg.bankRootDirList[i]);
            xmlcfg->endbranch();
        }

    for(int i = 0; i < MAX_BANK_ROOT_DIRS; ++i)
        if(!cfg.presetsDirList[i].empty()) {
            xmlcfg->beginbranch("PRESETSROOT", i);
            xmlcfg->addparstr("presets_root", cfg.presetsDirList[i]);
            xmlcfg->endbranch();
        }

    xmlcfg->addpar("interpolation", cfg.Interpolation);

    xmlcfg->endbranch();

    xmlcfg->saveXMLfile(filename);

    delete (xmlcfg);
}

std::string Config::GetConfigFileName()
{
#ifdef WIN32
    char win[256] = { 0 };
    _getcwd(win, 256);
    return std::string(win) + std::string("/.zynaddsubfxXML.cfg");
#else
    return std::string(getenv("HOME")) + std::string("/.zynaddsubfxXML.cfg");
#endif
}
