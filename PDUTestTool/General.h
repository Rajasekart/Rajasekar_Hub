#ifndef _GENERAL_HEADER_04324322346
#define _GENERAL_HEADER_04324322346

#define PM10_4_CHANNELS					0x30	/* PM10 - each channel running independently */
#define PM10_3_PHASE_DELTA				0x31/* PM10-Delta config; ch 1-3 as phase A, B, C, ch4 is not used */ 
#define PM9S9_3_PHASE_DELTA				0x32/* PM9S9-Delta config;one MCU controls one (1) 3-phase channel. */ 
#define PM9S9_SINGLE_PHASE_9_CHANNELS	0x33/* PM9S9-single phase; one MCU controls 3 channels */
#define PM10_PLUS_NETWORK_4_CHANNELS	0x34	/* PM10 with Network circuitry - each channel running independently */
#define PM10_PLUS_NETWORK_3_PHASE_DELTA 0x35 /* PM10 with Network circuity - Delta config - channel 1-3 as phase A, B, C, ch4 is not used */

#define MINUS_ONE						-1
#define HARDWARE_MODEL					0x02
#define RELAY_CHANGE					0x1D		// CHANGE OCB ID
#define RELAY_RESET						0x1E		// OCB RESET
#define RELAY_STATE						0x07
#define RELAY_OPEN						0x00		// OUTLET POWER OFF
#define RELAY_CLOSE						0x80		// OUTLET POWER ON
#define RELAY_REBOOT					0x01		// OUTLET POWER REBOOT
#define RELAY_TRIP						0x02		// OUTLET POWER TRIP
#define RELAY_SHED						0x03		// OUTLET POWER SHED
#define RELAY_STATUS					0xFE		// OUTLET RELAY STATUS
#define RELAY_UNKNOWN					0xFF		// OUTLET RELAY STATUS UNKNOWN
#define MAX_PDU_BUFF_SIZE				65
#define ZERO_ID							0x00
#define PDU_BUFF_END					0xFF
#define PDU_BUFF_NULL					0x00
#define PDU_BUFF_ERR					0xEE

#define PDUAPI_DLL_NM					_T("ePowerUSB.dll")
#define STR_PDU_UNITS					_T("CS_Device_Units.ini")
#define STR_LOG_FILE_NM					_T("DEVICE_LOG.txt")
#define STR_PDU_TEST_LOG				_T("PDU_Test_Log.txt")
#define STR_PDU_CHNG_ID_LOG				_T("PDU_Change_OCB_ID_Log.txt")
#define STR_SEC_PDU_SETTINGS			_T("PDU_SETTINGS")
#define STR_KEY_TOT_BANKS				_T("TOT_BANKS")
#define STR_KEY_TOT_OUTLETS_PB			_T("TOT_OUTLETS_PER_BANK")
#define STR_KEY_OCB_SEL1				_T("OCB_SEL1")
#define STR_KEY_OCB_SEL2				_T("OCB_SEL2")
#define STR_KEY_OCB_SEL3				_T("OCB_SEL3")
#define STR_KEY_OCB_SEL4				_T("OCB_SEL4")
#define STR_KEY_OCB_SEL5				_T("OCB_SEL5")
#define STR_KEY_OCB_SEL6				_T("OCB_SEL6")
#define STR_STAGGER_ON_DELAY			_T("STAGGER_ON_DELAY")
#define STR_STAGGER_OFF_DELAY			_T("STAGGER_OFF_DELAY")
#define STR_FINAL_RELASE				_T("FINAL_RELEASE")
#define STR_LOG_FILE_PATH				_T("LOG_FILE_PATH")
#define STR_NEXT_LINE					_T("\r\n")
#define STR_INIT_WRITE_CMD				_T("INITIATE_WRITE_CMD")
#define STR_PDU_LOG						_T("DEVICE_LOG")
#define STR_DEVICE_HANDLE				_T("DEVICE_HANDLE")
#define STR_DEV_DATA_WRTN_BYTES			_T("DATA_WRITN_BYTES")
#define STR_DEV_DATA_READ_BYTES			_T("DATA_READ_BYTES")
#define STR_DEVICE_DETAILS				_T("DEVICE_DETAILS")
#define STR_CLOSE_DEV_HAND				_T("DEVICE_HANDLE_CLOSE")
#define STR_TOTAL						_T("TOTAL")
#define STR_VID_NO						_T("VID_%d")
#define STR_PID_NO						_T("PID_%d")
#define STR_FAIL						_T("FAIL")
#define STR_DEVICE_DETECT				_T("DEVICE_DETECTION")
#define STR_DEVICE_PATH					_T("DEVICE_PATH")
#define STR_DEVICE_VID					_T("DEVICE_VID")
#define STR_DEVICE_PID					_T("DEVICE_PID")
#define STR_PID_VID						_T("PID_VID_STATUS")
#define ST_SECT_OCBSTATE 				_T("OcbState")
#define ST_SECT_CHANGEOCB 				_T("ChangeOcb")
#define ST_SECT_OUTLET_STATE			_T("GetOutletState")


#pragma pack(1)
typedef	struct	tagTESTx
{
	DWORD	dwInstance;

} TEST, *PTEST;
#pragma pack()

typedef enum _USER_OPTIONSx
{
	OPT_RELAY_TEST = 1,
	OPT_OCB_CHANGE = 2

} USER_OPTIONS;

typedef enum _USER_RESULTx
{
	UNKNOWN_ANS = 0,
	YES_ANS,
	NO_ANS

} USER_RES;

typedef enum _STATUS_TXT_COLORx
{
	BLACK_TEXT = 0,
	RED_TEXT,
	GREEN_TEXT,
	BLUE_TEXT

} STATUS_TXT_COLOR;

#pragma pack(1)
typedef	struct PDU_SETTINGSx
{
	BYTE		byTotalOCB;							// Total BANKS in PDU
	BYTE		byNumOfOutletsPerOCB;				// Total number of Outlets per Bank in PDU
	BYTE		byNoOfOCB;							// Total OCB selected to process
	BYTE		byOCBids[6];						// 6 OCB ids to process
	BYTE		byOCB_ID;							// Selected OCB ID
	DWORD		dwStaggerOnDelay;					// STAGGER ON DELAY
	DWORD		dwStaggerOffDelay;					// STAGGER OFF DELAY
	BYTE		byFinalRelease;						// Outlet Relay state in Final release

} PDU_SETTINGS, *PPDU_SETTINGS;



















#endif