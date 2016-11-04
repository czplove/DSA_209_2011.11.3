/*****************************************************************************/
/*       FileName  :   LFP20.C                                             */
/*       Content   :   DSA-208 LFP20    Module                             */
/*       Date      :   Thu  02-28-2003                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/



#pragma  regconserve

#include _SFR_H_
#include _FUNCS_H_

#include "common.h"
#include "device.h"
#include "reg.h"
#include "ram.h"
#include "rom.h"
#include "comuse.h"





    
    
    
/*---------------------------------------------------------------------------*/
/*                    Definition  of  global  variables                      */
/*---------------------------------------------------------------------------*/

//    none


/*---------------------------------------------------------------------------*/
/*                    Definition  of  local  variables                       */
/*---------------------------------------------------------------------------*/
register  far  WORD                          *LFP20_PARA_BASE_ADDR;

#define   LFP20_PORT_NO                     port_no

#define   TIME_LFP20_POLL_TIME_VALUE        800                 // 1000ms=1s

#define   LFP20_MON_IDLE                    0x00
#define   LFP20_MON_TRANSING_TO_IED         0x01
#define   LFP20_MON_TRANSING_FROM_HOST      0x02  // no use
#define   LFP20_MON_WAIT_HOST_REPLY         0x03
#define   LFP20_MON_WAIT_IED_REPLY          0x04
#define   LFP20_MON_SEND_OK                 0x05
#define   LFP20_MON_SEND_ERROR              0x06
#define   LFP20_MON_WAIT_NOTHING            0x00
#define   LFP20_MON_WAIT_IED_YX             0x01
#define   LFP20_MON_WAIT_IED_YC             0x02

#define   LFP20_SHORT_FRAME                 0x01
#define   LFP20_LONG_FRAME                  0x02

#define   LFP20_CODE_ACK                    0x06
#define   LFP20_CODE_NAK                    0x15

#define   LFP20_CODE_C1                     0x40
#define   LFP20_CODE_C2                     0x41
#define   LFP20_CODE_C3                     0x42
#define   LFP20_CODE_C4                     0x43
#define   LFP20_CODE_C5                     0x44
#define   LFP20_CODE_C6                     0x45
#define   LFP20_CODE_C7                     0x46
#define   LFP20_CODE_C8                     0x47
#define   LFP20_CODE_C9                     0x48
#define   LFP20_CODE_C10                    0x49
#define   LFP20_CODE_C11                    0x4a
#define   LFP20_CODE_C12                    0x4b
#define   LFP20_CODE_C13                    0x4c
#define   LFP20_CODE_C14                    0x4f
#define   LFP20_CODE_C15                    0x50
#define   LFP20_CODE_C16                    0x51
#define   LFP20_CODE_C17                    0x52
#define   LFP20_CODE_C18                    0x53
#define   LFP20_CODE_C19                    0x54
#define   LFP20_CODE_C20                    0x55

#define   LFP20_CODE_R1                     0x40
#define   LFP20_CODE_R2                     0x41
#define   LFP20_CODE_R3                     0x42
#define   LFP20_CODE_R4                     0x43
#define   LFP20_CODE_R5                     0x44
#define   LFP20_CODE_R6                     0x45
#define   LFP20_CODE_R7                     0x46
#define   LFP20_CODE_R8                     0x47
#define   LFP20_CODE_R9                     0x48
#define   LFP20_CODE_R10                    0x49
#define   LFP20_CODE_R15                    0x50
#define   LFP20_CODE_R16                    0x51
#define   LFP20_CODE_R17                    0x52
#define   LFP20_CODE_R18                    0x53
#define   LFP20_CODE_R19                    0x54
#define   LFP20_CODE_R20                    0x55

// all use ram at &P554_All_Protocol_Common_RAM
#define   LFP20_port_no                  byte0(*(LFP20_PARA_BASE_ADDR+0x0000))  // 0x0000-0x0000

#define   LFP20_mon_state                byte1(*(LFP20_PARA_BASE_ADDR+0x0000))  // 0x0001-0x0001
#define   LFP20_mon_need_reply           byte0(*(LFP20_PARA_BASE_ADDR+0x0001))  // 0x0002-0x0002
#define   LFP20_mon_need_exch_to_host    byte1(*(LFP20_PARA_BASE_ADDR+0x0001))  // 0x0003-0x0003

#define   LFP20_begin_veriiedclk_time    (*(LFP20_PARA_BASE_ADDR+0x0002))  // 0x0004-0x0005

#define   LFP20_begin_wait_ied_time      (*(LFP20_PARA_BASE_ADDR+0x0003))  // 0x0006-0x0007

#define   LFP20_wait_type                byte0(*(LFP20_PARA_BASE_ADDR+0x0004))  // 0x0008-0x0008

#define   LFP20_wait_host_state          byte1(*(LFP20_PARA_BASE_ADDR+0x0004))  // 0x0009-0x0009
#define   LFP20_begin_wait_host_time     (*(LFP20_PARA_BASE_ADDR+0x0005))  // 0x000a-0x000b

#define   LFP20_now_poll_wait_addr       byte0(*(LFP20_PARA_BASE_ADDR+0x0006))  // 0x000f-0x000f
#define   LFP20_YK_execute_flag          byte1(*(LFP20_PARA_BASE_ADDR+0x0006))  // 0x000f-0x000f
#define   LFP20_now_poll_type            byte0(*(LFP20_PARA_BASE_ADDR+0x0007))  // 0x000e-0x000e
#define   LFP20_now_poll_addr_no         byte1(*(LFP20_PARA_BASE_ADDR+0x0007))  // 0x000f-0x000f
#define   LFP20_begin_poll_rly_time      (*(LFP20_PARA_BASE_ADDR+0x0008))  // 0x0010-0x0011
#define   LFP20_host_port_no             byte0(*(LFP20_PARA_BASE_ADDR+0x0009))  // 0x0010-0x0011
#define   txd_ctl                        byte1(*(LFP20_PARA_BASE_ADDR+0x0009))  // 0x0010-0x0011
#define   LFP20_host_protocol_type       (*(LFP20_PARA_BASE_ADDR+0x000a))  // 0x0010-0x0011

#define   LFP20_unit_tx_state            (*(LFP20_PARA_BASE_ADDR+0x000b))  // 2 bytes
#define   LFP20_YK_redo_time             (*(LFP20_PARA_BASE_ADDR+0x000c))  // 2 bytes
#define   LFP20_YK_fx_wait_time          (*(LFP20_PARA_BASE_ADDR+0x000d))  // 2 bytes
#define   LFP20_YK_fx_flag               byte0(*(LFP20_PARA_BASE_ADDR+0x000e))  // 0x0010-0x0011
#define   LFP20_YK_redo_count            byte1(*(LFP20_PARA_BASE_ADDR+0x000e))  // 0x0010-0x0011
#define   LFP20_YK_save                  byte0(*(LFP20_PARA_BASE_ADDR+0x000f))  // 0x0010-0x0011
#define   LFP20_transmit_yk_host_port    byte1(*(LFP20_PARA_BASE_ADDR+0x000f))  // 0x0010-0x0011
#define   LFP20_cn_unit_tx_err           ( (unsigned char *)(LFP20_PARA_BASE_ADDR+0x0010) ) // 16 bytes  [16]




/*---------------------------------------------------------------------------*/
/*                        IMPORT            functions                        */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/

/************************************************/
/* LFP20_transmit_head function               */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void LFP20_transmit_head(void)
{
    far  BYTE *ram_base_addr;

    ram_base_addr=&(port_send[LFP20_PORT_NO][0]);
    
    ram_base_addr[0]=SYN_WORD_LSA[0];
    ram_base_addr[1]=SYN_WORD_LSA[1];
    ram_base_addr[2]=SYN_WORD_LSA[2];
    ram_base_addr[3]=SYN_WORD_LSA[3];
    ram_base_addr[4]=SYN_WORD_LSA[4];
}

/************************************************/
/* LFP20_send         function                */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void LFP20_send(void)  // for the protocol use the uniform msg format may write a uniform send proc!
{
    far  BYTE *ram_base_addr;
         BYTE  ram_axl;
         BYTE  ram_axh;
         WORD  ram_bx;
    
    // LFP20_get_sum
    ram_bx       =  0;
    ram_axh      =  port_send[LFP20_PORT_NO][9];  // LEN
    ram_base_addr=&(port_send[LFP20_PORT_NO][5]);
    ram_base_addr[2]=0x01;
    ram_base_addr[3]=0x01;
    for(ram_axl=0;ram_axl<ram_axh+1;ram_axl++)
    {
        ram_bx=ram_bx+ram_base_addr[0];
        ram_base_addr++;
    }
    ram_base_addr[0]=byte0(ram_bx);
    ram_base_addr[1]=byte1(ram_bx);
    ram_base_addr[2]=SYN_WORD_LSA[5];

    port_send_len[LFP20_PORT_NO]=port_send[LFP20_PORT_NO][9]+9;//+1; 

    LFP20_mon_state=LFP20_MON_TRANSING_TO_IED;
    port_send_begin();
    
}

/************************************************/
/* LFP20_inquire_task_proc     function       */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static BYTE LFP20_inquire_task_proc(void)
{
    far BYTE *ram_base_addr;


    ram_base_addr=&(port_send[LFP20_PORT_NO][0]);
    if(Judge_Time_In_MainLoop(LFP20_begin_poll_rly_time,TIME_LFP20_POLL_TIME_VALUE)==YES)
    {
        LFP20_begin_poll_rly_time=Time_1ms_Counter;
        LFP20_now_poll_wait_addr=LFP20_now_poll_addr_no;
        LFP20_transmit_head();
        switch(txd_ctl)
         {
          case 1:       //C1    
            ram_base_addr[5]=LFP20_now_poll_addr_no;
            ram_base_addr[6]=0x40;
	    ram_base_addr[9]=0x04;
	    txd_ctl=2;
	    break;
          case 2:       //C15   YC
            txd_ctl=3;
            if(unit_info[byte0(port_info[LFP20_PORT_NO].mirror_unit)
        	      +LFP20_now_poll_addr_no-byte0(port_info[LFP20_PORT_NO].scan_panel)].yc_val_num!=0)
             {
              ram_base_addr[5]=LFP20_now_poll_addr_no;
              ram_base_addr[6]=0x50;
	      ram_base_addr[9]=0x04;
	     }
	    else
	     return NO;
	    break;
          case 3:      //C19    YX
            ram_base_addr[5]=LFP20_now_poll_addr_no;
            ram_base_addr[6]=0x54;
	    ram_base_addr[9]=0x04;
	    txd_ctl=1;
            LFP20_now_poll_addr_no++;
            if(LFP20_now_poll_addr_no>(byte1(port_info[LFP20_PORT_NO].scan_panel)))
             {
              if(Portx_Poll_First[LFP20_PORT_NO]==NO)          
                Portx_Poll_First[LFP20_PORT_NO]=UNKNOW;
              else
                if(Portx_Poll_First[LFP20_PORT_NO]==UNKNOW)  
                    Portx_Poll_First[LFP20_PORT_NO]=YES;
              LFP20_now_poll_addr_no=byte0(port_info[LFP20_PORT_NO].scan_panel);
             }
            if(unit_info[byte0(port_info[LFP20_PORT_NO].mirror_unit)
        	      +LFP20_now_poll_addr_no-byte0(port_info[LFP20_PORT_NO].scan_panel)].yx_num==0)
             return NO;	    
	    break;
	  default:
	    break;
	 }
	LFP20_mon_need_reply=YES;
        LFP20_send();
        
        return YES;
    }
    else
    {
        return NO;
    }
        
}


/************************************************/
/* LFP20_ask_ied                   function       */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static BYTE LFP20_ask_ied(void)
{
    far BYTE *ram_base_addr;
        BYTE ram_axl;
        BYTE ram_axh;

    ram_base_addr=&(port_send[LFP20_PORT_NO][0]);

    if(Judge_Time_In_MainLoop(LFP20_begin_poll_rly_time,TIME_LFP20_POLL_TIME_VALUE)==YES)
    {
        LFP20_begin_poll_rly_time=Time_1ms_Counter;
        temp_ptS_B=&(port_deal_buf[LFP20_PORT_NO][0]);
        LFP20_now_poll_wait_addr=temp_ptS_B[5];
        
	LFP20_mon_need_reply=YES;
	ram_axh=temp_ptS_B[9]+9;//报文总长度
          for(ram_axl=0;ram_axl<ram_axh;ram_axl++)
    {
        ram_base_addr[ram_axl]=temp_ptS_B[ram_axl];
        
    }
    

    port_send_len[LFP20_PORT_NO]=ram_axh;//+1; 

    LFP20_mon_state=LFP20_MON_TRANSING_TO_IED;
    port_send_begin();
      
        return YES;
    }
    else
    {
        return NO;
    }
        
}


/************************************************/
/* LFP20_ask_from_host        function            */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static BYTE LFP20_ask_from_host(void)
{
    temp_ptS_B=&(port_deal_buf[LFP20_PORT_NO][0]);
    temp_ptS_B[0]=0xeb;
    temp_ptS_B[1]=0x90;
    temp_ptS_B[2]=0xeb;
    temp_ptS_B[3]=0x90;
    temp_ptS_B[4]=0x02;
    for(temp_loop=0;temp_loop<(WORD)(port_transmit[LFP20_PORT_NO][20]+3);temp_loop++)
    {
    	temp_ptS_B[5+temp_loop]=port_transmit[LFP20_PORT_NO][16+temp_loop];
    }
    temp_ptS_B[5+temp_loop]=0x03;
    LFP20_now_poll_wait_addr=temp_ptS_B[5];
    return YES;    	      	
}

/************************************************/
/* LFP20_ok_or_error          function            */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static BYTE LFP20_ok_or_error(BYTE ok_or_error)
{
    far BYTE *ram_base_addr;


    ram_base_addr=&(port_send[LFP20_PORT_NO][0]);

    if(Judge_Time_In_MainLoop(LFP20_begin_poll_rly_time,TIME_LFP20_POLL_TIME_VALUE)==YES)
    {
        LFP20_begin_poll_rly_time=Time_1ms_Counter;
        LFP20_transmit_head();
        ram_base_addr[5]=LFP20_now_poll_wait_addr;
        if(ok_or_error==GOOD) ram_base_addr[6]=0x06;
        else ram_base_addr[6]=0x15;
	ram_base_addr[9]=0x04;
	LFP20_mon_need_reply=NO;
	LFP20_send();
    return YES;    
    }
    else
    {
    return NO;
    }
        
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* LFP20_transmit_yk_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void LFP20_transmit_yk_cmd(void)  
{
    temp_ptS_B=(far BYTE *)&(port_send[LFP20_PORT_NO]);
    
    temp_ptS_B[0] =0xeb;
    temp_ptS_B[1] =0x90;
    temp_ptS_B[2] =0xeb;
    temp_ptS_B[3] =0x90;
    temp_ptS_B[4] =0x02;
    temp_ptS_B[5] =port_transmit[LFP20_PORT_NO][5]+byte0(port_info[LFP20_PORT_NO].scan_panel);
    temp_ptS_B[7] =0x01;
    temp_ptS_B[8] =0x00;
    temp_ptS_B[9] =0x0a;
    temp_ptS_B[10] =port_transmit[LFP20_PORT_NO][17];
    temp_ptS_B[12] =~port_transmit[LFP20_PORT_NO][17];
    temp_ptS_B[14] =port_transmit[LFP20_PORT_NO][17];
    if(port_transmit[LFP20_PORT_NO][8]==CORE_CODE_YK_CHOOSE)
      {
       temp_ptS_B[6] =0x52;
       if(port_transmit[LFP20_PORT_NO][16]==CORE_CODE2_YK_CLOSE)  //?
      	 {
      	  temp_ptS_B[11] =0xcc;
      	  temp_ptS_B[13] =0x33;
      	  temp_ptS_B[15] =0xcc;
	  LFP20_mon_need_reply=YES;
          LFP20_send();
      	 }
       else
     	 if(port_transmit[LFP20_PORT_NO][16]==CORE_CODE2_YK_TRIP)
      	  {
      	  temp_ptS_B[11] =0x33;
      	  temp_ptS_B[13] =0xcc;
      	  temp_ptS_B[15] =0x33;
	  LFP20_mon_need_reply=YES;
          LFP20_send();
      	  }
       LFP20_YK_save=port_transmit[LFP20_PORT_NO][16];  
      }	
    else
     {
      if(port_transmit[LFP20_PORT_NO][8]==CORE_CODE_YK_EXECUTE)
       {
       	temp_ptS_B[6] =0x53;
        if(LFP20_YK_save==CORE_CODE2_YK_CLOSE)  //?
      	 {
      	  temp_ptS_B[11] =0xcc;
      	  temp_ptS_B[13] =0x33;
      	  temp_ptS_B[15] =0xcc;
	  LFP20_mon_need_reply=YES;
          LFP20_send();
      	 }
        else
     	 if(LFP20_YK_save==CORE_CODE2_YK_TRIP)
      	  {
      	  temp_ptS_B[11] =0x33;
      	  temp_ptS_B[13] =0xcc;
      	  temp_ptS_B[15] =0x33;
	  LFP20_mon_need_reply=YES;
          LFP20_send();
      	  }       	
       else
        return;  
       LFP20_YK_save=CORE_CODE2_YK_ERROR;  
       }
      else
         return;
     }
}

/************************************************/
/* LFP20_poll_reply_proc      function            */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static BYTE LFP20_poll_reply_proc(void)//接收的0x40,0x46,0x47保护报文转发
{
	BYTE ram_axh;
	BYTE ram_axl;
// save event
   if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
   temp_ptD_B = &port_deal_buf[LFP20_PORT_NO][0];
   if ((temp_ptD_B[9]+3)>24)
   {// event save_0	
    temp_ptS_B=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
    temp_ptS_B[0]=PROTOCOL_LFP20%0x100;
    temp_ptS_B[1]=temp_ptD_B[6];
    temp_ptS_B[2]=0x00;             //两帧中的第0帧
    temp_ptS_B[3]=0x1a;             //本帧中有效报文长度
    temp_ptS_B[4]=temp_ptS_B[0]+temp_ptS_B[1]+temp_ptS_B[2]+temp_ptS_B[3];
    temp_ptS_B[5]=PROTOCOL_LFP20/0x100;
    temp_ptS_B[6]=0x02;             //表示这一条故障报告有两帧
    temp_ptS_B[7]=0x00;
    for(ram_axh=0;ram_axh<24;ram_axh++)
      {
    	temp_ptS_B[8+ram_axh]=temp_ptD_B[5+ram_axh];
      }   
    BH_Report_Bank_Sav_Ptr++;
    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
   
   
   //event save_1
    temp_ptS_B=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
    ram_axl=temp_ptD_B[9]+3;
    temp_ptS_B[0]=PROTOCOL_LFP20%0x100;
    temp_ptS_B[1]=temp_ptD_B[6];
    temp_ptS_B[2]=0x01;                   //两帧中的第1帧
    temp_ptS_B[3]=ram_axl-24;             //第1帧有效报文长度
    temp_ptS_B[4]=temp_ptS_B[0]+temp_ptS_B[1]+temp_ptS_B[2]+temp_ptS_B[3];
    temp_ptS_B[5]=PROTOCOL_LFP20/0x100;
    for(ram_axh=0;ram_axh<temp_ptS_B[3];ram_axh++)
      {
         temp_ptS_B[6+ram_axh]=temp_ptD_B[ram_axh+29];	
      }
    BH_Report_Bank_Sav_Ptr++;
    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
    }


    if((temp_ptD_B[9]+3)<25)
    {
    // event save_0	
    temp_ptS_B=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);   //-保护的信息被保存在这个里面
    temp_ptS_B[0]=PROTOCOL_LFP20%0x100;
    temp_ptS_B[1]=temp_ptD_B[6];
    temp_ptS_B[2]=0x00;
    temp_ptS_B[3]=temp_ptD_B[9]+5;             //等看规约
    temp_ptS_B[4]=temp_ptS_B[0]+temp_ptS_B[1]+temp_ptS_B[2]+temp_ptS_B[3];
    temp_ptS_B[5]=PROTOCOL_LFP20/0x100;
    temp_ptS_B[6]=0x01;              //wait
    temp_ptS_B[7]=0x00;
    for(ram_axh=0;ram_axh<(temp_ptD_B[9]+3);ram_axh++)
      {
    	temp_ptS_B[8+ram_axh]=temp_ptD_B[ram_axh+5];
      }
   
    BH_Report_Bank_Sav_Ptr++;
    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;	
    }	
    return YES;
}


/************************************************/
/* LFP20_poll_reply_to_host   function            */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static BYTE LFP20_poll_reply_to_host(void)
{
    WORD ram_bx;
    BYTE ram_axh;
    BYTE ram_axl;
    temp_ptS_B=&port_deal_buf[LFP20_PORT_NO][0];
    port_transmit_flag[LFP20_host_port_no]=0xaa;	
    port_transmit[LFP20_host_port_no][1]=LFP20_PORT_NO;
    port_transmit[LFP20_host_port_no][2]=PROTOCOL_LFP20%0x100;
    port_transmit[LFP20_host_port_no][3]=PROTOCOL_LFP20/0x100;
    port_transmit[LFP20_host_port_no][8]=temp_ptS_B[6];
    ram_axl=temp_ptS_B[9];
    ram_bx=ram_axl+3;
    port_transmit[LFP20_host_port_no][9]=byte0(ram_bx);
    port_transmit[LFP20_host_port_no][10]=byte1(ram_bx);
    for(ram_axh=0;ram_axh<byte0(ram_bx);ram_axh++)
    {	
        port_transmit[LFP20_host_port_no][16+ram_axh]=temp_ptS_B[5+ram_axh];	
    }	
	
    return YES;	
}


/************************************************/
/* LFP20_scheduler      function              */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void LFP20_scheduler(void)
{
    BYTE  ram_axl;
    BYTE  ram_axh;
    WORD  ram_bx;
    WORD  ram_cx;
    WORD  ram_dx;
    WORD  ram_ex;
    
    if(LFP20_mon_state==LFP20_MON_IDLE)
    {
    	 if(port_transmit_flag[LFP20_PORT_NO]==0xaa)  // EXCHANGE_STA，处理后台命令
        {
            port_transmit_flag[LFP20_PORT_NO]=0x55;
            LFP20_host_port_no=port_transmit[LFP20_PORT_NO][1];
	    LFP20_host_protocol_type=port_transmit[LFP20_PORT_NO][2]+port_transmit[LFP20_PORT_NO][3]*256;	    	  
    	   switch(port_transmit[LFP20_PORT_NO][8])    //特征码
    	   {
    		case LFP20_CODE_C2://0x41,取保护定值
    		case LFP20_CODE_C3://0x42,申请故障报告
    		case LFP20_CODE_C4://0x43,申请模拟量
    		case LFP20_CODE_C5://0x44,申请开关量
    		case LFP20_CODE_C6://0x45,申请自检报告
    		case LFP20_CODE_C7://0x46,申请信号复归
    	
    	            LFP20_mon_state=LFP20_MON_TRANSING_FROM_HOST;
    	        
    	      	    LFP20_ask_from_host();
    	            LFP20_ask_ied();
    		    break;
    	        case CORE_CODE_YK_CHOOSE:
    	            LFP20_transmit_yk_host_port=port_transmit[LFP20_PORT_NO][1];
    	            LFP20_transmit_yk_cmd();
    	            LFP20_mon_state=LFP20_MON_TRANSING_TO_IED;
                    LFP20_YK_fx_flag=YES;
                    LFP20_YK_fx_wait_time=Time_2048ms_Counter;
                    LFP20_YK_redo_time=Time_1ms_Counter;
                    LFP20_YK_execute_flag=NO;
    	            break;
    	        case CORE_CODE_YK_EXECUTE:
    	            LFP20_transmit_yk_cmd();
    	            LFP20_mon_state=LFP20_MON_TRANSING_TO_IED;
    	            LFP20_YK_fx_flag=YES;
    	            LFP20_YK_fx_wait_time=Time_2048ms_Counter;
                    LFP20_YK_redo_time=Time_1ms_Counter;
                    LFP20_YK_execute_flag=YES;
    	            break;
    	        case CORE_CODE_YK_CANCEL:
    	            
    	            break;
    	        
    	        
    	        
    	        
	    	default:
	    	LFP20_mon_state=LFP20_MON_IDLE;
	    		break;
    	
    	   }
    	}
        else  //正常发0x40查询
        {
           if(LFP20_inquire_task_proc()!=YES)
           {
            // do other task
           }
        }
    }  // not LFP20_MON_IDLE
    else
    {
    	if(LFP20_mon_state==LFP20_MON_TRANSING_FROM_HOST)
    	  LFP20_ask_ied();
        if(LFP20_mon_state==LFP20_MON_WAIT_IED_REPLY)
        {
            ram_dx=port_recv_pt[LFP20_PORT_NO];//报文尾
            ram_ex=port_recv_dl[LFP20_PORT_NO];//报文头
            ram_cx=ram_dx-ram_ex;            //长度

            if(ram_cx<12) //最短长度
            {
                goto judge_rece_time_out;
            }
       
re_syn: 
          while((ram_ex<ram_dx)&&(port_recv[LFP20_PORT_NO][ram_ex]!=0xeb))
            {
                ram_ex++;
            }
             port_recv_dl[LFP20_PORT_NO]=ram_ex;

        	ram_cx=ram_dx-ram_ex;
        	if(ram_cx<12)         
        	    goto judge_rece_time_out;    
                
             temp_ptS_B = &port_recv[LFP20_PORT_NO][ram_ex];
                
             if(temp_ptS_B[1]!=0x90||temp_ptS_B[2]!=0xeb||temp_ptS_B[3]!=0x90)
                {   
                ram_ex++;
                goto re_syn;
                }
         
             if(temp_ptS_B[4]!=0x02)//||temp_ptS_B[5]!=LFP20_now_poll_wait_addr
                {
                ram_ex++;
                goto re_syn;
                }	   
              
             if((BYTE)ram_cx<temp_ptS_B[9]+9)
                    goto judge_rece_time_out;
              
             ram_bx=0;
             for(temp_loop=0;temp_loop<(WORD)(temp_ptS_B[9]+1);temp_loop++)//校验和
                {
                   ram_bx+=temp_ptS_B[temp_loop+5];
                }
                
             if(temp_ptS_B[temp_ptS_B[9]+8]!=0x03||temp_ptS_B[temp_ptS_B[9]+6]!=byte0(ram_bx)
                ||temp_ptS_B[temp_ptS_B[9]+7]!=byte1(ram_bx))
                {
                
                ram_ex++;
                goto re_syn;
                }
                
              temp_ptD_B = &port_deal_buf[LFP20_PORT_NO][0];
              for(ram_bx=0;ram_bx<(WORD)(temp_ptS_B[9]+9);ram_bx++)
                {
                 temp_ptD_B[ram_bx]=temp_ptS_B[ram_bx];
                }
                 port_recv_dl[LFP20_PORT_NO]=ram_ex+ram_bx;            
//报文接收正确。            
            
            ram_axh=byte0(port_info[LFP20_PORT_NO].mirror_unit);
            ram_axl=LFP20_now_poll_wait_addr-byte0(port_info[LFP20_PORT_NO].scan_panel);
            YX_State[IED_TX_STATE_START_WORD+(ram_axh+ram_axl)/16] &= 0xffff-(1<<((ram_axh+ram_axl)%16));
            
            if(Portx_Poll_First[LFP20_PORT_NO]!=YES)
            {
                if(ram_axl<16) 
                {
                    LFP20_cn_unit_tx_err[ram_axl]=0;
                    LFP20_unit_tx_state=LFP20_unit_tx_state & (0xffff-(1<<ram_axl));
                }   
            }
            else
            {
                if(ram_axl<16)
                {
                    if((LFP20_unit_tx_state & (1<<ram_axl))!=0)  // already err
                    {
                        if(LFP20_cn_unit_tx_err[ram_axl]>1)
                            LFP20_cn_unit_tx_err[ram_axl]=LFP20_cn_unit_tx_err[ram_axl]-2;
                        else
                            LFP20_cn_unit_tx_err[ram_axl]=0;
                        
                        if(LFP20_cn_unit_tx_err[ram_axl]==0)
                            LFP20_unit_tx_state=LFP20_unit_tx_state & (0xffff-(1<<ram_axl));
                    }
                    else
                    {
                        LFP20_cn_unit_tx_err[ram_axl]=0;
                    }
                }    
            }
            
            //  bao wen receive all correct, msg proc now
            switch(temp_ptS_B[6])
            {
            case LFP20_CODE_ACK:
            case LFP20_CODE_NAK:
                  LFP20_mon_state=LFP20_MON_IDLE;
                  break;
            
            case LFP20_CODE_R1://0x40,跳合事件上送
            case LFP20_CODE_R7://0x46,故障报告上送
            case LFP20_CODE_R8://0x47,自检报告上送
                  if(LFP20_poll_reply_proc()!=YES)
            	{
        	}
       		 //BH to Soe  
          	temp_ptD_B = &port_deal_buf[LFP20_PORT_NO][0];
        	if (temp_ptD_B[6]==LFP20_CODE_R1)
        		{
        		byte0(ram_dx)=temp_ptD_B[18];//指向事件字节，一个字节
                        if(ram_dx!=0)
                            {
                            	for(ram_axh=0;ram_axh<8;ram_axh++)
                            	{
                            	    if((byte0(ram_dx) & (1<<ram_axh))!=0)
                            	    {
                            		break;
                            	    }
                            	}
                            	
                            	ram_bx=0;
		                while(ram_bx<512)
		                {
        	        	  if(BH_Event_To_SOE[ram_bx].YX_State_No>0x0fff) break;
            	    
			          if((BH_Event_To_SOE[ram_bx].DeviceType==0x02)
                                      &&(BH_Event_To_SOE[ram_bx].RLYAddr   ==temp_ptD_B[5])
				       &&(BH_Event_To_SOE[ram_bx].ActionType==ram_axh))
                		    {
				      BH_To_YX_Delay_Begin_Time[ram_bx]=Time_2048ms_Counter;
                    
                		      ram_bx =BH_Event_To_SOE[ram_bx].YX_State_No;
			              YX_State[ram_bx/16]=YX_State[ram_bx/16] | (1<<(ram_bx % 16));
    	                        	        				
			              temp_ptD_B[0x100]= temp_ptD_B[16];  //msL
                		      temp_ptD_B[0x101]= temp_ptD_B[17];  //msH
				      temp_ptD_B[0x102]= temp_ptD_B[15];  // second
                		      temp_ptD_B[0x103]= temp_ptD_B[14];  // minute
				      temp_ptD_B[0x104]= temp_ptD_B[13];  // hour
                	              temp_ptD_B[0x105]= temp_ptD_B[12];  // date
				      temp_ptD_B[0x106]= byte0(ram_bx);
	                              temp_ptD_B[0x107]=(byte1(ram_bx) & 0x0f)+0x80;
	                              Core_Src_Unit=0x00; 
			              Core_Src_Pt_B = &temp_ptD_B[0x100];
        	        	      core_insert_SOECOS();
                    
			              break;
                	            }
                 
				   ram_bx++;
                                }
                            }	
        			
        	        }
        	        
        	if(temp_ptD_B[6]==LFP20_CODE_R7)
        		{
        		byte0(ram_dx)=temp_ptD_B[18];
                        byte1(ram_dx)=temp_ptD_B[19];//跳闸信息1，取前两个字节
                        if(ram_dx!=0)
                            {
                            	for(ram_axh=0;ram_axh<16;ram_axh++)
                            	{
                            	    if((ram_dx & (1<<ram_axh))!=0)
                                     {
                            	        break;
                            	     }
                            	}
                            	
                            	ram_bx=0;
			        while(ram_bx<512)
		                {
        	        	   if(BH_Event_To_SOE[ram_bx].YX_State_No>0x0fff) break;
            	    
			           if((BH_Event_To_SOE[ram_bx].DeviceType==0x02)
                		       &&(BH_Event_To_SOE[ram_bx].RLYAddr==temp_ptD_B[5])
				        &&(BH_Event_To_SOE[ram_bx].ActionType==(ram_axh+0x10)))
                		       {
				           BH_To_YX_Delay_Begin_Time[ram_bx]=Time_2048ms_Counter;
                    
                		           ram_bx =BH_Event_To_SOE[ram_bx].YX_State_No;
					   YX_State[ram_bx/16]=YX_State[ram_bx/16] | (1<<(ram_bx % 16));
    	                
        	        	           ram_cx =(temp_ptD_B[17]+temp_ptD_B[23])*256+temp_ptD_B[16]+temp_ptD_B[22];//总时间ms
                			   ram_ex=ram_cx%1000;
                		           temp_ptD_B[0x100]= byte0(ram_ex);
                	                   temp_ptD_B[0x101]= byte1(ram_ex);   
                			     if(ram_cx>999)   ram_axl=temp_ptD_B[15]+1;
                			         else         ram_axl=temp_ptD_B[15];
                			             
                		           temp_ptD_B[0x102]= ram_axl%60;  // second
                			     if(ram_axl>59)   ram_axl=temp_ptD_B[14]+1;
                			         else         ram_axl=temp_ptD_B[14];
                			         
                		           temp_ptD_B[0x103]= ram_axl%60;  // minute
                			     if(ram_axl>59)   ram_axl=temp_ptD_B[13]+1;    
                				 else         ram_axl=temp_ptD_B[13];
                				 
				           temp_ptD_B[0x104]= ram_axl%60;  // hour
                			   temp_ptD_B[0x105]= temp_ptD_B[12];    // date
                			   temp_ptD_B[0x106]= byte0(ram_bx);
	                		   temp_ptD_B[0x107]=(byte1(ram_bx) & 0x0f)+0x80;
	                		   Core_Src_Unit=0x00; 
					   Core_Src_Pt_B = &temp_ptD_B[0x100];
        	        		   core_insert_SOECOS();
                    
					    break;
                		       }
                 
				       ram_bx++;
				}
                            }

				            // BH to SOE
                            byte0(ram_dx)=temp_ptD_B[24];
                            byte1(ram_dx)=temp_ptD_B[25];//跳闸信息2，两个字节
                            if(ram_dx!=0)
                            {
                            	for(ram_axh=0;ram_axh<16;ram_axh++)//有多个信号一起发？
                            	{
                            	    if((ram_dx & (1<<ram_axh))!=0)
                            	     {
                            		   break;
                            	     }
                            	}
                            	
                            	ram_bx=0;
			        while(ram_bx<512)
			        {
        	                 if(BH_Event_To_SOE[ram_bx].YX_State_No>0x0fff) break;
            	    
			         if((BH_Event_To_SOE[ram_bx].DeviceType==0x02)
                		      &&(BH_Event_To_SOE[ram_bx].RLYAddr   ==temp_ptD_B[5])
				       &&(BH_Event_To_SOE[ram_bx].ActionType==(ram_axh+0x10)))
                		   {
				      BH_To_YX_Delay_Begin_Time[ram_bx]=Time_2048ms_Counter;
                    
                		      ram_bx =BH_Event_To_SOE[ram_bx].YX_State_No;
				      YX_State[ram_bx/16]=YX_State[ram_bx/16] | (1<<(ram_bx % 16));
    	                
        	        	      ram_cx =(temp_ptD_B[17]+temp_ptD_B[29])*256+temp_ptD_B[16]+temp_ptD_B[28];
                		      ram_ex=ram_cx%1000;
                		      temp_ptD_B[0x100]= byte0(ram_ex);
                		      temp_ptD_B[0x101]= byte1(ram_ex);   
                			if(ram_cx>999)   ram_axl=temp_ptD_B[15]+1;
                			        else     ram_axl=temp_ptD_B[15];
                			        
                		      temp_ptD_B[0x102]= ram_axl%60;  // second
                			if(ram_axl>59)   ram_axl=temp_ptD_B[14]+1;
                				else     ram_axl=temp_ptD_B[14];
                				
                		      temp_ptD_B[0x103]= ram_axl%60;  // minute
                			if(ram_axl>59)   ram_axl=temp_ptD_B[13]+1;   
                				else     ram_axl=temp_ptD_B[13];
                				 
				      temp_ptD_B[0x104]= ram_axl%60;  // hour
                		      temp_ptD_B[0x105]= temp_ptD_B[12];    // date
                		      temp_ptD_B[0x106]= byte0(ram_bx);
	                	      temp_ptD_B[0x107]=(byte1(ram_bx) & 0x0f)+0x80;
	                	      Core_Src_Unit=0x00; 
			              Core_Src_Pt_B = &temp_ptD_B[0x100];
        	        	      core_insert_SOECOS();
                    
				      break;
                		   }
                 
				   ram_bx++;
				}
                            }

				            // BH to SOE
                            byte0(ram_dx)=temp_ptD_B[30];
                            byte1(ram_dx)=temp_ptD_B[31];//跳闸信息3，两个字节
                            if(ram_dx!=0)
                            {
                            	for(ram_axh=0;ram_axh<16;ram_axh++)
                            	{
                            		if((ram_dx & (1<<ram_axh))!=0)
                            		{
                            		   break;
                            		}
                            	}
                            	
                            	ram_bx=0;
			        while(ram_bx<512)
			        {
        	        	 if(BH_Event_To_SOE[ram_bx].YX_State_No>0x0fff) break;
            	    
			         if((BH_Event_To_SOE[ram_bx].DeviceType==0x02)
                		     &&(BH_Event_To_SOE[ram_bx].RLYAddr   ==temp_ptD_B[5])
				      &&(BH_Event_To_SOE[ram_bx].ActionType==(ram_axh+0x10)))
                		   {
				     BH_To_YX_Delay_Begin_Time[ram_bx]=Time_2048ms_Counter;
                    
                		     ram_bx =BH_Event_To_SOE[ram_bx].YX_State_No;
			             YX_State[ram_bx/16]=YX_State[ram_bx/16] | (1<<(ram_bx % 16));
    	                
        	        	     ram_cx =(temp_ptD_B[17]+temp_ptD_B[35])*256+temp_ptD_B[16]+temp_ptD_B[34];
                		     ram_ex=ram_cx%1000;
                		     temp_ptD_B[0x100]= byte0(ram_ex);
                		     temp_ptD_B[0x101]= byte1(ram_ex);   
                		        if(ram_cx>999)   ram_axl=temp_ptD_B[15]+1;
                				else     ram_axl=temp_ptD_B[15];
                				
                		     temp_ptD_B[0x102]= ram_axl%60;  // second
                			if(ram_axl>59)   ram_axl=temp_ptD_B[14]+1;
                				else     ram_axl=temp_ptD_B[14];
                				
                		     temp_ptD_B[0x103]= ram_axl%60;  // minute
                			if(ram_axl>59)   ram_axl=temp_ptD_B[13]+1;    
                				else     ram_axl=temp_ptD_B[13];
                				
				     temp_ptD_B[0x104]= ram_axl%60;  // hour
                		     temp_ptD_B[0x105]= temp_ptD_B[12];    // date
                		     temp_ptD_B[0x106]= byte0(ram_bx);
	                	     temp_ptD_B[0x107]=(byte1(ram_bx) & 0x0f)+0x80;
	                	     Core_Src_Unit=0x00; 
			             Core_Src_Pt_B = &temp_ptD_B[0x100];
        	        	     core_insert_SOECOS();
                    
			             break;
                		   }
                 
				   ram_bx++;
				}
                            }
                            
                            if((temp_ptD_B[36]%2)==1)
                            {
                              ram_bx=0;
		              while(ram_bx<512)
			        {
        	        	 if(BH_Event_To_SOE[ram_bx].YX_State_No>0x0fff) break;
            	    
			         if((BH_Event_To_SOE[ram_bx].DeviceType==0x02)
                		     &&(BH_Event_To_SOE[ram_bx].RLYAddr   ==temp_ptD_B[5])
				      &&(BH_Event_To_SOE[ram_bx].ActionType==0x20))//重合闸代码被修改
                		   {
				     BH_To_YX_Delay_Begin_Time[ram_bx]=Time_2048ms_Counter;
                    
                		     ram_bx =BH_Event_To_SOE[ram_bx].YX_State_No;
				     YX_State[ram_bx/16]=YX_State[ram_bx/16] | (1<<(ram_bx % 16));
    	                
        	        	     ram_cx =(temp_ptD_B[17]+temp_ptD_B[41])*256+temp_ptD_B[16]+temp_ptD_B[40];
                		     ram_ex=ram_cx%1000;
                		     temp_ptD_B[0x100]= byte0(ram_ex);
                		     temp_ptD_B[0x101]= byte1(ram_ex);   
                			if(ram_cx>999)   ram_axl=temp_ptD_B[15]+1;
                				else     ram_axl=temp_ptD_B[15];
                				
                		     temp_ptD_B[0x102]= ram_axl%60;  // second
                			if(ram_axl>59)   ram_axl=temp_ptD_B[14]+1;
                				else     ram_axl=temp_ptD_B[14];
                				
                		     temp_ptD_B[0x103]= ram_axl%60;  // minute
                			if(ram_axl>59)   ram_axl=temp_ptD_B[13]+1;    
                				else     ram_axl=temp_ptD_B[13];
                				
				     temp_ptD_B[0x104]= ram_axl%60;  // hour
                		     temp_ptD_B[0x105]= temp_ptD_B[12];    // date
                		     temp_ptD_B[0x106]= byte0(ram_bx);
	                	     temp_ptD_B[0x107]=(byte1(ram_bx) & 0x0f)+0x80;
	                	     Core_Src_Unit=0x00; 
				     Core_Src_Pt_B = &temp_ptD_B[0x100];
        	        	     core_insert_SOECOS();
                    
			             break;
                		   }
                 
				   ram_bx++;
			        }
                            }                                                               			       			       			        			      			
        		}
                	LFP20_mon_state=LFP20_MON_SEND_OK;
                	LFP20_ok_or_error(GOOD);
           
            	  break;
            case LFP20_CODE_R2:
            case LFP20_CODE_R3:
            case LFP20_CODE_R4:
            case LFP20_CODE_R5:
            case LFP20_CODE_R6:	  
                     LFP20_poll_reply_to_host();
                     LFP20_mon_state=LFP20_MON_SEND_OK;
                     LFP20_ok_or_error(GOOD);
            	  break;
            case LFP20_CODE_R15:    //yc
              Core_Src_Unit=byte0(port_info[LFP20_PORT_NO].mirror_unit)
        	      +temp_ptS_B[5]-byte0(port_info[LFP20_PORT_NO].scan_panel);
              Core_Det_Pt=(WORD *)&(YC_State[unit_info[Core_Src_Unit].yc_val_start]);
              ram_bx=(temp_ptS_B[9]-4)/2;  
              if(ram_bx<unit_info[Core_Src_Unit].yc_val_num)
               {
                for(temp_loop=0;temp_loop<ram_bx;temp_loop++)
                 {
                  ram_cx=temp_ptS_B[2*temp_loop+11]*0x100+temp_ptS_B[2*temp_loop+10];
                  Core_Det_Pt[temp_loop]=ram_cx&0xfff;
                 }
               }
              else
               {
                for(temp_loop=0;temp_loop<unit_info[Core_Src_Unit].yc_val_num;temp_loop++)
                 {
                  ram_cx=temp_ptS_B[2*temp_loop+11]*0x100 +temp_ptS_B[2*temp_loop+10];
                  Core_Det_Pt[temp_loop]=ram_cx&0xfff;
                 }
               }
              LFP20_mon_state=LFP20_MON_SEND_OK;
              LFP20_ok_or_error(GOOD);
              break;
            case LFP20_CODE_R19:    //yx
              Core_Src_Unit=byte0(port_info[LFP20_PORT_NO].mirror_unit)
        	      +temp_ptS_B[5]-byte0(port_info[LFP20_PORT_NO].scan_panel);
              Core_Src_Len=temp_ptS_B[9]-4;
              Core_Src_Pt_B=(BYTE *)&port_deal_buf[LFP20_PORT_NO][0];
              for(temp_loop=0;temp_loop<Core_Src_Len;temp_loop++)
               {
               	Core_Src_Pt_B[temp_loop]=temp_ptS_B[temp_loop+10];
               }
             core_update_YX(); 
             LFP20_mon_state=LFP20_MON_SEND_OK;
             LFP20_ok_or_error(GOOD);
             break;
            case LFP20_CODE_R17:    //yk select verify
             LFP20_YK_redo_count=0;
             LFP20_YK_fx_flag=NO;
             LFP20_YK_execute_flag=NO; 
             Core_Src_Unit=byte0(port_info[LFP20_PORT_NO].mirror_unit)
        	      +temp_ptS_B[5]-byte0(port_info[LFP20_PORT_NO].scan_panel);
             if(port_transmit[LFP20_PORT_NO][8]==CORE_CODE_YK_CHOOSE)
              {
               exchange_target_port=LFP20_transmit_yk_host_port;
               exchange_buf[1] = LFP20_PORT_NO;
               exchange_buf[2] = PROTOCOL_LFP20%0x100;
               exchange_buf[3] = PROTOCOL_LFP20/0x100;
               exchange_buf[5] = temp_ptS_B[5];
               exchange_buf[8] = CORE_CODE_YK_VERIFY;
               exchange_buf[9] = 0x02;
               exchange_buf[10]= 0x00;
               exchange_buf[16]=port_transmit[LFP20_PORT_NO][16]; 
               exchange_buf[17]=temp_ptS_B[10]+unit_info[Core_Src_Unit].yk_start;
               Ex_Produce_Transmit_Info();
              }
             LFP20_mon_state=LFP20_MON_SEND_OK;
             LFP20_ok_or_error(GOOD);
             break;
            case LFP20_CODE_R18:    //yk select verify
             LFP20_YK_redo_count=0;
             LFP20_YK_fx_flag=NO;
             LFP20_YK_execute_flag=NO; 
             LFP20_mon_state=LFP20_MON_SEND_OK;
             LFP20_ok_or_error(GOOD);
             break;
            default:
                     LFP20_mon_state=LFP20_MON_IDLE;
                  break;
            }

         if(LFP20_YK_fx_flag==YES)
          {
           if((Judge_Time_In_MainLoop(LFP20_YK_redo_time,2000)==YES)&&(LFP20_YK_redo_count<3))
            {
             port_transmit_flag[LFP20_PORT_NO]=YES;
             LFP20_YK_redo_time=Time_1ms_Counter;
             LFP20_YK_redo_count++;
            }
           if(Judge_LongTime_In_MainLoop(LFP20_YK_fx_wait_time,10)==YES)
            {
         	LFP20_YK_fx_flag=NO;
       	     if(LFP20_YK_execute_flag==NO)
       	      {
               exchange_target_port=LFP20_transmit_yk_host_port;
               exchange_buf[1] = LFP20_PORT_NO;
               exchange_buf[2] = PROTOCOL_LFP20%0x100;
               exchange_buf[3] = PROTOCOL_LFP20/0x100;
               exchange_buf[5] = 0;
               exchange_buf[8] = CORE_CODE_YK_VERIFY;
               exchange_buf[9] = 0x02;
               exchange_buf[10]= 0x00;
               exchange_buf[16]=CORE_CODE2_YK_ERROR; 
               exchange_buf[17]=port_transmit[LFP20_PORT_NO][17];
               Ex_Produce_Transmit_Info();
              }
             LFP20_YK_redo_count=0;        	
            }
          }           
        return;
judge_rece_time_out:
            if(Judge_Time_In_MainLoop(LFP20_begin_wait_ied_time,800)==YES)
            {
            ram_axh=byte0(port_info[LFP20_PORT_NO].mirror_unit);
            ram_axl=LFP20_now_poll_wait_addr-byte0(port_info[LFP20_PORT_NO].scan_panel);
                if(Portx_Poll_First[LFP20_PORT_NO]!=YES)
                {
                    if(ram_axl<16) 
                    {
                        LFP20_cn_unit_tx_err[ram_axl]=4;
                        LFP20_unit_tx_state=LFP20_unit_tx_state | (1<<ram_axl);
                    }   
                }
                else
                {
                    if(ram_axl<16)
                    {
                        if((LFP20_unit_tx_state & (1<<ram_axl))==0)  // already GOOD
                        {
                            LFP20_cn_unit_tx_err[ram_axl]++;
                            if(LFP20_cn_unit_tx_err[ram_axl]>0x03)
                                LFP20_unit_tx_state=LFP20_unit_tx_state | (1<<ram_axl);
                        }
                        else
                        {
                            LFP20_cn_unit_tx_err[ram_axl]=4;
                        }
                    }
                }    
		if(LFP20_cn_unit_tx_err[ram_axl]==0x04)
		{
		 YX_State[IED_TX_STATE_START_WORD+(ram_axh+ram_axl)/16] |= (1<<((ram_axh+ram_axl)%16));	
		}	
                LFP20_mon_state=LFP20_MON_IDLE;
            }
        }    
        else  // no proc 'LFP20_MON_WAIT_HOST_REPLY' state,when wait_host,CMU need poll units as normal.
        {
		if(LFP20_mon_state==LFP20_MON_SEND_OK) LFP20_ok_or_error(GOOD);
		if(LFP20_mon_state==LFP20_MON_SEND_ERROR) LFP20_ok_or_error(ERR);
        }

    }
    
}



/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/

/************************************************/
/* LFP20_Init     function                    */
/************************************************/
/*===========================================================================*/
void LFP20_Init()
{
    BYTE the_ram_axl;
    
    LFP20_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];

    LFP20_mon_state=LFP20_MON_IDLE;

    LFP20_begin_poll_rly_time=Time_1ms_Counter+10000;  
    // +anyvalue let next time at judge mainlooptime clk: Time_1ms_Counter<LFP20_begin_poll_rly_time
    LFP20_now_poll_addr_no=byte0(port_info[LFP20_PORT_NO].scan_panel);  // start addr
    LFP20_now_poll_type=0;
    
    LFP20_unit_tx_state=0;  // all 16 bits
    txd_ctl=1;
    LFP20_YK_redo_count=0;
    LFP20_YK_fx_flag=NO;
    LFP20_YK_execute_flag=NO;
    for(the_ram_axl=0;the_ram_axl<16;the_ram_axl++)
        LFP20_cn_unit_tx_err[the_ram_axl]=0;
}

/************************************************/
/* LFP20_Main     function                    */
/************************************************/
/*===========================================================================*/
void LFP20_Main()
{
    LFP20_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];

    LFP20_scheduler();
    
    if(LFP20_mon_state==LFP20_MON_TRANSING_TO_IED)
    {
        if(port_send_pt[LFP20_PORT_NO]>port_send_len[LFP20_PORT_NO])
        {
            if(LFP20_mon_need_reply==YES)
            {
                LFP20_mon_state=LFP20_MON_WAIT_IED_REPLY;
                LFP20_begin_wait_ied_time=Time_1ms_Counter;
                disable();
                port_recv_dl[LFP20_PORT_NO]=0;
                port_recv_pt[LFP20_PORT_NO]=0;
                enable();
            }    
            else
                LFP20_mon_state=LFP20_MON_IDLE;    
        }    
    }
    
    if((Portx_Poll_First[LFP20_PORT_NO]!=UNKNOW)&&(Portx_Poll_First[LFP20_PORT_NO]!=NO))
        Portx_Poll_First[LFP20_PORT_NO]=YES;
        
    
}


/************************************************/
/* LFP20_Monitor     function                 */
/************************************************/
/*===========================================================================*/
void LFP20_Monitor()
{
    LFP20_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];

}

