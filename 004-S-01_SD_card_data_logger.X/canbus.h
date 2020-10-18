// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CANBUS_H
#define	CANBUS_H

#include <stdint.h>

#define CAN_BUS_SEND_PERIOD_MS  1000

// Initializes the can bus.
void can_bus_init(void);

// Sends and receives messages.
// Should be called once per main loop.
void can_bus_process(void);

#define NODE_ID_MG_BATTERY              0x02
#define NODE_ID_MG_MPPT                 0x04
#define NODE_ID_MG_MPPT_TOTAL           10
#define NODE_ID_SUNFLARE_MPPT           0x20
#define NODE_ID_SUNFLARE_MPPT_TOTAL     8
#define NODE_ID_SLS                     0x10
#define NODE_ID_FOIL_CONTROL            0x11


typedef struct {
    uint16_t voltage_mv;
    int16_t current_10ma;
    int16_t discharge_current_10ma;
    int16_t charge_current_10ma;
    uint8_t soc;
    uint16_t time_to_go_min;
    uint32_t bms_state;
    uint8_t temp[4];
    uint16_t cell_voltage_mv[12];
    uint8_t power_level;
}mg_battery_t;

typedef struct {
    int16_t current_in_ma;
    uint16_t voltage_in_mv;
    uint16_t voltage_out_mv;
    int16_t power_in_100mw;
}mg_mppt_t;

typedef struct {
    uint32_t mppt_status;
    uint32_t solder_jumper_status;
    uint16_t solar_input_voltage_in_mv;
    uint16_t solar_input_current_in_ma;
    uint16_t current_channel_1_in_ma;
    uint16_t current_channel_2_in_ma;
    uint32_t solar_inpunt_power_in_mw;
    uint16_t batt_output_voltage_in_mv;
    int16_t  boost_pd_error_in_mv;
    int16_t  boost_pd_derivitive_in_mv_dt;
    int32_t  boost_power_change_in_mw;
    uint32_t boost_req_output_power_in_mw;
    uint16_t boost_req_solar_input_in_ma;
    int32_t  mppt_delta_solar_power_in_mw;
    int16_t  mppt_delta_solar_input_current_in_ma;
    int16_t  mppt_step_change_in_ma;
    int16_t  mppt_req_solar_input_current_in_ma;
}sunflare_mppt_t;

typedef struct {
    uint32_t status;
    uint32_t limiting;
    int16_t temp_power_100mdeg;
    int16_t temp_electronics_100mdeg;
    int16_t temp_motor_1_100mdeg;
    int16_t temp_motor_2_100mdeg;
    uint16_t uzk_10mv;
    int16_t motor_current_100ma;
    int16_t input_currect_100ma;
    int16_t rpm;
}sls_t;

typedef struct {
    uint16_t primary_input_position;
    uint16_t primary_output_position;
}foil_control_t;

mg_battery_t get_can_data_mg_battery(void);

mg_mppt_t get_can_data_mg_mppt(uint8_t nr);

sunflare_mppt_t get_can_data_sunflare_mppt(uint8_t nr);

sls_t get_can_data_sls(void);

foil_control_t get_can_data_foil_control(void);

#endif	
