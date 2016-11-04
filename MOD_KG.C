/*****************************************************************************/
/*       FileName  :   MOD_KG.C                                              */
/*       Content   :   DSA-208 MOD_KG Module                                 */
/*       Date      :   Wed  07-11-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/
//-本程序的通讯是为采集盒量身打造的


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
register  far  WORD                         *MOD_KG_PARA_BASE_ADDR;	//-这个应该是定义的全球变量,下面的方法是这个系统认可的

#define   MOD_KG_PORT_NO                     port_no

#define   MOD_KG_YC_FRAME                   0x01	//-不太像数字,有点像标志位
#define   MOD_KG_YX_FRAME                   0x02
#define   MOD_KG_TS_FRAME                   0x04 
#define   MOD_KG_YK_FRAME                   0x08

#define   TIME_MOD_KG_VERIFY_TIME_VALUE     150    // 2048ms*150=307s
#define   MOD_KG_WAIT_TIME_VALUE            2000    //等待接收时间 1ms*500
#define   MOD_KG_YK_FX_WAIT_TIME_VALUE      10    //遥控等待返校时间 2048ms*15

#define   MOD_KG_begin_veriiedclk_time           (*(MOD_KG_PARA_BASE_ADDR+0x0000))
#define   MOD_KG_rxd_wait_time                   (*(MOD_KG_PARA_BASE_ADDR+0x0004))
#define   MOD_KG_transmit_disable_begin_time     (*(MOD_KG_PARA_BASE_ADDR+0x0005))
#define   MOD_KG_rxd_head_flag              byte0(*(MOD_KG_PARA_BASE_ADDR+0x0006))
#define   MOD_KG_transmit_control           byte1(*(MOD_KG_PARA_BASE_ADDR+0x0006)) 
#define   MOD_KG_comm_err_counter                 (MOD_KG_PARA_BASE_ADDR+0x0028) //0x0008-0x0027 
//receive
#define   MOD_KG_transmit_yk_host_port      byte0(*(MOD_KG_PARA_BASE_ADDR+0x0048))
#define   MOD_KG_transmit_task_busy         byte1(*(MOD_KG_PARA_BASE_ADDR+0x0048))
#define   MOD_KG_now_poll_addr              byte0(*(MOD_KG_PARA_BASE_ADDR+0x0049))
#define   MOD_KG_transmit_flag              byte1(*(MOD_KG_PARA_BASE_ADDR+0x0049))
#define   MOD_KG_rec_OK                     byte0(*(MOD_KG_PARA_BASE_ADDR+0x004a))
#define   MOD_KG_begin_addr                 byte0(*(MOD_KG_PARA_BASE_ADDR+0x004b))
#define   MOD_KG_end_addr                   byte1(*(MOD_KG_PARA_BASE_ADDR+0x004b))
#define   MOD_KG_YK_fx_wait_time                 (*(MOD_KG_PARA_BASE_ADDR+0x004c))
#define   MOD_KG_YK_fx_flag                 byte0(*(MOD_KG_PARA_BASE_ADDR+0x004d))
#define   MOD_KG_YK_poll_addr               byte1(*(MOD_KG_PARA_BASE_ADDR+0x004d))
#define   MOD_KG_transmit_wait_time              (*(MOD_KG_PARA_BASE_ADDR+0x004e))
#define   MOD_KG_begin_zf_disable_time           (*(MOD_KG_PARA_BASE_ADDR+0x004f))
#define   MOD_KG_wait_replay                byte0(*(MOD_KG_PARA_BASE_ADDR+0x0050))
#define   MOD_KG_rec_frame_type             byte1(*(MOD_KG_PARA_BASE_ADDR+0x0050))



/*---------------------------------------------------------------------------*/
/*                        IMPORT            functions                        */
/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/
/************************************************/
/* MOD_KG_MOD_KG_CRC16_cal         function              */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
unsigned int MOD_KG_CRC16(unsigned char *MOD_KG_CRC16_start,unsigned char MOD_KG_CRC16_bytes)    //*x为指向每行前5个数据的指针
{	//-计算冗余校验CRC
unsigned int bx, cx, i, j;

    bx = 0xffff;
    cx = 0xa001;
    for(i=0;i<MOD_KG_CRC16_bytes;i++)
     {
      bx=bx^MOD_KG_CRC16_start[i];
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
/* MOD_KG_fcb_check         function              */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static BYTE MOD_KG_not_comm_count(void)
{
    BYTE  the_ram_axl;
    BYTE  the_ram_axh;
    the_ram_axl=port_report[0];
    the_ram_axh=port_send[MOD_KG_PORT_NO][0];	//-他们的0内容表示的什么呢???
    if(the_ram_axl!=the_ram_axh)
      return YES;
    else 
      return NO;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_KG_YX_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_KG_YX_cmd(BYTE slave_adr)  //-这个报文就是发送给终端装置的,并且还是从这个端口发送出去的
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MOD_KG_PORT_NO]);
    temp_ptS_B[0] =slave_adr;
    temp_ptS_B[1] =0x02;	//-功能码
    temp_ptS_B[2] =0x00; //YX起始地址
    temp_ptS_B[3] =0x80;
    temp_ptS_B[4] =0x00;  //访问YX区字数
    temp_ptS_B[5] =0x04;
    the_ram_ax=MOD_KG_CRC16(temp_ptS_B,6);
    temp_ptS_B[6] =byte0(the_ram_ax);
    temp_ptS_B[7] =byte1(the_ram_ax);	//-直到这里所有的报文内容都已经准备好了
    port_send_len[MOD_KG_PORT_NO]=8;    
    port_send_begin();	//-启动发送,但是仅仅可能是启动了一个中断,实际的全部发送是在中断中进行的
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_KG_scheduler      function               */
/************************************************/
//-切记没有什么严格意义上的固定,程序的执行是逻辑的结果,诱因才有果
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_KG_scheduler(void)	//-调度程序,就是处理字程序个人习惯问题
{
    #define    delta_len            temp_lp_int
    #define    bv_yx_no             port_check

    far  BYTE *temp_fptS;
         WORD  the_ram_ax;
         WORD  the_ram_bx;
         BYTE  temp_axl;
    
    temp_fptS=(far BYTE *)&(port_send[MOD_KG_PORT_NO]);	//-得发送口的地址,以便向里面写内容
      
    if((port_recv_pt[MOD_KG_PORT_NO]!=port_recv_dl[MOD_KG_PORT_NO])&&(MOD_KG_wait_replay==YES))	//-说明本身接收到了新的报文
    {	//-正常情况下,接收到应答报文,是从这里开始处理的
       if(MOD_KG_rxd_head_flag==NO)	//-接收到的数据还没有处理的时候就是NO
       {
           disable();
           if(port_recv_dl[MOD_KG_PORT_NO]>port_recv_pt[MOD_KG_PORT_NO])	//-前面的是处理指针比实际的接收指针进行比较
               delta_len=(port_recv_pt[MOD_KG_PORT_NO]+512)-port_recv_dl[MOD_KG_PORT_NO];
           else
               delta_len=port_recv_pt[MOD_KG_PORT_NO]-port_recv_dl[MOD_KG_PORT_NO];	//-一共的长度
           enable();
           for(temp_loop=port_recv_dl[MOD_KG_PORT_NO];temp_loop<(delta_len+port_recv_dl[MOD_KG_PORT_NO]);temp_loop++)
           {
        	   if(port_recv[MOD_KG_PORT_NO][temp_loop]==port_send[MOD_KG_PORT_NO][0])	//-这个地方比较的是从站地址,但是我觉得没有任何规律就是通讯
        	   {	//-利用一切可以利用的
        	     the_ram_ax=(port_recv_dl[temp_loop]+1)&0x1ff;
        	     if(port_recv[MOD_KG_PORT_NO][the_ram_ax]==port_send[MOD_KG_PORT_NO][1])	//-比较的是功能码
        	     {
        	         MOD_KG_rxd_head_flag=YES;	//-表示已经成功识别接收到的新报文的头了
        	         break;
        	     }
        	   }  
        	   port_recv_dl[MOD_KG_PORT_NO]++;	//-舍弃一个字的报文
        	   port_recv_dl[MOD_KG_PORT_NO]&=0x1ff;       	        	 
           }
       }
       if(MOD_KG_rxd_head_flag==YES)	//-果真是不一样的风格
       {
           disable();
           if(port_recv_dl[MOD_KG_PORT_NO]>port_recv_pt[MOD_KG_PORT_NO])
               delta_len=(port_recv_pt[MOD_KG_PORT_NO]+512)-port_recv_dl[MOD_KG_PORT_NO];
           else
               delta_len=port_recv_pt[MOD_KG_PORT_NO]-port_recv_dl[MOD_KG_PORT_NO];
           enable();
           if(delta_len>=3)
           {
               the_ram_ax=(port_recv_dl[MOD_KG_PORT_NO]+1)&0x1ff;
               if(port_recv[MOD_KG_PORT_NO][the_ram_ax]==port_send[MOD_KG_PORT_NO][1])	//-功能码
               {
                    temp_int=(port_recv_dl[MOD_KG_PORT_NO]+2)&0x1ff;
                	if(delta_len>=(unsigned short)(port_recv[MOD_KG_PORT_NO][temp_int]+5))	//-得到的报文长度和理论上的报文长度进行比较
                    {
                  	    MOD_KG_rxd_head_flag=NO;
                  	    MOD_KG_wait_replay=NO;
                  	    MOD_KG_rec_OK=YES;             
                  	    goto rec_ok_deal;	//-经过重重考核,到这里就认为是成功接收到一个返回报文了
                    }
               }
               else if(delta_len>=8)
               {	//-功能码不同怎么也认为是成功接收到了呢,难道是说,这个可能就是他准备接收的宁一种返回报文,功能码不同的处理地方
                  	MOD_KG_rxd_head_flag=NO;
                  	MOD_KG_wait_replay=NO;
                  	MOD_KG_rec_OK=YES;             
                  	goto rec_ok_deal;
               }            
           }
       }                      
    }         
    goto rxd_out_time;
rec_ok_deal: 
    if(MOD_KG_rec_OK==YES)	//-肯定是防止跑飞的.
    {	//-到这里就可以说明应答报文已经可以了
        MOD_KG_rec_OK=NO;	//-成功接收到的数据开始处理了之后,就恢复0
        MOD_KG_transmit_flag=YES;
        MOD_KG_transmit_wait_time=Time_1ms_Counter;	//-虽然上面说可以发送数据了,但是还是要延时一段时间,因为接收到的数据需要处理
        //-下面是对实际数据的处理,处理的是核心部分
        the_ram_bx=(port_recv_dl[MOD_KG_PORT_NO]+1)&0x1ff;;
        if(port_recv[MOD_KG_PORT_NO][the_ram_bx]!=0x05)	//-这个是对功能码的判断,功能码不同判断的依据也不一样
        {	//-这里是宁外一种处理现在可以不管
          	the_ram_ax=(port_recv_dl[MOD_KG_PORT_NO]+2)&0x1ff;
          	temp_int=port_recv[MOD_KG_PORT_NO][the_ram_ax]+5+port_recv_dl[MOD_KG_PORT_NO];        
          	for(temp_loop=port_recv_dl[MOD_KG_PORT_NO];temp_loop<temp_int;temp_loop++)	//-上面这样干的秘密就是保证定位到需要处理的报文字节
          	{	//-简单的不需要这样处理但是复杂的还是需要的,那么这样用了得话兼容性就会很好
                 if(temp_loop<=511)
           	       port_report[temp_loop-port_recv_dl[MOD_KG_PORT_NO]]=port_recv[MOD_KG_PORT_NO][temp_loop];
                 else
           	       port_report[temp_loop-port_recv_dl[MOD_KG_PORT_NO]]=port_recv[MOD_KG_PORT_NO][temp_loop-512];	//-难道是高速更新的缘故需要提前复制出来
          	}	//-或者还有一种可能性就是统一处理
         	port_recv_dl[MOD_KG_PORT_NO]+=delta_len;	//-这个地方就舍弃了这样的处理报文
         	port_recv_dl[MOD_KG_PORT_NO]&=0x1ff;
         	temp_int=MOD_KG_CRC16(&port_report[0],port_report[2]+3);
         	if((byte0(temp_int)!=port_report[port_report[2]+3])||(byte1(temp_int)!=port_report[port_report[2]+4]))	//-进行CRC检查
          		goto inrda;	//-到这里说明成功接收到的报文CRC校验没有通过
        }
        else
        {
         	temp_int=port_recv_dl[MOD_KG_PORT_NO]+8;
         	for(temp_loop=port_recv_dl[MOD_KG_PORT_NO];temp_loop<temp_int;temp_loop++)
            {
                 if(temp_loop<=511)
                      port_report[temp_loop-port_recv_dl[MOD_KG_PORT_NO]]=port_recv[MOD_KG_PORT_NO][temp_loop];
                 else
                      port_report[temp_loop-port_recv_dl[MOD_KG_PORT_NO]]=port_recv[MOD_KG_PORT_NO][temp_loop-512];
            }
            port_recv_dl[MOD_KG_PORT_NO]+=delta_len;	//-到这里就可以把接收缓冲区中的内容舍弃了,已经拷贝出来了
            port_recv_dl[MOD_KG_PORT_NO]&=0x1ff;
            temp_int=MOD_KG_CRC16(&port_report[0],6);
            if((byte0(temp_int)!=port_report[6])||(byte1(temp_int)!=port_report[7]))
                goto inrda;	//-校验和不正确直接舍弃,那么没有收到有效报文就直接进行下个装置吗???
        }
        
        if(MOD_KG_comm_err_counter[port_report[0]]!=0)
        {
            MOD_KG_comm_err_counter[port_report[0]]=0;	//-这里说明了必须是连续出错才有效.
            temp_int=port_report[0]+MOD_KG_begin_addr-byte0(port_info[MOD_KG_PORT_NO].scan_panel);	//-得该PORT所连设备所对应208内部数据库内的单元地址
            YX_State[IED_TX_STATE_START_WORD+temp_int/16]&=(0xffff-(0x0001<<(temp_int%16)));	//-站内所有设备(IED)的工况作为YX存储,,那个是预定义的遥信起始号
        }    //-0表示通讯正常
    }
    switch(MOD_KG_rec_frame_type)	//-这值是在发送报文的时候给的,指明是否是需要接收的报文形式
    {
          case MOD_KG_YX_FRAME:       //yx
                  Core_Src_Unit=port_report[0]+MOD_KG_begin_addr-byte0(port_info[MOD_KG_PORT_NO].scan_panel);	//-得该PORT所连设备所对应208内部数据库内的单元地址
                  Core_Src_Pt_B=(BYTE *)&port_deal_buf[MOD_KG_PORT_NO][0];	//-接收有接收缓冲区,报告有报告缓冲区,现在处理了还有处理缓冲区
                  Core_Src_Len=2*unit_info[Core_Src_Unit].yx_num;	//-每一个单元都有一个独立的数据库,数据库说白了就是属性,,说明了单位是字
                  for(temp_loop=0;temp_loop<unit_info[Core_Src_Unit].yx_num;temp_loop++)
                  {	//-上送上来的新遥信在报文里,下面还没有看,我猜是先把老得暂存,再判断处理,可能是可靠的处理目的
                      Core_Src_Pt_B[2*temp_loop]=byte0(YX_State[unit_info[Core_Src_Unit].yx_start+temp_loop]);	//-数据库中一个元素就是一个属性,只要定义不重叠就行
                      Core_Src_Pt_B[2*temp_loop+1]=byte1(YX_State[unit_info[Core_Src_Unit].yx_start+temp_loop]);
                  }
                  Core_Src_Pt_B=(BYTE *)&port_deal_buf[MOD_KG_PORT_NO][0];
                  //-赵达军很多东西,可能由于目的不同,不一定都是正确的,比如说上面
                  for(temp_loop=0;temp_loop<unit_info[Core_Src_Unit].yx_num;temp_loop++)
                  {
                      Core_Src_Pt_B[2*temp_loop]=port_report[temp_loop*2+5];	//-没有任何固定的东西,实现功能就行,,取有效信息
                      Core_Src_Pt_B[2*temp_loop+1]=port_report[temp_loop*2+5+1];	//-进一步去头只留需要处理的有效数据,这个地方直接舍弃了一个字的遥信量
                  }
                  core_update_YX();    
                  break;
          default:
             break;
    }
     //-发送报文超时没有收到返回消息,并不会从新初始化硬件,而仅仅是一个通讯错误记录而已,也就是说,我配置了下位机,但是没有
     //-把下位机挂接上去,仅仅是一个状态记录,没有得到新的数据,不会更新数据库而已,其它的并不收影响     
rxd_out_time:	//-执行到这里说明接收超时,或处理成功,反正现在可以继续发送了,,正常情况都会执行这里的,首先
    if(Judge_Time_In_MainLoop(MOD_KG_rxd_wait_time,MOD_KG_WAIT_TIME_VALUE)==YES)	//-正常通信的话是不应该超时的,若过时就认为出错,下面重新初始化
    {	//-当发送一个报文之后,超过等待回答时间之后,就可以从新发送一次
      	MOD_KG_rec_OK=NO;
      	MOD_KG_rxd_wait_time=Time_1ms_Counter; 
      	MOD_KG_transmit_flag=YES;	//-表示现在可以组织发送内容
      	MOD_KG_wait_replay=NO;	//-表示现在还没有等待回复
      	MOD_KG_transmit_wait_time=Time_1ms_Counter;
//      if(MOD_KG_not_comm_count()==YES)
//       {
       	MOD_KG_comm_err_counter[port_send[MOD_KG_PORT_NO][0]]++;	//-超时出错计数
        if(MOD_KG_comm_err_counter[port_send[MOD_KG_PORT_NO][0]]>3)
        {
           MOD_KG_comm_err_counter[port_send[MOD_KG_PORT_NO][0]]=0;
           temp_int=port_send[MOD_KG_PORT_NO][0]+MOD_KG_begin_addr-byte0(port_info[MOD_KG_PORT_NO].scan_panel);	//-得该PORT所连设备所对应208内部数据库内的单元地址
           YX_State[IED_TX_STATE_START_WORD+temp_int/16]|=(0x0001<<(temp_int%16));	//-难道这个遥信不是普通的YX,怎么出错也在这里记录,是不是思维定势了啊      	
       	}
//       }
    }
inrda:	//-CRC不通过也有可能走到这里
    if(MOD_KG_transmit_flag==YES)
    {	//-正常情况下这里属于问的开始地方
     	if(Judge_Time_In_MainLoop(MOD_KG_transmit_wait_time,80)==YES)	//-只有过了等待时间才会发送
           switch(MOD_KG_transmit_control)
           {
                case 1:
                     MOD_KG_rec_frame_type=MOD_KG_YX_FRAME;	//-表示需要接收的帧的类型是这个值,接收到这样的值才是正确的
                     MOD_KG_wait_replay=YES;
                     MOD_KG_YX_cmd(MOD_KG_now_poll_addr);	//-本规约定义的内容是,问答型的,这个端口上的所有装置都是接收查询
                     MOD_KG_now_poll_addr++;	//-从机地址
                     disable();
      	             port_recv_pt[MOD_KG_PORT_NO]=0;	//-这个小初始化是为了接收处理报文用的
   	             port_recv_dl[MOD_KG_PORT_NO]=0;
   	             enable();
                     MOD_KG_transmit_flag=NO;	//-现在开始不能发送报文了
                     MOD_KG_rxd_head_flag=NO;	//-为类同步准备的,但是并不是所有的东西都是那样
                     MOD_KG_rxd_wait_time=Time_1ms_Counter;	//-应答报文也是有时效性的
                     if(MOD_KG_now_poll_addr>byte1(port_info[MOD_KG_PORT_NO].scan_panel))	//-所管理设备的地址(或序号)范围 HIGH byte:end - LOW byte:start
                     {
                         MOD_KG_now_poll_addr=byte0(port_info[MOD_KG_PORT_NO].scan_panel);	//-对所有地址进行循环计数
                         MOD_KG_transmit_control=1;	//-所有的端口装置查询一遍之后,再换下一类报文查询
                     }              
                     break;
                default:              
                     break;
           }
    }

}


/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/
/************************************************/
/* MOD_KG_Init       function                   */
/************************************************/
/*===========================================================================*/
void MOD_KG_Init()
{
    BYTE temp_axl;        
    MOD_KG_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];

    MOD_KG_begin_veriiedclk_time=Time_2048ms_Counter;
    MOD_KG_rxd_wait_time=Time_1ms_Counter;
    MOD_KG_transmit_task_busy=NO;
    MOD_KG_wait_replay=NO;
    MOD_KG_begin_addr=byte0(port_info[MOD_KG_PORT_NO].mirror_unit); 
    MOD_KG_end_addr=byte1(port_info[MOD_KG_PORT_NO].mirror_unit); 
    MOD_KG_now_poll_addr=byte0(port_info[MOD_KG_PORT_NO].scan_panel);	//-初始化的时候取最小从机地址
    MOD_KG_rxd_head_flag=NO;
    MOD_KG_transmit_flag=YES;	//-这个值表示现在可以启动端口间传递信息命令了
    MOD_KG_transmit_wait_time=Time_1ms_Counter;
    MOD_KG_rec_OK=NO;
    MOD_KG_rec_frame_type=0;
    MOD_KG_YK_fx_flag=NO;
    MOD_KG_begin_zf_disable_time=Time_2048ms_Counter;
    disable();
    port_recv_pt[MOD_KG_PORT_NO]=0;
    port_recv_dl[MOD_KG_PORT_NO]=0;
    enable();
    MOD_KG_transmit_control=1;
    for(temp_axl=0;temp_axl<32;temp_axl++)
     MOD_KG_comm_err_counter[temp_axl]=0;   
}

/************************************************/
/* MOD_KG_Main     function                     */
/************************************************/
/*===========================================================================*/
void MOD_KG_Main()
{
    MOD_KG_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];	//-每一个端口都有自己的一个缓冲区

    MOD_KG_scheduler();  
    
    if(Judge_LongTime_In_MainLoop(MOD_KG_begin_zf_disable_time,15)==YES) // after initialed 1s, let ZF be enable.  
    {
        Portx_Poll_First[MOD_KG_PORT_NO]=YES;	//-表示口现在可以发送内容了,如果主动上送的话
    }
    if(Judge_LongTime_In_MainLoop(MOD_KG_transmit_disable_begin_time,30)==YES)
    {
        MOD_KG_transmit_task_busy=NO;	//-计时以判断状态
    }

}


/************************************************/
/* MOD_KG_Monitor     function                  */
/************************************************/
/*===========================================================================*/
void MOD_KG_Monitor()
{
  MOD_KG_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];	//-指针指向一个固定变量区域,这个文件里面的所有值就使用这个区域内的存储单元
  
}

