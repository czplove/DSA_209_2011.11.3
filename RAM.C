/*****************************************************************************/
/*       FileName  :   RAM.C                                                 */
/*       Content   :   DSA-208 RAM Module                                    */
/*       Date      :   Fri  02-22-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/



#pragma  regconserve


#include _SFR_H_
#include _FUNCS_H_

#include "common.h"




/***********************************************/
/* Definition of global variables      [NEAR]  */
/***********************************************/

//    for CPU use
//    -----------------------------------------------------------
near  WORD                             RAM_CPU_RST_TAG1;
near  WORD                             RAM_CPU_RST_TAG2;
#pragma locate (RAM_CPU_RST_TAG2       = 0x002002)
#pragma locate (RAM_CPU_RST_TAG1       = 0x002000)

near  TYPE_CPU_INT_MONI                RAM_CPU_Int_Moni;
near  WORD                             RAM_CPU_INT_Rst_Cn[8];
#pragma locate (RAM_CPU_Int_Moni       = 0x002010)
#pragma locate (RAM_CPU_INT_Rst_Cn     = 0x002020)

near  BYTE                             Rcd_Info_Myself_Tmp[8];  // for data trans from function call
near  BYTE                             Rcd_Info_System_Tmp[8];  // for data trans from function call
#pragma locate (Rcd_Info_Myself_Tmp    = 0x002030)
#pragma locate (Rcd_Info_System_Tmp    = 0x002038)
//    for CPU use end


//    for CAN use
//    -----------------------------------------------------------
near  TYPE_CAN_RAM_PARA                RAM_CAN0_Para;
#pragma locate (RAM_CAN0_Para          = 0x002040)

near  BYTE                             CAN0_CurrentSta_Buf[MAX_CAN_NUMBER];
near  BYTE                             CAN0_IED_StaChg[MAX_CAN_NUMBER];
#pragma locate (CAN0_CurrentSta_Buf    = 0x002080)
#pragma locate (CAN0_IED_StaChg        = 0x0020c0)

near  BYTE                             CAN0_ReceCOS_Buf[3*512];
near  BYTE                             CAN0_COSACK_Buf[16];
near  BYTE                             CAN0_ReceYCCHG_Buf[64];
#pragma locate (CAN0_ReceCOS_Buf       = 0x002100)
#pragma locate (CAN0_COSACK_Buf        = 0x002700)
#pragma locate (CAN0_ReceYCCHG_Buf     = 0x002710)

near  BYTE                             CAN0_RecePort_Buf[12*256];
near  BYTE                             CAN0_ReceProc_Buf[512];
near  BYTE                             CAN0_TranProc_Buf[512];
#pragma locate (CAN0_RecePort_Buf      = 0x002750)
#pragma locate (CAN0_ReceProc_Buf      = 0x003350)
#pragma locate (CAN0_TranProc_Buf      = 0x003550)

near  BYTE                             CN_CAN0_Comm_Error[MAX_CAN_NUMBER];
near  BYTE                             CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER];
near  BYTE                             CAN0_Comm_CurrentSta[MAX_CAN_NUMBER];
#pragma locate (CN_CAN0_Comm_Error     = 0x003750)
#pragma locate (CAN0_Comm_Error_LastSta= 0x003790)
#pragma locate (CAN0_Comm_CurrentSta   = 0x0037d0)


near  BYTE                             CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr;
near  BYTE                             CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr;
#pragma locate (CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr   = 0x003810)
#pragma locate (CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr   = 0x003811)

near  WORD                             CAN_Time2048ms_Last_Value;
near  WORD                             CAN_Not_Idle_Cn;
#pragma locate (CAN_Time2048ms_Last_Value          = 0x003812)
#pragma locate (CAN_Not_Idle_Cn                    = 0x003814)
near  BYTE                             CAN_NOW_POL_YCALL_FLAG;
#pragma locate (CAN_NOW_POL_YCALL_FLAG             = 0x003816)
near  WORD			       CAN_RESTART_ASK_TIME;
#pragma locate (CAN_RESTART_ASK_TIME             = 0x003818)
//    for CAN use end


//    for PORT (P554 & CAN & other) COMUSE
//    -----------------------------------------------------------
//    internal program: 0-11  , 16c554
//                      12    , CAN0
//                      13    , CAN1
//                      14    , CPU_COM
//                      15    , reserved
near  WORD                             Cn_SIO_CAN_Reset[16];
#pragma locate (Cn_SIO_CAN_Reset       = 0x003820)

near  WORD                             SIO_CAN_Need_Reset;
#pragma locate (SIO_CAN_Need_Reset     = 0x00381e)
//    for PORT (P554 & CAN & other) COMUSE end


//    for P554 use
//    -----------------------------------------------------------
near  WORD                             port_recv_pt[12];
near  WORD                             port_send_pt[12];
near  WORD                             port_recv_dl[12];
near  WORD                             port_send_len[12];
#pragma locate (port_recv_pt           = 0x003840)
#pragma locate (port_send_pt           = 0x003860)
#pragma locate (port_recv_dl           = 0x003880)
#pragma locate (port_send_len          = 0x0038a0)

near  WORD                             P554_Port_Tran_Close_RTS_Cn[12];
near  BYTE                             P554_Port_Transing_Lastbyte[12];
#pragma locate (P554_Port_Tran_Close_RTS_Cn          = 0x0038c0)
#pragma locate (P554_Port_Transing_Lastbyte          = 0x0038e0)
//    for P554 use end


//    for Protocol use
//    -----------------------------------------------------------
near  WORD                             YC_State[2048];  // 2048 words
near  WORD                             YX_State[256];   //  256 words 256*16 bit 
near  UL                               YM_State[256];   //  256 dwords
#pragma locate (YC_State               = 0x004000)
#pragma locate (YX_State               = 0x005000)
#pragma locate (YM_State               = 0x005200)


near  BYTE                             BCH_Array[6];    // used only in main_loop
#pragma locate (BCH_Array              = 0x006000)

near  BYTE                             Portx_Poll_First[14];
near  BYTE                             HOST_ZF_enable;
near  BYTE                             HOST_YK_Doing;
near  BYTE                             HOST_YK_Port_No;
near  WORD                             HOST_YK_Doing_Begin_Time;
#pragma locate (Portx_Poll_First       = 0x006006)
#pragma locate (HOST_ZF_enable         = 0x006018)
#pragma locate (HOST_YK_Doing          = 0x006019)
#pragma locate (HOST_YK_Port_No        = 0x00601a)
#pragma locate (HOST_YK_Doing_Begin_Time = 0x00601c)

near  WORD                             BH_To_YX_Delay_Begin_Time[512];
#pragma locate (BH_To_YX_Delay_Begin_Time    = 0x006100)
near  BYTE       YX_FG_FLAG;
#pragma locate (YX_FG_FLAG = 0x006020)
near  BYTE       YX_FG_WAIT_FLAG;
#pragma locate (YX_FG_WAIT_FLAG = 0x006021)
near  WORD       YX_FG_WAIT_TIME;
#pragma locate (YX_FG_WAIT_TIME = 0x006022)
near  BYTE       VQC_ON_FLAG[12];
#pragma locate (VQC_ON_FLAG = 0x006024)
near  BYTE       VQC_OFF_FLAG[12];
#pragma locate (VQC_OFF_FLAG = 0x006044)
near  BYTE       DZ_RESEND_FLAG;
#pragma locate (DZ_RESEND_FLAG = 0x006064)
near  BYTE       dz_resend_cnt;
#pragma locate (dz_resend_cnt = 0x006065)
near  BYTE       DSA8000_DZLB_FLAG;
#pragma locate (DSA8000_DZLB_FLAG = 0x006066)
near  WORD                             BH_Report_Bank_Sav_Ptr;    
near  WORD                             BH_Report_Bank_Tak_Ptr[12];    
#pragma locate (BH_Report_Bank_Sav_Ptr = 0x006500)
#pragma locate (BH_Report_Bank_Tak_Ptr = 0x006502)
near  BYTE                      IEC_last_frame_buf[12][256];
#pragma locate (IEC_last_frame_buf       = 0x006520)

near  BYTE                             temp_buf[13];
#pragma locate (temp_buf       = 0x007120)
near  BYTE                             time_buf[32][32];
#pragma locate (time_buf       = 0x007140)

near WORD  			YC_Chg_FLAG[2048];
#pragma locate (YC_Chg_FLAG    = 0x8200)

near WORD            YC_Chg_Last_Value[4][256];
#pragma locate (YC_Chg_Last_Value=0x9200)      //7200-b1ff

near BYTE 	     MAIN_DSA_208_VER_INFO[16];
#pragma locate (MAIN_DSA_208_VER_INFO=0xf000)      //f000-f010
/***********************************************/
/* Definition of global variables      [FAR]   */
/***********************************************/
// before all below is from rambank.c
BYTE            Register_554[12][512]; 
#pragma locate (Register_554        = 0xf61000)	//-对应什么具体的可视含义是无意义的,实际有价值的是具体译码地址.这里把这个数组映射了这个空间




WORD            main_loop;
#pragma locate (main_loop           = 0x010000)
BYTE            p2_img;
#pragma locate (p2_img              = 0x010002)
BYTE            Dsa208_Work_State;
#pragma locate (Dsa208_Work_State   = 0x010003)
// here reserved 0003H-06FFH

/********** Set Port ********/
BYTE            set_port_rece_send_buf[256];
#pragma locate (set_port_rece_send_buf   = 0x010700)
BYTE            set_port_rs_buf_sav_pt;
#pragma locate (set_port_rs_buf_sav_pt   = 0x010800)
BYTE            set_port_rs_buf_tak_pt;
#pragma locate (set_port_rs_buf_tak_pt   = 0x010801)
// here reserved 0802H-087FH
BYTE            set_rpt_recv_finish;
BYTE            set_transmit_flag;
WORD            set_send_pt;
WORD            set_send_len;
#pragma locate (set_rpt_recv_finish = 0x010880)
#pragma locate (set_transmit_flag   = 0x010881)
#pragma locate (set_send_pt         = 0x010882)
#pragma locate (set_send_len        = 0x010884)

BYTE            set_len;
BYTE            set_type;
WORD            set_offset;
WORD            set_locate;
#pragma locate (set_len             = 0x010886)
#pragma locate (set_type            = 0x010887)
#pragma locate (set_offset          = 0x010888)
#pragma locate (set_locate          = 0x01088a)   

BYTE            set_syn_char_no;
BYTE            set_rpt_char_no;  
#pragma locate (set_syn_char_no     = 0x01088c)
#pragma locate (set_rpt_char_no     = 0x01088d)   

BYTE            set_port_mon_status;  
BYTE            set_port_mon_need_reply;  
WORD            set_mon_begin_wait_ied_clk;  
WORD            set_mon_begin_wait_host_clk;  
#pragma locate (set_port_mon_status         = 0x01088e)   
#pragma locate (set_port_mon_need_reply     = 0x01088f)   
#pragma locate (set_mon_begin_wait_ied_clk  = 0x010890)   
#pragma locate (set_mon_begin_wait_host_clk = 0x010892)   
// here reserved 0894H-08FFH

WORD            set_buffer[256];
#pragma locate (set_buffer          = 0x010900)

BYTE            set_transmit[256];
BYTE            set_send_buf[512];
BYTE            set_recv_buf[512];
#pragma locate (set_transmit        = 0x010b00)
#pragma locate (set_send_buf        = 0x010c00)
#pragma locate (set_recv_buf        = 0x010e00)



// move from '0x010a00' to '0x011000'
WORD           port_flag[14][128];
#pragma locate (port_flag           = 0x011000)
	
// here reserved 1e00H-1FFFH
BYTE            port_recv[12][512];
#pragma locate (port_recv           = 0x012000)
// here reserved 3800H-3FFFH
BYTE            port_send[12][512];
#pragma locate (port_send           = 0x014000)
// here reserved 5800H-5FFFH
BYTE            port_transmit[16][512];    // can include
#pragma locate (port_transmit       = 0x016000)
BYTE            port_efficacy[12][512];
#pragma locate (port_efficacy       = 0x018000)
// here reserved 9800H-9FFFH

BYTE            port_deal_buf[12][512];
#pragma locate (port_deal_buf       = 0x01a000)

BYTE            port_report[512];
#pragma locate (port_report         = 0x01b800)

WORD            port_mon[16];              // can include
#pragma locate (port_mon            = 0x01ba00)
BYTE            port_transmit_flag[16];    // can include
#pragma locate (port_transmit_flag  = 0x01ba20)
// here reserved BA40H-BFFFH


// Exchange
WORD            Ex_YK_start[14];
#pragma locate (Ex_YK_start         = 0x01c000)
WORD            Ex_YK_end[14];
#pragma locate (Ex_YK_end           = 0x01c020)

WORD            exchange_unit;
#pragma locate (exchange_unit       = 0x01c040)
WORD            exchange_src_port;
#pragma locate (exchange_src_port   = 0x01c042)
WORD            exchange_target_port;
#pragma locate (exchange_target_port= 0x01c044)
WORD            exchange_type;
#pragma locate (exchange_type       = 0x01c046)
WORD            exchange_Len;
#pragma locate (exchange_Len        = 0x01c048)
BYTE           *exchange_src_pt;
#pragma locate (exchange_src_pt     = 0x01c04a)
BYTE            exchange_buf[512];
#pragma locate (exchange_buf        = 0x01c100)
// here reserved C300H-C3FFH

//-下面这个数组结构体记录的顺序是这样的一个结构体单元可以2个字的数据,为了保持连续性若出现落单的
//-还是连续的原则,比如第一个字空着从第二个字开始记录,以便和后面的统一
struct YX_CHANGE {unsigned short offset_no;  //确定变位YX的偏移量,,这个值只有可能是0,2,4,6等的偏移量
                  unsigned short chg_cont0;  //变位YX帧的YX值，每帧2个字，WORD0
                  unsigned short chg_cont1;  //变位YX帧的YX值，每帧2个字，WORD1
                  unsigned short chg_bit0;   //如chg_bit0对应的位值为1，则表示该位YX产生变位
                  unsigned short chg_bit1;   //如chg_bit1对应的位值为1，则表示该位YX产生变位
                 }yx_change[256];    
#pragma locate (yx_change           = 0x01c400)

struct YX_EVENT { unsigned short soe_ms;
                  unsigned short soe_sec_min;
                  unsigned short soe_hor_date;
                  unsigned short soe_chn_sta; //SOE对象号和性质（合/分） 
                 }yx_event[1024];    
#pragma locate (yx_event            = 0x01ce00)

/*!!!!!!!!!!!!!!!!!!!!! data transmite buffer !!!!!!!!!!!!!!!!!!!!!!!!*/
unsigned short YX_state_tr[4][64];  // 64*16=1024
#pragma locate (YX_state_tr         = 0x01ee00)

struct YX_CHANGE_TR {unsigned short offset_no;
                     unsigned short chg_cont0;
                     unsigned short chg_cont1;
                     unsigned short chg_bit0;
                     unsigned short chg_bit1;
                    }yx_change_tr[4][256];    
#pragma locate (yx_change_tr        = 0x01f000)

struct YX_EVENT yx_event_tr[4][1024];    
#pragma locate (yx_event_tr         = 0x021800)


WORD            yx_chg_out_pt[14]; //变位YX帧已发送出去的帧数
#pragma locate (yx_chg_out_pt       = 0x029800)

WORD            yx_soe_out_pt[14]; //SOE帧已发送出去的帧数
#pragma locate (yx_soe_out_pt       = 0x029820)

WORD            yx_chg_ex_pt[14]; //统计每个端口中变位YX个数
#pragma locate (yx_chg_ex_pt        = 0x029840)

WORD            yx_soe_ex_pt[14];
#pragma locate (yx_soe_ex_pt        = 0x029860)

WORD            yx_chg_tr_ex_pt[4]; //统计变位YX帧的个数
#pragma locate (yx_chg_tr_ex_pt     = 0x029880)

WORD            yx_soe_tr_ex_pt[4];
#pragma locate (yx_soe_tr_ex_pt     = 0x029888)

WORD            yx_chg_tr_in_pt[4]; //统计每个yx_transmite_table中变位YX个数
#pragma locate (yx_chg_tr_in_pt     = 0x029890)

WORD            yx_soe_tr_in_pt[4];
#pragma locate (yx_soe_tr_in_pt     = 0x029898)

WORD            yx_chg_in_pt;   //统计变位YX帧的个数
#pragma locate (yx_chg_in_pt        = 0x0298a0)

WORD            yx_soe_in_pt;  //统计SOE帧的个数
#pragma locate (yx_soe_in_pt        = 0x0298a2)





// below are for each port self use area:
BYTE            bus_unit_err[64];
#pragma locate (bus_unit_err        = 0x0298c0)

BYTE            bus_err_rec[224];
#pragma locate (bus_err_rec         = 0x029900)

BYTE            bus_err_rec_pt;
#pragma locate (bus_err_rec_pt      = 0x0299e0)
// here reserved 189E1H-189FFH

BYTE           unit_set_flag[256];                       // for what use by LHY?
#pragma locate (unit_set_flag       = 0x029a00)





BYTE           BH_Saved_Report_Bank[256][32];  // include CAN
#pragma locate (BH_Saved_Report_Bank= 0x029b00)  // +0x2000
// here reserved 2bb00H-2daffH     for BH_Saved_Report_Bank expand to [512][32]



// [0] Trans_state     [1] CAN246_ADDR   [2] Port_No         [3] Unit_No
// [4] Action_Type     [5] Switch_No     [6] CAN243_ADDR     [7] CAN_YK_No
BYTE           CAN_246_Trans_YK_Buf[8];
#pragma locate (CAN_246_Trans_YK_Buf          = 0x02dd00)  
WORD           CAN_246_YK_Begin_WAIT_VERIFY;
#pragma locate (CAN_246_YK_Begin_WAIT_VERIFY  = 0x02dd08)  
WORD           CAN_246_YK_Begin_WAIT_CONFIRM;
#pragma locate (CAN_246_YK_Begin_WAIT_CONFIRM = 0x02dd0a)  

// reserved 0x02dd10-0x02dd9f
BYTE           bh_event_alarm_warn;    
BYTE           bh_event_alarm_action;    
BYTE           bh_event_alarm_now_action_warn;    
BYTE           bh_event_alarm_Reserved;    
WORD           bh_event_alarm_warn_Begin_Clk;    
WORD           bh_event_alarm_action_Begin_Clk;    
#pragma locate (bh_event_alarm_warn            = 0x02dda0)
#pragma locate (bh_event_alarm_action          = 0x02dda1)
#pragma locate (bh_event_alarm_now_action_warn = 0x02dda2)
#pragma locate (bh_event_alarm_Reserved        = 0x02dda3)
#pragma locate (bh_event_alarm_warn_Begin_Clk  = 0x02dda4)
#pragma locate (bh_event_alarm_action_Begin_Clk= 0x02dda6)


//   xxxx'yyyy
//   bit 4 =1        trans receive
//   bit 5 =1        trans send
//   bit 6, bit 7    no use
//   bit 3-0         destin port_no   0-11 P554; 12 CAN0; 13 CAN1; 14 cpucom;
BYTE            port_mirror[16];   
#pragma locate (port_mirror              = 0x02ddb0)


//   bit 7 = 1      Now may send on CAN1
//   bit 6 = 1      Now may send on CAN0
//   bit 5 = 1      Now may rece on CAN1
//   bit 4 = 1      Now may rece on CAN0
BYTE            Now_CANx_Sw_Channel_Sta[256];  // include addr 64-255
#pragma locate (Now_CANx_Sw_Channel_Sta  = 0x02de00)


BYTE           CAN0_CPUASKCMU_Trans_Buf[256*12];
#pragma locate (CAN0_CPUASKCMU_Trans_Buf = 0x02e000)

WORD           SC1801_YC_Last_State[192];    
#pragma locate (SC1801_YC_Last_State     = 0x02ec00)

WORD           SC1801_YC_Current_State[192];    
#pragma locate (SC1801_YC_Current_State   = 0x02ed80)

BYTE           SC1801_YX_Last_State[64];    
#pragma locate (SC1801_YX_Last_State     = 0x02ef00)

BYTE           SC1801_YX_Current_State[64];    
#pragma locate (SC1801_YX_Current_State  = 0x02ef40)
BYTE    moni_yx_chg_sav_ptr;
#pragma locate (moni_yx_chg_sav_ptr=0x2ef80)  // 
BYTE    moni_yx_chg_tak_ptr[12];
#pragma locate (moni_yx_chg_tak_ptr=0x2ef90)  //  

WORD    moni_yx_chg[256];
#pragma locate (moni_yx_chg        = 0x2efa0)  // 
WORD    moni_yx_last_state[32];
#pragma locate (moni_yx_last_state = 0x2f1a0)  //
BYTE            LC_WBL_Set_Flag;   
WORD            LC_WBL_Set_New;   
WORD            LC_WBL_Set_Current;   
#pragma locate (LC_WBL_Set_Flag     = 0x2f1e0)
#pragma locate (LC_WBL_Set_New      = 0x2f1e2)
#pragma locate (LC_WBL_Set_Current  = 0xf41e80)
WORD            yc_chg_in_count[16];
#pragma locate (yc_chg_in_count     = 0x2f1e4)
WORD            yc_chg_out_count[16];
#pragma locate (yc_chg_out_count     = 0x2f204)
WORD            last_yc[16][512];
#pragma locate (last_yc     = 0x2f224)

struct YC_CHANGE_TR {unsigned short chg_yc;
                     unsigned short yc_inf;
                    }yc_change_tr[16][512];    
#pragma locate (yc_change_tr        = 0x33224)
BYTE    mon_ok_flag[16];
#pragma locate (mon_ok_flag=0x3b224)  //
WORD    mon_err_start_time[16];
#pragma locate (mon_err_start_time=0x3b234)  // 

struct PRINTF_INFO{unsigned char bh_pannel;
                   unsigned char bh_unit;
                   unsigned char bhdz_printf_flag;
                   unsigned char bh_printf_flag;
                   unsigned char soe_printf_flag;
                  }printf_info;    
#pragma locate (printf_info        = 0x3b254)

BYTE           BH_Bank_Report[11][512];
#pragma locate (BH_Bank_Report      = 0x03B260)
BYTE           log_out_yx_state[16];
#pragma locate (log_out_yx_state      = 0x03c860)

struct LED_MEA_BUF{
	                 unsigned char  xianlu_nums;
	                 unsigned short xianlu_no[32];
                   unsigned short yc_state[512];
                  }LED_mea_buf;
#pragma locate (LED_mea_buf        = 0x03c870)

struct LED_GJ_BUF{
	                 unsigned char  xianlu_nums;
	                 unsigned short xianlu_no[32];
                   unsigned short yx_state[2];
                  }LED_GJ_buf;
#pragma locate (LED_GJ_buf        = 0x03ccb2)



  