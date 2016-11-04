#ifndef _common_h
#define _common_h



// 'compile condtionally' set here
//#define BH_EVENT_ALARM_ENABLE     



#define byte0(x)                                  (*((unsigned char  *)&x+0))
#define byte1(x)                                  (*((unsigned char  *)&x+1))
#define byte2(x)                                  (*((unsigned char  *)&x+2))
#define byte3(x)                                  (*((unsigned char  *)&x+3))

#define low_word(x)                               (*((unsigned short *)&x+0))
#define high_word(x)                              (*((unsigned short *)&x+1))

#define YES                                       0xaa
#define NO                                        0x55
#define UNKNOW                                    0x00
#define GOOD                                      0xaa
#define ERR                                       0x55

#define ON                                        0xaa
#define OFF                                       0x55

#define PARITY_DISABLE                            0x00
#define PARITY_ODD                                0x01
#define PARITY_EVEN                               0x02

#define MAX_CAN_NUMBER                            0x40
#define MAX_COM_NUMBER                            0x10





typedef unsigned char                             BYTE;
typedef unsigned short                            WORD;
typedef unsigned short                            UI;
typedef unsigned long                             UL;




typedef struct{ WORD EXINT0_INT_DEAD_COUNTER;    // moni EXINT0 hold triggle level,not reset, so can't come into short again
                WORD EXINT1_INT_DEAD_COUNTER;    // as above
                WORD EXINT2_INT_DEAD_COUNTER;    // as above
                WORD EXINT3_INT_DEAD_COUNTER;    // as above
                WORD   EPA0_INT_DEAD_COUNTER;    // as above
                WORD   EPA1_INT_DEAD_COUNTER;    // moni with some reason , only soft_time_1ms is disabled.
                WORD   MAINLOOP_DEAD_COUNTER;    // moni main_loop come into dead loop
              }TYPE_CPU_INT_MONI;


typedef struct{ BYTE RECE_CPU_REPLYINFO_ENABLE   ;
                BYTE RECE_CPU_REPLYINFO_ADDR     ;
                BYTE RECE_CPU_REPLYINFO_WISHPROC ;
                BYTE RECE_CPU_LONGFRAME_FRAMESUM ;
                BYTE RECE_CPU_LONGFRAME_FRAMECUR ;

                BYTE TRAN_CPU_INFO_ADDR          ;
                BYTE TRAN_CPU_LONGFRAME_FRAMESUM ;
                BYTE TRAN_CPU_LONGFRAME_FRAMECUR ;
                BYTE TRAN_CPU_IS_LAST_FRAME      ;
                BYTE F_TRANS_STATUS              ;
              }TYPE_CAN_PORT;

typedef struct{ BYTE CAN_PROHIBITPOLLADDR_FROM_COMx   ;
                BYTE CAN_PROHIBITPOLLADDR_COMP   ;
                BYTE CAN_MON_WAIT_STATYP         ;
                BYTE CAN_MON_STATUS              ;
                BYTE CAN_MON_NEED_REPLY          ;     
                BYTE CAN_NEEDTRANSTOHOST         ;      
              }TYPE_CAN_PROC; 


typedef struct{ WORD CAN_BEGINWAITCPUREPLY_TIME  ;
                WORD CAN_BEGINWAITRTUREPLY_TIME  ;
                WORD CAN_BEGIN_VERICPUCLK_TIME   ;
                WORD CAN_BEGIN_yk_control_TIME   ;
                WORD CAN_BEGIN_POL_YCALL_TIME    ;     
                WORD CAN_BEGIN_POL_YXALL_TIME    ;
                WORD CAN_BEGIN_POL_YMALL_TIME    ;
                WORD CAN_BEGIN_POL_DD_AC_TIME    ;
                BYTE CAN_NOW_POL_INFO_TYPE       ;
                BYTE CAN_NOW_POL_CPU_MAC_ADDR  ;
                
                BYTE CAN_RECEYCCHG_BUF_SAV_PTR   ;
                BYTE CAN_RECEYCCHG_BUF_TAK_PTR   ;
                
                BYTE CAN_RECECOS_RETACK_SAV_PTR  ;
                BYTE CAN_RECECOS_RETACK_TAK_PTR  ;
                
                WORD CAN_RECECOS_BUF_SAV_PTR     ;
                WORD CAN_RECECOS_BUF_TAK_PTR     ;

                BYTE CAN_PORT_RECE_BUF_SAV_PTR   ;
                BYTE CAN_PORT_RECE_BUF_TAK_PTR   ;
		BYTE CAN_PORT_NOW_TEST		 ;
              }TYPE_CAN_RAM_PARA;







// define types of Rcd info
// ------------ RCD_INFO_MYSELF_AREA0 ----------------
// chip info    0 - undefined
#define RCD_INFO_MYSELF_AREA0_CPU                 0xe1
#define RCD_INFO_MYSELF_AREA0_ROM                 0xe2
#define RCD_INFO_MYSELF_AREA0_RAM                 0xe3
#define RCD_INFO_MYSELF_AREA0_FLASH               0xe4
#define RCD_INFO_MYSELF_AREA0_DALLAS              0xe5
#define RCD_INFO_MYSELF_AREA0_MAX706              0xe6
#define RCD_INFO_MYSELF_AREA0_SJA1000             0xe7
#define RCD_INFO_MYSELF_AREA0_16C554              0xe8
#define RCD_INFO_MYSELF_AREA0_ALTRA               0xe9

// port info
#define RCD_INFO_MYSELF_AREA0_554_0_A             0x00
#define RCD_INFO_MYSELF_AREA0_554_0_B             0x01
#define RCD_INFO_MYSELF_AREA0_554_0_C             0x02
#define RCD_INFO_MYSELF_AREA0_554_0_D             0x03
#define RCD_INFO_MYSELF_AREA0_554_1_A             0x04
#define RCD_INFO_MYSELF_AREA0_554_1_B             0x05
#define RCD_INFO_MYSELF_AREA0_554_1_C             0x06
#define RCD_INFO_MYSELF_AREA0_554_1_D             0x07
#define RCD_INFO_MYSELF_AREA0_554_2_A             0x08
#define RCD_INFO_MYSELF_AREA0_554_2_B             0x09
#define RCD_INFO_MYSELF_AREA0_554_2_C             0x0a
#define RCD_INFO_MYSELF_AREA0_554_2_D             0x0b
#define RCD_INFO_MYSELF_AREA0_CAN                 0x0c
#define RCD_INFO_MYSELF_AREA0_CAN1                0x0d
#define RCD_INFO_MYSELF_AREA0_CPUCOM              0x0e

// ------------ RCD_INFO_MYSELF_AREA1 ----------------
// CPU ACTION   varible  0 - undefined
#define RCD_INFO_MYSELF_AREA1_CPU_RST             0x01
#define RCD_INFO_MYSELF_AREA1_CPU_EXINT0_INT      0x02
#define RCD_INFO_MYSELF_AREA1_CPU_EXINT1_INT      0x03
#define RCD_INFO_MYSELF_AREA1_CPU_EXINT2_INT      0x04
#define RCD_INFO_MYSELF_AREA1_CPU_EXINT3_INT      0x05
#define RCD_INFO_MYSELF_AREA1_CPU_EPA0_INT        0x06
#define RCD_INFO_MYSELF_AREA1_CPU_EPA1_INT        0x07
#define RCD_INFO_MYSELF_AREA1_CPU_MAINLOOP        0x08

// Dallas Mem varible  0 - undefined
#define RCD_INFO_MYSELF_AREA1_RCD_MYSELF_PTR_ADDR 0x01
#define RCD_INFO_MYSELF_AREA1_RCD_SYSTEM_PTR_ADDR 0x02

#define RCD_INFO_MYSELF_AREA1_DALLAS_YEAR         0x03
#define RCD_INFO_MYSELF_AREA1_DALLAS_MONTH        0x04
#define RCD_INFO_MYSELF_AREA1_DALLAS_DATE         0x05
#define RCD_INFO_MYSELF_AREA1_DALLAS_DAY          0x06
#define RCD_INFO_MYSELF_AREA1_DALLAS_HOUR         0x07
#define RCD_INFO_MYSELF_AREA1_DALLAS_MINUTE       0x08
#define RCD_INFO_MYSELF_AREA1_DALLAS_SECOND       0x09


// ------------ RCD_INFO_MYSELF_AREA2 ----------------
// CPU ACTION   varible  0 - undefined
#define RCD_INFO_MYSELF_AREA2_CPURST_COLD         0x01
#define RCD_INFO_MYSELF_AREA2_CPURST_HOT          0x02
#define RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD        0x03

// Dallas Mem varible  0 - undefined
#define RCD_INFO_MYSELF_AREA2_PTR_ADDR_XOR_ERR    0x01
#define RCD_INFO_MYSELF_AREA2_BCD_DATA_ERR        0x02

// PORTx      varible  0 - undefined
#define RCD_INFO_MYSELF_AREA2_PORT_STA_RET_IDLE   0x01


// ---------------------------------------------------
// ------------ RCD_INFO_SYSTEM_AREA0 ----------------
// chip info    0 - undefined
#define RCD_INFO_SYSTEM_AREA0_CPU                 0xe1
#define RCD_INFO_SYSTEM_AREA0_ROM                 0xe2
#define RCD_INFO_SYSTEM_AREA0_RAM                 0xe3
#define RCD_INFO_SYSTEM_AREA0_FLASH               0xe4
#define RCD_INFO_SYSTEM_AREA0_DALLAS              0xe5
#define RCD_INFO_SYSTEM_AREA0_MAX706              0xe6
#define RCD_INFO_SYSTEM_AREA0_SJA1000             0xe7
#define RCD_INFO_SYSTEM_AREA0_16C554              0xe8
#define RCD_INFO_SYSTEM_AREA0_ALTRA               0xe9

// port info
#define RCD_INFO_SYSTEM_AREA0_554_0_A             0x00
#define RCD_INFO_SYSTEM_AREA0_554_0_B             0x01
#define RCD_INFO_SYSTEM_AREA0_554_0_C             0x02
#define RCD_INFO_SYSTEM_AREA0_554_0_D             0x03
#define RCD_INFO_SYSTEM_AREA0_554_1_A             0x04
#define RCD_INFO_SYSTEM_AREA0_554_1_B             0x05
#define RCD_INFO_SYSTEM_AREA0_554_1_C             0x06
#define RCD_INFO_SYSTEM_AREA0_554_1_D             0x07
#define RCD_INFO_SYSTEM_AREA0_554_2_A             0x08
#define RCD_INFO_SYSTEM_AREA0_554_2_B             0x09
#define RCD_INFO_SYSTEM_AREA0_554_2_C             0x0a
#define RCD_INFO_SYSTEM_AREA0_554_2_D             0x0b
#define RCD_INFO_SYSTEM_AREA0_CAN                 0x0c
#define RCD_INFO_SYSTEM_AREA0_CAN1                0x0d
#define RCD_INFO_SYSTEM_AREA0_CPUCOM              0x0e

// ------------ RCD_INFO_SYSTEM_AREA1 ----------------
// CPU ACTION   varible  0 - undefined
#define RCD_INFO_SYSTEM_AREA1_CPU_RST             0x01
#define RCD_INFO_SYSTEM_AREA1_CPU_EXINT0_INT      0x02
#define RCD_INFO_SYSTEM_AREA1_CPU_EXINT1_INT      0x03
#define RCD_INFO_SYSTEM_AREA1_CPU_EXINT2_INT      0x04
#define RCD_INFO_SYSTEM_AREA1_CPU_EXINT3_INT      0x05
#define RCD_INFO_SYSTEM_AREA1_CPU_EPA0_INT        0x06
#define RCD_INFO_SYSTEM_AREA1_CPU_EPA1_INT        0x07
#define RCD_INFO_SYSTEM_AREA1_CPU_MAINLOOP        0x08

// Dallas Mem varible  0 - undefined
#define RCD_INFO_SYSTEM_AREA1_DALLAS_YEAR         0x03
#define RCD_INFO_SYSTEM_AREA1_DALLAS_MONTH        0x04
#define RCD_INFO_SYSTEM_AREA1_DALLAS_DATE         0x05
#define RCD_INFO_SYSTEM_AREA1_DALLAS_DAY          0x06
#define RCD_INFO_SYSTEM_AREA1_DALLAS_HOUR         0x07
#define RCD_INFO_SYSTEM_AREA1_DALLAS_MINUTE       0x08
#define RCD_INFO_SYSTEM_AREA1_DALLAS_SECOND       0x09


// ------------ RCD_INFO_SYSTEM_AREA2 ----------------
// CPU        ACTION   varible  0 - undefined
#define RCD_INFO_SYSTEM_AREA2_CPURST_COLD         0x01
#define RCD_INFO_SYSTEM_AREA2_CPURST_HOT          0x02
#define RCD_INFO_SYSTEM_AREA2_CPU_RUN_DEAD        0x03

// Dallas     Mem      varible  0 - undefined
#define RCD_INFO_SYSTEM_AREA2_PTR_ADDR_XOR_ERR    0x01
#define RCD_INFO_SYSTEM_AREA2_BCD_DATA_ERR        0x02

// CAN        ACTION   varible  0 - undefined
#define RCD_INFO_SYSTEM_AREA2_YK_EXECUTE          0x01
#define RCD_INFO_SYSTEM_AREA2_SET_MODI            0x02
#define RCD_INFO_SYSTEM_AREA2_SETGROUP_MODI       0x03
#define RCD_INFO_SYSTEM_AREA2_246_YK_EXECUTE      0x04
#define RCD_INFO_SYSTEM_AREA2_246_YK_CANCEL       0x05







//-----------------  SER definition  ----------------------
#define PORT_NO_P554_00                           0x00
#define PORT_NO_P554_01                           0x01
#define PORT_NO_P554_02                           0x02
#define PORT_NO_P554_03                           0x03
#define PORT_NO_P554_04                           0x04
#define PORT_NO_P554_05                           0x05
#define PORT_NO_P554_06                           0x06
#define PORT_NO_P554_07                           0x07
#define PORT_NO_P554_08                           0x08
#define PORT_NO_P554_09                           0x09
#define PORT_NO_P554_10                           0x0a
#define PORT_NO_P554_11                           0x0b

#define PORT_NO_CAN_0                             0x0c
#define PORT_NO_CAN_1                             0x0d

#define PORT_NO_CPU_SIO                           0x0e


#define RCD_INFO_PTR_LENGTH                       0x80  
#define AND_RCD_INFO_PTR_LENGTH                   0x7f

#define EPA1_PERIOD                              12000   // f=48M, for 1ms


#define DATABASE_YC_INFO_LEN                      0x02
#define DATABASE_YX_INFO_LEN                      0x02
#define DATABASE_YM_INFO_LEN                      0x04
#define DATABASE_DD_INFO_LEN                      0x04





// IDLE
#define PORT_EXCHANGE_STA_IDLE                    0x00
// PROCEEDING
#define PORT_EXCHANGE_STA_START                   0x41
#define PORT_EXCHANGE_STA_PROCING                 0x42
// END
#define PORT_EXCHANGE_STA_END_OK                  0x81
#define PORT_EXCHANGE_STA_END_ERR                 0x82
#define PORT_EXCHANGE_STA_END_WAIT_TIME_OUT       0x83
// WAIT
#define PORT_EXCHANGE_STA_WAIT_EXCHANGE_INFO      0xc1



//----------------  CPU   definition  ---------------------
#define CBS_38400                               0x804D
#define CBS_9600                                0x8137
#define CBS_1200                                0x89C3

#define CPU_SIO_BAUD_TO_PC                   CBS_38400
#define CPU_SIO_BAUD_TO_LED                  CBS_38400

#define SETPORT                                   0xf0

#define WHOLE_PACKAGE                             0xff


//----------------  CORE  definition  ---------------------
#define CORE_CODE_YK_CHOOSE                       0xe0
#define CORE_CODE_YK_VERIFY                       0xe1
#define CORE_CODE_YK_EXECUTE                      0xe2
#define CORE_CODE_YK_CANCEL                       0xe3


#define CORE_CODE2_YK_CLOSE                       0xcc
#define CORE_CODE2_YK_TRIP                        0x33
#define CORE_CODE2_YK_ERROR                       0xff
#define CORE_CODE2_YK_EXECUTE                     0xaa
#define CORE_CODE2_YK_CANCEL                      0x55




// none
#define PROTOCOL_NONE                           0x0000

// IED- BUS protocol for all info
#define PROTOCOL_INTERNAL_BUS                   0x0101  // JK

#define PROTOCOL_MODBUS                         0x0201  // JK
#define PROTOCOL_WD_SEU                         0x0202  // reserved 02xx-05xx for other type MODBUS
#define PROTOCOL_MOD_HT                         0x0203  // JK
#define PROTOCOL_MODBUS_ABB                     0x0204  // JK
#define PROTOCOL_MOD_HFHK                       0x0205  // JK
#define PROTOCOL_MODBUS_ST2                     0x0206  // JK
#define PROTOCOL_MOD_kd                         0x0207  // JK 
#define PROTOCOL_MOD_CW2		                  	0x0208	// jk	YK 0x10(固定)(modbus)
#define PROTOCOL_MOD_PMAC			                  0x0209
#define PROTOCOL_MOD_TC                         0x020a  //
#define PROTOCOL_MOD_XH                         0x020b  //
#define PROTOCOL_MOD_AEC                        0x020c  //
#define PROTOCOL_MOD_EPS                        0x020d  //
#define PROTOCOL_MOD_ZLP                        0x020e  //
#define PROTOCOL_MOD_XX                         0x020f  //
#define PROTOCOL_MOD_XDL                        0x0210  //
#define PROTOCOL_MOD_INT                        0x0211  //
#define PROTOCOL_MOD_KS                         0x0212  //
#define PROTOCOL_MOD_HY                         0x0213  //
#define PROTOCOL_MOD_XJ                         0x0214  //
#define PROTOCOL_LSA2000                        0x021e  //
#define PROTOCOL_MODBUS_IPC                     0x021f  // JK
#define PROTOCOL_MOD_JYDK                       0x0220  // JK
#define PROTOCOL_MODBUS_3S                      0x0221  // JK
#define PROTOCOL_MODBUS_KG                      0x0222  // JK
#define PROTOCOL_MOD_YDYB                       0x0223  // JK


#define PROTOCOL_IEC103_RCS                     0x0801
#define PROTOCOL_IEC103_ISA                     0x0802
#define PROTOCOL_IEC103_MGT                     0x0803
#define PROTOCOL_IEC103_LSA                     0x0804
#define PROTOCOL_IEC103_NAZI                    0x0805
#define PROTOCOL_IEC103_BJSF                    0x0806
#define PROTOCOL_IEC103_XMZ                     0x0807
#define PROTOCOL_IEC103_XMZ600                  0x0808
#define PROTOCOL_IEC103_XUJI			              0x0809
#define PROTOCOL_IEC103_DSA8000 			          0x080a
#define PROTOCOL_IEC103_MQR       		          0x080b
#define PROTOCOL_IEC103_NTS       		          0x080c

// IED- ZLP
#define PROTOCOL_ZLP_CDT_YT                     0x1101  // JK	ZLP_CDT:61、72、b3,ZLPCDT:61
#define PROTOCOL_ZLP_CDT_PSMA10                 0x1102  // JK	61、c2(yc_line_start)
#define PROTOCOL_ZLP_MK                         0x1103  // JK	eb 90 05
#define PROTOCOL_ZLP_DH                         0x1104  // JK	eb 90 44 48 90 eb
#define PROTOCOL_ZLP_SHCY                       0x1105  // JK
#define PROTOCOL_ZLP_JRK                        0x1106  // JK   eb 90 eb 90 02 sc_p 4D(40) 02 SUML SUMH 03
#define PROTOCOL_ZLP_DPMZ                       0x1107  // JK	eb 90 eb 90 02 64 20 84 03	port_recv[][6]=3a
#define PROTOCOL_ZLP_LNJC                       0x1108  // JK	eb 90 eb 90 02 64 20 84 03      port_recv[][6]=38
#define PROTOCOL_ZLP_WCK3                       0x1109  // JK   eb 90 eb 90 10 ff 02 a5 00 90 eb
#define PROTOCOL_ZLP_ZWK                        0x110a  // JK	eb 90 eb 90 eb 90 02 01 10(40) 0d
#define PROTOCOL_ZLP_DH2                        0x110b  // JK	
#define PROTOCOL_ZLP_WZJK                       0x110c  // JK	68 03 03 68 00 mirr
#define PROTOCOL_ZLP_SPC                        0x110d  // JK	68 03 03 68 00 sc_p
#define PROTOCOL_ZLP_JZDQ                       0x110e  // JK	eb 90 eb 90 eb 90 71 ab(ac) 00 00 sc_u bch 0c
#define PROTOCOL_ZLP_JK2K1                      0x110f  // JK	ff ff ff ff ff ff 0a sc_u 00 sum ff
#define PROTOCOL_ZLP_EASY			                  0x1111  // jk	收发可以简单设置的规约
#define PROTOCOL_ZLP_ZJHY			                  0x1112  // jk	主动上送eb 90 eb 90 eb 90
#define PROTOCOL_ZLP_JZDQ1                      0x1113  // JK	与JZDQ区别在jzdq1:71b3->yc	JZDQ:71c2->yc
#define PROTOCOL_ZLP_CDT1                       0x1114  // JK   与ZLP_CDT区别在YC高前低后,负数为符号位+绝对值
#define PROTOCOL_ZLP_ZJSC                       0x1115  // JK	IED 03 00 63 00 0a CRCL CRCH
#define PROTOCOL_ZLPTG                          0x1116	//	sc_p 01(03) 00 20(60) crc
#define PROTOCOL_ZLP_MK80                       0x1117  // JK	eb 90 ff 61(71) 00 bch
#define PROTOCOL_ZLP_POD                        0x1118  // JK	02 IED(ASIC) 32 30 30 31 30 30.....
#define PROTOCOL_ZLP_STD                        0x1119  // JK   4e 21 05 
#define PROTOCOL_ZLP_BJSF                       0x111a  // JK	aa 03 IED 80 30(07)
#define PROTOCOL_ZLP_GZDW                       0x111B  // JK   adr 69(6a) aa aa
#define PROTOCOL_ZLP_ET_SZ5                     0x111c  // JK	10 4a 01 4b 16
#define PROTOCOL_ZLP_GZDW2                      0x111d  // JK   eb 90 eb 90 eb 90 02 sc_u 10(40) 0d 0a 
#define PROTOCOL_ZLP_XY				                  0x111e	//	01 05 fa 12(10) 12(10)
#define PROTOCOL_ZLP_GZG20			                0x111f	//	cc 41 41(45) cc(c8)
#define PROTOCOL_ZLP_TEP			                  0x1120	//	68 03 type scp 00 SUML 16
#define PROTOCOL_ZLP_hat			                  0x1121	//	modbus(yx 01 yc 03)
#define PROTOCOL_ZLP_WDYL			                  0x1122	//      modbus(yx 02 yc 04)
#define PROTOCOL_MOD_MK80			                  0x1123	//      modbus(yx 02 yc 03) 
#define PROTOCOL_MOD_CZ				                  0x1124	//      modbus(yx 01 yc 02)地址固定不可设 常州科海
#define PROTOCOL_ZLP_RHD			                  0x1125  //	modbus格式内含(68 L L 68)
#define PROTOCOL_ZLP_HMK			                  0x1126 
#define PROTOCOL_ZLP_DH1                        0x1127  // JK	
#define PROTOCOL_ZLP_KY                         0x1128  // JK	
#define PROTOCOL_ZLP_PSME10                     0x1601  // JK	
#define PROTOCOL_ZLP_SHCY2                      0x1602  // JK
#define PROTOCOL_ZLP_SC                         0x1603  // JK	
#define PROTOCOL_ZLP_NK                         0x1604  // JK   eb 90 eb 90 02 01 48 02 03
#define PROTOCOL_ZLP_ATX                        0x1605  // JK	05 目标地址 源地址(CDT) 30(31) SUML SUMH 
#define PROTOCOL_ZLPDPMZ                        0x1606  // JK	eb 90 eb 90 02 64 20 84 03
#define PROTOCOL_DN_D2000		                   	0x1607  // JK   eb 90 eb 90 fc scp
#define PROTOCOL_ZLP_DFDL                       0x1608  // JK
#define PROTOCOL_ZLP_BDL                        0x1609  // JK
#define PROTOCOL_MOD_KM                         0x160a  // JK
#define PROTOCOL_ZLP_XA                         0x160b  // JK
#define PROTOCOL_D709CDT                        0x160C  // JK
// IED- GPS
#define PROTOCOL_GPS_NARI                       0x1201  // JK
#define PROTOCOL_GPS12                          0x1202  // JK
// IED- JZ
#define PROTOCOL_JZPLC                          0x1301  // JK
#define PROTOCOL_JZXJY                          0x1302  // JK
#define PROTOCOL_PLC_GE                         0X1303   //
#define PROTOCOL_PLC                            0X1304  //
#define PROTOCOL_WKQ_JN                         0X1305  //

// IED- XJD
#define PROTOCOL_XJD_SDX                        0x1401  // JK
#define PROTOCOL_TYJCD                          0x1402  // JK                        0x1402  // JK
#define PROTOCOL_JCY_JX                         0x1403  // JK
#define PROTOCOL_LC_PP41                        0x1404  // JK
#define PROTOCOL_LC_HWJT                        0x1405  // JK
#define PROTOCOL_TSQ_TY                         0x1406  // JK
#define PROTOCOL_YD2000                         0x1407  // JK
#define PROTOCOL_TSQ_CJ                         0x1408  // JK
#define PROTOCOL_SAVR2000                       0x1409  // JK

// IED- VQC
#define PROTOCOL_VQC_DWK                        0x1501  // JK
#define PROTOCOL_VQC_SDJX                       0x1502  // JK
#define PROTOCOL_VQC_DWK3                       0x1503  // JK
#define PROTOCOL_VQC_MCVQ1                      0x1504  // JK
#define PROTOCOL_YZTY_HMK                       0x1505  // JK
#define PROTOCOL_VQC_VQC100                     0x1506  // JK
#define PROTOCOL_VQC_JX                         0x1507  // JK
#define PROTOCOL_DR_SHKF			0x1540	// 上海可方电容器
#define PROTOCOL_DR_DMP				0x1541	//山东DMP-366
#define PROTOCOL_VQC_SY				0x1542
#define PROTOCOL_VQC_DS  			0x1543
#define PROTOCOL_DW_KZ  			0x1544

// IED- XHXQ
#define PROTOCOL_XHXQ_KDXH                      0x1701  // JK
#define PROTOCOL_XHXQ_SHSY                      0x1702  // JK
#define PROTOCOL_XHXQ_ZGTD                      0x1703  // JK
#define PROTOCOL_XHXQ_HDXH                      0x1704  // JK
#define PROTOCOL_XHXQ_KMWF                      0x1705  // JK
#define PROTOCOL_BW_XX                          0x1706  // JK
#define PROTOCOL_BW_XDL                         0x1707
#define PROTOCOL_YE_XX                          0x1708  // JK
// IED- WD
#define PROTOCOL_WD_FJLD                        0x1801  // JK
#define PROTOCOL_WD_TCM100			0x1802  //	鲍尔温度巡检TCM-100
#define PROTOCOL_MOD_TDS  			0x1803
#define PROTOCOL_MOD_WKLF  			0x1804

// IED- JLP
#define PROTOCOL_JLP_PJD10                      0x1901  // JK
#define PROTOCOL_JLP_ATX                        0x1902  // JK
#define PROTOCOL_JLP_SS                         0x1903  // JK

// IED- TQ
#define PROTOCOL_TQ_CM320                       0x1a01  // JK
//IED-Li ci xi tong
#define PROTOCOL_LC_ZYDL                        0x1b01  // JK
// IED- DDB
#define PROTOCOL_DDB_BB                         0x2101  // JK
#define PROTOCOL_DDB_WS                         0x2102  // JK
#define PROTOCOL_DDB_HT                         0x2103  // JK
#define PROTOCOL_DDB_HX                         0x2104  // JK
#define PROTOCOL_DDB_WSKZ                       0x2105  // JK
#define PROTOCOL_DDB_XMZ                        0x2106  // JK
#define PROTOCOL_DDB_NJKD                       0x2107  // JK
#define PROTOCOL_DDB_CD194E                     0x2108  // JK
#define PROTOCOL_DDB_WS1                        0x2109  // JK
#define PROTOCOL_DDB_RH				0x210a  // JK
#define PROTOCOL_MOD_DDB			0x210b
#define PROTOCOL_DDB_DG 			0x210c
#define PROTOCOL_DDB_BB1      0x210d  // JK

// IED- rece CDT
#define PROTOCOL_C9302_CDT                      0x3101  // JK
#define PROTOCOL_WRTU                           0x3108
// IED- BH
#define PROTOCOL_LFP                            0x4101  // BH 
#define PROTOCOL_LFP_RTU                        0x4101  // BH 
#define PROTOCOL_DFP201                         0x4121
#define PROTOCOL_DFP500                         0x4201
#define PROTOCOL_LSA                            0x4202
#define PROTOCOL_XX_HYR12                       0x4202  
#define PROTOCOL_ISA                            0x4202  
#define PROTOCOL_REC_HOST                       0x4203  
#define PROTOCOL_LFP20                          0x4204
#define PROTOCOL_ISA300_CDT                     0x4205
#define PROTOCOL_LSA601                         0x4206
#define PROTOCOL_REC9702			                  0x4207

// IED- JK
#define PROTOCOL_DSA301                         0x4901  // JK PORT 
#define PROTOCOL_PA100                          0x4902  // JK+BH
#define PROTOCOL_BJKR101                        0x4903  // JK+BH
#define PROTOCOL_FSA2000                        0x4904  // JK+BH
#define PROTOCOL_DISA1                          0x4905  // JK+BH
#define PROTOCOL_XJBH_FCK                       0x4906  // JK+BH
#define PROTOCOL_NAZI94                         0x4907  // JK+BH
#define PROTOCOL_BJHX                           0x4908  // JK+BH
#define PROTOCOL_NAZI94JK                       0x4909  // JK
#define PROTOCOL_DBJ				                    0x490a  // jk+bh
#define PROTOCOL_ZH_WLD				                  0x490b
#define PROTOCOL_INTPA100			                  0x490d
#define PROTOCOL_HNBH		    	                  0x490c	

// IED- BH+JK
#define PROTOCOL_CAN_DSA                        0x4C01  // JK+BH


// HOST- CDT
#define PROTOCOL_CDT_BB                         0x8101  // HOST
#define PROTOCOL_CDT_D709                       0x8102  
#define PROTOCOL_CDT_HOSTZF                     0x8103  
#define PROTOCOL_CDT_XT9702                     0x8104  
#define PROTOCOL_CDT_HOSTZF_VQC                 0x8108  
#define PROTOCOL_MODBUS_HH                      0x8109  
#define PROTOCOL_CDT_XT9702M                    0x8105
// HOST- OTHER
#define PROTOCOL_1801                           0x8301
#define PROTOCOL_1801_DF                        0x8302
#define PROTOCOL_U4F                            0x8401

#define PROTOCOL_IEC101                         0x8801
#define PROTOCOL_IEC101_YTDF                    0x8802
#define PROTOCOL_IEC101_LNJC                    0x8803
#define PROTOCOL_IEC101_NRDW                    0x8804

// HOST- MONIPIN
#define PROTOCOL_MONI_SX                        0x9101
#define PROTOCOL_MONI_SY                        0x9102
#define PROTOCOL_MONI_HT                        0x9103
#define PROTOCOL_MONI_LJ			0x9104	//aa 

#define PROTOCOL_PRINTF                         0xb001
// HOST- OTHER
#define PROTOCOL_SETPORT                        0xe001



#endif /* _common_h */
