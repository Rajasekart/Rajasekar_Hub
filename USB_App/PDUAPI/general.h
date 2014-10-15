#ifndef _GEN_DATA_DEFN_22665544128
	#define _GEN_DATA_DEFN_22665544128

#define VOLTAGE_ID				0x01
#define CURRENT_ID				0x02
#define POWER_FACTOR_ID			0x03
#define CREST_FACTOR_ID			0x04
#define FREQUENCY_ID			0x05
#define REAL_ENERGY_ID			0x06

#define CAN_CMD_ID				0x0D
#define ZERO_ID					0x00
#define TIME_STAMP_ID			0x00
#define PDU_STATIC_WRITE		0x81
#define PDU_STATIC_READ			0x82

#define PDU_BUFF_END			0xFF
#define PDU_BUFF_NULL			0x00
#define PDU_BUFF_ERR			0xEE

// ------------- VCB COMMANDS ------------
#define CSCAN_CONFIG_VCB		0x0C
#define CSCAN_ENABLE_VCB		0x15

// To set 1.80999...
#define VCB_MIN_DELAY_VAL1		0xBF	
#define VCB_MIN_DELAY_VAL2		0xE7	
#define VCB_MIN_DELAY_VAL3		0xAE	
#define VCB_MIN_DELAY_VAL4		0x14

// To set 0.30
#define VCB_CURVE_CONSTANT_VAL1	0x3E	
#define VCB_CURVE_CONSTANT_VAL2 0x99	
#define VCB_CURVE_CONSTANT_VAL3 0x99	
#define VCB_CURVE_CONSTANT_VAL4 0x9A

// To set 999.9
#define VCB_THRESHOLD_VAL1		0x44	
#define VCB_THRESHOLD_VAL2		0x79	
#define VCB_THRESHOLD_VAL3		0xF9	
#define VCB_THRESHOLD_VAL4		0x9A

#define MAX_PDU_BUFF_SIZE		65

typedef enum _VCB_CONFIGx 
{
	VCB_METHOD			= 0x00,
	VCB_THRESHOLD		= 0x01,
	VCB_MAX_DELAY		= 0x02,
	VCB_MIN_DELAY		= 0x03,
	VCB_CURVE_CONSTANT	= 0x04

}VCB_CONFIG;

typedef enum _VCB_ENB_VALx
{
	VCB_ENABLE	= 0x01,
	VCB_DISABLE	= 0x02

}VCB_ENB_VAL;
//---------------------------------------

#define RELAY_STATE				0x07
#define RELAY_OPEN				0x00		// OUTLET POWER OFF
#define RELAY_CLOSE				0x80		// OUTLET POWER ON
#define RELAY_REBOOT			0x01		// OUTLET POWER REBOOT
#define RELAY_TRIP				0x02		// OUTLET POWER TRIP
#define RELAY_SHED				0x03		// OUTLET POWER SHED
#define RELAY_STATUS			0xFE		// OUTLET RELAY STATUS
#define RELAY_UNKNOWN			0xFF		// OUTLET RELAY STATUS UNKNOWN

/* -----------------------------------------------------------------------
Below are the define for writing static information on PDU .	
----------------------------------------------------------------------- */
#if 0
#define MODEL_NAME				0x01
#define MODEL_NUMBER			0x02
#define SERIAL_NO				0x03  
#define FIRMWARE_VER			0x04
#define INPUT_CURR_RATING  		0x05
#define RATED_VOLTAGE			0x06
#define RATED_POWER				0x07
#define NUM_OF_OUTLETS			0x08
#define NUM_OF_BANKS			0x09
#define NUM_OF_OUTLETS_PER_BANK	0x0A
#define NUM_OF_INPUTS			0x0B

#define MODEL_NAME				0x01
#define MODEL_NUMBER			0x03
#define SERIAL_NO				0x02  
#define FIRMWARE_VER			0x09
#define INPUT_CURR_RATING  		0x05
#define RATED_VOLTAGE			0x06
#define RATED_POWER				0x07
#define NUM_OF_OUTLETS			0x08
#define NUM_OF_BANKS			0x04
#define NUM_OF_OUTLETS_PER_BANK	0x0A
#define NUM_OF_INPUTS			0x0B
#endif

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
	PDU_FACTORY_RESET
}COLMN_ID;

typedef enum _ALERT_TYPES_x
{
	ALERT_RESERVED_0x00		= 0x00,
	ALERT_VCB				= 0x01,	/* IEEE 754 */
	ALERT_HIGH_CURRENT		= 0x02,	/* IEEE 754 */
	ALERT_LOW_CURRENT		= 0x03,	/* IEEE 754 */
	ALERT_RESERVED_0xFF		= 0xFF

}ALERT_TYPES;

typedef enum _MESS_STATUSx
{
	MESS_DELETED				= 0,
	MESS_CREATED				= 1,
	MESS_UPDATED,

} MESS_STATUS;

#pragma pack(1)
typedef	struct PDU_ADDRESSx
{
	BYTE		byTotalPdu;							// Total PDU connected to the system
	BYTE		byPduAdd[30][MAX_PATH];				// PDU address (path)

} PDU_ADDRESS, *PPDU_ADDRESS;
#pragma pack()

#pragma pack(1)
typedef	struct BANK_OUTLET_INFOx
{
	DWORD	dwTotalBanks;		// Total No of Bank on PDU
	DWORD	dwOutletsOnBank[32]; 	// Totoal OUTLET on each Bank

} BANK_OUTLET_INFO, *PBANK_OUTLET_INFO;
#pragma pack()

#pragma pack(1)
typedef	struct OUTLET_DATA_INFOx
{
	float		fVoltage;  							//For Voltage 
	float		fCurrent;							//For Current
	float		fPoFactor;							//for Power Factor
	float		fCrFactor;
	float		fFrequency;
	float		fApparentPow;
	float		fReEnergy;

} OUTLET_DATA_INFO, *POUTLET_DATA_INFO;
#pragma pack()

#pragma pack(1)
typedef struct PDU_OUTLET_ALERT_INFOx
{
	BOOL							bHCAEnabled;						// TRUE if high current alert enabled otherwise FALSE
	float							fHCAThreshold;						// Threshold value for high current alert
	UINT							nHCAMaxGrcPer;						// Max Grace Period for High current alert in seconds

	BOOL							bLCAEnabled;						// TRUE if low current alert enabled otherwise FALSE
	float							fLCAThreshold;						// Threshold value for low current alert
	UINT							nLCAMaxGrcPer;						// Max Grace Period for low current alert in seconds

	BOOL							bVCBEnabled;						// TRUE if virtual circuit breaker alert enabled otherwise FALSE
	float							fVCBThreshold;						// Threshold value for virtual circuit breaker alert
	UINT							nVCBMaxGrcPer;						// Max Grace Period for virtual circuit breaker alert in seconds

} PDU_OUTLET_ALERT_INFO, *PPDU_OUTLET_ALERT_INFO;
#pragma pack()


//Log section string 
#define ST_SECT_PDUENUM		 				"pduEnumerate"
#define ST_SECT_BANKOCB_INFO				"pduBankOcb"
#define ST_SECT_OCBSTATE 					"pduOcbState"
#define ST_SECT_OCBDATA						"pduOcbData"
#define ST_SECT_STATIC_RDWR					"PduStaticInfoRdAndWr"
#define ST_SECT_OUTLET_STATE				"PduGetOutletState"

///////////////////////////////// ALERT CONFIGURATION DEFINITIONS //////////////////
#define STR_PDU_CONG_INI_NM					"PDUCONFIG.INI"
#define STR_PDU_ALERT_MESS_INI_NM			"ALERTMESS.INI"
#define STR_PDU_CONFG_DB_NM					"ePowerConfg.db"
#define STR_ALERT_UPDATE_SEC				"ALERT_UPDATE"
#define STR_ALERT_NM_KEY					"ALERT_%d"
#define STR_THRES_VALUE						"THRES_VALUE_%d"
#define STR_DATE_TIME						"DATE_TIME_%d"

#define STR_PDU_LIST_NM						"PDU_LIST"
#define STR_PDU_SER_NUM						"PDU_SER_NUM_%d"

#define STR_PDU_KEY_NM						"PDU_NM"
#define STR_PDU_PRESENT						"PDU_AVAIL_STATUS"
#define STR_TOTAL_NM_KEY					"TOTAL"
#define STR_ALERT_BANK_KEY					"BANK_NO_%d"
#define STR_ALERT_OCB_KEY					"OCB_NO_%d"
#define STR_ALERT_OUTLET_KEY				"OUTLET_NO_%d"
#define STR_ALERT_REAL_OUTLET_KEY			"REAL_OUTLET_NO_%d"

#define STR_RELAY_STATUS_NM					"RELAY_STATUS_%d"

#define STR_ALERT_VCB_ENABLED_KEY			"VCBENABLED_%d"			// FOR VCB
#define STR_ALERT_VCB_THRESHOLD_KEY			"VCBTHRESHOLD_%d"
#define STR_ALERT_VCB_MAX_GRC_PER_KEY		"VCBMAXGRCPER_%d"
#define STR_ALERT_VCB_ALERT					"VCBALERT_%d"

#define STR_ALERT_HCA_ENABLED_KEY			"HCAENABLED_%d"			// FOR HCA
#define STR_ALERT_HCA_THRESHOLD_KEY			"HCATHRESHOLD_%d"
#define STR_ALERT_HCA_MAX_GRC_PER_KEY		"HCAMAXGRCPER_%d"
#define STR_ALERT_HCA_ALERT					"HCAALERT_%d"

#define STR_ALERT_LCA_ENABLED_KEY			"LCAENABLED_%d"			// FOR LCA
#define STR_ALERT_LCA_THRESHOLD_KEY			"LCATHRESHOLD_%d"
#define STR_ALERT_LCA_MAX_GRC_PER_KEY		"LCAMAXGRCPER_%d"
#define STR_ALERT_LCA_ALERT					"LCAALERT_%d"

#define STR_YES								"YES"
#define STR_NO								"NO"
#define STR_FAIL							"FAIL"
#define STR_VID_NO							_T("VID_%d")
#define STR_PID_NO							_T("PID_%d")
#define STR_TOTAL							_T("TOTAL")
#define STR_DEVICE_DETAILS					_T("DEVICE_DETAILS")
#define STR_PDU_UNITS						_T("CS_Device_Units.ini")

#define CONFIRMLOG(szSection, szKey, szData, szFile, bLog){\
	  										CString szMsg;\
  											szMsg.Format szData;\
											if(bLog)\
											{\
  												WritePrivateProfileString(szSection, szKey, szMsg, szFile);\
											}\
										}



/* -----------------------------------------------------------------------
	API of the PDUAPI.DLL
	----------------------------------------------------------------------- */
__declspec(dllexport)BOOL PduEnumerateDevice(PPDU_ADDRESS pPdu_address, WORD wPid, WORD wVid, BYTE *byErrCode);
__declspec(dllexport)BOOL PduGetBankOcbInformation(CHAR *byPduadd, PBANK_OUTLET_INFO pBankOcbInfo, BYTE *byErrCode, BOOL bLog);
__declspec(dllexport) BOOL PduGetOUTLETData(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, POUTLET_DATA_INFO pOcb_Data_Info, BYTE *byErrCode, BOOL bLog);
__declspec(dllexport) BOOL PduSwitchOutletState(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE byCurrState, BYTE byInpValue, BYTE *byErrCode, BOOL bLog);
__declspec(dllexport) BOOL PduGetOutletState(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog);
__declspec(dllexport) BOOL PduStaticInfoWriteAndRead(CHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BYTE *byErrCode, BOOL bLog, BYTE byOutletNum = 0);
__declspec(dllexport) BOOL PduStoreOutletAlertSettings(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, PDU_OUTLET_ALERT_INFO stPDUOutletAlertInfo, BYTE *byErrCode, BOOL bLog);
__declspec(dllexport) BOOL PduGetOutletAlertSettings(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, PPDU_OUTLET_ALERT_INFO pPDUOutletAlertInfo, BYTE *byErrCode, BOOL bLog);
__declspec(dllexport) BOOL PduInitiateAlertEvent(BYTE *byErrCode, BOOL bLog);
__declspec(dllexport) BOOL PduCloseAllAlertEvent(BYTE *byErrCode, BOOL bLog);
__declspec(dllexport) BOOL PduGetOutletStateFromConfig(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog);
__declspec(dllexport) BOOL PduStoreOutletStateToConfig(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE byStateValue, BYTE *byErrCode, BOOL bLog);
__declspec(dllexport) BOOL PduOutletConfigVCB(CHAR *byPduAdd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE byVCBEnable, float fThreshold, UINT nMaxGracPer, BYTE *byErrCode, BOOL bLog);


//void RegisterForDeviceNotifications();
BOOL LoadVidPidValues();
DWORD PduReadStaggerDelays(CHAR *byPduadd, BYTE byDelayType);
BOOL PduWriteReadBuffer(CHAR *byPduAdd, BYTE byDataBuffer[], DWORD *dwRWBytes, BOOL bWriteOnly, BYTE *byErrCode, BOOL bLog);
void GetDeviceCapabilities(CHAR *byDevPath, BOOL bLog, HANDLE hDeviceHandle);
float PduGetOutletCurrent(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE *byErrCode, BOOL bLog);


/// ---------------------- ALERT IMPLEMENTATION ------------------------------------------ ///

extern CByteArray		m_byThreadExit;
extern BOOL				g_bReadWrite;
extern BOOL				g_bQuickExit;
extern CStringArray		gcszPidValues;
extern CStringArray		gcszVidValues;
extern CString			gcszBasePath;

#pragma pack(1)
typedef struct _THRD_ALERT_PASS_FMTx
{
	UINT			nThreadPos;
	BYTE			byPDUPos;
	UINT			nIniFilePos;
	UINT			nBankNum;
	UINT			nOCBNum;
	UINT			nOutletNum;
	UINT			nRealOutlet;

} THRD_ALERT_PASS_INFO, *PTHRD_ALERT_PASS_INFO;
#pragma pack()


//UINT ThreadChkVCBAlert(PVOID pParam);
//UINT ThreadChkLowCurrAlert(PVOID pParam);
UINT ThreadChkHighCurrAlert(PVOID pParam);
int callback(void *NotUsed, int argc, char **argv, char **azColName);
BOOL UpdateAlertMessage(CString cszPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, UINT nAlertType, float fCurrThres);



#endif 
