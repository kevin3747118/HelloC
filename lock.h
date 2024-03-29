/*
 * lock.h
 *
 *  Created on: 2022�~12��22��
 *      Author: Fred
 */

#ifndef LOCK_H_
#define LOCK_H_

#ifdef ENKORE_D2
#define DEVICE_MODEL                    "EKS-D7X1A"
#endif

#ifdef ENKORE_W1
#define DEVICE_MODEL                    "EKS-WR1N9"
#endif

#ifdef ENKORE_L1
#define DEVICE_MODEL                    "EKS-LNP5C"
#endif

//L1 doesn't have COVER_OPEN

#define FIRMWARE_VERSION                "000133"
#define DEFAULT_RTC_YEAR                2000         // recovery will reset RTC to 2000/01/01 00:00:01
#define UTC_TIME_20230101               1672502401   // if device RTC more than 2023/01/01 00:00:01 means already time synced
#define APSSID_MAX_SIZE                 32
#define APPASSKEY_MAX_SIZE              32
#define MAX_AP_NO                       5            // There are 5 sets of AP in Flash
#define MAX_KEY_LIST_BYTE               512
#define WDT_TIMEOUT_REBOOT              7000         // 7 seconds = (RESET_WDT_DURATION * 2) +1
#define RESET_WDT_DURATION              3000000      // 3 seconds
#define RETRY_REPORT_E0                 7            // boot ~ hibernation need 7s, retry 1 times
#define SECONDS_WEEK                    604800       // seconds in a week
#define SECONDS_15MINS                  900
#define REPORT_TIME                     900          // 900s
#define W1_DEFAULT_AUTO_LOCK_TIME       10           // 10s
#define L1_DEFAULT_AUTO_LOCK_TIME       5           // 5s

#ifdef ENKORE_W1
#define MAXMUM_MAIN_LOOP_COUNT          50           // 5s
#else
#define MAXMUM_MAIN_LOOP_COUNT          20           // 2s
#endif

#define VECK_SIZE                       16
#define VECK_TOTAL                      9000
#define FORCE_DOWNLOAD                  0
#define NORMAL_DOWNLOAD                 1
#define BOTH_KEY                        0
#define CARD_KEY                        1
#define MOBILE_KEY                      2
#define CARD_LEN                        7
#define NFC_DISABLE                     0
#define NFC_BUSY                        1
#define TASK_BUSY                       1
#define TASK_IDLE                       0
#define FAILURE                         -1
#define SUCCESS                         1
#define BATTERY_WEAK                    20

//NFC card type
#define CRYPTO_TYPE                     0x00
#define CONSTRUCTION_TYPE               0x55
#define PROGRAMMING_TYPE                0x66
#define RECOVER_TYPE                    0x77
#define MASTER_TYPE                     0x99
#define TENANT_TYPE                     0x80
#define STAFF_TYPE                      0x81
#define VENDOR_TYPE                     0x82
#define GUEST_TYPE                      0x83
#define TEMP_TYPE                       0x84
#define USER_LIMIT_TYPE                 0x89

typedef enum {
    INITIAL_STATE = 0,
    INITIAL_CRYPTO,
    REGISTERD_STATE,
    UNREGISTERD_STATE,
    REGISTERING_STATE,
} LOCK_STATUS_ENUM;

typedef enum {
    SCHEDDULE_OFF_NORMAL = 0,
    SCHEDDULE_ON_NORMAL_CLOSE,
    SCHEDDULE_ON_PRIVACY,
    SCHEDDULE_ON_NORMAL_OPEN,
    SCHEDDULE_ON_PASSAGE,
} SCHEDULE_STUTES_ENUM;

typedef enum {
    NORMAL = 0,
    PASSAGE,
    PRIVACY,
} WORKING_MODE_ENUM;

typedef enum {
    MUTE = 0,
    LOW,
    MEDIUM,
    HIGH,
} VOLUME_TYPE_ENUM;

typedef enum {
    ALLOW_TENANT = 0,
    ALLOW_GUEST,
    ALLOW_TEMP,
    ALLOW_STAFF,
    ALLOW_VENDOR,
    END_ALLOW_KEY_TYPE = 16,
} SCHEDULE_ALLOW_KEY_TYPE_ENUM;

typedef struct {                // RTC Format (YYYYMMDDHHMMSS)
    unsigned short Year;        // Year: 2017 ~ 9999
    char Month;                 // Month: 1-12
    char Day;                   // Day: 1-31
    char Hour;                  // Hour: 0-23
    char Minute;                // Minute: 0-59
    char Second;                // Second: 0-59
} Time_Struct;

typedef struct {
    char            Card[CARD_LEN];
    Time_Struct     Rtc;
    char            EventCode;
    uint16_t        VcekIndex;
} EVENT_DATA_STRUCT;

#pragma pack(push)
#pragma pack(1)

typedef struct {
    unsigned char   UiStatus;
    unsigned char   InvalidCount;
    unsigned long   FirstInvalidTime;
    unsigned long   ReleaseTime;       // when (Lockout_Struct.count > Lockout_Struct.Time) is valid

    //Shutdown lock for 3 mins if 4 times fail tries within 1 min
    //Interval = 60, InvalidThreshold = 4, BlockSeconds = 180
    unsigned short  Interval;          // "1" ~ "1000"
    unsigned short  InvalidThreshold;  // "1" ~ "10"
    unsigned short  BlockSeconds;      // "1" ~ "1000"
} Alzk_Lockout_Struct;

typedef struct {
    unsigned char Volume;              // "0" ~ "3"
    unsigned int Sec;                  // auto lock time
    unsigned int Time;                 // Time of report to server
    char Reserved[3];
} Alzk_System_Struct;

typedef struct {
    unsigned char   CardKeyFileIndex;
    unsigned char   MobileKeyFileIndex;
    unsigned char   SyncKey;
    unsigned char   SyncMobileKey;
} Alzk_Key_Struct;

typedef struct {
    char                Flag;               // 1 means schedule have been download
    char                StaffRequired;
    char                TimeTable[672];
    unsigned char       StaffChecked[7];
    unsigned long       StartTime;
    unsigned char       AllowKeyType[16];   // currently used 0~4, 5~15 reserved
    bool                Schon;
} Alzk_Schedule_Struct;

typedef struct {
    signed char      AP_SSID_NAME[APSSID_MAX_SIZE+1];
    unsigned char    AP_Security_Type;
    signed char      AP_Security_Key[APPASSKEY_MAX_SIZE+1];     // Change to 32 chars
} Alzk_Ssid_Struct;

typedef struct {
    Alzk_Lockout_Struct     Lockout;            // Lockout setting
    Alzk_System_Struct      System;
    Alzk_Key_Struct         KeyInfo;
    Alzk_Schedule_Struct    ScheduleInfo;
    Alzk_Ssid_Struct        ProfilesAp[MAX_AP_NO];

    char                    State;              // 0: Initial, 1: Encrypted, 2: Registered
    char                    AMK[16+1];          // Application Master Key
    char                    Direction;

    bool                    OtaFlag;
    bool                    RecoveryFlag;
    bool                    StaffCardDetect;    // if true must report 3 logs
    unsigned char           WorkingMode;        // 0:Normal 1:Passage 2:Privacy
    unsigned char           BatteryLevel;       // 0xFF means Unknown, 200 means no battery type

    //event log
    unsigned long           LastReportTime;
    unsigned char           WriteFileIndex;
    unsigned char           WriteItems;
    unsigned char           ReadFileIndex;
    unsigned char           ReadItems;

    unsigned char           Card_tmp[8]; // length is 8

    //network info
    unsigned char           MAC[6];
    unsigned char           ServerIp[4];
    unsigned short          ServerPort;
    unsigned char           Projects[APSSID_MAX_SIZE+1];
    unsigned char           ApTotal;
    unsigned char           ReCheckLatch;
    unsigned char           WifiTimeout;

    bool                    FlagBatteryLow;
    bool                    FlagBatteryOff;
} Lock_Struct;

typedef struct {
    unsigned char CardUuid[8];
    char HttpCmdType;           //e0,e1,e2,e3,re
    char EventCode;             //d=1,d=2,d=j,d=k
    uint16_t VcekIndex;
    Time_Struct Rtc;
}LOCK_EVENT_STRUCT;

typedef struct {
    unsigned int Active;            // File state
    unsigned int Total;             // total record amount
    unsigned int CardKeyIndex[256];        // index of record
    //Card_Data   Card[K_MEDIUM]; // The capacity of card list per read (2000 cards)
} KxCardFile_Struct;

#pragma pack(pop)

#endif /* LOCK_H_ */
