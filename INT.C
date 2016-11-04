/*****************************************************************************/
/*       FileName  :   INT.C                                                 */
/*       Content   :   DSA-208 INT Module                                    */
/*       Date      :   Fri  02-22-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/
//-代码兼容intel 80C196 
//-编译器的语法要求
//-#pragma interrupt( )表示中断服务子程序
//-我觉得作为一个系统而言,并没有什么既定步骤,而是周而复始的执行需要的处理程序,然后
//-根据实际情况进行判断,以"智能"逻辑判断程序进程.

//-赵达军在这样的系统中你就不要再想解密甚至什么突破了,这是效率低下的行为,你需要的是
//-灵活的应用这样的系统,把他的能力最大优化,但是必须能够提炼出自己需要的内容,放到自己
//-的系统中,绝不能被别人卡住




#pragma  regconserve

#include _SFR_H_
#include _FUNCS_H_

#include "common.h"
#include "device.h"
#include "reg.h"
#include "ram.h"
#include "rom.h"
#include "comuse.h"


//-现在可以想象,中断是这样的,首先通过寄存器配置中断,然后特定信号就会触发对应中断
//-程序就会进入中断服务子程序进行处理,然后就是中断返回
//-现在的问题就是我是怎样把中断地址放入对应向量表的?
//-现在没有资料,姑且认为下面就是实现这个功能的程序段
#pragma  interrupt(NMI_int        =0x1f)    /*  NMI        for None     int  */
#pragma  interrupt(port_IL_intr   =0x1e)    /*  EXINT3     for 554_2    int  */	//-本中断是如何触发的
#pragma  interrupt(port_EH_intr   =0x1d)    /*  EXINT2     for 554_1    int  */
#pragma  interrupt(EPAOvr2_3_int  =0x1c)    /*  EPAOvr2_3  for None     int  */
#pragma  interrupt(EPAOvr0_1_int  =0x1b)    /*  EPAOvr0_1  for None     int  */
#pragma  interrupt(EPA3_int       =0x1a)    /*  EPA3       for None     int  */
#pragma  interrupt(EPA2_int       =0x19)    /*  EPA2       for None     int  */
#pragma  interrupt(soft1ms_int    =0x18)    /*  EPA1       for soft_1ms int  */
#pragma  interrupt(port_AD_intr   =0x07)    /*  EPA0       for 554_0    int  */
#pragma  interrupt(SIO_int        =0x06)    /*  SIORec     for SIORec   int  */
#pragma  interrupt(SIO_int        =0x05)    /*  SIOTran    for SIOTran  int  */
//#pragma  interrupt(exint1_int     =0x04)    /*  EXINT1     for CAN_1    int  */
//#pragma  interrupt(exint0_int     =0x03)    /*  EXINT0     for CAN_0    int  */
#pragma  interrupt(reserved_int   =0x02)    /*  reserve    for None     int  */
#pragma  interrupt(T2OverFlow_int =0x01)    /*  T2Ovr      for None     int  */
#pragma  interrupt(T1OverFlow_int =0x00)    /*  T1Ovr      for None     int  */

//-本系统中断总结:三个554各触发一个中断(从外部),1ms的中断不知道是怎么触发的(定时器1计时触发),还有一个SIO接收发送中断
//-总共是开放了6个中断,特殊的串口接收和发送中断是和液晶板上单片机通讯用的.





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

extern void set_receive();
extern void set_send();






//-接收中断已经可以解释了,就是在没有收到字符的情况下是一直不会发生中断的,当收到有效
//-字符或者无效字符(需要舍弃)时才会发生中断
/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/

/***********************************************/
/* P554_int_proc      procedure                */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void P554_int_proc(BYTE the_port_no)
{
         BYTE  ram_axl;
         BYTE  ram_axh;
         BYTE  the_port_no_mirr;
         BYTE  the_port_rece_temp;
    far  BYTE *p554_base_addr;
    far  BYTE *the_ram_base_addr;
    
    //-所有的这些东西,肯定是符合常识的,至少是最优的,虽然可能不一定就是我想的那样
    //-但是肯定不是天马行空,更不会是无法解释的,他被创造出来的目的就是为了使用
    //-现在我对于发送保持寄存器为空的解释是这样的(可能怎么实现的我说不清楚,但是应用的
    //-目的之一肯定有这种可能性存在)就是当出现了保持寄存器为空中断之后,当这个中断被
    //-清除后,要想中断再次被悬挂的前提条件就是对必须向发送保持寄存器写过至少一个数据
    //-否则何谈中断被清除呢,
    p554_base_addr = (far  BYTE *)Register_554[the_port_no];
    ram_axh = p554_base_addr[P554_IIR];	//-中断识别寄存器,,这次读操作清除了发送保持寄存器中断,要想再次出现,必须至少向THR写1个字符

	//-IIR0 被清零时说明中断处于未决状态
	//-IIR1	确定最高优先级的未决中断
	//-IIR2
    if((ram_axh & 0x07)==0x04)     // receive int
    {
        ram_axh=0;
        ram_axl=p554_base_addr[P554_LSR];	//-***切忌只要有一点可能性就不要再去看中文数据手册了,除非中文是原版
        if((port_mirror[the_port_no] & 0x10)!=0)
        {
            the_port_no_mirr=port_mirror[the_port_no] & 0x0f;	//-目的何在
        }
        else
        {
            the_port_no_mirr=0xff;
        }

        while((ram_axl & 0x01)!=0)	//-值为1说明数据接收到了,CPU读后自动清零
        {
            the_port_rece_temp=p554_base_addr[P554_RBR];	//-器件接收到的数值被读出来了,这就是接收单个数据处
            
            port_recv[the_port_no][port_recv_pt[the_port_no]] = the_port_rece_temp;	//-实际接收的内容保存在这个里面
            port_recv_pt[the_port_no]++;	//-成功接收一个字节的内容,这个值就加一个
            port_recv_pt[the_port_no] &= 0x1ff;
            //-现在的简单程序下面的是没有处理的,直接跳过的	
            if(the_port_no_mirr<0x0c)
            {
                port_send[the_port_no_mirr][ram_axh]=the_port_rece_temp;
                ram_axh++;
            }
            else
            {
            	if(the_port_no_mirr==0x0e)
            	{
            		set_port_rece_send_buf[set_port_rs_buf_sav_pt]=the_port_rece_temp;
            		set_port_rs_buf_sav_pt++;
            		if(set_port_rs_buf_sav_pt==set_port_rs_buf_tak_pt) set_port_rs_buf_tak_pt++;
            	}
            }
            
            if((ram_axl & 0x0c)!=0)  // not no parity error, frame error;
            {
                //save to rcd_info_myself ???
                SIO_CAN_Need_Reset=SIO_CAN_Need_Reset | (0x0001<<the_port_no);   // bit x
            }
    
            ram_axl=p554_base_addr[P554_LSR];	//-继续读状态看是否还有数据需要接收,这是一个连续读得过程
        }
            
        if(ram_axh!=0)
        {
            port_send_len[the_port_no_mirr]=ram_axh;
            port_send_begin_no_monitor(the_port_no_mirr);
        }
    }
    else
    {
        if((ram_axh & 0x07)==0x02) // transmit int
        {
            if(port_send_pt[the_port_no]<port_send_len[the_port_no])	//-小于说明有数据需要发送
            {
                the_ram_base_addr=&port_send[the_port_no][port_send_pt[the_port_no]];
                if( (port_send_pt[the_port_no]+8)<port_send_len[the_port_no] )
                {	//-表示至少可以一次性发送8个字节
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[0];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[1];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[2];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[3];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[4];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[5];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[6];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[7];
           
                    port_send_pt[the_port_no]=port_send_pt[the_port_no]+8;
                }
                else
                {
                    do
                    {
                        p554_base_addr[P554_THR]=the_ram_base_addr[0];	//-发送器保持寄存器,,有些东西,现在已经不用说明了,也许就说不明白,这样就对了
                        the_ram_base_addr++;
                        port_send_pt[the_port_no]++;
                    }
                    while(port_send_pt[the_port_no]<port_send_len[the_port_no]);
                }
            }
            else
            {
/*                if(the_port_no==0x0b)
                {
                    p554_base_addr[P554_MCR]=0x08;//0x0a;
                }
                else
                {*/
                    P554_Port_Tran_Close_RTS_Cn[the_port_no]=Time_1ms_Counter
                                                            +P554_XTAL16M_CLOSE_RTS_DELAY[port_info[the_port_no].bauds];
                    P554_Port_Transing_Lastbyte[the_port_no]=YES;
//                }    
                port_mon[the_port_no]=0;
            }
        }
    }
    
}


/*---------------------------------------------------------------------------*/
/*                       PUBLIC             functions                        */
/*---------------------------------------------------------------------------*/

/***********************************************/
/* interrupt NMI     for none   svr procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void NMI_int(void)
{
    // do nothing
}



/***********************************************/
/* interrupt EXINT3  for 554_2  svr procedure  */
/***********************************************/
//-同样的驱动芯片,但是外引的信号线是不一样的,这样他们实现的功能也是不同的
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void port_IL_intr(void)
{
    BYTE  port_noiii;

    RAM_CPU_Int_Moni.EXINT3_INT_DEAD_COUNTER=0;
    
re_check_int_com2:
    for(port_noiii=8;port_noiii<12;port_noiii++)
    {
        P554_int_proc(port_noiii);	//-每个554芯片是4个通道,一起启动发送
    }

    port_noiii=p3_pin;
    if((port_noiii & 0x80)==0x00) //-针对出现中断的情况,结束中断服务的条件就是中断消失了
    	goto re_check_int_com2;
    port_noiii=p3_pin;
    if((port_noiii & 0x80)==0x00) 
    	goto re_check_int_com2;

}



/***********************************************/
/* interrupt EXINT2  for 554_1  svr procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void port_EH_intr(void)
{
    BYTE  port_noii;

    RAM_CPU_Int_Moni.EXINT2_INT_DEAD_COUNTER=0;
    
re_check_int_com1:
    for(port_noii=4;port_noii<8;port_noii++)
    {
        P554_int_proc(port_noii);
    }

    port_noii=p3_pin;
    if((port_noii & 0x40)==0x00) 
    	goto re_check_int_com1;
    port_noii=p3_pin;
    if((port_noii & 0x40)==0x00) 
    	goto re_check_int_com1;

}



/***********************************************/
/* interrupt EPAOvr2_3 for none svr procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void EPAOvr2_3_int(void)
{
    // do nothing
}



/***********************************************/
/* interrupt EPAOvr0_1 for none svr procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void EPAOvr0_1_int(void)
{
    // do nothing
}



/***********************************************/
/* interrupt EPA3  for none    svr  procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void EPA3_int(void)
{
    // do nothing
}



/***********************************************/
/* interrupt EPA2  for none    svr  procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void EPA2_int(void)
{
    // do nothing
}



/***********************************************/
/* interrupt soft1ms_int svr procedure         */
/***********************************************/
//-这个中断是怎么触发的,是外部信号,还是内部软件捕捉?
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void soft1ms_int(void)   // EPA1
{
    register WORD  reg_ibx;
    far BYTE *ram_base_addr;


//  process 1ms run
    reg_ibx=REG_TimeTemp+EPA1_PERIOD;

    REG_TimeTemp=timer1;

    epa1_time=timer1+EPA1_PERIOD;	//-设置下次中断的时间

    REG_Surplus_Time=REG_Surplus_Time+REG_TimeTemp-reg_ibx;

    while(REG_Surplus_Time>=EPA1_PERIOD)
    {
        REG_Surplus_Time=REG_Surplus_Time-EPA1_PERIOD;
        REG_1Msecond++;
    }
  
    REG_1Msecond++;

    Clock_Process();
    
    Time_1ms_Counter++;
    
    if((Time_1ms_Counter & 0x7ff)==0) Time_2048ms_Counter++;
    
//  process CAN transmit

//  process 554 transmit
    for(reg_ibx=0;reg_ibx<12;reg_ibx++)
    {
        //if(Port_Info[reg_ibx].PORT_SEMIDUPLEX==YES)
        {
            if(P554_Port_Transing_Lastbyte[reg_ibx]==YES)
            {
                if(Time_1ms_Counter==P554_Port_Tran_Close_RTS_Cn[reg_ibx])
                {
                    port_send_pt[reg_ibx]++;
                    ram_base_addr=(far BYTE *)Register_554[reg_ibx];
                    
//                    if(reg_ibx==0x0b)
//                    {
//                        ram_base_addr[P554_MCR]=0x08;//0x0a;
//                    }
//                    else
//                    {
//                        ram_base_addr[P554_MCR]=0x0a;//0x08;          // disable txd, enable rxd.
//                    }   
                    if(reg_ibx!=10)
                    	ram_base_addr[P554_MCR]=0x08;	//-外部串行通道中断被使能,使485处于接收状态,检查到没有发送的时候
                    if(reg_ibx==7)
                    	ram_base_addr[P554_MCR]=0x0a;
                    P554_Port_Transing_Lastbyte[reg_ibx]=NO;
                }
            }
        }
    }
    
    
//  process wachdog 
    RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER=0;
    
    RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER++;
    if(RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER>50000)  // 50s
    {
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_MAINLOOP;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();
        
        RAM_CPU_INT_Rst_Cn[6]++;
        while(1) {};
    }
}


/***********************************************/
/* interrupt EPA0  for 554_0   svr  procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void port_AD_intr(void)
{
    BYTE  port_noi;
    WORD  ram_ax;


    ram_ax=epa_time0;
    ram_ax=epa_time0;

    RAM_CPU_Int_Moni.EPA0_INT_DEAD_COUNTER=0;
    
re_check_int_com0:
    for(port_noi=0;port_noi<4;port_noi++)
    {
        P554_int_proc(port_noi);
    }

    port_noi=p1_pin;
    if((port_noi & 0x01)==0x00) 
    	goto re_check_int_com0;	//-为什么非要处于高电平才能结束中断处理呢?
    port_noi=p1_pin;
    if((port_noi & 0x01)==0x00) //-也就是554没有一个中断信号有效,全为0
    	goto re_check_int_com0;

}



/***********************************************/
/* interrupt SIOReceive   svr       procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//void SIOReceive_int(void)
//{
    
//}



/***********************************************/
/* interrupt SIOTransmit  svr       procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//void SIOTransmit_int(void)
//{

//}



/***********************************************/
/* interrupt SIO          svr       procedure  */
/***********************************************/
//-这里也是极其不一样的地方,一般仅仅使用一个中断,顶多使用两个中断处理所有的问题
//-但是这里使用的中断,却是五花八门,不仅量多而且种类多,有外部中断,有捕捉中断,有内部等等
//-可能也许这个就根本就是一个扩充,甚至是误导呢
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void SIO_int(void)	//-这个串口设置的目的可能就是为了下载程序使用的
{
    unsigned char sp_sta_img;

    sp_sta_img = sp_status;
    if((sp_sta_img & 0x40)!=0)     //RI
    {
        set_receive();
    }
    //else
    if((sp_sta_img & 0x20)!=0)  //TI
    {
        set_send(); 
    }
}




/***********************************************/
/* interrupt reserved     svr       procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void reserved_int(void)
{
    // do nothing
}




/***********************************************/
/* interrupt T2OverFlow   svr       procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void T2OverFlow_int(void)
{
    // do nothing
}



/***********************************************/
/* interrupt T1OverFlow   svr       procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void T1OverFlow_int(void)
{
    // do nothing
}









/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/

//    none

