#ifndef _GENERAL_HEADER_2356235632463623234286
#define _GENERAL_HEADER_2356235632463623234286


#define MAX_PDU_BUFF_SIZE				65
#define TIME_STAMP_ID					0x00
#define CAN_CMD_ID						0x0D
#define PDU_STATIC_WRITE				0x81
#define PDU_STATIC_READ					0x82
#define ZERO_ID							0x00
#define VOLTAGE_ID						0x01
#define CURRENT_ID						0x02
#define PDU_BUFF_END					0xFF
#define PDU_BUFF_NULL					0x00
#define PDU_BUFF_ERR					0xEE
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
#define STR_PDU_UNITS					_T("CS_Device_Units.ini")
#define STR_LOG_FILE_NM					_T("DEVICE_LOG.txt")
#define ST_SECT_OUTLET_STATE			_T("GetOutletState")
#define ST_SECT_OCBSTATE 				_T("OcbState")
#define STR_SEC_PDU_SETTINGS			_T("PDU_SETTINGS")
#define STR_KEY_TOT_BANKS				_T("TOT_BANKS")
#define STR_KEY_TOT_OUTLETS_PB			_T("TOT_OUTLETS_PER_BANK")
#define STR_STAGGER_ON_DELAY			_T("STAGGER_ON_DELAY")
#define STR_STAGGER_OFF_DELAY			_T("STAGGER_OFF_DELAY")
#define STR_FINAL_RELASE				_T("FINAL_RELEASE")
#define STR_LOG_FILE_PATH				_T("LOG_FILE_PATH")
#define STR_NEXT_LINE					_T("\r\n")


typedef enum _COLMN_IDx
{
	PDU_MODEL_NM = 1,
	PDU_SERIAL_NUM,
	PDU_MODEL_NUM,
	PDU_MANUFAC_DATE,
	PDU_HOST_NM,
	PDU_MAC_ADDRS,
	PDU_STAGGER_ON_DELAY,
	PDU_STAGGER_OFF_DELAY,
	PDU_FIRMWARE_VERSION,
	PDU_TOTAL_OUTLETS,
	PDU_TOTAL_BANKS,
	PDU_OUTLETS_PER_BANK,
	PDU_PHASE_INP_CONN_TYPE,
	PDU_NETWORK_ENB_DHCP,
	PDU_NETWORK_IP_ADDRS,
	PDU_NETWORK_SUBNET_MSK,
	PDU_NETWORK_DEF_GATEWAY,
	PDU_NETWORK_PRIM_DNS_ADDRS,
	PDU_NETWORK_SECN_DNS_ADDRS,
	PDU_SNMP_READ_COMMUNITY,
	PDU_SNMP_WRITE_COMMUNITY,
	PDU_SNMP_ENB_SNMP_TRAP,
	PDU_SNMP_RECEVR_IP_ADDRS,
	PDU_SNMP_RECEVR_PORT_NUM,
	PDU_EMAIL_ENB_EMAIL_NOTIFY,
	PDU_EMAIL_SMTP_SERVER_ADDRS,
	PDU_EMAIL_SMTP_PORT_NUM,
	PDU_EMAIL_USER_NAME,
	PDU_EMAIL_PASSWORD,
	PDU_EMAIL_RECEVR_EMAIL_ID,
	PDU_GENERAL_SYSTEM_NAME,
	PDU_GENERAL_SYSTEM_LOCATE,
	PDU_GENERAL_SYSTEM_CONTACT,
	PDU_GET_SET_OUTLET_NAME,
	PDU_ALERT_ENB_VCB,
	PDU_ALERT_VCB_MAX_GRAC_PER,
	PDU_ALERT_VCB_THRES_VALUE,
	PDU_ALERT_ENB_LCA,
	PDU_ALERT_LCA_MAX_GRAC_PER,
	PDU_ALERT_LCA_THRES_VALUE,
	PDU_ALERT_ENB_HCA,
	PDU_ALERT_HCA_MAX_GRAC_PER,
	PDU_ALERT_HCA_THRES_VALUE,
	PDU_FACTORY_RESET,
	PDU_PRODUCT_GROUP,
	PDU_BANK_PHASE_ID,
	PDU_CAPACITY,
	PDU_BANK_CAPACITY,
	PDU_OUTLET_CAPACITY,
	PDU_MONITOR_ONLY,
	PDU_FULL_CONTROL,
	PDU_OUTLET_VOLTAGE,
	PDU_OUTLET_FREQUENCY,
	PDU_SET_DATE_TIME,
	PDU_WRITE_OVER
}COLMN_ID;





#pragma pack(1)
typedef	struct	tagTESTx
{
	DWORD	dwInstance;

} TEST, *PTEST;
#pragma pack()

#pragma pack(1)
typedef	struct PDU_SETTINGSx
{
	BYTE		byTotalOCB;							// Total BANKS in PDU
	BYTE		byNumOfOutletsPerOCB;				// Total number of Outlets per Bank in PDU
	DWORD		dwStaggerOnDelay;					// STAGGER ON DELAY
	DWORD		dwStaggerOffDelay;					// STAGGER OFF DELAY
	BYTE		byFinalRelease;						// Outlet Relay state in Final release

} PDU_SETTINGS, *PPDU_SETTINGS;

typedef enum _STATUS_TXT_COLORx
{
	BLACK_TEXT = 0,
	RED_TEXT,
	GREEN_TEXT,
	BLUE_TEXT

} STATUS_TXT_COLOR;

#pragma pack(1)
typedef struct _CS_DEVICE_INFOx
{
	BYTE			byModelNum[22];
	BYTE			bySerialNum[16];
	BYTE			byTotBanks;
	BYTE			byTotOutlets;
	BYTE			byOutletsPerBank;
	BYTE			byOCBNum[48];
	BYTE			byRealOutletNum[48];

} CS_DEV_INFO, *PCS_DEV_INFO;

#pragma pack()
























#endif           /////////////////////  _GENERAL_HEADER_2356235632463623234286