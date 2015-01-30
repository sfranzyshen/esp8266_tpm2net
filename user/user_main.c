#include "mem.h"
#include "user_interface.h"
#include "espconn.h"
#include "ws2812.h"
#include "tpm2net.h"

#define procTaskPrio        0
#define procTaskQueueLen    1

//Tasks that happen all the time.
os_event_t procTaskQueue[procTaskQueueLen];
static void ICACHE_FLASH_ATTR procTask(os_event_t *events)
{
	system_os_post(procTaskPrio, 0, 0 );
	if( events->sig == 0 && events->par == 0 )
	{
		//Idle Event.
	}
}

void user_init(void)
{
	int wifiMode = wifi_get_opmode();

	wifi_set_opmode( 2 ); //We broadcast our ESSID, wait for peopel to join.

	tpm2net_init();

	wifi_station_dhcpc_start();

	ws2812_init();

	//Add a process
	system_os_task(procTask, procTaskPrio, procTaskQueue, procTaskQueueLen);
	system_os_post(procTaskPrio, 0, 0 );
}


