//ddns for arduino
//by lisper <lisper.li@dfrobot.com>
//GPL 3.0

#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "utility/debug.h"
#include "utility/socket.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   7  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
		SPI_CLOCK_DIVIDER); // you can change this clock speed



#define WLAN_SSID       "DFRobot-Internal"           // cannot be longer than 32 characters!
#define WLAN_PASS       "zwrobot2014"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define LISTEN_PORT           80   // What TCP port to listen on for connections.

uint32_t timeout;
char checkipData[20];
uint32_t ddnsip;
uint32_t ddns_checkip = cc3000.IP2U32 (209,208,4,56);
char *ddns_get_string = "GET /nic/update?u=jshield&p=dfrobot2014&hostname=outlet.ddns.info";
uint32_t ddns_update_time = 1000*60;

//
void setup(void) {
	pinMode (13, OUTPUT);
	pinMode (12, OUTPUT);
	Serial.begin(115200);
	Serial.println(F("Hello, CC3000!\n")); 
	//while (!Serial);
	//Serial.println ("Input any key to start:");
	//while (!Serial.available ());
	Serial.print("Free RAM: "); 
	Serial.println(getFreeRam(), DEC);

	/* Initialise the module */
	Serial.println(F("\nInitializing..."));
	if (!cc3000.begin()) {
		Serial.println(F("Couldn't begin()! Check your wiring?"));
		while(1);
	}

	Serial.print(F("\nAttempting to connect to ")); 
	Serial.println(WLAN_SSID);
	if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
		Serial.println(F("Failed!"));
		while(1);
	}

	Serial.println(F("Connected!"));

	Serial.println(F("Request DHCP"));
	while (!cc3000.checkDHCP()) {
		delay(100); // ToDo: Insert a DHCP timeout!
	}  

	/* Display the IP address DNS, Gateway, etc. */
	while (! displayConnectionDetails()) {
		delay(1000);
	}

	/*********************************************************/
	/* You can safely remove this to save some flash memory! */
	/*********************************************************/


	// Start listening for connections
	//webServer.begin();

	while (ddnsip == 0) {
		if (! cc3000.getHostByName("nic.changeip.com", &ddnsip)) {
			Serial.println(F("Couldn't resolve!"));
		}
		delay(500);
	}
	cc3000.printIPdotsRev(ddnsip);

	Serial.println(F("Listening for connections..."));
}



//
void loop(void) {
	///////////////////////////////////////////////////////
	ddns_update ();

	/////////////////////////////////////////////////////////
}

//
void ddns_update () {
	static uint32_t timeout = 0;
	if (millis () - timeout > ddns_update_time) {
		timeout = millis ();  
		Serial.println ("checkIP");
		Adafruit_CC3000_Client checkClient = cc3000.connectTCP (ddns_checkip, 80);
		if (checkClient.connected ()) {
			checkClient.fastrprintln ("GET /");
			checkClient.fastrprintln ("");
			while (!checkClient.available() && millis () - timeout < 1000);
			if (checkClient.available () > 0) {
				char checkipBuffer[20];
				checkClient.read (checkipBuffer, 20, 0);
				checkClient.close();
				char *sub = strchr (checkipBuffer, '\n');
				if (sub)
					*sub = '\0';
				Serial.println (checkipBuffer);
				if (strcmp (checkipData, checkipBuffer) != 0) {
					Serial.println ("ip is changed");
					Adafruit_CC3000_Client ddnsClient = cc3000.connectTCP (ddnsip, 80);     
					strcpy (checkipData, checkipBuffer);
					if (ddnsClient.connected()) {
						Serial.println ("connected ddns server");
						ddnsClient.fastrprintln (ddns_get_string);
						uint32_t ddnstimeout = millis ();
						while (!ddnsClient.available() && millis () - ddnstimeout < 1000);
						while (ddnsClient.available ()) {
							char data = ddnsClient.read ();
							Serial.print (data);
						}
					}
					ddnsClient.close();
				}

			}
		}
	}
}

bool displayConnectionDetails(void) {
	uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
	if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv)) {
		Serial.println(F("Unable to retrieve the IP Address!\r\n"));
		return false;
	} 
	else {
		Serial.print(F("\nIP Addr: ")); 
		cc3000.printIPdotsRev(ipAddress);
		Serial.print(F("\nNetmask: ")); 
		cc3000.printIPdotsRev(netmask);
		Serial.print(F("\nGateway: ")); 
		cc3000.printIPdotsRev(gateway);
		Serial.print(F("\nDHCPsrv: ")); 
		cc3000.printIPdotsRev(dhcpserv);
		Serial.print(F("\nDNSserv: ")); 
		cc3000.printIPdotsRev(dnsserv);
		Serial.println();
		return true;
	}
}


