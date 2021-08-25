#ifdef _MSC_VER // is Microsoft compiler?
#   if _MSC_VER < 1300  // is 'old' VC 6 compiler?
#       pragma warning( disable : 4786 ) // 'identifier was truncated to '255' characters in the debug information'
#   endif // #if _MSC_VER < 1300
#endif // #ifdef _MSC_VER
#include <iostream>
#include <apps/Common/exampleHelper.h>
#include <mvIMPACT_CPP/mvIMPACT_acquire.h>
#include <mvIMPACT_CPP/mvIMPACT_acquire_GenICam.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>

#ifdef _WIN32
#   define USE_DISPLAY
#   include <mvDisplay/Include/mvIMPACT_acquire_display.h>
using namespace mvIMPACT::acquire::display;
#endif // #ifdef _WIN32

using namespace mvIMPACT::acquire;
using namespace std;


int pow(int e, int b);
uint32_t str_to_int(string IPblock);
uint32_t  IPStr_To_Numeric(string strIPaddress);
string IPNumeric_To_String(uint32_t IPaddresse);

int pow(int e, int b)
{
  int t=1;
  int result = 1;
  while(t<=b)
  {
    result=e*result;
    t++;
  }
  return result;
}

uint32_t str_to_int(string IPblock)
{
	int stringlength = 0;
	stringlength = IPblock.length();
	uint32_t tmpInt = 0;
	uint32_t result = 0;
	int powresult = 0;
	for (int index = 0; index < stringlength; index++)
	{
	    powresult = pow(10, stringlength- index - 1);
		tmpInt = (IPblock[index] - 0x30)* (uint32_t)powresult;
		result += tmpInt;
	}
	return result;
}

uint32_t  IPStr_To_Numeric(string strIPaddress)
{
	uint32_t IPNumeric = 0;
	uint32_t block1, block2, block3, block4 = 0;
	int position = 0;
	string tmptext = "";

	for (int index = 0; index < 3; index++)
	{
		position = strIPaddress.find(".");
		tmptext = strIPaddress.substr(0, position);
		strIPaddress = strIPaddress.substr(position + 1, strIPaddress.length());

		if (index == 0){ block1 = str_to_int(tmptext); }

		if (index == 1){ block2 = str_to_int(tmptext); }

		if (index == 2){
			block3 = str_to_int(tmptext);
			block4= str_to_int(strIPaddress);
		}

	}

	IPNumeric =  (block1 << 24) + (block2 << 16) + (block3 << 8) + (block4);
	return IPNumeric;

}

string IPNumeric_To_String(uint32_t IPaddresse)
{
	uint32_t intIP = IPaddresse;
	uint32_t block1, block2, block3, block4 = 0;
	block1 = ((intIP & 0xff000000) >> 24);
	block2 = ((intIP & 0x00ff0000) >> 16);
	block3 = ((intIP & 0x0000ff00) >> 8);
	block4 = ((intIP & 0x000000ff));

	string strIP = to_string(block1) + '.' + to_string(block2) + '.' + to_string(block3) + '.' + to_string(block4);
	return strIP;

}
//-----------------------------------------------------------------------------
int main( void )
//-----------------------------------------------------------------------------
{


	DeviceManager devMgr;

	mvIMPACT::acquire::GenICam::SystemModule sm(int64_type(0));
	const int64_type interfaceCnt = sm.getInterfaceModuleCount();
	cout << "******************This program is used for configure the IP, without UI******************"<< endl;
	cout << "Run sequence:" << endl;
	cout << "1. find ethernet interface and find the camera on the interface" << endl;
	cout << "2. set the temporary IP to the camera" << endl;
	cout << "3. open the camera£¬set fix persistent IP, restart the camera, persistent IP will be valid after restart." << endl;
	cout << "********************************************************************" << endl;
	cout << "Discover " << interfaceCnt << " GIGE/USB interface on the PC:"<< endl;

	unsigned int index = 0;
	while (index < interfaceCnt) {
		sm.interfaceSelector.write(index);
		if (sm.interfaceType.readS() == "GEV") {
			cout << "index[" << index << "]:" << sm.interfaceDisplayName.readS() << ",IP address:" << IPNumeric_To_String(sm.gevInterfaceDefaultIPAddress.read()) << endl;
		}
		index++;
	}

	cout << "above interface are GEV interface, please choose the one with your camera:";

	unsigned int InterfaceNr = 0;
	std::cin >> InterfaceNr;
	// remove the '\n' from the stream
	std::cin.get();

	sm.interfaceSelector.write(InterfaceNr);
	mvIMPACT::acquire::GenICam::InterfaceModule im(InterfaceNr);
	cout << "open the interface:" << im.interfaceDisplayName.readS() << endl;

	im.deviceSelector.write(0);
	im.mvGevAdvancedDeviceDiscoveryEnable.write(bTrue);
	im.deviceUpdateList.call();
	cout << "********************************************************************" << endl;
	cout << "Use advanced discover mode to find the camera, camreas not in the same subnet can be found:" << im.deviceSerialNumber.readS() << im.deviceID.readS() << endl;
	cout << "the IP of connected camera:" << endl;
	cout << "IP:" << IPNumeric_To_String(im.gevDeviceIPAddress.read()) << endl;
	cout << "Subnet:" << IPNumeric_To_String(im.gevDeviceSubnetMask.read()) << endl;

	cout << "*******************************************************************" << endl;
	cout << "Want to force a temporary IP address? y/n?" << endl;

	string bforceIPChange = "";
	std::cin >> bforceIPChange;
	// remove the '\n' from the stream
	std::cin.get();

	string ipAddresse = "";
	string subNet = "";
	string gateWay = "";

	if (bforceIPChange == "y") {
		cout << "input IP addresse, interface IP:" <<  IPNumeric_To_String(sm.gevInterfaceDefaultIPAddress.read()) << ":" ;
		std::cin >> ipAddresse;
		// remove the '\n' from the stream
		std::cin.get();
		im.gevDeviceForceIPAddress.write(IPStr_To_Numeric(ipAddresse));

		cout << "input subnet, interface subnet:" << IPNumeric_To_String(sm.gevInterfaceDefaultSubnetMask.read()) << ":";
		std::cin >> subNet;
		// remove the '\n' from the stream
		std::cin.get();
		im.gevDeviceForceSubnetMask.write(IPStr_To_Numeric(subNet));

		cout << "input gateway, interface gateway:" << IPNumeric_To_String(sm.gevInterfaceDefaultGateway.read()) << ":";
		std::cin >> gateWay;
		// remove the '\n' from the stream
		std::cin.get();
		im.gevDeviceForceGateway.write(IPStr_To_Numeric(gateWay));
        cout << "set IP now "<< endl;
		im.gevDeviceForceIP.call();
	}

	cout << "*******************************************************************" << endl;
	cout << "update device manager" << endl;

	/*************************************************************************/

	devMgr.updateDeviceList();

	cout << "Discover followed device, input index + enter to choose the camera:" << endl;
    Device* pDev = getDeviceFromUserInput( devMgr );
    if( !pDev )
    {
        cout << "Unable to continue! Press [ENTER] to end the application" << endl;
        cin.get();
        return 1;
    }


    try
    {
        pDev->open();
    }
    catch( const ImpactAcquireException& e )
    {
        // this e.g. might happen if the same device is already opened in another process...
        cout << "An error occurred while opening the device(error code: " << e.getErrorCode() << ")." << endl
             << "Press [ENTER] to end the application" << endl;
        cin.get();
        return 1;
    }


    FunctionInterface fi( pDev );
	mvIMPACT::acquire::GenICam::DeviceControl dc(pDev);
	mvIMPACT::acquire::GenICam::TransportLayerControl tlc(pDev);
	cout << "*******************************************************************" << endl;
	cout << "Camera will use followed IP after start:" << endl;
	cout << "persistent IP:" << IPNumeric_To_String(tlc.gevPersistentIPAddress.read()) << endl;
	cout << "persistent subnet:" << IPNumeric_To_String(tlc.gevPersistentSubnetMask.read()) << endl;
	cout << "persistent gateway:" << IPNumeric_To_String(tlc.gevPersistentDefaultGateway.read()) << endl;

	/*************set fix IP********/
	string bfixIPChange = "";
	if (ipAddresse == "" || subNet == "" || gateWay == "") {

		cout << "you want to change persistent IP?  y/n?" << endl;
		std::cin >> bfixIPChange;
		// remove the '\n' from the stream
		std::cin.get();
        if (bfixIPChange == "y"){
		cout << "input IP addresse, interface IP:" <<  IPNumeric_To_String(sm.gevInterfaceDefaultIPAddress.read()) << ":" ;
		std::cin >> ipAddresse;
		// remove the '\n' from the stream
		std::cin.get();

		cout << "input subnet, interface subnet:" << IPNumeric_To_String(sm.gevInterfaceDefaultSubnetMask.read()) << ":";
		std::cin >> subNet;
		// remove the '\n' from the stream
		std::cin.get();

		cout << "input gateway, , interface gateway:" << IPNumeric_To_String(sm.gevInterfaceDefaultGateway.read()) << ":";
		std::cin >> gateWay;
		// remove the '\n' from the stream
		std::cin.get();
		}

	}
	else
	{
		cout << "you want to set input ip as persistent IP? y/n?" << endl;
		std::cin >> bfixIPChange;
		// remove the '\n' from the stream
		std::cin.get();
	}


	if (bfixIPChange == "y"){

    tlc.gevInterfaceSelector.write(0);
    tlc.gevCurrentIPConfigurationDHCP.write(bFalse);
    tlc.gevCurrentIPConfigurationPersistentIP.write(bTrue);
    tlc.gevPersistentIPAddress.write(IPStr_To_Numeric(ipAddresse));
    tlc.gevPersistentSubnetMask.write(IPStr_To_Numeric(subNet));
    tlc.gevPersistentDefaultGateway.write(IPStr_To_Numeric(gateWay));
	cout << "configuration finished, camera is restarting, wait 10 sec to reuse camera" << endl;


	sleep(2);
	dc.deviceReset.call();
	}

    cout << "Press [ENTER] to end the application" << endl;
    cin.get();
    return 0;
}
