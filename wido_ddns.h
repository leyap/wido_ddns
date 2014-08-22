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
 *	Author:				HuoSen, lisper <lisper.li@dfrobot.com>
 *	Date:				2014-08-21
 *	official website:	http://www.dfrobot.com
 *	Description:		ddns that run on wido board
 */

#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "utility/debug.h"
#include "utility/socket.h"

class wido_ddns {
	public:
		Adafruit_CC3000 *_cc3000;
		wido_ddns (Adafruit_CC3000 &my_cc3000);
		char checkipData[20];
		uint32_t ddnsip;
		char ddns_get_strings[90];
		uint32_t ddns_update_time;// = 1000*2;
		uint32_t checkip;

		void set_ddnsip (uint32_t myddnsip);
		void set_string (char *username, char *password, char *hostname);
		void set_delay_time (uint32_t my_time);
		void update ();

};

