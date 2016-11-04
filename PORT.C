/*****************************************************************************/
/*       FileName  :   PORT.C                                                */
/*       Content   :   DSA-208 PORT Module                                   */
/*       Date      :   Fri  02-22-2002                                       */
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



/*---------------------------------------------------------------------------*/
/*                    Definition  of  global  variables                      */
/*---------------------------------------------------------------------------*/

//    none


/*---------------------------------------------------------------------------*/
/*                    Definition  of  local  variables                       */
/*---------------------------------------------------------------------------*/

//    none




/*---------------------------------------------------------------------------*/
/*                        IMPORT            functions                        */
/*---------------------------------------------------------------------------*/
extern void set_init_reset();

/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/






/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/


//-假说:554处于中断状态和296处于中断状态是两会事,对于这里的程序是以296为主体的,现在的假设就是这样
//-可能554一直处于中断状态,但是296在执行自己的事情,并不是在中断状态.296对于外部中断的启动方式是出现
//-上升沿信号,这样对于296的信号输出就必须为一个下降沿信号(经过了反向驱动器),而要出现下降沿信号就必须
//-是从中断中出现的(因为高电平是表示处于中断状态),那么也就是说,其实554开设了2个中断一个接收数据有效
//-中断和超时中断+发送器保持寄存器为空中断---正常情况下,554芯片不接收内容,也不发送内容时,一直是处于
//-中断有效状态的,因为没有收到内容所以接收超时中断,没有发送内容发送保持寄存器为空中断,而且这些中断同时
//-存在并处于未决状态,但是这个信号,并不会引起296的中断,要想296出现中断,必须使得554中断结束,再次出现中断
//-也就是说296处理554的中断只会处理第一次的,而是整个有效阶段的.中断有效的时候是处于高电平的,但是只要有一个
//-中断是无效的,那么就是处于低电平的,也就是说开通的两个中断,必须同时有效才会输出高点平,否则中断引脚上就是低
//-低电平
//-超时中断不是时时都发生的,是指非法接收才发生的接收中断一种,比如说接收了一个字符之后,却没有了下文,这样这个
//-字符就是无意义的必须通过读写把他舍弃,如果根本没有发生接收字符,那么是根本不可能发生什么超时中断的
//  for later running in practise only need rename 'port_send_begin_no_monitor' as 'port_send_begin'
void port_send_begin_no_monitor(BYTE the_port_no)
{
  struct WR_REG_16554 *m_wr_554;

  m_wr_554 = (struct WR_REG_16554 *)Register_554[the_port_no];
//  if((port_info[port_no].semiduplex_parity >> 8)==0xaa)
  {
//     if(the_port_no==0x0b)
//       m_wr_554->MCR = 0x0a;//0x08;
//     else 
//       m_wr_554->MCR = 0x08;//0x0a;
      m_wr_554->MCR = 0x0a;
      if(the_port_no==7)
      	  m_wr_554->MCR = 0x08;
  }
  if(the_port_no==10) 	//-可能是由于端口硬件不同,所以需要处理不同
  	  m_wr_554->MCR=0x0b;
  port_send_pt[the_port_no] = 1; 	//-表示已经发送了一个数据
  m_wr_554->THR = port_send[the_port_no][0];	//-发送器保持寄存器,,这样做的目的是什么,是启动中断吗,还是就是发送一个无用东西而已
  //-上面写这样一个发送保持寄存器数据的目的是为了激活发送保持寄存器空中断,只有这样当再次出现空的情况才
  //-会产生中断信号输出,之前虽然发送保持寄存器一直为空,但是并不会出现中断有效信号
}
//-所有的发送统一由这里启动  
void port_send_begin()
{
    BYTE the_port_no;
    WORD the_ram_ax;
    WORD the_ram_bx;
    
    disable();
    port_send_begin_no_monitor(port_no);  //-在这个精简过的程序里并没有启动映射端口的作用

////// for monitor port msg:  
    if((port_mirror[port_no] & 0x20)!=0)	//-哪个端口启动发送,这个值就是哪个值
    {
        the_port_no=port_mirror[port_no] & 0x0f;	//-这个地方进行了端口映射,就是说我在这个发送通道内的内容,在发送之前可以映射到宁一个通道发送
        if(the_port_no<0x0c)	//-destin port_no   0-11 P554; 12 CAN0; 13 CAN1; 14 cpucom;
        {
            the_ram_bx=port_send_len[port_no];
            for(the_ram_ax=0;the_ram_ax<the_ram_bx;the_ram_ax++)
            {
                port_send[the_port_no][the_ram_ax]=port_send[port_no][the_ram_ax];	//-后面是对应端口准备发送的数据,前面是映射发送的数据
            }
            port_send_len[the_port_no]=port_send_len[port_no];
            port_send_begin_no_monitor(the_port_no);
        }
        else
        {
        	if(the_port_no==0x0e)	//-相等表示CPU通道,或者仅仅是内部的,甚至仅仅是虚拟的
        	{
        		the_ram_bx=port_send_len[port_no];
        		the_ram_ax=0;
        		while(the_ram_bx>0)
        		{
        			set_port_rece_send_buf[set_port_rs_buf_sav_pt]=port_send[port_no][the_ram_ax];
        			the_ram_ax++;
        			the_ram_bx--;
        			set_port_rs_buf_sav_pt++;
        			if(set_port_rs_buf_sav_pt==set_port_rs_buf_tak_pt) set_port_rs_buf_tak_pt++;
        		}
        	}
        }
    }
    enable();
}


/************************************************/
/* init_port_not_clr_ptr   function        DI   */
/************************************************/
/*===========================================================================*/
void init_port_not_clr_ptr()	//-这里是对554的初始化
{
    far  BYTE *p554_base_addr;
         BYTE  ram_axl;
         BYTE  ram_axh;


    if(port_no>11) return;

    p554_base_addr=(far  BYTE *)Register_554[port_no];

//     initial   FCR
    p554_base_addr[P554_FCR]=0x41;        // receive trigger at 4 bytes ,,其实向这个数组写数据,就是向这个空间写数据
                                          // enable FIFO
//     initial   LCR
    ram_axh=(byte0(port_info[port_no].bits)-5) & 0x03;	//-通信位数
    if(byte0(port_info[port_no].stopbit)==1)	//-停止位数
    {
        //ram_axl=ram_axl & 0xfb;
    }
    else
    {
        ram_axh=ram_axh+0x04;
    }

    if(byte0(port_info[port_no].semiduplex_parity)==PARITY_ODD)	//-奇偶校验
    {
        ram_axh=ram_axh+0x08; 
    }
    else
    {
        if(byte0(port_info[port_no].semiduplex_parity)==PARITY_EVEN)
        {
            ram_axh=ram_axh+0x18; 
        }
    }
    
    p554_base_addr[P554_LCR]=ram_axh | 0x80;	//-该位必须被置1,以便在读或写器件访问波特率发生器
    
//     initial   BAUD
    p554_base_addr[P554_DLL]=byte0(BaudRate[port_info[port_no].bauds]);	//-这里设置了波特率,而且简化了配置过程
    p554_base_addr[P554_DLM]=byte1(BaudRate[port_info[port_no].bauds]);
    
//     reset    LCR
    p554_base_addr[P554_LCR]=ram_axh;	//-位7必须被清零,以便访问接收缓冲寄存器,发送保持寄存器,中断使能寄存器
    ram_axh=p554_base_addr[P554_RBR];	//-读这个只读寄存器的目的是什么,接收器缓冲寄存器,清楚这个寄存器的值
//     initial  IER
    p554_base_addr[P554_IER]=0x03;   //-IEO0 在FIFO模式时使能接收数据有效中断和超时中断  IEO1 发送器保持寄存器为空中断 
//     initial  MCR
//    if(port_no==0x0b)
//        p554_base_addr[P554_MCR]=0x08;//0x0a;   // disable trans
//    else
//        p554_base_addr[P554_MCR]=0x0a;//0x08;   // disable trans
    if(port_no==10)
    	 p554_base_addr[P554_MCR]=0x0b;		//-有些东西虽然配置了,但是并不一定就都使用了
    else
    {
    	if(port_no==7)
    	   p554_base_addr[P554_MCR]=0x0a;
    	else
    	   p554_base_addr[P554_MCR]=0x08;	//-使能接收
    }
    if(Cn_SIO_CAN_Reset[port_no]<0xffff)   
    	Cn_SIO_CAN_Reset[port_no]++;
}


/************************************************/
/* init_port   function  Must DI before call it */
/************************************************/
/*===========================================================================*/
void init_port()
{
    init_port_not_clr_ptr();
    
    port_recv_pt[port_no] = 0;
    port_recv_dl[port_no] = 0;
    port_send_pt[port_no] = 0;
    port_send_len[port_no]= 0;
}

/************************************************/
/* Initial_CAN function                         */
/************************************************/
/*===========================================================================*/
/*void Initial_CAN(BYTE the_port_no)
{
    far BYTE *can_base_addr;
        BYTE ram_iaxl;

    if(the_port_no>1) return;
    
    can_base_addr=(far BYTE *)CAN_BaseAddr+(unsigned long)the_port_no*CAN_AddrDistance;
    *(far BYTE *)(can_base_addr+CAN_MODE     ) =0x01;
    *(far BYTE *)(can_base_addr+CAN_CLKDIVIDE) =0x88;

    *(far BYTE *)(can_base_addr+CAN_INTEN    ) =0x0d;   // open overrun_int, error_warn_int,
                                                        //      TX_int     , RX_int         

    ram_iaxl=port_info[PORT_NO_CAN_0+the_port_no].bauds;
    *(far BYTE *)(can_base_addr+CAN_BUST0    ) =byte0(CAN_BUST_XTAL16M[ram_iaxl]);
    *(far BYTE *)(can_base_addr+CAN_BUST1    ) =byte1(CAN_BUST_XTAL16M[ram_iaxl]);
    *(far BYTE *)(can_base_addr+CAN_OUTCONT  ) =0xda;

    *(far BYTE *)(can_base_addr+CAN_ACCCODE0 ) =0x00;
    *(far BYTE *)(can_base_addr+CAN_ACCMASK0 ) =0x00;

    *(far BYTE *)(can_base_addr+CAN_ACCCODE1 ) =0x00;
    *(far BYTE *)(can_base_addr+CAN_ACCMASK1 ) =0xff;

    *(far BYTE *)(can_base_addr+CAN_ACCCODE2 ) =0xff;
    *(far BYTE *)(can_base_addr+CAN_ACCMASK2 ) =0xff;    // to receive all, set it with 0xff

    *(far BYTE *)(can_base_addr+CAN_ACCCODE3 ) =0x00;
    *(far BYTE *)(can_base_addr+CAN_ACCMASK3 ) =0xff;

    *(far BYTE *)(can_base_addr+CAN_MODE     ) =0x00;

    ram_iaxl =*(far BYTE *)(can_base_addr+CAN_STATUS);
    ram_iaxl =*(far BYTE *)(can_base_addr+CAN_INTSTA);
    
    if(Cn_SIO_CAN_Reset[the_port_no+PORT_NO_CAN_0]<0xffff)   Cn_SIO_CAN_Reset[the_port_no+PORT_NO_CAN_0]++;
}
*/

/***********************************************/
/*   Judge_P554_CAN_Reset                      */
/***********************************************/  
/*===========================================================================*/
void Judge_P554_CAN_Reset(void)
{
//         BYTE the_port_no;
         BYTE the_ram_axl;
         WORD the_ram_bx;
    far  BYTE *p554_base_addr;
    
    for(port_no=0;port_no<12;port_no++)
    {
        p554_base_addr=(far BYTE *)Register_554[port_no];
        the_ram_axl=p554_base_addr[P554_LSR];	//-线路状态寄存器
        the_ram_bx=0x0001;
        the_ram_bx=the_ram_bx<<port_no;
        if((the_ram_axl & 0x0c)!=0)	//-判断位 2 3的值,,检查是否发生错误,有的话就需要复位处理
        {
            SIO_CAN_Need_Reset=SIO_CAN_Need_Reset | the_ram_bx;	//-记录需要复位的通道号
//            if(port_no==7) (*((far BYTE *)(SOFT_ERR_FLAG+0x0000)))=the_ram_axl;
        }
        
        if((SIO_CAN_Need_Reset & the_ram_bx)!=0)	//-不等于0说明需要复位处理
        {
            disable();
            //port_no=the_port_no;
            init_port_not_clr_ptr();
            enable();
            the_ram_bx=~the_ram_bx;
            SIO_CAN_Need_Reset=SIO_CAN_Need_Reset & the_ram_bx;
        }
        
    }
        
    // to avoid be can't syn for a long period,while receiving error codes,
    //    Here add a Timer to Monitor receive msg Time. at 04/26/02pm 
////    for(port_no=0;port_no<12;port_no++)
////    {
        ///if(P554_Port_ReceBuf_WaitHalfMsg[port_no]==YES)
////        {
            // later write.  accord baudrate, judge_time then init receive position.
////        }
////    }    


/*    p554_base_addr=(far BYTE *)CAN0_BaseAddr;
    if(((SIO_CAN_Need_Reset & 0x1000)!=0) || ((p554_base_addr[CAN_MODE] & 0x01)!=0) || ((p554_base_addr[CAN_STATUS] & 0x80)!=0))
    {
        disable();
//        Initial_CAN(0);
        enable();
        SIO_CAN_Need_Reset=SIO_CAN_Need_Reset & 0xefff;
    }
    
    p554_base_addr=(far BYTE *)CAN1_BaseAddr;
    if(((SIO_CAN_Need_Reset & 0x2000)!=0) || ((p554_base_addr[CAN_MODE] & 0x01)!=0) || ((p554_base_addr[CAN_STATUS] & 0x80)!=0))
    {
        disable();
        Initial_CAN(1);
        enable();
        SIO_CAN_Need_Reset=SIO_CAN_Need_Reset & 0xdfff;
    }
*/    
}


/***********************************************/
/*   init_all_port  function                   */
/***********************************************/  
/*===========================================================================*/
void init_all_port(void)
{
//缺硬件复位
  for(port_no=0;port_no<12;port_no++)
  {
     init_port();
     port_mon[port_no] = 0;
  }
}

    