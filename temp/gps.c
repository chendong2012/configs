#include "kal_trace.h"
#include "stack_common.h"
#include "stack_msgs.h"
#include "syscomp_config.h"
#include "app_ltlcom.h"         /* Task message communiction */
#include "stacklib.h"           /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* Event scheduler */
#include "task_config.h"        /* Task creation */
#include "dcl.h"

kal_bool gps_CLI_enable=KAL_FALSE;

extern void cli_rx_callback(void);
extern kal_uint8 u1CliUseUartPort;
extern kal_uint8 u1CliUseUartModID;

kal_semid	gps_sem_id = NULL;

extern void gps_kal_take_sem(void);
extern void gps_kal_give_sem(void);
extern void gps_kal_sleep_task(kal_uint32 time_in_ms);

DCL_STATUS gps_UART_function(DCL_DEV port, DCL_CTRL_CMD cmd, DCL_CTRL_DATA_T *data)
{
	DCL_STATUS status;
	DCL_HANDLE handle;

	handle = DclSerialPort_Open(port,0);
	status = DclSerialPort_Control(handle, cmd, data);
	DclSerialPort_Close(handle);
	return status;
}

DCL_UINT16 gps_UART_GetBytes(DCL_DEV port, DCL_UINT8 *Buffaddr, DCL_UINT16 Length, DCL_UINT8 *status, DCL_UINT32 ownerid)
{
	UART_CTRL_GET_BYTES_T data;
	data.u4OwenrId = ownerid;
	data.u2Length = Length;
	data.puBuffaddr = Buffaddr;

	gps_UART_function(port, SIO_CMD_GET_BYTES, (DCL_CTRL_DATA_T*)&data);
	return data.u2RetSize;
}

DCL_UINT16 gps_UART_GetBytesAvail(DCL_DEV port)
{
	UART_CTRL_RX_AVAIL_T data;

	gps_UART_function(port, SIO_CMD_GET_RX_AVAIL, (DCL_CTRL_DATA_T*)&data);
	return data.u2RetSize;
}


DCL_UINT16 gps_UART_GetTxRoomLeft(DCL_DEV port)
{
	UART_CTRL_TX_AVAIL_T data;

	gps_UART_function(port, SIO_CMD_GET_TX_AVAIL, (DCL_CTRL_DATA_T*)&data);
	return data.u2RetSize;
}

DCL_UINT16 gps_UART_PutBytes(DCL_DEV port, DCL_UINT8 *Buffaddr, DCL_UINT16 Length, DCL_UINT32 ownerid)
{
	UART_CTRL_PUT_BYTES_T data;
	data.u4OwenrId = ownerid;
	data.u2Length = Length;
	data.puBuffaddr = Buffaddr;

	gps_UART_function(port, SIO_CMD_PUT_BYTES, (DCL_CTRL_DATA_T*)&data);
	return data.u2RetSize;	
}

void gps_PutUARTByte(DCL_DEV port, DCL_UINT8 data)
{
	UART_CTRL_PUT_UART_BYTE_T data1;
	data1.uData = data;
	gps_UART_function(port, SIO_CMD_PUT_UART_BYTE, (DCL_CTRL_DATA_T*)&data1);
}

void gps_UART_TurnOnPower(UART_PORT port, kal_bool enable)
{
	UART_CTRL_POWERON_T data;
	data.bFlag_Poweron = enable;

	gps_UART_function(port, UART_CMD_POWER_ON, (DCL_CTRL_DATA_T*)&data);
}

kal_bool gps_UART_Open(DCL_DEV port, DCL_UINT32 ownerid)
{
	UART_CTRL_OPEN_T data;
	DCL_HANDLE handle;

	DCL_STATUS status;
	data.u4OwenrId = ownerid;

	status = gps_UART_function(port, SIO_CMD_OPEN, (DCL_CTRL_DATA_T*)&data);
	if(STATUS_OK != status)
		return KAL_FALSE;
	else
		return KAL_TRUE;
}

void gps_UART_Register_RX_cb(DCL_DEV port, DCL_UINT32 ownerid, DCL_UART_RX_FUNC func)
{
	UART_CTRL_REG_RX_CB_T data;
	DCL_HANDLE handle;
	data.u4OwenrId = ownerid;
	data.func = func;

	gps_UART_function(port, SIO_CMD_REG_RX_CB, (DCL_CTRL_DATA_T*)&data);
}

void gps_UART_SetDCBConfig(DCL_DEV port, UART_CONFIG_T *UART_Config, DCL_UINT32 ownerid)
{
	UART_CTRL_DCB_T data;
	DCL_HANDLE handle;

	data.u4OwenrId = ownerid;
	data.rUARTConfig.u4Baud = UART_Config->u4Baud;
	data.rUARTConfig.u1DataBits = UART_Config->u1DataBits;
	data.rUARTConfig.u1StopBits = UART_Config->u1StopBits;
	data.rUARTConfig.u1Parity = UART_Config->u1DataBits;
	data.rUARTConfig.u1FlowControl = UART_Config->u1FlowControl;
	data.rUARTConfig.ucXonChar = UART_Config->ucXonChar;
	data.rUARTConfig.ucXoffChar = UART_Config->ucXoffChar;
	data.rUARTConfig.fgDSRCheck = UART_Config->fgDSRCheck;

	gps_UART_function(port, SIO_CMD_SET_DCB_CONFIG, (DCL_CTRL_DATA_T*)&data);
}

void gps_UART_Close(DCL_DEV port, DCL_UINT32 ownerid)
{
	UART_CTRL_CLOSE_T data;
	DCL_HANDLE handle;
	data.u4OwenrId = ownerid;

	gps_UART_function(port, SIO_CMD_CLOSE, (DCL_CTRL_DATA_T*)&data);
}


kal_int8 CLI_RECIEVE_CMD_DONE = 0;
void gps_task_uartRx(DCL_DEV port)
{
	//		 cli_rx_callback();
	CLI_RECIEVE_CMD_DONE = 1;
}

static UART_CONFIG_T gps_uart_dcb=
{
	UART_BAUD_9600,
	LEN_8,
	SB_1,
	PA_NONE,
	FC_NONE,
	0,
	0,
	DCL_FALSE
};

kal_uint8 gps_init(DCL_DEV port)
{
	kal_bool ret;
	gps_UART_TurnOnPower(port, KAL_TRUE);
	ret= gps_UART_Open(port, MOD_GPS);
	if(ret) {	
		gps_UART_Register_RX_cb(port, MOD_GPS, gps_task_uartRx);
		gps_UART_SetDCBConfig(port,&gps_uart_dcb, MOD_GPS);

		u1CliUseUartPort=port;
		u1CliUseUartModID=MOD_GPS;
	}

	return ret;
}

extern kal_bool bgps_powerOn;
kal_eventgrpid gps_event_id;
void gps_task_main(task_entry_struct *task_entry_ptr)
{
	kal_uint32 event_group;
	int wait_ms;

	while(1) {
		kal_retrieve_eg_events(gps_event_id,1,KAL_OR_CONSUME,&event_group,KAL_SUSPEND);
		/*!!!*/
		gps_kal_take_sem();

		if(gps_CLI_enable && (u1CliUseUartPort==uart_port_null)) {
			gps_init(uart_port2);	//	For build error	 DCL_UART1
		}
		if(gps_CLI_enable) {
			if(CLI_RECIEVE_CMD_DONE == 1) {
				cli_rx_callback();
				CLI_RECIEVE_CMD_DONE =0;
			}
		}

		/**!!!!!!!!!!!!!!!!!!**/
		wait_ms=gpsMainloop();
		/**********************/

		if(!gps_CLI_enable) {
			gps_UART_Close(u1CliUseUartPort, MOD_GPS);
			u1CliUseUartPort=uart_port_null;
		}

		gps_kal_give_sem();
		gps_kal_sleep_task((wait_ms));
		if(bgps_powerOn)
			kal_set_eg_events(gps_event_id,1,KAL_OR);

	}
}

kal_bool gps_task_init(task_indx_type task_indx)
{
	u1CliUseUartPort=uart_port_null;
	gps_sem_id = kal_create_sem("GPS_SEM", 1);
	gps_event_id = kal_create_event_group("gps_wait_events");
	return KAL_TRUE;
}

kal_bool gps_task_reset(task_indx_type task_indx)
{
	return KAL_TRUE;
}

/*task entry!!!*/
kal_bool gps_task_create(comptask_handler_struct **handle)
{
	static const comptask_handler_struct gps_task_handler_info = {
		gps_task_main,  /* task entry function */
		gps_task_init,	/* task initialization function */
		NULL,		/* task configuration function */
		gps_task_reset,	/* task reset handler */
		NULL,		/* task termination handler */
	};

	*handle = (comptask_handler_struct*) & gps_task_handler_info;
	return KAL_TRUE;
}

/*Coded by chendong
 *Email:china.gd.sz.cd@gmail.com
 *Tel:135-433-29239
 */
