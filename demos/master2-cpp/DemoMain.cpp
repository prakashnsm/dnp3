//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  Green Enery Corp licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//

#include "MasterDemo.h"

#include <signal.h>

#include <APL/Log.h>
#include <APL/Lock.h>

using namespace std;
using namespace apl;
using namespace apl::dnp;

#include "../cOptions.h"

SigLock gLock;
MasterDemoBase* gpDemo = NULL;

void SetDemo(MasterDemoBase* apDemo)
{
	CriticalSection cs(&gLock);
	gpDemo = apDemo;
}

void Terminate(int sig)
{
	CriticalSection cs(&gLock);
	std::cout << "Signal " << sig << ", shutdown... " << std::endl;
	if (gpDemo)
		gpDemo->Shutdown();
}

/*
 * Command line syntax:
 *
 *    master2 --help
 *
 */
int main(int argc, char* argv[])
{
	// Check command line options

	cOptions options;
	options.set_master_defaults();
	if( options.decode( argc, argv ) != 0 )
		return 1;

	try {

	// Create a log object for the stack to use and configure it
	// with a subscriber that print alls messages to the stdout.
	EventLog log;
	log.AddLogSubscriber(LogToStdio::Inst());

	// Specify a FilterLevel for the stack/physical layer to use.
	// Log statements with a lower priority will not be logged.
	const FilterLevel LOG_LEVEL = LEV_INFO;

	// Create our demo application that handles commands and
	// demonstrates how to publish data give it a logger with a
	// unique name and log level.
	MasterDemoApp app(log.GetLogger(LOG_LEVEL, "demoapp"));

	// This is the main point of interaction with the stack. The
	// AsyncStackManager object instantiates master/slave DNP
	// stacks, as well as their physical layers.
	AsyncStackManager mgr(log.GetLogger(LOG_LEVEL, "dnp"));

	if( ! options.flagcom ) {
		// Connect via a TCPClient socket to a slave.  The server will
		// wait 3000 ms in between failed bind calls.
		mgr.AddTCPClient(
			options.PortName,
			PhysLayerSettings(LOG_LEVEL, 3000),
			options.ip.c_str(),
			options.port
			);
	} else {

		// Con nect to slave over serial COM port
		mgr.AddSerial(
			options.PortName,
			PhysLayerSettings(LOG_LEVEL, 3000),
			options.serialsets 
			);
	}

	// The master config object for a master. The default are
	// useable, but understanding the options are important.
	MasterStackConfig stackConfig;

	// Override the default link addressing
	stackConfig.link.LocalAddr  = options.local_dnp3;
	stackConfig.link.RemoteAddr = options.remote_dnp3;

	// Set the app instance as a callback for state change notices
	stackConfig.master.mpObserver = &app;

	// Create a new master on a previously declared port, with a
	// name, log level, command acceptor, and config info This
	// returns a thread-safe interface used for processing Master
	// messages.
	app.SetCommandAcceptor(
		mgr.AddMaster(
			options.PortName,      // port name
			"master",              // stack name
			LOG_LEVEL,             // log filter level
			app.GetDataObserver(), // callback for data processing
			stackConfig            // stack configuration
		)
	);

	// Configure signal handlers so we can exit gracefully
	SetDemo(&app);
	signal(SIGTERM, &Terminate);
	signal(SIGABRT, &Terminate);
	signal(SIGINT,  &Terminate);

	app.Run();

	SetDemo(NULL);

	}

	// sometime the dnp3 code will raise an exception
	// for example if you try to connect to a non-existent COM port
	// this catches the exception, preventing a crash,
	// and prints out a hint as to what went wrong

	catch ( Exception & e ) {
		printf("Exception raised by DNP3 code\n%s\n",e.GetErrorString().c_str());
	}




	return 0;
}

