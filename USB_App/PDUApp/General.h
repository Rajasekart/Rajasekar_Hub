#ifndef _GENERAL_HEADER_08032013
#define _GENERAL_HEADER_08032013


#define STR_COUNT_DOWN_FONT_NAME		_T("Arial")
#define PDUAPI_DLL_NM					_T("ePowerUSB.dll")
#define STR_PDU_CONFG_DB_NM				_T("ePowerConfg.db")
#define STR_PDU_ALERT_MESS_TABLE		_T("pduAlertMess")

#define ALERT_TABLE_COLUMN_TOT			7		// Actually it is 8 (we don't need the last column)
#define ALERT_TABLE_SER_NO				_T("SER_NO")
#define ALERT_TABLE_PDU_NAME			_T("PDU_NAME")
#define ALERT_TABLE_BANK_NUM			_T("BANK_NUM")
#define ALERT_TABLE_OUTLET_NUM			_T("OUTLET_NUM")
#define ALERT_TABLE_ALERT_TYPE			_T("ALERT_TYPE")
#define ALERT_TABLE_THRES_VALUE			_T("THRES_VALUE")
#define ALERT_TABLE_DATE_TIME			_T("DATE_TIME")
#define ALERT_TABLE_STATUS_NUM			_T("STATUS_NUM")

#define RELAY_OPEN						0x00		// OUTLET POWER OFF
#define RELAY_CLOSE						0x80		// OUTLET POWER ON
#define RELAY_REBOOT					0x01		// OUTLET POWER REBOOT
#define RELAY_TRIP						0x02		// OUTLET POWER TRIP
#define RELAY_SHED						0x03		// OUTLET POWER SHED
#define RELAY_STATUS					0xFE		// OUTLET RELAY STATUS
#define RELAY_UNKNOWN					0xFF		// OUTLET RELAY STATUS UNKNOWN

#define PDU_ICON						0
#define BANK_ICON						1
#define OUTLET_OFF_ICON					2
#define OUTLET_ON_ICON					3
#define OUTLET_TRIP_ICON				4
#define OUTLET_SHED_ICON				5

#define LOG_SERNUM						0
#define LOG_MESS_TYPE					1
#define LOG_MESSAGE						2
#define LOG_MESS_DATE					3

#define OUTLET_PER_OCB					4
// To read static information from selected PDU
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
//#else
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

#define UPDATE_PDU_DATA			6

typedef enum _MESS_STATUSx
{
	MESS_DELETED				= 0,
	MESS_CREATED				= 1,
	MESS_UPDATED,

} MESS_STATUS;

typedef enum _TABLE_COLUMN_IDx
{
	TABLE_SER_NO_COL_ID = 0,
	TABLE_PDU_NAME_COL_ID = 1,
	TABLE_BANK_NUM_COL_ID,
	TABLE_OCB_NUM_COL_ID,
	TABLE_OUTLET_NUM_COL_ID,
	TABLE_REAL_OUTLET_NUM_COL_ID,
	TABLE_ALERT_TYPE_COL_ID,
	TABLE_THRES_VALUE_COL_ID,
	TABLE_DATE_TIME_COL_ID,
	TABLE_STATUS_NUM_COL_ID,	

}TABLE_COLUMN_ID;

typedef enum _ALERT_TYPES_x
{
	ALERT_RESERVED_0x00		= 0x00,
	ALERT_VCB				= 0x01,	/* IEEE 754 */
	ALERT_HIGH_CURRENT		= 0x02,	/* IEEE 754 */
	ALERT_LOW_CURRENT		= 0x03,	/* IEEE 754 */
	ALERT_RESERVED_0xFF		= 0xFF

}ALERT_TYPES;

typedef enum _TREELIST_COL_IDx
{
	VOLTAGE_COL_ID = 1,
	CURRENT_COL_ID,
	POWER_FACTOR_COL_ID,
	FREQUENCY_COL_ID,
	APPARENT_POWER_COL_ID,
	REAL_POWER_COL_ID,

}TREELIST_COL_ID;

typedef enum _TREE_ITEM_EVENTx
{
	TREE_ITEM_LCLICK = 1,
	TREE_ITEM_LDCLICK,
	TREE_ITEM_RCLICK,
	TREE_ITEM_RDCLICK,
	TREE_ITEM_ITEMCHANGED,
	
}TREE_ITEM_EVENT;

typedef enum _TREELIST_POSx
{
	ITEM_NONE = 0,
	PARENT_PDU = 1,
	CHILD_BANK,
	CHILD_OUTLET,

}TREELIST_POS;

typedef enum _MESS_TYPESx
{
	LOG_MESS_NO_IMAGE = -1,
	LOG_MESS_HIGH_VOLT = 0,
	LOG_MESS_LOW_VOLT = 1,
	LOG_MESS_FAIL = 2,
	LOG_MESS_PASS = 3,
	LOG_MESS_WARN,
	LOG_MESS_CRIC,
}MESS_TYPES;

#pragma pack(1)
typedef	struct	tagTESTx
{
	DWORD	dwInstance;

} TEST, *PTEST;
#pragma pack()

#pragma pack(1)
typedef struct CURR_SEL_DETSx
{
	BYTE		byCurrPDUPos;
	BYTE		bySerialNum[16];
	BYTE		byPDUAddress[MAX_PATH];
	BYTE		byCurrBANKPos;
	BYTE		byCurrOCBPos;
	BYTE		byCurrRealOutlet;
	BYTE		byCurrOUTLETPos;

} CURR_SEL_DETAILS, *PCURR_SEL_DETAILS;
#pragma pack()

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
typedef struct PDU_TREE_OUTLET_DETSx
{
	BYTE							byOCBNum;							// OCB number
	BYTE							byRealOutlet;						// Outlet Number
	BYTE							byOUTLETPos;						// Position of the OUTLET in BANK N
	HTREEITEM						hOUTLETTreeHandle;					// OUTLET Tree Control Handle

	BYTE							byRelayStatus;						// It has values like ON, OFF, 

	float							fVoltage;  							//For Voltage 
	float							fCurrent;							//For Current
	float							fPoFactor;							//For Power Factor
	float							fCrFactor;							//For Crest Factor
	float							fFrequency;							//For Frequency
	float							fApparentPow;						//For Apparent Power
	float							fReEnergy;							//For Real Energy

	struct PDU_TREE_OUTLET_DETSx	*pNextOutletInfo;					// Next OUTLET Information
	struct PDU_TREE_OUTLET_DETSx	*pPrevOutletInfo;					// Previous OUTLET Information

} PDU_OUTLET_INFO, *PPDU_OUTLET_INFO;
#pragma pack()

#pragma pack(1)
typedef struct PDU_TREE_BANK_DETSx
{
	BYTE							byBANKPos;							// Position of the BANK in PDU Device
	HTREEITEM						hBANKTreeHandle;					// BANK Tree Control Handle
	DWORD							dwTotOUTLET;						// Total OUTLET available in the BANK
	PPDU_OUTLET_INFO				pOutletInfo;						// Pointer of the OUTLET Information

	struct PDU_TREE_BANK_DETSx		*pNextBankInfo;						// Next BANK Information
	struct PDU_TREE_BANK_DETSx		*pPrevBankInfo;						// Previous BANK Information

} PDU_BANK_INFO, *PPDU_BANK_INFO;
#pragma pack()

#pragma pack(1)
typedef	struct PDU_TREE_DETAILSx
{
	BYTE							byPDUPos;							// Position of the PDU Device in the Tree List
	BYTE							bySerialNum[16];
	BYTE							byPDUAddrs[MAX_PATH];				// PDU address (path)
	HTREEITEM						hPDUTreeHandle;						// PDU Tree Control Handle
	BYTE							byTotBANK;							// Total BANK available in the PDU
	PPDU_BANK_INFO					pBankInfo;							// Pointer of the BANK Information

	struct PDU_TREE_DETAILSx		*pNextPDUInfo;						// Next PDU Information
	struct PDU_TREE_DETAILSx		*pPrevPDUInfo;						// Previous PDU Information

} PDU_TREE_DET, *PPDU_TREE_DET;
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

#pragma pack(1)
typedef struct PDU_STATIC_INFORMx
{
	BYTE			byModelNm[42];
	BYTE			bySerialNo[14];
	BYTE			byModelNo[22];
	BYTE			byManfDate[14];
	DWORD			dwStaggOnDelay;
	DWORD			dwStaggOffDelay;
	BYTE			byFirmwareVer[6];
	UINT			nTotalOutlets;
	UINT			nTotalBanks;
	UINT			nOutletsPerBank;
	UINT			nPhaseType;
	BYTE			byInpConnType;
	
} PDU_STATIC_INTO, *PPDU_STATIC_INFO;
#pragma pack()


//============================= GLOBAL VARIABLES =======================================
extern	PPDU_TREE_DET				gpHeadPDUInfo;
extern	PPDU_TREE_DET				gpCurrPDUInfo;
extern  PPDU_TREE_DET				gpNewPDUInfo;
extern	PPDU_BANK_INFO				gpNewBankInfo;
extern  PPDU_BANK_INFO				gpCurrBankInfo;
extern  PPDU_OUTLET_INFO			gpNewOutletInfo;
extern  PPDU_OUTLET_INFO			gpCurrOutletInfo;

// Functions

VOID *Malloc(size_t size);
VOID Free(PVOID pNew);

//============================= Interfaces ==============================================
typedef BOOL (*FN_GETPDUDEVICE)(PPDU_ADDRESS pPdu_address, WORD wVID, WORD wPID, BYTE *byErr, BOOL bLog);
typedef BOOL (*FN_GET_BANK_OCB_COUNTS)(BYTE *byPduadd, PBANK_OUTLET_INFO pBankOcbInfo, BYTE *byErrCode, BOOL bLog);
//__declspec(dllexport) BOOL PduGetBankOcbInformation(BYTE *byPduadd, PBANK_OCB_INFO pBankOcbInfo, BYTE *byErrCode, BOOL bLog)
typedef BOOL (*FN_GET_OUTLET_DATA) (CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, POUTLET_DATA_INFO pOcb_Data_Info, BYTE *byErrCode, BOOL bLog);
//__declspec(dllexport) BOOL PduGetOutletData(CHAR *byPduadd, BYTE byBank, BYTE byOCB, POUTLET_DATA_INFO pOcb_Data_Info, BYTE *byErrCode, BOOL bLog)
typedef BOOL (*FN_SET_OUTLET_STATE) (CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE byCurrState, BYTE byInpValue, BYTE *byErrCode, BOOL bLog);
//__declspec(dllexport) BOOL PduSwitchOutletState(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE byCurrState, BYTE byInpValue, BYTE *byErrCode, BOOL bLog);
typedef BOOL (*FN_GET_STATIC_INFO)(CHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BYTE *byErrCode, BOOL bLog, BYTE byOutletNum);
//__declspec(dllexport) BOOL PduStaticInfoWriteAndRead(CHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BYTE *byErrCode, BOOL bLog);
typedef BOOL (*FN_GET_OCB_RELAY_STATE)(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE *byCurrState, BYTE *byErrCode, BOOL bLog);
//__declspec(dllexport) BOOL PduGetOcbState(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE *byOcbValue, BYTE *byErrCode, BOOL bLog);
typedef BOOL (*FN_STORE_OUTLET_ALERT_SETTINGS)(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, PDU_OUTLET_ALERT_INFO stPDUOutletAlertInfo, BYTE *byErrCode, BOOL bLog);
//__declspec(dllexport) BOOL PduStoreOutletAlertSettings(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, PDU_OUTLET_ALERT_INFO stPDUOutletAlertInfo, BYTE *byErrCode, BOOL bLog);
typedef BOOL (*FN_GET_OUTLET_ALERT_SETTINGS) (CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, PPDU_OUTLET_ALERT_INFO pPDUOutletAlertInfo, BYTE *byErrCode, BOOL bLog);
//__declspec(dllexport) BOOL PduGetOutletAlertSettings(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOutlet, PPDU_OUTLET_ALERT_INFO pPDUOutletAlertInfo, BYTE *byErrCode, BOOL bLog);
typedef BOOL (*FN_INITIATE_ALERT_EVENT) (BYTE *byErrCode, BOOL bLog);
//__declspec(dllexport) BOOL PduInitiateAlertEvent(BYTE *byErrCode, BOOL bLog)
typedef BOOL (*FN_CLOSE_ALL_ALERT_EVENT) (BYTE *byErrCode, BOOL bLog);
//__declspec(dllexport) BOOL PduCloseAllAlertEvent(BYTE *byErrCode, BOOL bLog)
typedef BOOL (*FN_GET_OUTLET_STATE_FRM_CONFIG)(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog);
//__declspec(dllexport) BOOL PduGetOutletStateFromConfig(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog)
typedef BOOL (*FN_STORE_OUTLET_STATE_TO_CONFIG)(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE byStateValue, BYTE *byErrCode, BOOL bLog);
//__declspec(dllexport) BOOL PduStoreOutletStateToConfig(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOutlet, BYTE byStateValue, BYTE *byErrCode, BOOL bLog)
typedef BOOL (*FN_GET_OUTLET_STATE)(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog);
//__declspec(dllexport) BOOL PduGetOutletState(CHAR *byPduadd, BYTE byBank, BYTE byOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog)



UINT ThreadGetNUpdatePDUData(PVOID pParam);
UINT ThreadGetAlertMess(PVOID pParam);

#endif
