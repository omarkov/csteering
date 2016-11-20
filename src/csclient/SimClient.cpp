#include "FANClasses.h"
#include "simRemoteTypes.h"
#include "RemoteInterface.h"

int main (int argc, char **argv)  
{    
	FAN_ENTER;

	if(argc < 3)
	{
	        cout << "\nUsage: client [modelhost] [modelport]\n\n";
		return 0;
	}

	char *host = argv[1];
	char *port = argv[2];

	FAN *app = new FAN("../etc/model.conf",false);

	FAN_Connection *conn = app->connect( NULL, host, port);

	if(conn != NULL)
	{
        conn->rpc("model::logout");
        conn->rpc("sim::stop");
        conn->rpc("sys::halt");
		conn->rpc("exit", 0);

		delete conn;	
	}

	delete app;
	FAN_RETURN 0;
}
