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


//-��˵:554�����ж�״̬��296�����ж�״̬��������,��������ĳ�������296Ϊ�����,���ڵļ����������
//-����554һֱ�����ж�״̬,����296��ִ���Լ�������,���������ж�״̬.296�����ⲿ�жϵ�������ʽ�ǳ���
//-�������ź�,��������296���ź�����ͱ���Ϊһ���½����ź�(�����˷���������),��Ҫ�����½����źžͱ���
//-�Ǵ��ж��г��ֵ�(��Ϊ�ߵ�ƽ�Ǳ�ʾ�����ж�״̬),��ôҲ����˵,��ʵ554������2���ж�һ������������Ч
//-�жϺͳ�ʱ�ж�+���������ּĴ���Ϊ���ж�---���������,554оƬ����������,Ҳ����������ʱ,һֱ�Ǵ���
//-�ж���Ч״̬��,��Ϊû���յ��������Խ��ճ�ʱ�ж�,û�з������ݷ��ͱ��ּĴ���Ϊ���ж�,������Щ�ж�ͬʱ
//-���ڲ�����δ��״̬,��������ź�,����������296���ж�,Ҫ��296�����ж�,����ʹ��554�жϽ���,�ٴγ����ж�
//-Ҳ����˵296����554���ж�ֻ�ᴦ����һ�ε�,����������Ч�׶ε�.�ж���Ч��ʱ���Ǵ��ڸߵ�ƽ��,����ֻҪ��һ��
//-�ж�����Ч��,��ô���Ǵ��ڵ͵�ƽ��,Ҳ����˵��ͨ�������ж�,����ͬʱ��Ч�Ż�����ߵ�ƽ,�����ж������Ͼ��ǵ�
//-�͵�ƽ
//-��ʱ�жϲ���ʱʱ��������,��ָ�Ƿ����ղŷ����Ľ����ж�һ��,����˵������һ���ַ�֮��,ȴû��������,�������
//-�ַ�����������ı���ͨ����д��������,�������û�з��������ַ�,��ô�Ǹ��������ܷ���ʲô��ʱ�жϵ�
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
  if(the_port_no==10) 	//-���������ڶ˿�Ӳ����ͬ,������Ҫ������ͬ
  	  m_wr_554->MCR=0x0b;
  port_send_pt[the_port_no] = 1; 	//-��ʾ�Ѿ�������һ������
  m_wr_554->THR = port_send[the_port_no][0];	//-���������ּĴ���,,��������Ŀ����ʲô,�������ж���,���Ǿ��Ƿ���һ�����ö�������
  //-����д����һ�����ͱ��ּĴ������ݵ�Ŀ����Ϊ�˼���ͱ��ּĴ������ж�,ֻ���������ٴγ��ֿյ������
  //-������ж��ź����,֮ǰ��Ȼ���ͱ��ּĴ���һֱΪ��,���ǲ���������ж���Ч�ź�
}
//-���еķ���ͳһ����������  
void port_send_begin()
{
    BYTE the_port_no;
    WORD the_ram_ax;
    WORD the_ram_bx;
    
    disable();
    port_send_begin_no_monitor(port_no);  //-�����������ĳ����ﲢû������ӳ��˿ڵ�����

////// for monitor port msg:  
    if((port_mirror[port_no] & 0x20)!=0)	//-�ĸ��˿���������,���ֵ�����ĸ�ֵ
    {
        the_port_no=port_mirror[port_no] & 0x0f;	//-����ط������˶˿�ӳ��,����˵�����������ͨ���ڵ�����,�ڷ���֮ǰ����ӳ�䵽��һ��ͨ������
        if(the_port_no<0x0c)	//-destin port_no   0-11 P554; 12 CAN0; 13 CAN1; 14 cpucom;
        {
            the_ram_bx=port_send_len[port_no];
            for(the_ram_ax=0;the_ram_ax<the_ram_bx;the_ram_ax++)
            {
                port_send[the_port_no][the_ram_ax]=port_send[port_no][the_ram_ax];	//-�����Ƕ�Ӧ�˿�׼�����͵�����,ǰ����ӳ�䷢�͵�����
            }
            port_send_len[the_port_no]=port_send_len[port_no];
            port_send_begin_no_monitor(the_port_no);
        }
        else
        {
        	if(the_port_no==0x0e)	//-��ȱ�ʾCPUͨ��,���߽������ڲ���,���������������
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
void init_port_not_clr_ptr()	//-�����Ƕ�554�ĳ�ʼ��
{
    far  BYTE *p554_base_addr;
         BYTE  ram_axl;
         BYTE  ram_axh;


    if(port_no>11) return;

    p554_base_addr=(far  BYTE *)Register_554[port_no];

//     initial   FCR
    p554_base_addr[P554_FCR]=0x41;        // receive trigger at 4 bytes ,,��ʵ���������д����,����������ռ�д����
                                          // enable FIFO
//     initial   LCR
    ram_axh=(byte0(port_info[port_no].bits)-5) & 0x03;	//-ͨ��λ��
    if(byte0(port_info[port_no].stopbit)==1)	//-ֹͣλ��
    {
        //ram_axl=ram_axl & 0xfb;
    }
    else
    {
        ram_axh=ram_axh+0x04;
    }

    if(byte0(port_info[port_no].semiduplex_parity)==PARITY_ODD)	//-��żУ��
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
    
    p554_base_addr[P554_LCR]=ram_axh | 0x80;	//-��λ���뱻��1,�Ա��ڶ���д�������ʲ����ʷ�����
    
//     initial   BAUD
    p554_base_addr[P554_DLL]=byte0(BaudRate[port_info[port_no].bauds]);	//-���������˲�����,���Ҽ������ù���
    p554_base_addr[P554_DLM]=byte1(BaudRate[port_info[port_no].bauds]);
    
//     reset    LCR
    p554_base_addr[P554_LCR]=ram_axh;	//-λ7���뱻����,�Ա���ʽ��ջ���Ĵ���,���ͱ��ּĴ���,�ж�ʹ�ܼĴ���
    ram_axh=p554_base_addr[P554_RBR];	//-�����ֻ���Ĵ�����Ŀ����ʲô,����������Ĵ���,�������Ĵ�����ֵ
//     initial  IER
    p554_base_addr[P554_IER]=0x03;   //-IEO0 ��FIFOģʽʱʹ�ܽ���������Ч�жϺͳ�ʱ�ж�  IEO1 ���������ּĴ���Ϊ���ж� 
//     initial  MCR
//    if(port_no==0x0b)
//        p554_base_addr[P554_MCR]=0x08;//0x0a;   // disable trans
//    else
//        p554_base_addr[P554_MCR]=0x0a;//0x08;   // disable trans
    if(port_no==10)
    	 p554_base_addr[P554_MCR]=0x0b;		//-��Щ������Ȼ������,���ǲ���һ���Ͷ�ʹ����
    else
    {
    	if(port_no==7)
    	   p554_base_addr[P554_MCR]=0x0a;
    	else
    	   p554_base_addr[P554_MCR]=0x08;	//-ʹ�ܽ���
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
        the_ram_axl=p554_base_addr[P554_LSR];	//-��·״̬�Ĵ���
        the_ram_bx=0x0001;
        the_ram_bx=the_ram_bx<<port_no;
        if((the_ram_axl & 0x0c)!=0)	//-�ж�λ 2 3��ֵ,,����Ƿ�������,�еĻ�����Ҫ��λ����
        {
            SIO_CAN_Need_Reset=SIO_CAN_Need_Reset | the_ram_bx;	//-��¼��Ҫ��λ��ͨ����
//            if(port_no==7) (*((far BYTE *)(SOFT_ERR_FLAG+0x0000)))=the_ram_axl;
        }
        
        if((SIO_CAN_Need_Reset & the_ram_bx)!=0)	//-������0˵����Ҫ��λ����
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
//ȱӲ����λ
  for(port_no=0;port_no<12;port_no++)
  {
     init_port();
     port_mon[port_no] = 0;
  }
}

    