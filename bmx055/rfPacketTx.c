/***** Includes *****/
#include <stdlib.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Drivers */
#include <ti/drivers/rf/RF.h>
#include <ti/display/Display.h>

/* Board Header files */
#include "Board.h"
#include "smartrf_settings.h"
#include "bmx055.h"

/* For usleep() */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

/***** Defines *****/
#define TX_TASK_STACK_SIZE 1024
#define TX_TASK_PRIORITY   2

/* Packet TX Configuration */
#define PAYLOAD_LENGTH      30
#define PACKET_INTERVAL     (uint32_t)(4000000*0.5f) /* Set packet interval to 500ms */



/***** Prototypes *****/
void txTaskFunction(void);
void read_data_and_send_packet(void);
extern Display_Handle display;

/***** Variable declarations *****/
Task_Params txTaskParams;
Task_Struct txTask;    /* not  so you can see in ROV */
 uint8_t txTaskStack[TX_TASK_STACK_SIZE];

 RF_Object rfObject;
 RF_Handle rfHandle;

uint32_t curtime;
uint8_t packet[PAYLOAD_LENGTH];
uint16_t seqNumber;
PIN_Handle pinHandle;

void txTaskFunction(void)
{

    RF_Params rfParams;
    RF_Params_init(&rfParams);

    RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
    RF_cmdPropTx.pPkt = packet;
    RF_cmdPropTx.startTrigger.triggerType = TRIG_ABSTIME;
    RF_cmdPropTx.startTrigger.pastTrig = 1;
    RF_cmdPropTx.startTime = 0;

    /* Request access to the radio */
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);

    /* Set the frequency */
    RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

    /* Get current time */
    curtime = RF_getCurrentTime();
}


void read_data_and_send_packet(void)
{
       /* Create packet with incrementing sequence number and random payload */

           packet[0] = (uint8_t)(seqNumber >> 8);
           packet[1] = (uint8_t)(seqNumber++);

//            for(i = 2; i < PAYLOAD_LENGTH; i++)
//           {
//               packet[i] = rand();
//           }
           readAccelData(&packet[2]);

           Display_printf(display,0,0, "in read_data\n");
           /* Set absolute TX time to utilize automatic power management */
           curtime += PACKET_INTERVAL;
           RF_cmdPropTx.startTime = curtime;

           /* Send packet */
           RF_EventMask result = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal, NULL, 0);
           if (!(result & RF_EventLastCmdDone))
           {
               /* Error */
               while(1);
           }
 }
