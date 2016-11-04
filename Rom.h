#ifndef _rom_h
#define _rom_h

#include "common.h"


extern const unsigned short BaudRate[8];

extern const WORD CAN_BUST_XTAL16M[8];

extern const unsigned char BCH_table[256];

extern const WORD CPU_SIO_BAUD_XTAL48M[8];

extern const BYTE P554_XTAL16M_CLOSE_RTS_DELAY[8];  // 300-39400:  0-7

extern const BYTE SYN_WORD_CDT_EB90[6];
extern const BYTE SYN_WORD_CDT_D709[6];
extern const BYTE SYN_WORD_BUS[6];
extern const BYTE SYN_WORD_LSA[6];
extern const BYTE SYN_WORD_SET[6];
extern const BYTE PROTOCOL_CDTBB_SOURCE_ADDR[12];
extern const BYTE PROTOCOL_CDTBB_DESTIN_ADDR[12];
extern const BYTE PROTOCOL_CDTBB_CONTROL_BYTE[12];
extern const BYTE PROTOCOL_CDT9702_SOURCE_ADDR[12];
extern const BYTE PROTOCOL_CDT9702_DESTIN_ADDR[12];
extern const BYTE PROTOCOL_HOSTZF_SOURCE_ADDR[12];
extern const BYTE PROTOCOL_HOSTZF_DESTIN_ADDR[12];
extern const BYTE PROTOCOL_C9302_SOURCE_ADDR[12];
extern const BYTE PROTOCOL_C9302_DESTIN_ADDR[12];
extern const BYTE PROTOCOL_ZLPCDT_SOURCE_ADDR[12];
extern const BYTE PROTOCOL_ZLPCDT_DESTIN_ADDR[12];
extern const BYTE CAN0_AS_BH_MON_ADDR_SET;
extern const BYTE HOSTZF_PROTOCOL_YK_TYPE[12];
//extern const BYTE TIME_VERIFY_PORT;

extern const BYTE LSA_INTERN_POLLING_TURN[16];
extern const WORD LSA_WAIT_TIME_VALUE[32];
extern const WORD YC_f_value_selected[8];    
extern const BYTE  IED_TX_STATE_START_WORD;
extern const BYTE  IED_TX_STATE_DWORD_NUM;
extern const BYTE  PROTOCOL_CDT_YKFG;
extern const WORD  BH_EVENT_TO_SOE_YX_RETURN_TIME;
extern const BYTE  IED_SOURCE_ADDRESS[12];
extern const BYTE MON_FUN_NO[12][5];
extern struct  UNIT_INFO {unsigned short yc_line_start; unsigned short yc_line_num;
                          unsigned short yc_val_start ; unsigned short yc_val_num;   
                          unsigned short yx_start;      unsigned short yx_num;
                          unsigned short yk_start;      unsigned short yk_num;
                          unsigned short dc_start;      unsigned short dc_num;
                          unsigned short ym_start;      unsigned short ym_num;
                          unsigned short protect_addr;  unsigned short protect_unit;
                           //H: pannel L:unit
                          unsigned short unit_type;     unsigned short channel_no;
                         } const unit_info[256];

extern struct  PORT_INFO {unsigned short protocal_type;  
                          unsigned short mirror_unit; // start - end
                          unsigned short bits;
                          unsigned short stopbit;
                          unsigned short bauds;
                          unsigned short semiduplex_parity;
                          unsigned short scan_panel;  // start - end
                          unsigned short scan_unit;   // start - end
                          unsigned short YK_enable;
                          unsigned short protect_enable;
                          unsigned short time_enable;
                          unsigned short table_no;    // YC - YX
                          unsigned short rebound;     //flag  char
                         } const port_info[14];

extern struct HOOK_UP               //������ͼ���ñ�
{
      WORD total_num;
      BYTE name[32];
      BYTE type[64];               //ZY    Y(obj type)--3q Tran: 0;   2q Tran: 1;  Cap: 2;  Line: 3;
      BYTE start_no[64];           //      Z(xs  type)--0:     208;   1: DSA-CAN;
}const  hook_up;

extern struct ELE                   //�������ñ�
{
      BYTE mt_name[32];
      WORD Ua_No;
      WORD Ub_No;
      WORD Uc_No;
      WORD Ia_No;
      WORD Ib_No;
      WORD Ic_No;
      WORD P_No;
      WORD COS_No;
      WORD YX_No;
}const ele[64];
extern const char code_name[16];
extern const char authorize_date[16];
extern const int  code_start_addr;
extern const int code_end_addr;
extern const int code_struct_cn;

extern const WORD YX_property[256];
extern const WORD YX_YM[256];
extern const WORD YX_double[256];
extern const WORD YX_delay[512];
extern const WORD YC_transmite_table[4][512];
extern const WORD YX_transmite_table[4][1024];
extern const BYTE YM_transmite_table[4][256];


extern const BYTE YC_coef_table[1024];

extern struct BYQ_DW { WORD yc_val_no;    
                       WORD yx_no[32];
                       BYTE yx_weight[32];
                     }const Byq_Dw[8];

extern struct BH_EVENT_TO_SOE { WORD YX_State_No;
	                            BYTE GroupAddr;    
                                BYTE RLYAddr;
                                BYTE ActionType;
                                BYTE DeviceType;
                              }const BH_Event_To_SOE[512];

extern struct VQC_INFO { BYTE DSA_243_Addr;
                         BYTE Reserved;
	                     BYTE YK0_208_YK_BOARD_Addr;
	                     BYTE YK0_208_YK_BOARD_YKNO;
	                     BYTE YK1_208_YK_BOARD_Addr;
	                     BYTE YK1_208_YK_BOARD_YKNO;
	                     WORD YC_NO_HighSide[16];
	                     WORD YC_NO_LowSide[16];
	                     WORD YX_NO[32];
                       }const Vqc_Info[2];

extern struct  DDB_ADDR { BYTE DDB_PORT_ADDR[6];
                        }const DDB_Addr[128];

extern const BYTE  BH_Act_Alarm_Type[256];  

extern const WORD  YX_OR_Table[64][9];
	                              
extern struct  MONI_SY_YX_TABLE{ BYTE   YX_Box_Def;  // d7: if YX led exist; d6:if is double YX; d3-d0:YX_NO in a box
                                 BYTE   YX_Box_Addr;
                                 WORD   YX0_Source_No;  // if single YX only YX0 valid
                                 WORD   YX1_Source_No;
                               }const MONI_SY_YX_Table[256];
                               
extern struct  MONI_SY_YC{ WORD      YC_BOX_ADDR;
						   WORD      YC_SELECT_NO;
	                       short     YC_MUL_XS;    
                           short     YC_DIV_XS;
                           short     YC_ADD_OFFSET;
                         }const Moni_SY_YC[256];

extern struct  YC_ADD{ WORD      YC_SELECT_NO[8];
	                   short     YC_MUL_XS[8];    
                       short     YC_DIV_XS[8];
                       WORD      YC_DESTIN_NO;
                     }const YC_Add[8];

extern struct  SC1801_YK_TABLE {unsigned char  sc1801_no;
                                unsigned char  sys_yk_no;
                               }const SC1801_YK_Table[128];
extern  struct MODBUS_INFO 
              {
               unsigned short yc_address; unsigned short yc_num;
               unsigned short yx_address; unsigned short yx_num;
               unsigned short ts_address; unsigned short ts_num;
               unsigned short yk_address; unsigned short yk_num;
               unsigned short yk_h_addr[4]; unsigned short yk_f_addr[4];
              }const MODBUS_info[32];
              
extern struct BH_EVENT_TO_SOE_RET
              {
               WORD YX_State_No;
               BYTE GroupAddr;
               BYTE RLYAddr;
               BYTE ActionType;
               BYTE DeviceType;
              }const BH_Event_To_SOE_Ret[512];
extern const  WORD  YC_LIMIT_VALUE[512];

extern struct SOE_ZF_INFO
{
     BYTE SOE_START_YX_NO;    //SOE �ڱ���Ԫ�д�ŵ���ʼ��ţ��ں�̨���ʱ��Ӧ���Ϊ����ԪYX��ʼ��+SOE_START_YX_NO[]+ƫ����
     BYTE SOE_SOURCE_NO[48];  //IED ��Ԫ�����ṩ��SOE��Ϣ���
}const Soe_zf_info[12][32];

extern const WORD  XH_SHSY_LINE_NO[6][32];

extern struct ZLP_SEND_SET
{
     BYTE length;         //0Ϊ����������ֱ����
     BYTE addr_no;        //��ַ�ڷ��ͱ����еĵ�� >32Ϊ�޵�ַ
     BYTE check_type;     //У�������  0Ϊ��У�� 1��У�� 2���У�� 3 bchУ�� 4 CRC16У��
     BYTE check_start;    //��У�� �����е���ʼ���
     BYTE check_length;   //��У�� �����е�У�鳤��
     BYTE set;            //����
     WORD check_no;       //У���ڱ����еĵ��,���ͱ�����У��Ϊһ���ֽ���0xff**(��λ��Ӧ��š���λ��Ӧ���)
     			  //		�����ͱ�������У�飬���ձ�����У��,��0xfff0����һ���ֽ�У��
     			  //          0xf1f0���������ֽڵ�ǰ�ߺ�,0xf0f1���������ֽڸ�ǰ�ͺ�			
     BYTE value[32]; 	  //���͵�����
}const Zlp_send_set[8];
extern struct ZLP_REC_SET
{
     BYTE addr_no;        //��ַ�ڷ��ͱ����еĵ��
     BYTE num_length_no;  //���ݵĳ������ڵ��    
     BYTE check_start;    //��У�� �����е���ʼ���
     BYTE check_length;   //��У�� �����е�У�鳤�ȼ�ȥ���ݳ��ȵĲ�ֵ(��ȥ���ݣ����м�λ������У��)
     BYTE head[10];       //�̶������ڱ����еĵ��(0xff����)
     BYTE yc_type;        //ң������� 00--16����  01--10����
     BYTE yc_div;	  //��С��ϵ��
     BYTE length;	  //�����������ֽڵĳ��ȣ��ܱ��ĵĳ��ȼ�ȥ�����ڳ���ֵ��
     BYTE set1;	  	  //����
     BYTE set2;	  	  //����
     BYTE set3;	  	  //����
     BYTE value[48];      //���յ�����	ң������Ϊf0,f1,f2....fe
     			  //		ң������Ϊ00,01,02.....����00Ϊ��һ��ң��ĸ�λ01��һ��ң��ĵ�λ
			  //            02Ϊ�ڶ���ң���λ03Ϊ�ڶ���ң���λ��
			  //����Ҫ�����������ff��	

}const Zlp_rec_set[8];
extern const struct LED_MEA_INFO
{ 
	WORD     xianlu_no;
	WORD     YC_NO[16];
}LED_mea_info[32];
extern const struct LED_GJ_INFO
{ 
	WORD     xianlu_no;
	WORD     YX_NO;
}LED_GJ_info[32];
extern const WORD m_crc16_table[256];
extern const BYTE CAN_important_yc_unit[128];
extern  struct YC_OFFSET
{
     WORD yc_no;         //������վ��Ӧң����
     WORD offset;        //��Ӧң��ƫ����,��վң��ֵ��YC_State[]-ƫ����
}const yc_offset[4][32];

extern struct BH_EVENT_ALARM
{
     BYTE warn_yk_add;    //dsa208 yk address:0x30~0x3f
     BYTE warn_yk_no;     //dsa208 yk hao:0~4
     BYTE warn_yk_hf;     //dsa208 yk  H:0xcc  F:0x33
     BYTE warn_FG_yk_add; //dsa208 yk address:0x30~0x3f
     BYTE warn_FG_yk_no;  //dsa208 yk hao:0~4
     BYTE warn_FG_yk_hf;  //dsa208 yk  H:0xcc  F:0x33     
     BYTE action_yk_add;  //dsa208 yk address:0x30~0x3f
     BYTE action_yk_no;   //dsa208 yk hao:0~4
     BYTE action_yk_hf;   //dsa208 yk  H:0xcc  F:0x33
     BYTE action_FG_yk_add; //dsa208 yk address:0x30~0x3f
     BYTE action_FG_yk_no;  //dsa208 yk hao:0~4
     BYTE action_FG_yk_hf;  //dsa208 yk  H:0xcc  F:0x33      
}const bh_event_alarm ;

extern struct YCUP_TO_YX
	{
	WORD yc_no;		//ң����
	WORD yx_no;		//ң�ŵ��
	WORD yc_up_value;       //Խ��ֵ
	WORD yc_hc_value;	//�ز�
	BYTE yc_up_down;	//AA���޸澯��55���޸澯
}const ycup_to_yx[32];	

extern const WORD IEC101_YC_Chg_Out_Table[512];
extern struct YX_AND_Table
	{
	 WORD yx_destion_no;		      //ң����Ŀ����
	 WORD yx_source_no[8];		    //ң����Դ���
	 BYTE yx_source_not_flag[8];  //��Դ��ң������ȡ����   0xaa:��Ӧң�ŵ�ֵΪ0,ȡ�� 0x55:��Ӧң�ŵ�ֵΪ1,��ȡ��
	}const yx_and_table[64];	

extern struct LOG_OUT
	{
	 BYTE yk_add;		//ң�ذ��ַ 0x30~0x3f
	 BYTE yk_no;		//ң�ص��   0~7
	 WORD yx_no;		//ң�ŵ��
	}const log_out[64];
extern struct NET_INFO
	{
	 BYTE IP_MSB;		
	 BYTE IP1;	
	 BYTE IP2;	
	 BYTE IP_LSB;
	 BYTE SUBNETMASK_MSB;
	 BYTE SUBNETMASK1;
	 BYTE SUBNETMASK2;
	 BYTE SUBNETMASK_LSB;
	 BYTE GATEWAY_MSB;
	 BYTE GATEWAY1;
	 BYTE GATEWAY2;
	 BYTE GATEWAY_LSB;
	 BYTE MAC_ADD_MSB;
	 BYTE MAC_ADD1;
	 BYTE MAC_ADD2;
	 BYTE MAC_ADD3;
	 BYTE MAC_ADD4;
	 BYTE MAC_ADD5;
	 BYTE MAC_ADD_LSB;		 
	 WORD TCP_PORT_NO;		
	 WORD UDP_REC_PORT_NO;		
	 WORD UDP_SEND_PORT_NO;	
	}const net_info[8];
extern struct NTS_SOE_ZF_INFO
{
     BYTE SOE_START_YX_NO;    //SOE �ڱ���Ԫ�д�ŵ���ʼ��ţ��ں�̨���ʱ��Ӧ���Ϊ����ԪYX��ʼ��+SOE_START_YX_NO[]+ƫ����
     BYTE NTS_GIN_ZuNO[32];
     BYTE NTS_GIN_ItemNO[32];  //
}const NTS_Soe_zf_info[4][32];
	
#endif /* _rom_h */
