#ifndef _MSGTYPE_H
#define _MSGTYPE_H

// Message Type
enum {
    MHTYPE_NONE = 0,
    MHTYPE_KEY, 	//  1
    MHTYPE_ONLINEREG,	//  2
    MHTYPE_HOST,	//  3
    MHTYPE_USER,	//  4
    MHTYPE_SPD, 	//  5
    MHTYPE_LOG,		//  6
    MHTYPE_FWUPGRADE,	//  7
    MHTYPE_REVLIST,	//  8
    MHTYPE_CENTER,	//  9 Comm. between Ceners
    MHTYPE_GATE,	// 10 Comm. between two Gates
    MHTYPE_IDS,		// 11 IDS (Should inform Wins Tech of this, anyong 2001/1/4)
    MHTYPE_AUTH,	// 12 K4 User Authentication(SecuwayPass)
    MHTYPE_WEBADM,	// 13 WEB Admin Related
    MHTYPE_RTMON,	// 14 Realtime Monitor

    MHTYPE_AUTOUP = 0x0f, // 16 Auto upgrade
    
    MHTYPE_PKI = 0x20,	// 32 PKI-related
    MHTYPE_MAX,
    MHTYPE_ERROR = 0xff,
};

// Message Code
enum {
    MHCODE_NONE = 0,

    // Key Related Command
    MHCODE_KEYCHG_REQ = 0x01,	// Center Request
    MHCODE_KEYCHG_ACK,		// Gate Response
    MHCODE_KEYTK_REQ,		// Gate Request
    MHCODE_KEYTK_ACK,		// Center Response
    MHCODE_KEYCHG_REQ_V2,	// Center Request
    MHCODE_KEYCHG_ACK_V2,	// Gate Response
    MHCODE_KEYTK_REQ_V2,	// Gate Request
    MHCODE_KEYTK_ACK_V2,	// Center Response

    MHCODE_GATE_UP_KEYCHG_REQ_V2,	// Gate Upgrade ½Ã KCP Request
    MHCODE_GATE_UP_KEYCHG_ACK_V2,	// Gate Upgrade ½Ã KCP Response
    MHCODE_APP_KEYTK_REQ,
    
    // Online Register Command   : SecuwayClient Related !!
    MHCODE_OLKEY_REQ = 0x11,
    MHCODE_OLKEY_ACK,
    MHCODE_TKENROLL_REQ = 0x01,
    MHCODE_TKENROLL_ACK,
    MHCODE_TKDATA_REQ,
    MHCODE_TKDATA_ACK,
    MHCODE_USRENROLL_REQ,
    MHCODE_USRKEY_REQ,
    MHCODE_USRKEY_ACK,
    MHCODE_SPD_SEND,
    MHCODE_SPD_ACK,
    MHCODE_TKENROLL_REQ_V2 = 0x21,
    MHCODE_TKENROLL_ACK_V2,
    MHCODE_TKDATA_REQ_V2,
    MHCODE_TKDATA_ACK_V2,
    MHCODE_USRENROLL_REQ_V2,
    MHCODE_USRKEY_REQ_V2,
    MHCODE_USRKEY_ACK_V2,
    MHCODE_SPD_SEND_V2,
    MHCODE_SPD_ACK_V2,
    
    // Host Command : Gate Related !!
    MHCODE_GTIME_REQ = 0x01,		// Time Request
    MHCODE_GTIME_ACK,			// Time Ack
    MHCODE_GCHK_REQ,			// Check Request
    MHCODE_GCHK_ACK,			// Check Ack
    MHCODE_GOPR_REQ,			// Operation Start/Stop/Reset Request
    MHCODE_GOPR_ACK,			// Operation Start/Stop/Reset Ack
    MHCODE_GGETSTS_REQ,			// Get Status Request
    MHCODE_GGETSTS_ACK,			// Get Status Ack
    MHCODE_GTIME_REQ_V2 = 0x09,		// Time Request (V2.0)
    MHCODE_GTIME_ACK_V2,		// Time Ack (V2.0)
    MHCODE_GCHK_REQ_V2,			// Check Request (V2.0)
    MHCODE_GCHK_ACK_V2,			// Check Ack (V2.0)
    MHCODE_GOPR_REQ_V2,			// Operation Start/Stop/Reset Req (V2.0)
    MHCODE_GOPR_ACK_V2,			// Operation Start/Stop/Reset Ack (V2.0)
    MHCODE_GGETSTS_REQ_V2,		// Get Status Request (V2.0)
    MHCODE_GGETSTS_ACK_V2,		// Get Status Ack (V2.0)
    MHCODE_OBJ_REFRESH_REQ_V2,		// Object Refresh Request (V2.0)

    MHCODE_GATE_UP_REQ_V2,		// Gate Upgrade Request (V2.0)
    MHCODE_GATE_UP_ACK_V2,		// Gate Upgrade Ack (V2.0)

    MHCODE_SPD_CNT_INFO_ACK_V2 = 0x70,	// SPD Count Info Ack (V2.0)

    MHCODE_HOST_TIME_ACK_ERR_V2 = 0x81,	// Host Time Ack Error (V2.0)
    
    // SPD Manage Command
    MHCODE_SPDSND_REQ = 0x01,
    MHCODE_SPDSND_ACK,
    MHCODE_STROBJ_REQ,
    MHCODE_STROBJ_RES,
    MHCODE_SPDSND_REQ_V2,
    MHCODE_SPDSND_ACK_V2,

    // Web Admin Related Command
    MHCODE_WEBADM_LOGON_REQ = 0x01,
    MHCODE_WEBADM_LOGON_RN,
    MHCODE_WEBADM_LOGON_HASH,
    MHCODE_WEBADM_LOGON_RES,
    MHCODE_WEBADM_REQ,
    MHCODE_WEBADM_RES,
    MHCODE_WEBADM_ACK,
    MHCODE_WEBADM_TIMESYNC,
    MHCODE_WEBADM_LOGREQ,		// for Log Request
    MHCODE_WEBADM_LOGRES,		// for Log Response
    MHCODE_WEBADM_RESET,		// for Reboot Request 

    // Web Admin Certificate Related Command
    MHCODE_WEBADM_CERT_REQ  = 0xC1,	// Cert Request		(GA -> G)
    MHCODE_WEBADM_CERT_RES,		// Cert Response	(G  -> GA)
    MHCODE_WEBADM_CERT_SEND,		// Cert Send 		(GA -> G)
    MHCODE_WEBADM_CERT_SEND_ACK, 	// Cert Send Reply 	(G  -> GA)
    MHCODE_WEBADM_CERT_MAKE, 		// PIC Cert Make Request(GA -> G)
    MHCODE_WEBADM_CERT_MAKE_ACK, 	// PIC Cert Make Reply  (G  -> GA)

    // Web Admin Related Command
    MHCODE_WEBADM_LOGON_RN_V2 = 0xE2,
    MHCODE_WEBADM_LOGON_HASH_V2,
    MHCODE_WEBADM_LOGON_RES_V2,
    MHCODE_WEBADM_REQ_V2,
    MHCODE_WEBADM_RES_V2,
    MHCODE_WEBADM_ACK_V2,
    MHCODE_WEBADM_TIMESYNC_V2,
    MHCODE_WEBADM_LOGREQ_V2,		// for Log Request
    MHCODE_WEBADM_LOGRES_V2,		// for Log Response
    MHCODE_WEBADM_RESET_V2,		// for Reboot Request 

    // LOG Manage Command
    MHCODE_LOG_REQ = 0x01,
    MHCODE_LOG_SEND,			// Send Log
    MHCODE_LOG_ACK,			// Log Received Ack
    MHCODE_LOG_NEWSEND,			// Send Log New Version 2001/3/8 
    MHCODE_NETSTAT_ALL_REQ,		// Get Net Status Request(Total)
    MHCODE_NETSTAT_ALL_RES,		// Get Net Status Request(Total)
    MHCODE_NETSTAT_SVC_REQ,		// Get Net Status Request(Service)
    MHCODE_NETSTAT_SVC_RES,		// Get Net Status Request(Service)
    MHCODE_NETSTAT_SESSION_REQ,		// Get Net Status Request(Session)
    MHCODE_NETSTAT_SESSION_RES,		// Net Status Response(Session)
    MHCODE_LOG_REQ_V2 = 0x0B,		// (V2.0)
    MHCODE_LOG_SEND_V2,			// Send Log
    MHCODE_LOG_ACK_V2,			// Log Received Ack
    MHCODE_LOG_NEWSEND_V2,		// Send Log New Version 
    MHCODE_NETSTAT_ALL_REQ_V2,		// Net Status Request(Total)
    MHCODE_NETSTAT_ALL_RES_V2,		// Net Status Response(Total)
    MHCODE_NETSTAT_SVC_REQ_V2,		// Net Status Request(Service)
    MHCODE_NETSTAT_SVC_RES_V2,		// Net Status Response(Service)
    MHCODE_NETSTAT_SESSION_REQ_V2,	// Net Status Request(Session)
    MHCODE_NETSTAT_SESSION_RES_V2,	// Net Status Response(Session)
    MHCODE_NETSTAT_ALL_UTM_REQ_V2 = 0x16,	// Net Status Request(Total)
    MHCODE_NETSTAT_ALL_UTM_RES_V2,		// Net Status Response(Total)
    MHCODE_NETSTAT_SVC_UTM_REQ_V2,		// Net Status Request(Service)
    MHCODE_NETSTAT_SVC_UTM_RES_V2,		// Net Status Response(Service)

    MHCODE_NETSTAT_SESSION_UTM_REQ_V2 = 0x20,      // Net Status Request(Session)
    MHCODE_NETSTAT_SESSION_UTM_RES_V2 = 0x21,      // Net Status Response(Session)

    MHCODE_ALERT = 0x80,		// Alerting
    MHCODE_ALERT_V2,			// Alerting
    
    MHCODE_REV_REQ = 0x01,
    MHCODE_REV_SEND,
    MHCODE_REV_ACK,
    MHCODE_REV_REQ_V2,
    MHCODE_REV_SEND_V2,
    MHCODE_REV_ACK_V2,
    
    MHCODE_IDS_MSG = 0x01,
    MHCODE_IDS_CLIENT_MSG = 0x02,
    
    MHCODE_GATE_RHOST_REQ = 0x01,
    MHCODE_GATE_RHOST_REP,

    // Client-User Logon Command : SecuwayPass Related !!
    MHCODE_LOGON_REQ = 0x01,
    MHCODE_LOGON_CHALL,
    MHCODE_LOGON_RES,
    MHCODE_LOGON_RESULT,

    // SecuwayPass User Auth Command : Gate Related !!
    MHCODE_AUTHINFO_REQ,		// User AuthInfo Request
    MHCODE_AUTHINFO_RES,		// User AuthInfo Response

    // Realtime Monitor 
    MHCODE_RTSUM_REQ = 0x01,
    MHCODE_RTSUM_RES,
    MHCODE_RTMON_REQ,
    MHCODE_RTMON_RES,
    MHCODE_RTRULE_REQ,
    MHCODE_RTRULE_RES,
    MHCODE_RTLOGRULE_REQ,
    MHCODE_RTLOGMAN_REQ,
    MHCODE_RTLOG_RES,
    MHCODE_RTOOB_MSG,	// 0x0a
    MHCODE_RTMON_END,
    MHCODE_RTLOGON_REQ,
    MHCODE_RTLOGON_RN,
    MHCODE_RTLOGON_HASH,
    MHCODE_RTLOGON_RES,
    MHCODE_RTIDS_REQ,	// 0x10
    MHCODE_RTLOG_UDP,
    MHCODE_RTIP_REG,
    MHCODE_RTTUNNEL_REQ,
    MHCODE_RTTUNNEL_ACK,
    MHCODE_RTOOB_MSG_V2,	// 0x15
    MHCODE_RTIPS_REQ,		// 0x16 RTM for IPS only
    MHCODE_RTIPS_RES,		// RTM for IPS only
    MHCODE_RTIPS_LOG,		// RTM for IPS only
    MHCODE_RTIPS_LOG_WITH_RAW_DATA,	// RTM for IPS only
    MHCODE_RTIPS_SESS_REQ,	// RTM for IPS only
    MHCODE_RTIPS_SESS_RES,	// RTM for IPS only
    MHCODE_RTIPS_SESS_DEL,	// RTM for IPS only
    MHCODE_RTIPS_SESS_END,	// RTM for IPS only
    MHCODE_RTIPS_SESS_ETC_RES,	// RTM for IPS only
    MHCODE_RTLOGON_UTM_RES = 0x1F,
    MHCODE_RTSUM_UTM_REQ,
    MHCODE_RTSUM_UTM_RES,
    MHCODE_RTMON_UTM_REQ,
    MHCODE_RTMON_UTM_RES,

    // MUS Related Command
    MHCODE_AUTOUP_VER_REQ = 0x01,
    MHCODE_AUTOUP_VER_ACK,
    
    // PKI Related Command
    MHCODE_CERT_REQ = 0x01,		// Client/Gate -> Center
    MHCODE_CERT_ACK,			// Client/Gate <- Center
    MHCODE_CERT_UPDATE_REQ_V2,		// Client/Gate <- Center
    MHCODE_CERT_UPDATE_ACK_V2,		// Client/Gate -> Center
    MHCODE_KEY_UPDATE_SEND,		// Client/Gate <- Center
    MHCODE_KEY_UPDATE_SEND_ACK,		// Client/Gate -> Center
    MHCODE_PKCS10_REQ,			// Client/Gate <- Center 2002.06.28
    MHCODE_PKCS10_ACK,			// Client/Gate -> Center 2002.06.28
    MHCODE_CERT_SEND_V2,		// Client/Gate <- Center 2002.06.28
    MHCODE_CERT_SEND_ACK_V2,		// Client/Gate -> Center 2002.06.28
    MHCODE_CERT_STATUS_REQ,		// Client/Gate -> Center 2002.06.28
    MHCODE_CERT_STATUS_ACK,		// Client/Gate <- Center 2002.06.28
    MHCODE_CERT_RESTORE_REQ,		// Client/Gate -> Center 2002.06.28
    MHCODE_CERT_RESTORE_ACK,		// Client/Gate <- Center 2002.06.28
    MHCODE_CERT_REVOCATE_REQ,		// Client/Gate -> Center 2002.06.28
    MHCODE_CERT_REVOCATE_ACK,		// Client/Gate <- Center 2002.06.28

    // NCA-Only
    MHCODE_CERT_SEND  =0x11,		// Client -> Center
    MHCODE_CERT_SEND_ACK,		// Client <- Center
    MHCODE_CERT_VERIFY_REQ,		// Gate -> Center
    MHCODE_CERT_VERIFY_ACK,		// Gate <- Center

};

// Error Code
enum {
    MHERR_NONE = 0,

    // K4 User Authentication(SecuwayPass)
    MHERR_AUTH_USERID, 
    MHERR_AUTH_USERPW,
    MHERR_AUTH_ETC,
};

#endif
