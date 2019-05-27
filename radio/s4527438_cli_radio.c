/** 
 **************************************************************
 * @file mylib/s4527438_cli_pantilt.c
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib cli pantilt driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 */
/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "FreeRTOS_CLI.h"

#include "s4527438_cli_radio.h"
#include "s4527438_os_radio.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define X_MAX_VALUE     319
#define Y_MAX_VALUE     319
#define Z_MAX_VALUE     199

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t current_x_value = 0;
static uint32_t current_y_value = 0;
static uint32_t current_z_value = 0;
/* Private function prototypes -----------------------------------------------*/
void s4527438_cli_radio_init(void);

static BaseType_t prvRadioSetChan(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioGetChan(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioSetTxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioSetRxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioGetRxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioGetTxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioJoin(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvRadioOriginCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioMoveCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioHeadCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioVacuumCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvRadioLoadSorter(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioLoadOrb(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

CLI_Command_Definition_t xRadioOrigin = {  /* Structure that defines the "pan" command line command. */
    "origin",
    "origin: origin \r\n",
    prvRadioOriginCommand,
    0
};

CLI_Command_Definition_t xRadioMove = {  /* Structure that defines the "tilt" command line command. */
    "move",
    "move: move <x coordinate> <y coordinate> \r\n",
    prvRadioMoveCommand,
    2
};

CLI_Command_Definition_t xRadioHead = {  /* Structure that defines the "pan" command line command. */
    "head",
    "head: head <raise|lower> \r\n",
    prvRadioHeadCommand,
    1
};

CLI_Command_Definition_t xRadioVacuum = {  /* Structure that defines the "pan" command line command. */
    "vacuum",
    "vacuum: vacuum <on|off> \r\n",
    prvRadioVacuumCommand,
    1
};

/***********************************************************************************************************/
CLI_Command_Definition_t xRadioSetChan = {  /* Structure that defines the "pan" command line command. */
    "setchan",
    "setchan: setchan <chan> \r\n",
    prvRadioSetChan,
    1
};

CLI_Command_Definition_t xRadioGetChan = {  /* Structure that defines the "pan" command line command. */
    "getchan",
    "getchan: getchan \r\n",
    prvRadioGetChan,
    0
};

CLI_Command_Definition_t xRadioSetTxAddr = {  /* Structure that defines the "pan" command line command. */
    "settxaddr",
    "settxaddr: settxaddr <TXaddress(HEX)> \r\n",
    prvRadioSetTxAddr,
    1
};

CLI_Command_Definition_t xRadioSetRxAddr = {  /* Structure that defines the "pan" command line command. */
    "setrxaddr",
    "setrxaddr: setrxaddr <RXaddress(HEX)> \r\n",
    prvRadioSetRxAddr,
    1
};

CLI_Command_Definition_t xRadioGetRxAddr = {  /* Structure that defines the "pan" command line command. */
    "getrxaddr",
    "getrxaddr: getrxaddr \r\n",
    prvRadioGetRxAddr,
    0
};

CLI_Command_Definition_t xRadioGetTxAddr = {  /* Structure that defines the "pan" command line command. */
    "gettxaddr",
    "gettxaddr: gettxaddr \r\n",
    prvRadioGetTxAddr,
    0
};

CLI_Command_Definition_t xRadioJoin = {  /* Structure that defines the "pan" command line command. */
    "join",
    "join: join \r\n",
    prvRadioJoin,
    0
};

/***********************************************************************************************************/
CLI_Command_Definition_t xRadioLoadSorter = {  /* Structure that defines the "pan" command line command. */
    "loadsorter",
    "loadsorter: loadsorter <LAB sorter index(1 ~ 4))> \r\n",
    prvRadioLoadSorter,
    1
};

CLI_Command_Definition_t xRadioLoadOrb = {  /* Structure that defines the "pan" command line command. */
    "loadorb",
    "loadorb: loadorb <LAB ORB index(1 ~ 4)> \r\n",
    prvRadioLoadOrb,
    1
};

void s4527438_cli_radio_init(void) {
    /* Register CLI commands */
    FreeRTOS_CLIRegisterCommand(&xRadioSetChan);
    FreeRTOS_CLIRegisterCommand(&xRadioGetChan);
    FreeRTOS_CLIRegisterCommand(&xRadioSetTxAddr);
    FreeRTOS_CLIRegisterCommand(&xRadioSetRxAddr);
    FreeRTOS_CLIRegisterCommand(&xRadioGetRxAddr);
    FreeRTOS_CLIRegisterCommand(&xRadioGetTxAddr);
    FreeRTOS_CLIRegisterCommand(&xRadioJoin);

    /* Register CLI commands */
    FreeRTOS_CLIRegisterCommand(&xRadioOrigin);
    FreeRTOS_CLIRegisterCommand(&xRadioMove);
    FreeRTOS_CLIRegisterCommand(&xRadioHead);
    FreeRTOS_CLIRegisterCommand(&xRadioVacuum);

    /* Register CLI commands */
    FreeRTOS_CLIRegisterCommand(&xRadioLoadSorter);
    FreeRTOS_CLIRegisterCommand(&xRadioLoadOrb);
}

/***********************************************************************************************************/
static BaseType_t prvRadioSetChan(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t chan_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        chan_index = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_set_chan(chan_index);

    return returnedValue;
}

static BaseType_t prvRadioGetChan(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
 
    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    s4527438_os_radio_get_chan();

    return returnedValue;
}

static BaseType_t prvRadioSetTxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t chan_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    uint8_t string_buffer[RADIO_OS_TX_RX_ADDR_STRING_WIDTH + 1];

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;

        memset(string_buffer,0x00,sizeof(string_buffer));
        memcpy(string_buffer,cCmd_string,lParam_len);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_set_txaddr(string_buffer);

    return returnedValue;
}

static BaseType_t prvRadioSetRxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t chan_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    uint8_t string_buffer[RADIO_OS_TX_RX_ADDR_STRING_WIDTH + 1];

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;

        memset(string_buffer,0x00,sizeof(string_buffer));
        memcpy(string_buffer,cCmd_string,lParam_len);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_set_rxaddr(string_buffer);

    return returnedValue;
}

static BaseType_t prvRadioGetRxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
 
    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    s4527438_os_radio_get_rxaddr();

    return returnedValue;
}

static BaseType_t prvRadioGetTxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
 
    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    s4527438_os_radio_get_txaddr();

    return returnedValue;
}

static BaseType_t prvRadioJoin(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
 
    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    s4527438_os_radio_send_join_packet();

    return returnedValue;
}
/***********************************************************************************************************/
static BaseType_t prvRadioOriginCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
 
    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    s4527438_os_radio_send_xyz_packet(0, 0, 0);

    return returnedValue;
}

static BaseType_t prvRadioMoveCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t x_coordinate = 0;
    uint32_t y_coordinate = 0;
    uint32_t z_coordinate = current_z_value;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        x_coordinate = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    /* Get parameters 2 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 2, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        y_coordinate = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    current_x_value = x_coordinate;
    current_y_value = y_coordinate;
    current_z_value = z_coordinate;
    s4527438_os_radio_send_xyz_packet(current_x_value, current_y_value, current_z_value);

    return returnedValue;
}

static BaseType_t prvRadioHeadCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t x_coordinate = current_x_value;
    uint32_t y_coordinate = current_y_value;
    uint32_t z_coordinate = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( strcmp(cCmd_string,"raise") == 0 ) {
        current_x_value = x_coordinate;
        current_y_value = y_coordinate;
        current_z_value = 0;
    } else if( strcmp(cCmd_string,"lower") == 0 ) {
        current_x_value = x_coordinate;
        current_y_value = y_coordinate;
        current_z_value = Z_MAX_VALUE;
    } else {
        return returnedValue;
    }

    s4527438_os_radio_send_xyz_packet(current_x_value, current_y_value, current_z_value);

    return returnedValue;
}

static BaseType_t prvRadioVacuumCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    Message_Vacuum_Action_Enum vacuum_action    =   VACUUM_OFF;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( strcmp(cCmd_string,"on") == 0 ) {
        vacuum_action    =   VACUUM_ON;
    } else if( strcmp(cCmd_string,"off") == 0 ) {
        vacuum_action    =   VACUUM_OFF;
    } else {
        return returnedValue;
    }
    s4527438_os_radio_send_vacuum_packet(vacuum_action);

    return returnedValue;
}

/***********************************************************************************************************/
static BaseType_t prvRadioLoadSorter(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t sorter_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        sorter_index = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_load_sorter_setting(sorter_index);

    return returnedValue;
}

static BaseType_t prvRadioLoadOrb(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t orb_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        orb_index = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_load_orb_setting(orb_index);

    return returnedValue;
}
