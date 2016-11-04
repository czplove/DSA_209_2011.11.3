/*****************************************************************************/
/*       FileName  :   MOD_YDYB.C                                              */
/*       Content   :   DSA-208 MOD_YDYB Module                                 */
/*       Date      :   Wed  07-11-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/
//-现在还不知道这是一个什么样的东西,但是可能就是一个误导,需要警惕
//-由于这个仅仅是一个查询遥测,可能就仅仅是与电度表通讯了
//-远动仪表,YDYB
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
register  far  WORD                         *MOD_YDYB_PARA_BASE_ADDR;

#define   MOD_YDYB_PORT_NO                     port_no

#define   MOD_YDYB_YC_FRAME                   0x03
#define   MOD_YDYB_YX_FRAME                   0x02
#define   MOD_YDYB_TS_FRAME                   0x04 
#define   MOD_YDYB_YK_FRAME                   0x08

#define   TIME_MOD_YDYB_VERIFY_TIME_VALUE     150    // 2048ms*150=307s
#define   MOD_YDYB_WAIT_TIME_VALUE            2000    //等待接收时间 1ms*500
#define   MOD_YDYB_YK_FX_WAIT_TIME_VALUE      10    //遥控等待返校时间 2048ms*15

#define   MOD_YDYB_begin_veriiedclk_time           (*(MOD_YDYB_PARA_BASE_ADDR+0x0000))
#define   MOD_YDYB_rxd_wait_time                   (*(MOD_YDYB_PARA_BASE_ADDR+0x0004))
#define   MOD_YDYB_transmit_disable_begin_time     (*(MOD_YDYB_PARA_BASE_ADDR+0x0005))
#define   MOD_YDYB_rxd_head_flag              byte0(*(MOD_YDYB_PARA_BASE_ADDR+0x0006))
#define   MOD_YDYB_transmit_control           byte1(*(MOD_YDYB_PARA_BASE_ADDR+0x0006)) 
#define   MOD_YDYB_comm_err_counter                 (MOD_YDYB_PARA_BASE_ADDR+0x0028) //0x0008-0x0027 
//receive
#define   MOD_YDYB_transmit_yk_host_port      byte0(*(MOD_YDYB_PARA_BASE_ADDR+0x0048))
#define   MOD_YDYB_transmit_task_busy         byte1(*(MOD_YDYB_PARA_BASE_ADDR+0x0048))
#define   MOD_YDYB_now_poll_addr              byte0(*(MOD_YDYB_PARA_BASE_ADDR+0x0049))
#define   MOD_YDYB_transmit_flag              byte1(*(MOD_YDYB_PARA_BASE_ADDR+0x0049))
#define   MOD_YDYB_rec_OK                     byte0(*(MOD_YDYB_PARA_BASE_ADDR+0x004a))
#define   MOD_YDYB_begin_addr                 byte0(*(MOD_YDYB_PARA_BASE_ADDR+0x004b))
#define   MOD_YDYB_end_addr                   byte1(*(MOD_YDYB_PARA_BASE_ADDR+0x004b))
#define   MOD_YDYB_YK_fx_wait_time                 (*(MOD_YDYB_PARA_BASE_ADDR+0x004c))
#define   MOD_YDYB_YK_fx_flag                 byte0(*(MOD_YDYB_PARA_BASE_ADDR+0x004d))
#define   MOD_YDYB_YK_poll_addr               byte1(*(MOD_YDYB_PARA_BASE_ADDR+0x004d))
#define   MOD_YDYB_transmit_wait_time              (*(MOD_YDYB_PARA_BASE_ADDR+0x004e))
#define   MOD_YDYB_begin_zf_disable_time           (*(MOD_YDYB_PARA_BASE_ADDR+0x004f))
#define   MOD_YDYB_wait_replay                byte0(*(MOD_YDYB_PARA_BASE_ADDR+0x0050))
#define   MOD_YDYB_rec_frame_type             byte1(*(MOD_YDYB_PARA_BASE_ADDR+0x0050))



/*---------------------------------------------------------------------------*/
/*                        IMPORT            functions                        */
/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/
/************************************************/
/* MOD_YDYB_MOD_YDYB_CRC16_cal         function              */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
unsigned int MOD_YDYB_CRC16(unsigned char *MOD_YDYB_CRC16_start,unsigned char MOD_YDYB_CRC16_bytes)    //*x为指向每行前5个数据的指针
{
unsigned int bx, cx, i, j;

    bx = 0xffff;
    cx = 0xa001;
    for(i=0;i<MOD_YDYB_CRC16_bytes;i++)
     {
      bx=bx^MOD_YDYB_CRC16_start[i];
      for(j=0;j<8;j++)
       {
        if ((bx&0x0001)==1)
         {
          bx=bx>>1;
          bx=bx&0x7fff;
          bx=bx^cx;
         }
        else
         {
          bx=bx>>1;
          bx=bx&0x7fff;
         }
       }
     }
    return(bx);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_YDYB_fcb_check         function              */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static BYTE MOD_YDYB_not_comm_count(void)
{
    BYTE  the_ram_axl;
    BYTE  the_ram_axh;
    the_ram_axl=port_report[0];
    the_ram_axh=port_send[MOD_YDYB_PORT_NO][0];
    if(the_ram_axl!=the_ram_axh)
      return YES;
    else 
      return NO;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_YDYB_YC_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_YDYB_YC_cmd(BYTE slave_adr)  
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MOD_YDYB_PORT_NO]);
    temp_ptS_B[0] =slave_adr;
    temp_ptS_B[1] =0x03;	//-每个东西的很多东西都可能不一样,但是很多都是一样的
    temp_ptS_B[2] =0x00; //YC起始地址
    temp_ptS_B[3] =0x00;
    temp_ptS_B[4] =0x00;  //访问YC区字数
    temp_ptS_B[5] =0x01; 
      the_ram_ax=MOD_YDYB_CRC16(temp_ptS_B,6);
    temp_ptS_B[6] =byte0(the_ram_ax);
    temp_ptS_B[7] =byte1(the_ram_ax);
    port_send_len[MOD_YDYB_PORT_NO]=8;    
    port_send_begin();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_YDYB_scheduler      function               */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_YDYB_scheduler(void)
{
    #define    delta_len            temp_lp_int
    #define    bv_yx_no             port_check

    far  BYTE *temp_fptS;
         WORD  the_ram_ax;
         WORD  the_ram_bx;
         BYTE  temp_axl;
    
    temp_fptS=(far BYTE *)&(port_send[MOD_YDYB_PORT_NO]);
      
    if((port_recv_pt[MOD_YDYB_PORT_NO]!=port_recv_dl[MOD_YDYB_PORT_NO])&&(MOD_YDYB_wait_replay==YES))
    {
      if(MOD_YDYB_rxd_head_flag==NO)
      {
       	disable();
       	if(port_recv_dl[MOD_YDYB_PORT_NO]>port_recv_pt[MOD_YDYB_PORT_NO])
           delta_len=(port_recv_pt[MOD_YDYB_PORT_NO]+512)-port_recv_dl[MOD_YDYB_PORT_NO];
        else
           delta_len=port_recv_pt[MOD_YDYB_PORT_NO]-port_recv_dl[MOD_YDYB_PORT_NO];
       	enable();
        for(temp_loop=port_recv_dl[MOD_YDYB_PORT_NO];temp_loop<(delta_len+port_recv_dl[MOD_YDYB_PORT_NO]);temp_loop++)
        {
       	  if(port_recv[MOD_YDYB_PORT_NO][temp_loop]==port_send[MOD_YDYB_PORT_NO][0])
       	  {
       	    the_ram_ax=(port_recv_dl[temp_loop]+1)&0x1ff;
       	    if(port_recv[MOD_YDYB_PORT_NO][the_ram_ax]==port_send[MOD_YDYB_PORT_NO][1])
       	    {
       	       MOD_YDYB_rxd_head_flag=YES;
       	       break;
       	    }
       	  }  
       	  port_recv_dl[MOD_YDYB_PORT_NO]++;
       	  port_recv_dl[MOD_YDYB_PORT_NO]&=0x1ff;       	        	 
        }
      }
      if(MOD_YDYB_rxd_head_flag==YES)
      {       	
       	disable();
       	if(port_recv_dl[MOD_YDYB_PORT_NO]>port_recv_pt[MOD_YDYB_PORT_NO])
           delta_len=(port_recv_pt[MOD_YDYB_PORT_NO]+512)-port_recv_dl[MOD_YDYB_PORT_NO];
        else
           delta_len=port_recv_pt[MOD_YDYB_PORT_NO]-port_recv_dl[MOD_YDYB_PORT_NO];
       	enable();
        if(delta_len>=3)
        {
          the_ram_ax=(port_recv_dl[MOD_YDYB_PORT_NO]+1)&0x1ff;
          if(port_recv[MOD_YDYB_PORT_NO][the_ram_ax]==port_send[MOD_YDYB_PORT_NO][1])
          {
           temp_int=(port_recv_dl[MOD_YDYB_PORT_NO]+2)&0x1ff;
           if(delta_len>=(unsigned short)(port_recv[MOD_YDYB_PORT_NO][temp_int]+5))
            {
             MOD_YDYB_rxd_head_flag=NO;
             MOD_YDYB_wait_replay=NO;
             MOD_YDYB_rec_OK=YES;             
             goto rec_ok_deal;
            }
          }
          else if(delta_len>=8)
          {
             MOD_YDYB_rxd_head_flag=NO;
             MOD_YDYB_wait_replay=NO;
             MOD_YDYB_rec_OK=YES;             
             goto rec_ok_deal;
          }            
        }
      }                      
    }         
    goto rxd_out_time;
rec_ok_deal: 
    if(MOD_YDYB_rec_OK==YES)
    {
      MOD_YDYB_rec_OK=NO;
      MOD_YDYB_transmit_flag=YES;
      MOD_YDYB_transmit_wait_time=Time_1ms_Counter;
      the_ram_bx=(port_recv_dl[MOD_YDYB_PORT_NO]+1)&0x1ff;;
      if(port_recv[MOD_YDYB_PORT_NO][the_ram_bx]!=0x05)
      {
        the_ram_ax=(port_recv_dl[MOD_YDYB_PORT_NO]+2)&0x1ff;
        temp_int=port_recv[MOD_YDYB_PORT_NO][the_ram_ax]+5+port_recv_dl[MOD_YDYB_PORT_NO];        
        for(temp_loop=port_recv_dl[MOD_YDYB_PORT_NO];temp_loop<temp_int;temp_loop++)
         {
          if(temp_loop<=511)
            port_report[temp_loop-port_recv_dl[MOD_YDYB_PORT_NO]]=port_recv[MOD_YDYB_PORT_NO][temp_loop];
          else
            port_report[temp_loop-port_recv_dl[MOD_YDYB_PORT_NO]]=port_recv[MOD_YDYB_PORT_NO][temp_loop-512];
         }
       port_recv_dl[MOD_YDYB_PORT_NO]+=delta_len;
       port_recv_dl[MOD_YDYB_PORT_NO]&=0x1ff;
       temp_int=MOD_YDYB_CRC16(&port_report[0],port_report[2]+3);
       if((byte0(temp_int)!=port_report[port_report[2]+3])||(byte1(temp_int)!=port_report[port_report[2]+4]))
        goto inrda;
      }
      else
      {
       	temp_int=port_recv_dl[MOD_YDYB_PORT_NO]+8;
       	for(temp_loop=port_recv_dl[MOD_YDYB_PORT_NO];temp_loop<temp_int;temp_loop++)
         {
          if(temp_loop<=511)
            port_report[temp_loop-port_recv_dl[MOD_YDYB_PORT_NO]]=port_recv[MOD_YDYB_PORT_NO][temp_loop];
          else
            port_report[temp_loop-port_recv_dl[MOD_YDYB_PORT_NO]]=port_recv[MOD_YDYB_PORT_NO][temp_loop-512];
         }
        port_recv_dl[MOD_YDYB_PORT_NO]+=delta_len;
        port_recv_dl[MOD_YDYB_PORT_NO]&=0x1ff;
        temp_int=MOD_YDYB_CRC16(&port_report[0],6);
        if((byte0(temp_int)!=port_report[6])||(byte1(temp_int)!=port_report[7]))
         goto inrda;        
      }
      
      if(MOD_YDYB_comm_err_counter[port_report[0]]!=0)
      {
        MOD_YDYB_comm_err_counter[port_report[0]]=0;
        temp_int=port_report[0]+MOD_YDYB_begin_addr-byte0(port_info[MOD_YDYB_PORT_NO].scan_panel);
        YX_State[IED_TX_STATE_START_WORD+temp_int/16]&=(0xffff-(0x0001<<(temp_int%16)));
      }    
    }
    switch(MOD_YDYB_rec_frame_type)
    {
      case MOD_YDYB_YC_FRAME:       //yc
         Core_Src_Unit =port_report[0]+MOD_YDYB_begin_addr-byte0(port_info[MOD_YDYB_PORT_NO].scan_panel);
         Core_Det_Pt=(WORD *)&(YC_State[unit_info[Core_Src_Unit].yc_val_start]);
           
         temp_int=port_report[2]/2;
         if(temp_int>unit_info[Core_Src_Unit].yc_val_num)
            temp_int=unit_info[Core_Src_Unit].yc_val_num;
         
         for(temp_loop=0;temp_loop<temp_int;temp_loop++)
         {
            the_ram_bx=port_report[2*temp_loop+3]*0x100+port_report[2*temp_loop+4];
            if(port_report[0]==0x01)
            	the_ram_bx=the_ram_bx/10;
            if(port_report[0]==0x03)
            	the_ram_bx=the_ram_bx/10;
            Core_Det_Pt[temp_loop]=(the_ram_bx&0xfff);	//-也是直接更新的数据库的值
         }
         break;
      default:
         break;
    }
          
rxd_out_time:
    if(Judge_Time_In_MainLoop(MOD_YDYB_rxd_wait_time,MOD_YDYB_WAIT_TIME_VALUE)==YES)
    {	//-出错的情况下,才会执行到这里
      MOD_YDYB_rec_OK=NO;
      MOD_YDYB_rxd_wait_time=Time_1ms_Counter; 
      MOD_YDYB_transmit_flag=YES;
      MOD_YDYB_wait_replay=NO;
      MOD_YDYB_transmit_wait_time=Time_1ms_Counter;
//      if(MOD_YDYB_not_comm_count()==YES)
//       {
       	MOD_YDYB_comm_err_counter[port_send[MOD_YDYB_PORT_NO][0]]++;
        if(MOD_YDYB_comm_err_counter[port_send[MOD_YDYB_PORT_NO][0]]>3)
        {
          MOD_YDYB_comm_err_counter[port_send[MOD_YDYB_PORT_NO][0]]=0;
          temp_int=port_send[MOD_YDYB_PORT_NO][0]+MOD_YDYB_begin_addr-byte0(port_info[MOD_YDYB_PORT_NO].scan_panel);
          YX_State[IED_TX_STATE_START_WORD+temp_int/16]|=(0x0001<<(temp_int%16));      	
       	}
//       }
    }
inrda:
    if(port_transmit_flag[MOD_YDYB_PORT_NO]==YES)
    {
       if(Judge_Time_In_MainLoop(MOD_YDYB_transmit_wait_time,200)==YES)
       {      
           if((MOD_YDYB_transmit_task_busy!=YES)||(MOD_YDYB_transmit_yk_host_port==port_transmit[MOD_YDYB_PORT_NO][1])) 
           {
              port_transmit_flag[MOD_YDYB_PORT_NO]=NO;                          
              switch(port_transmit[MOD_YDYB_PORT_NO][8])
              {
                 case CORE_CODE_YK_CHOOSE:
                      
                      break;
                      
                 case CORE_CODE_YK_EXECUTE:
                      MOD_YDYB_transmit_task_busy=NO;
                      break;
                      
                 case CORE_CODE_YK_CANCEL:
                      MOD_YDYB_transmit_task_busy=NO;
                      break;
                 
                 default:
                      MOD_YDYB_transmit_task_busy=NO;
                      break;
              }
           }
       }
    }  
    else
    {
      if(Judge_LongTime_In_MainLoop(MOD_YDYB_begin_veriiedclk_time,TIME_MOD_YDYB_VERIFY_TIME_VALUE)==YES)//校时
      {
        MOD_YDYB_begin_veriiedclk_time=Time_2048ms_Counter;
        //MOD_YDYB_transmit_time_cmd();
        MOD_YDYB_rec_frame_type=NO;
        MOD_YDYB_transmit_wait_time=Time_1ms_Counter;
        MOD_YDYB_transmit_flag=YES;
      }
      else if(MOD_YDYB_transmit_flag==YES)
      {
        if(MOD_YDYB_YK_fx_flag==YES)
        {
          MOD_YDYB_transmit_task_busy=YES;
          MOD_YDYB_wait_replay=YES;
          MOD_YDYB_transmit_disable_begin_time=Time_2048ms_Counter;
          MOD_YDYB_transmit_yk_host_port=port_transmit[MOD_YDYB_PORT_NO][1];
          //MOD_YDYB_transmit_yk_cmd();
          MOD_YDYB_rec_frame_type=MOD_YDYB_YK_FRAME;
          MOD_YDYB_transmit_flag=NO;
          MOD_YDYB_rxd_head_flag=NO;
          MOD_YDYB_rxd_wait_time=Time_1ms_Counter;
          MOD_YDYB_YK_fx_flag=YES;
          if(Judge_LongTime_In_MainLoop(MOD_YDYB_YK_fx_wait_time,MOD_YDYB_YK_FX_WAIT_TIME_VALUE)==YES)
          {
       	    MOD_YDYB_YK_fx_flag=NO;
            exchange_target_port=MOD_YDYB_transmit_yk_host_port;
            exchange_buf[1] = MOD_YDYB_PORT_NO;
            exchange_buf[2] = PROTOCOL_MOD_YDYB%0x100;
            exchange_buf[3] = PROTOCOL_MOD_YDYB/0x100;
            exchange_buf[5] = 0;
            exchange_buf[8] = CORE_CODE_YK_VERIFY;
            exchange_buf[9] = 0x02;
            exchange_buf[10]= 0x00;
            exchange_buf[16]=CORE_CODE2_YK_ERROR; 
            exchange_buf[17]=port_transmit[MOD_YDYB_PORT_NO][17];
           // Ex_Produce_Transmit_Info();        	
          }
        }
        else  if(Judge_Time_In_MainLoop(MOD_YDYB_transmit_wait_time,80)==YES)
           switch(MOD_YDYB_transmit_control)
           {
            case 1:
                 MOD_YDYB_rec_frame_type=MOD_YDYB_YC_FRAME;
                 MOD_YDYB_wait_replay=YES;
                 MOD_YDYB_YC_cmd(MOD_YDYB_now_poll_addr);
                 MOD_YDYB_now_poll_addr++;
                 disable();
    	   				port_recv_pt[MOD_YDYB_PORT_NO]=0;
    	   				port_recv_dl[MOD_YDYB_PORT_NO]=0;
    	   				enable();
                 MOD_YDYB_transmit_flag=NO;
                 MOD_YDYB_rxd_head_flag=NO;
                 MOD_YDYB_rxd_wait_time=Time_1ms_Counter;
                 if(MOD_YDYB_now_poll_addr>byte1(port_info[MOD_YDYB_PORT_NO].scan_panel))
                  {
                   MOD_YDYB_now_poll_addr=byte0(port_info[MOD_YDYB_PORT_NO].scan_panel);
                   MOD_YDYB_transmit_control=1;
                  }              
                 break;
           
            default:              
                 break;
           }
      } 
    }
}


/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/
/************************************************/
/* MOD_YDYB_Init       function                   */
/************************************************/
/*===========================================================================*/
void MOD_YDYB_Init()
{
    BYTE temp_axl;        
    MOD_YDYB_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];

    MOD_YDYB_begin_veriiedclk_time=Time_2048ms_Counter;
    MOD_YDYB_rxd_wait_time=Time_1ms_Counter;
    MOD_YDYB_transmit_task_busy=NO;
    MOD_YDYB_wait_replay=NO;
    MOD_YDYB_begin_addr=byte0(port_info[MOD_YDYB_PORT_NO].mirror_unit); 
    MOD_YDYB_end_addr=byte1(port_info[MOD_YDYB_PORT_NO].mirror_unit); 
    MOD_YDYB_now_poll_addr=byte0(port_info[MOD_YDYB_PORT_NO].scan_panel);
    MOD_YDYB_rxd_head_flag=NO;
    MOD_YDYB_transmit_flag=YES;
    MOD_YDYB_transmit_wait_time=Time_1ms_Counter;
    MOD_YDYB_rec_OK=NO;
    MOD_YDYB_rec_frame_type=0;
    MOD_YDYB_YK_fx_flag=NO;
    MOD_YDYB_begin_zf_disable_time=Time_2048ms_Counter;
    disable();
    port_recv_pt[MOD_YDYB_PORT_NO]=0;
    port_recv_dl[MOD_YDYB_PORT_NO]=0;
    enable();
    MOD_YDYB_transmit_control=1;
    for(temp_axl=0;temp_axl<32;temp_axl++)
        MOD_YDYB_comm_err_counter[temp_axl]=0;   
}

/************************************************/
/* MOD_YDYB_Main     function                     */
/************************************************/
/*===========================================================================*/
void MOD_YDYB_Main()
{
    MOD_YDYB_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];

    MOD_YDYB_scheduler();  
    
    if(Judge_LongTime_In_MainLoop(MOD_YDYB_begin_zf_disable_time,15)==YES) // after initialed 1s, let ZF be enable.  
    {
        Portx_Poll_First[MOD_YDYB_PORT_NO]=YES;
    }
    if(Judge_LongTime_In_MainLoop(MOD_YDYB_transmit_disable_begin_time,30)==YES)
    {
        MOD_YDYB_transmit_task_busy=NO;		//-超过时间就变的无效了
    }

}


/************************************************/
/* MOD_YDYB_Monitor     function                  */
/************************************************/
/*===========================================================================*/
void MOD_YDYB_Monitor()
{
  MOD_YDYB_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];	//-防错,正常情况下没有也无所谓
  
}

