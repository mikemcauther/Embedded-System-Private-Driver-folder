/** 
 **************************************************************
 * @file mylib/s4527438_os_radio.h
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib radio driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 */

#ifndef S4527438_OS_RADIO_H_
#define S4527438_OS_RADIO_H_

typedef enum{
    VACUUM_ON,
    VACUUM_OFF,
} Message_Vacuum_Action_Enum;

void s4527438_os_radio_set_chan(uint32_t channel);
void s4527438_os_radio_get_chan(void);
void s4527438_os_radio_set_txaddr(uint8_t *addr);
void s4527438_os_radio_set_rxaddr(uint8_t *addr);
void s4527438_os_radio_get_txaddr(void);
void s4527438_os_radio_get_rxaddr(void);

void s4527438_os_radio_init(void);
void s4527438_os_radio_send_join_packet(void);
void s4527438_os_radio_send_xyz_packet(uint32_t x_coordinate, uint32_t y_coordinate, uint32_t z_coordinate);
void s4527438_os_radio_send_vacuum_packet(Message_Vacuum_Action_Enum vacuum_action);

void s4527438_os_radio_load_sorter_setting(uint32_t sorter_index);
void s4527438_os_radio_load_orb_setting(uint8_t orb_index);

#define RADIO_OS_TX_RX_ADDR_STRING_WIDTH   8

#endif /* S4527438_OS_RADIO_H_ */
