/*
 * Copyright (C) 2014 DFRobot                                                  
 *                                                                             
 * wido_ddns is free software: you can redistribute it and/or         
 * modify it under the terms of the GNU General Public License as       
 * published by the Free Software Foundation, either version 3 of              
 * the License, or any later version.                                          
 *                                                                             
 * wido_ddns is distributed in the hope that it will be useful,       
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               
 * GNU General Public License for more details.                         
 *                                                                             
 * wido_ddns is distributed in the hope that it will be useful,       
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               
 * GNU General Public License for more details.                         
 *                                                                             
 * You should have received a copy of the GNU General Public            
 * License along with wido_ddns. If not, see                          
 * <http://www.gnu.org/licenses/>.                                             
 *                                                                             
 */

/*
 *	name:				wido_ddns library
 *	version:			0.1
 *	Author:				lisper <lisper.li@dfrobot.com>
 *	Date:				2014-08-21
 *	official website:	http://www.dfrobot.com
 *	Description:		ddns that run on wido board
 */

#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "utility/debug.h"
#include "utility/socket.h"
#include "wido_ddns.h"

//
wido_ddns::wido_ddns (Adafruit_CC3000 &my_cc3000) {
	_cc3000 = &my_cc3000;
	checkip = _cc3000->IP2U32 (209,208,4,56);
	ddns_update_time = 6000000;
}

//
void wido_ddns::set_ddnsip (uint32_t myddnsip) {
	ddnsip = myddnsip;
}

//
void wido_ddns::set_ddns_get_string (char *username, char *password, char *hostname) {
	sprintf (ddns_get_strings, "GET /nic/update?u=%s&p=%s&hostname=%s", 
			username, 
			password, 
			hostname);
}

//
void wido_ddns::set_ddns_time (uint32_t my_time) {
	ddns_update_time = my_time;
	//Serial.print ("time = ");
	//Serial.println (ddns_update_time);
}

//
void wido_ddns::ddns_update () {
	static uint32_t timeout = 0;
	if (millis () - timeout > ddns_update_time) {
		timeout = millis ();  
		Serial.println ("checkIP");
		Adafruit_CC3000_Client checkClient = _cc3000->connectTCP (checkip, 80);
		if (checkClient.connected ()) {
			checkClient.fastrprintln ("GET /");
			checkClient.fastrprintln ("");
			uint32_t checkip_timeout = millis ();
			while (!checkClient.available() && millis () - checkip_timeout < 1000);
			if (checkClient.available () > 0) {
				char checkipBuffer[20];
				checkClient.read (checkipBuffer, 20, 0);
				checkClient.close();
				char *sub = strchr (checkipBuffer, '\n');
				if (sub)
					*sub = '\0';
				Serial.print ("myip is: ");
				Serial.println (checkipBuffer);
				if (strcmp (checkipData, checkipBuffer) != 0) {
					Serial.println ("ip is changed");
					strcpy (checkipData, checkipBuffer);
					//Serial.println ("connecting ddns server..."); 
					Adafruit_CC3000_Client ddnsClient = _cc3000->connectTCP (ddnsip, 80);   
					if (ddnsClient.connected()) {
						//Serial.println ("connected ddns server");  
						ddnsClient.fastrprintln (ddns_get_strings);
						//Serial.println (ddns_get_strings);
						uint32_t ddnstimeout = millis ();
						while (!ddnsClient.available() && millis () - ddnstimeout < 1000);
						while (ddnsClient.available ()) {
							char data = ddnsClient.read ();
							Serial.print (data);
						}
						Serial.println ();
					} else
						Serial.println ("Can't connect ddns server");
					ddnsClient.close();
				} else 
					Serial.println ("ip is not changed");
			} else 
				Serial.println ("error! can't get checkip data");
		} else 
			Serial.println ("error! can't connect checkip server!");
	}
}

