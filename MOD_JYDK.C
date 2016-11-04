/*****************************************************************************/
/*       FileName  :   MOD_JYDK.C                                              */
/*       Content   :   DSA-208 MOD_JYDK Module                                 */
/*       Date      :   Wed  07-11-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/
//-本程序的通讯是为ATS栓电源转换开关量身打造的
//-ATS控制器,的报文没有主动上送的任何一个报文,全部是一问一答型
//-现在再一次查看的时候,发现了一个很好的解释,transmit表示传送,send表示发送
//所以说当一遍看不明白的时候再来一遍,还不行那就再来一遍,没有搞不定的东西
//-此处的传送是内部在各个端口之间的传送信息,而发送是从端口上发送报文出去

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
register  far  WORD                         *MOD_JYDK_PARA_BASE_ADDR;

#define   MOD_JYDK_PORT_NO                     port_no

#define   MOD_JYDK_YC_FRAME                   0x01
#define   MOD_JYDK_YX_FRAME                   0x02
#define   MOD_JYDK_TS_FRAME                   0x04 
#define   MOD_JYDK_YK_FRAME                   0x08

#define   TIME_MOD_JYDK_VERIFY_TIME_VALUE     150    // 2048ms*150=307s
#define   MOD_JYDK_WAIT_TIME_VALUE            2000    //等待接收时间 1ms*500
#define   MOD_JYDK_YK_FX_WAIT_TIME_VALUE      10    //遥控等待返校时间 2048ms*15

#define   MOD_JYDK_begin_veriiedclk_time           (*(MOD_JYDK_PARA_BASE_ADDR+0x0000))	//-也许这些具体的数值就没有任何意义,仅仅是一个定义空间
#define   MOD_JYDK_rxd_wait_time                   (*(MOD_JYDK_PARA_BASE_ADDR+0x0004))
#define   MOD_JYDK_transmit_disable_begin_time     (*(MOD_JYDK_PARA_BASE_ADDR+0x0005))
#define   MOD_JYDK_rxd_head_flag              byte0(*(MOD_JYDK_PARA_BASE_ADDR+0x0006))
#define   MOD_JYDK_transmit_control           byte1(*(MOD_JYDK_PARA_BASE_ADDR+0x0006)) 
#define   MOD_JYDK_comm_err_counter                 (MOD_JYDK_PARA_BASE_ADDR+0x0028) //0x0008-0x0027 
//receive
#define   MOD_JYDK_transmit_yk_host_port      byte0(*(MOD_JYDK_PARA_BASE_ADDR+0x0048))
#define   MOD_JYDK_transmit_task_busy         byte1(*(MOD_JYDK_PARA_BASE_ADDR+0x0048))
#define   MOD_JYDK_now_poll_addr              byte0(*(MOD_JYDK_PARA_BASE_ADDR+0x0049))
#define   MOD_JYDK_transmit_flag              byte1(*(MOD_JYDK_PARA_BASE_ADDR+0x0049))
#define   MOD_JYDK_rec_OK                     byte0(*(MOD_JYDK_PARA_BASE_ADDR+0x004a))
#define   MOD_JYDK_begin_addr                 byte0(*(MOD_JYDK_PARA_BASE_ADDR+0x004b))
#define   MOD_JYDK_end_addr                   byte1(*(MOD_JYDK_PARA_BASE_ADDR+0x004b))
#define   MOD_JYDK_YK_fx_wait_time                 (*(MOD_JYDK_PARA_BASE_ADDR+0x004c))
#define   MOD_JYDK_YK_fx_flag                 byte0(*(MOD_JYDK_PARA_BASE_ADDR+0x004d))
#define   MOD_JYDK_YK_poll_addr               byte1(*(MOD_JYDK_PARA_BASE_ADDR+0x004d))
#define   MOD_JYDK_transmit_wait_time              (*(MOD_JYDK_PARA_BASE_ADDR+0x004e))
#define   MOD_JYDK_begin_zf_disable_time           (*(MOD_JYDK_PARA_BASE_ADDR+0x004f))
#define   MOD_JYDK_wait_replay                byte0(*(MOD_JYDK_PARA_BASE_ADDR+0x0050))
#define   MOD_JYDK_rec_frame_type             byte1(*(MOD_JYDK_PARA_BASE_ADDR+0x0050))



/*---------------------------------------------------------------------------*/
/*                        IMPORT            functions                        */
/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/
/************************************************/
/* MOD_JYDK_MOD_JYDK_CRC16_cal         function              */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
unsigned int MOD_JYDK_CRC16(unsigned char *MOD_JYDK_CRC16_start,unsigned char MOD_JYDK_CRC16_bytes)    //*x为指向每行前5个数据的指针
{
	unsigned int bx, cx, i, j;

    bx = 0xffff;
    cx = 0xa001;
    for(i=0;i<MOD_JYDK_CRC16_bytes;i++)
     {
      bx=bx^MOD_JYDK_CRC16_start[i];
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
/* MOD_JYDK_fcb_check         function              */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static BYTE MOD_JYDK_not_comm_count(void)	//-检查是否通讯,也是判断条件之一
{
    BYTE  the_ram_axl;
    BYTE  the_ram_axh;
    the_ram_axl=port_report[0];
    the_ram_axh=port_send[MOD_JYDK_PORT_NO][0];
    if(the_ram_axl!=the_ram_axh)
      return YES;
    else 
      return NO;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_JYDK_transmit_yk_cmd    function           */
/************************************************/
//-下面是通讯管理机即本装置系统向下位机发送发送遥控命令,但是本装置并没有这个自主
//-权,是接收到后台通讯的命令做的
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_JYDK_transmit_yk_cmd(void)  
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MOD_JYDK_PORT_NO]);
    
    temp_ptS_B[0] =port_transmit[MOD_JYDK_PORT_NO][5]+byte0(port_info[MOD_JYDK_PORT_NO].scan_panel); //-结果是装置整定地址,后面是地址开始值,那么前面就是偏移量
    temp_ptS_B[1] =0x10;	//-功能码
    temp_ptS_B[2] =0x00;	//-存放数据地址
    temp_ptS_B[3] =0x98;
    temp_ptS_B[4] =0x00;	//-寄存器数
    temp_ptS_B[5] =0x02;
    temp_ptS_B[6] =0x04;	//-字节量
    if(port_transmit[MOD_JYDK_PORT_NO][8]==CORE_CODE_YK_CHOOSE)
     {
      temp_ptS_B[7] =port_transmit[MOD_JYDK_PORT_NO][17];	//-数据1,,实际意义就是切换模式值
      temp_ptS_B[8] =port_transmit[MOD_JYDK_PORT_NO][17];
     }
    temp_ptS_B[9] =0xaa;	//-数据2
    temp_ptS_B[10] =0xaa;
    the_ram_ax=MOD_JYDK_CRC16(temp_ptS_B,11);
    temp_ptS_B[11] =byte0(the_ram_ax);	//-校验CRC
    temp_ptS_B[12] =byte1(the_ram_ax);    
    port_send_len[MOD_JYDK_PORT_NO]=13;    
    port_send_begin();
   
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_JYDK_YC_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_JYDK_YC_cmd(BYTE slave_adr)	//-其实发送报文很简单就是下位机需要的格式,困难的是各种不同类型报文的,最小化变动处理	
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MOD_JYDK_PORT_NO]);	//-这个口中的信息是用于发送的,但是不一定是最后一个缓冲区
    temp_ptS_B[0] =slave_adr;
    temp_ptS_B[1] =0x03;
    temp_ptS_B[2] =0x01; //YC起始地址
    temp_ptS_B[3] =0x20;
    temp_ptS_B[4] =0x00;  //访问YC区字数 ,,其实这个在下位机中并没有处理
    temp_ptS_B[5] =0x13; 	
    the_ram_ax=MOD_JYDK_CRC16(temp_ptS_B,6);
    temp_ptS_B[6] =byte0(the_ram_ax);
    temp_ptS_B[7] =byte1(the_ram_ax);
    port_send_len[MOD_JYDK_PORT_NO]=8;    
    port_send_begin();
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_JYDK_YX_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_JYDK_YX_cmd(BYTE slave_adr)  
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MOD_JYDK_PORT_NO]);
    temp_ptS_B[0] =slave_adr;
    temp_ptS_B[1] =0x02;
    temp_ptS_B[2] =0x00; //YX起始地址
    temp_ptS_B[3] =0x80;
    temp_ptS_B[4] =0x00;  //访问YX区字数
    temp_ptS_B[5] =0x02;
    the_ram_ax=MOD_JYDK_CRC16(temp_ptS_B,6);	//-计算校验CRC
    temp_ptS_B[6] =byte0(the_ram_ax);
    temp_ptS_B[7] =byte1(the_ram_ax);
    port_send_len[MOD_JYDK_PORT_NO]=8;    
    port_send_begin();
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_JYDK_TS_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_JYDK_TS_cmd(BYTE slave_adr)	//-怎么感觉是对时的缩写呢,,错是特殊事件的缩写,在我的下位机中对应的是保护事件
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MOD_JYDK_PORT_NO]);
    temp_ptS_B[0] =slave_adr;
    temp_ptS_B[1] =0x03;
    temp_ptS_B[2] =0x06; //TS起始地址
    temp_ptS_B[3] =0x00;
    temp_ptS_B[4] =0x00;  //访问TS区字数
    temp_ptS_B[5] =0x10;
    the_ram_ax=MOD_JYDK_CRC16(temp_ptS_B,6);
    temp_ptS_B[6] =byte0(the_ram_ax);
    temp_ptS_B[7] =byte1(the_ram_ax);
    port_send_len[MOD_JYDK_PORT_NO]=8;    
    port_send_begin();
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_JYDK_transmit_time_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_JYDK_transmit_time_cmd(void)  	//-向下位机发送时间,进行校时
{
    WORD the_ram_ax;
    temp_ptS_B=(far BYTE *)&(port_send[MOD_JYDK_PORT_NO]);
    temp_ptS_B[0] =0x00;
    temp_ptS_B[1] =0x10;
    temp_ptS_B[2] =0x00;
    temp_ptS_B[3] =0xb0;
    temp_ptS_B[4] =0x00;
    temp_ptS_B[5] =0x04;
    temp_ptS_B[6] =0x08;
    temp_ptS_B[7] =0x00;	//-年高
    the_ram_ax=REG_Year % 100;
    temp_ptS_B[8] =byte0(the_ram_ax);	//-年低
    temp_ptS_B[9] =REG_Month;
    temp_ptS_B[10]=REG_Date;
    temp_ptS_B[11]=REG_Hour;
    temp_ptS_B[12]=REG_Minute;
    the_ram_ax=REG_Second*1000+REG_1Msecond;
    temp_ptS_B[13]=byte1(the_ram_ax);
    temp_ptS_B[14]=byte0(the_ram_ax);
    the_ram_ax=MOD_JYDK_CRC16(temp_ptS_B,15);
    temp_ptS_B[15] =byte0(the_ram_ax);
    temp_ptS_B[16] =byte1(the_ram_ax);
    port_send_len[MOD_JYDK_PORT_NO]=17;	//-记录共需要发送几个字节内容
    port_send_begin();	//-
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_JYDK_scheduler      function               */
/************************************************/
//-通过熟悉的内容给得到启发,然后在大胆猜测,这是一个很好的方法
//-程序逻辑:
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_JYDK_scheduler(void)
{
    #define    delta_len            temp_lp_int
    #define    bv_yx_no             port_check

    far  BYTE *temp_fptS;
         WORD  the_ram_ax;
         WORD  the_ram_bx;
         BYTE  temp_axl;
    
    temp_fptS=(far BYTE *)&(port_send[MOD_JYDK_PORT_NO]);
      
    if((port_recv_pt[MOD_JYDK_PORT_NO]!=port_recv_dl[MOD_JYDK_PORT_NO])&&(MOD_JYDK_wait_replay==YES))	//-只有处于等待回复,才有必要进行接收处理
    {
        if(MOD_JYDK_rxd_head_flag==NO)
        {
         	disable();
         	if(port_recv_dl[MOD_JYDK_PORT_NO]>port_recv_pt[MOD_JYDK_PORT_NO])
           		delta_len=(port_recv_pt[MOD_JYDK_PORT_NO]+512)-port_recv_dl[MOD_JYDK_PORT_NO];
            else
           		delta_len=port_recv_pt[MOD_JYDK_PORT_NO]-port_recv_dl[MOD_JYDK_PORT_NO];
         	enable();
            for(temp_loop=port_recv_dl[MOD_JYDK_PORT_NO];temp_loop<(delta_len+port_recv_dl[MOD_JYDK_PORT_NO]);temp_loop++)
            {
         	    if(port_recv[MOD_JYDK_PORT_NO][temp_loop]==port_send[MOD_JYDK_PORT_NO][0])	//-这种判断仅仅适合这种规约,不具有一般性
         	    {
         	        the_ram_ax=(port_recv_dl[temp_loop]+1)&0x1ff;
         	        if(port_recv[MOD_JYDK_PORT_NO][the_ram_ax]==port_send[MOD_JYDK_PORT_NO][1])	//-功能码比对看看是否正确
         	        {
         	            MOD_JYDK_rxd_head_flag=YES;
         	            break;
         	        }
         	    }  
         	    port_recv_dl[MOD_JYDK_PORT_NO]++;
         	    port_recv_dl[MOD_JYDK_PORT_NO]&=0x1ff;       	        	 
            }
        }
        if(MOD_JYDK_rxd_head_flag==YES)
        {
         	disable();
         	if(port_recv_dl[MOD_JYDK_PORT_NO]>port_recv_pt[MOD_JYDK_PORT_NO])
           		delta_len=(port_recv_pt[MOD_JYDK_PORT_NO]+512)-port_recv_dl[MOD_JYDK_PORT_NO];
            else
           		delta_len=port_recv_pt[MOD_JYDK_PORT_NO]-port_recv_dl[MOD_JYDK_PORT_NO];
         	enable();
            if(delta_len>=3)	//-本规约的性质决定了,除从站地址和功能码外,最小的有效报文也至少还有3个字节没有处理
            {	
            	the_ram_ax=(port_recv_dl[MOD_JYDK_PORT_NO]+1)&0x1ff;
                if(port_recv[MOD_JYDK_PORT_NO][the_ram_ax]==port_send[MOD_JYDK_PORT_NO][1])	//-完全就是配合报文内容处理了,,比较功能码
                {	//-在这个规约里面,可能这段程序还没有处理好,即错误的
             		temp_int=(port_recv_dl[MOD_JYDK_PORT_NO]+2)&0x1ff;
             		if(delta_len>=(unsigned short)(port_recv[MOD_JYDK_PORT_NO][temp_int]+5))
              		{
               			MOD_JYDK_rxd_head_flag=NO;
               			MOD_JYDK_wait_replay=NO;
               			MOD_JYDK_rec_OK=YES;             
               			goto rec_ok_deal;
              		}
                }
                else if(delta_len>=8)
                {
               		MOD_JYDK_rxd_head_flag=NO;
               		MOD_JYDK_wait_replay=NO;
               		MOD_JYDK_rec_OK=YES;             
               		goto rec_ok_deal;
                }
            }
        }
    }         
    goto rxd_out_time;
rec_ok_deal: 
    if(MOD_JYDK_rec_OK==YES)	//-表示接收好了,就是一种简单的标志
    {
        MOD_JYDK_rec_OK=NO;
        MOD_JYDK_transmit_flag=YES;	//-表示可以主动发送报文了
        MOD_JYDK_transmit_wait_time=Time_1ms_Counter;
        the_ram_bx=(port_recv_dl[MOD_JYDK_PORT_NO]+1)&0x1ff;;
        if(port_recv[MOD_JYDK_PORT_NO][the_ram_bx]!=0x05)	//-还是判断功能码
        {
            the_ram_ax=(port_recv_dl[MOD_JYDK_PORT_NO]+2)&0x1ff;	//-定位到数据字节长度单元上
            temp_int=port_recv[MOD_JYDK_PORT_NO][the_ram_ax]+5+port_recv_dl[MOD_JYDK_PORT_NO];  //-整个报文长度字节数      
            for(temp_loop=port_recv_dl[MOD_JYDK_PORT_NO];temp_loop<temp_int;temp_loop++)
            {
            	if(temp_loop<=511)
              	    port_report[temp_loop-port_recv_dl[MOD_JYDK_PORT_NO]]=port_recv[MOD_JYDK_PORT_NO][temp_loop];	//-双缓冲区
            	else
              	    port_report[temp_loop-port_recv_dl[MOD_JYDK_PORT_NO]]=port_recv[MOD_JYDK_PORT_NO][temp_loop-512];
            }
            port_recv_dl[MOD_JYDK_PORT_NO]+=delta_len;	//-至此接收到的最新报文已经舍弃了
            port_recv_dl[MOD_JYDK_PORT_NO]&=0x1ff;
            temp_int=MOD_JYDK_CRC16(&port_report[0],port_report[2]+3);
            if((byte0(temp_int)!=port_report[port_report[2]+3])||(byte1(temp_int)!=port_report[port_report[2]+4]))
            	  goto inrda;
        }
        else
        {
         	temp_int=port_recv_dl[MOD_JYDK_PORT_NO]+8;
         	for(temp_loop=port_recv_dl[MOD_JYDK_PORT_NO];temp_loop<temp_int;temp_loop++)
            {
                if(temp_loop<=511)
                    port_report[temp_loop-port_recv_dl[MOD_JYDK_PORT_NO]]=port_recv[MOD_JYDK_PORT_NO][temp_loop];
                else
                    port_report[temp_loop-port_recv_dl[MOD_JYDK_PORT_NO]]=port_recv[MOD_JYDK_PORT_NO][temp_loop-512];
            }
            port_recv_dl[MOD_JYDK_PORT_NO]+=delta_len;
            port_recv_dl[MOD_JYDK_PORT_NO]&=0x1ff;
            temp_int=MOD_JYDK_CRC16(&port_report[0],6);
            if((byte0(temp_int)!=port_report[6])||(byte1(temp_int)!=port_report[7]))
               goto inrda;        
        }
        
        if(MOD_JYDK_comm_err_counter[port_report[0]]!=0)
        {	//-连续错误才认为是错误的
            MOD_JYDK_comm_err_counter[port_report[0]]=0;
            temp_int=port_report[0]+MOD_JYDK_begin_addr-byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);
            YX_State[IED_TX_STATE_START_WORD+temp_int/16]&=(0xffff-(0x0001<<(temp_int%16)));
        }    
    }
     //-上面的处理仅仅是对报文的处理,跳转到inrda的处理还是发送报文类,下面的处理却是对接收到的具体内容的处理,这中间有一个时间差
    switch(MOD_JYDK_rec_frame_type)	//-这个值是发送的时候给的,那么他处理的报文就是下位机返回时的功能码和接收的功能码是一致的
    {	//-这里处理的是接收报文,用于更新内部数据库
      case MOD_JYDK_YC_FRAME:       //yc
         Core_Src_Unit =port_report[0]+MOD_JYDK_begin_addr-byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);
         Core_Det_Pt=(WORD *)&(YC_State[unit_info[Core_Src_Unit].yc_val_start]);
         //让遥测指针指向0000位置
         temp_int=port_report[2]/2;	//-数据字数目
         //计算遥测个数 
         if(temp_int>unit_info[Core_Src_Unit].yc_val_num)
            temp_int=unit_info[Core_Src_Unit].yc_val_num;	//-实际上传的数据量多与配置数目的话直接舍弃多余的部分,因为没有空间记录
         
         for(temp_loop=0;temp_loop<temp_int;temp_loop++)
         {
            Core_Det_Pt[temp_loop]=port_report[2*temp_loop+3]*0x100+port_report[2*temp_loop+4];
            //取值 赋值
            the_ram_bx=Core_Det_Pt[temp_loop];
            if(temp_loop<3)	//-不同位置的数据有不同的处理过程
            {
            	the_ram_bx=the_ram_bx/50;
            }
            else
            {
            		if(temp_loop<6)
            		{
            		   the_ram_bx=the_ram_bx;
            		}
            		else if(temp_loop<14)
            		{
            		   the_ram_bx=the_ram_bx/50;
            		}
            		else if(temp_loop<16)
            		{
            		   the_ram_bx=the_ram_bx/100;
            		}
            		else if(temp_loop<17)
            		   the_ram_bx=the_ram_bx;
            		else if(temp_loop<18)
            		   the_ram_bx=the_ram_bx/50;
            }
            //计算数值，对应数据放大或缩小，防止转发溢出
            Core_Det_Pt[temp_loop]=(the_ram_bx&0xfff);	//-这个地方的遥测没有使用统一模块,就是直接更新了数据库中数据
         }         
         break;
      case MOD_JYDK_YX_FRAME:       //yx
         Core_Src_Unit=port_report[0]+MOD_JYDK_begin_addr-byte0(port_info[MOD_JYDK_PORT_NO].scan_panel); //-既然处理的目的是数据库,当然求的就是数据库的偏移量
         Core_Src_Pt_B=(BYTE *)&port_deal_buf[MOD_JYDK_PORT_NO][0];
         the_ram_bx=(port_report[2]/2);

         for(temp_loop=0;temp_loop<unit_info[Core_Src_Unit].yx_num;temp_loop++)
         {
             Core_Src_Pt_B[2*temp_loop]=byte0(YX_State[unit_info[Core_Src_Unit].yx_start+temp_loop]);
             Core_Src_Pt_B[2*temp_loop+1]=byte1(YX_State[unit_info[Core_Src_Unit].yx_start+temp_loop]);
         }	//-这可能就是最简单的加密处理
         Core_Src_Pt_B=(BYTE *)&port_deal_buf[MOD_JYDK_PORT_NO][0];
         for(temp_loop=0;temp_loop<(Core_Src_Len/2);temp_loop++)
         {
             Core_Src_Pt_B[2*temp_loop]=port_report[2*temp_loop+4];	//-特别对待
             Core_Src_Pt_B[2*temp_loop+1]=port_report[2*temp_loop+3];
         }
         core_update_YX();    
         break;
      case MOD_JYDK_TS_FRAME:       //ts
         Core_Src_Unit=port_report[0]+MOD_JYDK_begin_addr-byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);
         Core_Src_Pt_B=(BYTE *)&port_deal_buf[MOD_JYDK_PORT_NO][0];
         if(port_report[2]==1)
         	 break;
         if(port_report[2]==0x14)
         {
             Core_Src_Unit=port_report[0]+MOD_JYDK_begin_addr-byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);
             Core_Src_Pt_B=(BYTE *)&port_deal_buf[MOD_JYDK_PORT_NO][0];
             temp_lp_int=port_report[10]*0x100+port_report[11];
             temp_int=temp_lp_int%1000;
             port_deal_buf[MOD_JYDK_PORT_NO][0]=byte0(temp_int);
             port_deal_buf[MOD_JYDK_PORT_NO][1]=byte1(temp_int);
             port_deal_buf[MOD_JYDK_PORT_NO][2]=port_report[9];
             port_deal_buf[MOD_JYDK_PORT_NO][3]=port_report[8];
             port_deal_buf[MOD_JYDK_PORT_NO][4]=port_report[7];
             port_deal_buf[MOD_JYDK_PORT_NO][5]=REG_Date; 
             bv_yx_no=32+port_report[12];              
             port_deal_buf[MOD_JYDK_PORT_NO][6]=bv_yx_no;
             port_deal_buf[MOD_JYDK_PORT_NO][7]=0;
             port_deal_buf[MOD_JYDK_PORT_NO][7]=port_deal_buf[MOD_JYDK_PORT_NO][7]|0x80;
             core_insert_SOE();                        	 
         }
         break;

      default:
         break;
    }
          
rxd_out_time:
    if(Judge_Time_In_MainLoop(MOD_JYDK_rxd_wait_time,MOD_JYDK_WAIT_TIME_VALUE)==YES)
    {	//-下面是错误处理
     	 MOD_JYDK_rec_OK=NO;
     	 MOD_JYDK_rxd_wait_time=Time_1ms_Counter; 
     	 MOD_JYDK_transmit_flag=YES;
     	 MOD_JYDK_wait_replay=NO;
     	 MOD_JYDK_transmit_wait_time=Time_1ms_Counter;
//   	   if(MOD_JYDK_not_comm_count()==YES)
//   	    {
     	   MOD_JYDK_comm_err_counter[port_send[MOD_JYDK_PORT_NO][0]]++;
     	   if(MOD_JYDK_comm_err_counter[port_send[MOD_JYDK_PORT_NO][0]]>3)
     	   {
     	       MOD_JYDK_comm_err_counter[port_send[MOD_JYDK_PORT_NO][0]]=0;
     	       temp_int=port_send[MOD_JYDK_PORT_NO][0]+MOD_JYDK_begin_addr-byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);
     	       YX_State[IED_TX_STATE_START_WORD+temp_int/16]|=(0x0001<<(temp_int%16));      	
     	   }
//   	    }
    }
inrda:	//-已经校验了CRC之后就到这里处理了
    if(port_transmit_flag[MOD_JYDK_PORT_NO]==YES)	//-说明其他端口有信息传送到这个端口,优先处理这类信息,处理好了之后才再处理周期信息
    {	//-问题是后台和这个端口不是接在同一个数据总线上的,他们是通过什么缓冲区来实现的呢,通过传送缓冲区
     	if(Judge_Time_In_MainLoop(MOD_JYDK_transmit_wait_time,200)==YES)	//-为YES说明发送等待时间已经过了,又满足一个发送条件
     	{
     	 	if((MOD_JYDK_transmit_task_busy!=YES)||(MOD_JYDK_transmit_yk_host_port==port_transmit[MOD_JYDK_PORT_NO][1])) 
     	 	{//-分两种情况,前面是可以发送判断或后面是对发送出去的信息返回报文的处理
     	 	    port_transmit_flag[MOD_JYDK_PORT_NO]=NO;	//-说明传递过来的内容已经处理过了,可以继续传递数据到这个端口了,指的是其它端口而不是下位机
     	 	    switch(port_transmit[MOD_JYDK_PORT_NO][8])	//-这就是具体传递过来的信息,根据这个指示信息进行处理
     	 	    {
     	 	        case CORE_CODE_YK_CHOOSE:	//-传递来这样的命令,那么就做如下处理
     	 	        	
     	 	             MOD_JYDK_transmit_task_busy=NO;	//-不忙是否说明处于非工作状态,不需要等待,返回传递吗???
     	 	             MOD_JYDK_wait_replay=NO;
     	 	             MOD_JYDK_transmit_disable_begin_time=Time_2048ms_Counter;	//-记录开始时间,这点时间之后一段就可以发送了,这段时间内是禁止的
     	 	             MOD_JYDK_transmit_yk_host_port=port_transmit[MOD_JYDK_PORT_NO][1];	//-记录是哪个端口接收的后台下发信息
     	 	             MOD_JYDK_transmit_yk_cmd();	//-通过这里猜测,不仅仅本系统可以自动定时发送程序给下位机,并接受回答,而且空闲时间,他也接收上位机
				  										//-比如说后台,调度等等发来的报文,这时他作为下位机也同样会做出反应
				  	   //-以前看上下就是一个矛盾的地方,但是现在看来,非常精妙,这里就达到了多级缓冲可以规约或端口转换									
     	 	             exchange_target_port=MOD_JYDK_transmit_yk_host_port;	//-哪个口来的传递信息,还有必要向那个口返回如下的数据
     	 	             exchange_buf[1] = MOD_JYDK_PORT_NO;	//-记录的是装置端口号
     	 	             exchange_buf[2] = PROTOCOL_MOD_JYDK%0x100;	//-规约类型
     	 	             exchange_buf[3] = PROTOCOL_MOD_JYDK/0x100;	//-前面的数据就是一个缓冲区,没有什么规约定义
     	 	             exchange_buf[5] =port_transmit[MOD_JYDK_PORT_NO][5]+MOD_JYDK_begin_addr;	//-内部数据库地址
     	 	             exchange_buf[8] = CORE_CODE_YK_VERIFY;	//-发给后台连接端口的,告知报文已经下发,需要等待下位机的遥控返校报文
     	 	             exchange_buf[9] = 0x02;	//-表示从16开始再传两个单元信息即16和17
     	 	             exchange_buf[10]= 0x00;
     	 	             exchange_buf[16]=port_transmit[MOD_JYDK_PORT_NO][16]; 
     	 	             exchange_buf[17]=port_transmit[MOD_JYDK_PORT_NO][17];
     	 	             Ex_Produce_Transmit_Info();	//-把上面信息传递给指定的端口,不是从哪个端口发送出去            
     	 	             break;
     	 	             
     	 	        case CORE_CODE_YK_EXECUTE:
     	 	             MOD_JYDK_transmit_task_busy=NO;
     	 	             break;
     	 	             
     	 	        case CORE_CODE_YK_CANCEL:
     	 	             MOD_JYDK_transmit_task_busy=NO;
     	 	             break;
     	 	        
     	 	        default:
     	 	             MOD_JYDK_transmit_task_busy=NO;
     	 	             break;
     	 	    }
     	 	}
     	}
    }
    else
    {	//-下面的内容是主动发送内容的判断
        if(Judge_LongTime_In_MainLoop(MOD_JYDK_begin_veriiedclk_time,TIME_MOD_JYDK_VERIFY_TIME_VALUE)==YES)//校时
        {	//-定时校时,优先级很高
            MOD_JYDK_begin_veriiedclk_time=Time_2048ms_Counter;
            MOD_JYDK_transmit_time_cmd();	//-向下位机ATS发送了校时报文(主动)
            MOD_JYDK_rec_frame_type=NO;
            MOD_JYDK_transmit_wait_time=Time_1ms_Counter;	//-记录开始发送时间,起点
            MOD_JYDK_transmit_flag=YES;	//-由于不需要返回报文,所以可以发送新的报文
        }
        else if(MOD_JYDK_transmit_flag==YES)	//-表示可以发送报文,周期查询报文
        {
         	if(Judge_Time_In_MainLoop(MOD_JYDK_transmit_wait_time,80)==YES)
            	switch(MOD_JYDK_transmit_control)	//-内部程序处理之后,给定的值,,这个值的给定是在初始值的基础上逻辑变化的
            	{	//-发送报文的处理
            	    case 1:
            	         MOD_JYDK_rec_frame_type=MOD_JYDK_YC_FRAME;		//-其实这里完全是为接收处理用的
            	         MOD_JYDK_wait_replay=YES;	//-表示开始等待返回报文
            	         MOD_JYDK_YC_cmd(MOD_JYDK_now_poll_addr);
            	         MOD_JYDK_now_poll_addr++;	//-表示需要发送报文的地址,是中端装置中整定的地址,并不是数据库中的偏移量
            	         disable();
    	  		         port_recv_pt[MOD_JYDK_PORT_NO]=0;
    	  		     	 port_recv_dl[MOD_JYDK_PORT_NO]=0;	//-开始发送报文,等待回答报文的时候,当然要把处理缓冲区初始化
    	  		     	 enable();
            	         MOD_JYDK_transmit_flag=NO;	//-为NO表示现在不能发送内容,在等待回复报文
            	         MOD_JYDK_rxd_head_flag=NO;
            	         MOD_JYDK_rxd_wait_time=Time_1ms_Counter;
            	         if(MOD_JYDK_now_poll_addr>byte1(port_info[MOD_JYDK_PORT_NO].scan_panel))
            	         {
            	             MOD_JYDK_now_poll_addr=byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);	//-到下一类报文的时候,还是从第一个装置开始查询
            	             MOD_JYDK_transmit_control=2;	//-决定了接下来需要发送报文的类型
            	         }
            	         break;
            	    case 2:
            	         MOD_JYDK_rec_frame_type=MOD_JYDK_YX_FRAME;
            	         MOD_JYDK_wait_replay=YES;	//-加这个标志的原因是,系统主动发送的报文,并不是所有的内容都需要等待回复报文的,有些报文就是下位机有回复,我也不需要
            	         MOD_JYDK_YX_cmd(MOD_JYDK_now_poll_addr);
            	         MOD_JYDK_now_poll_addr++;
            	         disable();
    	  		         port_recv_pt[MOD_JYDK_PORT_NO]=0;
   		    	         port_recv_dl[MOD_JYDK_PORT_NO]=0;
   		    	         enable();
            	         MOD_JYDK_transmit_flag=NO;
            	         MOD_JYDK_rxd_head_flag=NO;
            	         MOD_JYDK_rxd_wait_time=Time_1ms_Counter;
            	         if(MOD_JYDK_now_poll_addr>byte1(port_info[MOD_JYDK_PORT_NO].scan_panel))
            	         {
            	             MOD_JYDK_now_poll_addr=byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);
            	             MOD_JYDK_transmit_control=1;
            	         }              
            	         break;
            	    case 3:	//-这个仅仅是摆设,或者说预留的
            	         MOD_JYDK_rec_frame_type=MOD_JYDK_TS_FRAME;
            	         MOD_JYDK_wait_replay=YES;
            	         MOD_JYDK_TS_cmd(MOD_JYDK_now_poll_addr);
            	         MOD_JYDK_now_poll_addr++;
            	         disable();
    	  		         port_recv_pt[MOD_JYDK_PORT_NO]=0;
    	  		         port_recv_dl[MOD_JYDK_PORT_NO]=0;
    	  		         enable();
            	         MOD_JYDK_transmit_flag=NO;
            	         MOD_JYDK_rxd_head_flag=NO;
            	         MOD_JYDK_rxd_wait_time=Time_1ms_Counter;
            	         if(MOD_JYDK_now_poll_addr>byte1(port_info[MOD_JYDK_PORT_NO].scan_panel))
            	         {
            	             MOD_JYDK_now_poll_addr=byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);
            	             MOD_JYDK_transmit_control=1;
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
/* MOD_JYDK_Init       function                   */
/************************************************/
/*===========================================================================*/
void MOD_JYDK_Init()
{
    BYTE temp_axl;        
    MOD_JYDK_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];

    MOD_JYDK_begin_veriiedclk_time=Time_2048ms_Counter;
    MOD_JYDK_rxd_wait_time=Time_1ms_Counter;
    MOD_JYDK_transmit_task_busy=NO;
    MOD_JYDK_wait_replay=NO;
    MOD_JYDK_begin_addr=byte0(port_info[MOD_JYDK_PORT_NO].mirror_unit); 
    MOD_JYDK_end_addr=byte1(port_info[MOD_JYDK_PORT_NO].mirror_unit); 
    MOD_JYDK_now_poll_addr=byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);
    MOD_JYDK_rxd_head_flag=NO;
    MOD_JYDK_transmit_flag=YES;	//-表示可以发送报文
    MOD_JYDK_transmit_wait_time=Time_1ms_Counter;
    MOD_JYDK_rec_OK=NO;
    MOD_JYDK_rec_frame_type=0;
    MOD_JYDK_YK_fx_flag=NO;
    MOD_JYDK_begin_zf_disable_time=Time_2048ms_Counter;
    disable();
    port_recv_pt[MOD_JYDK_PORT_NO]=0;
    port_recv_dl[MOD_JYDK_PORT_NO]=0;
    enable();
    MOD_JYDK_transmit_control=1;
    for(temp_axl=0;temp_axl<32;temp_axl++)
         MOD_JYDK_comm_err_counter[temp_axl]=0;   
}

/************************************************/
/* MOD_JYDK_Main     function                     */
/************************************************/
/*===========================================================================*/
void MOD_JYDK_Main()
{
    MOD_JYDK_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];

    MOD_JYDK_scheduler();  
    
    if(Judge_LongTime_In_MainLoop(MOD_JYDK_begin_zf_disable_time,15)==YES) // after initialed 1s, let ZF be enable.  
    {
        Portx_Poll_First[MOD_JYDK_PORT_NO]=YES;
    }
    if(Judge_LongTime_In_MainLoop(MOD_JYDK_transmit_disable_begin_time,30)==YES)
    {
        MOD_JYDK_transmit_task_busy=NO;
    }

}


/************************************************/
/* MOD_JYDK_Monitor     function                  */
/************************************************/
/*===========================================================================*/
void MOD_JYDK_Monitor()
{
  MOD_JYDK_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];	//-这个监测其实完全可以没有,仅仅是为了防错,再次对应变量地址
  
}

