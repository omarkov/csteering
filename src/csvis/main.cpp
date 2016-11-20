// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Oliver Markovic <entrox@entrox.org> 
//   All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  o Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer. 
//  o Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution. 
//  o Neither the name of the author nor the names of the contributors may be
//    used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <osg/DisplaySettings>
#include "Application.h"
#include "Server.h"
#include "FANClasses.h"
#include "CommonServer.h"



void *visStarter(void *p)
{
    try {
        Application app((osg::ArgumentParser*)p);

        // enter the event loop
        app.run();
    } catch(std::exception& ex) {
        std::cout << "Caught unhandled exception:\n  " << ex.what() << std::endl;
    }

    return NULL;
}


int main( int argc, char **argv )
{
    // parse out arguments
    osg::ArgumentParser args(&argc, argv);

    // setup the usage document
    std::string appName = args.getApplicationName();
    osg::ApplicationUsage* appUsage = args.getApplicationUsage();

    appUsage->setApplicationName(appName);
    appUsage->setDescription(appName + " is the StuPro CS visualisation component (v1.0)");
    appUsage->setCommandLineUsage(appName + " [options]");
    appUsage->addCommandLineOption("-h or --help", "Display this information");
    appUsage->addCommandLineOption("-d <path> or --data-path <path>", "Set the data path");
    appUsage->addCommandLineOption("-vp <port> or --vis-port <port>", "The port on which the client listens");
    appUsage->addCommandLineOption("-sh <host> or --sim-host <host>", "Simulation server host");
    appUsage->addCommandLineOption("-sp <port> or --sim-port <host>", "Simulation server port");

    // add OSG default options
    osg::DisplaySettings::instance()->readCommandLine(args);

    // if user requests help, write it out to cout.
    if (args.read("-h") || args.read("--help")) {
        appUsage->write(std::cout);
        return 0;
    }

    // report any errors if they have occured when parsing the program arguments.
    if (args.errors()) {
        args.writeErrorMessages(std::cout);
        return 1;
    }

    g_fan = new FAN("../etc/vis.conf",false);
    g_fan->installIntHandler();

    // pass arguments to the FAN config
    {
        char *str; // ugly, but necessary because of non-constness in FAN_hash

        // match the client port
        std::string vis_port;
        if (args.read("-vp", vis_port) || args.read("--vis-port", vis_port)) {
            str = strdup(vis_port.c_str());
            FAN::app->config->insert("VISPORT", str);
            free(str);
        }

        // match the simulation host & port
        std::string sim_host;
        if (args.read("-sh", sim_host) || args.read("--sim-host", sim_host)) {
            str = strdup(sim_host.c_str());
            FAN::app->config->insert("MODELHOST", str);
            free(str);
        }

        std::string sim_port;
        if (args.read("-sp", sim_port) || args.read("--sim-port", sim_port)) {
            str = strdup(sim_port.c_str());
            FAN::app->config->insert("MODELPORT", str);
            free(str);
        }
    }

    FAN_init();


    FAN_createDomain("vis", true);
    FAN_loadExtCmd("vis::setSimStatus", &rSetSimStatus, true, true);
    FAN_loadExtCmd("vis::setSteps", &rSetSteps, true, true);
    FAN_loadExtCmd("vis::setSimulationBound", &rSetSimulationBound, true, true);
    FAN_loadExtCmd("vis::loadFile", &rLoadFile, true, true);
    FAN_loadExtCmd("vis::startData", &rStartData, true, true);
    FAN_loadExtCmd("vis::putData", &rPutData, true, true);
    FAN_loadExtCmd("vis::stopData", &rStopData, true, true);
    FAN_loadExtCmd("vis::setServerStatus", &rSetServerStatus, true, true);
    FAN_loadExtCmd("vis::putSample", &rPutSample, true, true);
    FAN_ThreadedDaemon *d = new FAN_ThreadedDaemon(NULL, "visbindhost", "visport");

    modelBuffer  = FAN::app->config->getValue("MODELBUFFER", "1000000");
    modelHost    = strdup(FAN::app->config->getValue("MODELHOST", "127.0.0.1"));
    modelPort    = atoi(FAN::app->config->getValue("MODELPORT", "30002"));

    FAN_Hash *master = FAN_initMasterHandler();
    FAN_registerHandler(master, "startData", &mStartData);
    FAN_registerHandler(master, "putData", &mPutData);
    FAN_registerHandler(master, "stopData", &mStopData);
    FAN_registerHandler(master, "login", &mLogin);
    masterCom = (FAN_Com*)master->getPointer("COM");

    FAN_registerMaster(&visStarter, (void*)&args);

    cout << FAN::app->config->getValue("VISPORT", "-1") << endl;
    cout << FAN::app->config->getValue("MODELHOST", "-1") << endl;
    cout << FAN::app->config->getValue("MODELPORT", "-1") << endl;

    while(!d->bindDaemon())
    {
        FAN_wait(2,0);
        FAN_xlog(FAN_DEBUG | FAN_SOCKET, "Retry");
    }

    delete d;
    delete g_fan;

    return 0;
}
