/*****************************************************************************/
/*       FileName  :   MOD_JYDK.C                                              */
/*       Content   :   DSA-208 MOD_JYDK Module                                 */
/*       Date      :   Wed  07-11-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/
//-�������ͨѶ��ΪATS˨��Դת���������������
//-ATS������,�ı���û���������͵��κ�һ������,ȫ����һ��һ����
//-������һ�β鿴��ʱ��,������һ���ܺõĽ���,transmit��ʾ����,send��ʾ����
//����˵��һ�鿴�����׵�ʱ������һ��,�������Ǿ�����һ��,û�и㲻���Ķ���
//-�˴��Ĵ������ڲ��ڸ����˿�֮��Ĵ�����Ϣ,�������ǴӶ˿��Ϸ��ͱ��ĳ�ȥ

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
#define   MOD_JYDK_WAIT_TIME_VALUE            2000    //�ȴ�����ʱ�� 1ms*500
#define   MOD_JYDK_YK_FX_WAIT_TIME_VALUE      10    //ң�صȴ���Уʱ�� 2048ms*15

#define   MOD_JYDK_begin_veriiedclk_time           (*(MOD_JYDK_PARA_BASE_ADDR+0x0000))	//-Ҳ����Щ�������ֵ��û���κ�����,������һ������ռ�
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
unsigned int MOD_JYDK_CRC16(unsigned char *MOD_JYDK_CRC16_start,unsigned char MOD_JYDK_CRC16_bytes)    //*xΪָ��ÿ��ǰ5�����ݵ�ָ��
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
static BYTE MOD_JYDK_not_comm_count(void)	//-����Ƿ�ͨѶ,Ҳ���ж�����֮һ
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
//-������ͨѶ����������װ��ϵͳ����λ�����ͷ���ң������,���Ǳ�װ�ò�û���������
//-Ȩ,�ǽ��յ���̨ͨѶ����������
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_JYDK_transmit_yk_cmd(void)  
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MOD_JYDK_PORT_NO]);
    
    temp_ptS_B[0] =port_transmit[MOD_JYDK_PORT_NO][5]+byte0(port_info[MOD_JYDK_PORT_NO].scan_panel); //-�����װ��������ַ,�����ǵ�ַ��ʼֵ,��ôǰ�����ƫ����
    temp_ptS_B[1] =0x10;	//-������
    temp_ptS_B[2] =0x00;	//-������ݵ�ַ
    temp_ptS_B[3] =0x98;
    temp_ptS_B[4] =0x00;	//-�Ĵ�����
    temp_ptS_B[5] =0x02;
    temp_ptS_B[6] =0x04;	//-�ֽ���
    if(port_transmit[MOD_JYDK_PORT_NO][8]==CORE_CODE_YK_CHOOSE)
     {
      temp_ptS_B[7] =port_transmit[MOD_JYDK_PORT_NO][17];	//-����1,,ʵ����������л�ģʽֵ
      temp_ptS_B[8] =port_transmit[MOD_JYDK_PORT_NO][17];
     }
    temp_ptS_B[9] =0xaa;	//-����2
    temp_ptS_B[10] =0xaa;
    the_ram_ax=MOD_JYDK_CRC16(temp_ptS_B,11);
    temp_ptS_B[11] =byte0(the_ram_ax);	//-У��CRC
    temp_ptS_B[12] =byte1(the_ram_ax);    
    port_send_len[MOD_JYDK_PORT_NO]=13;    
    port_send_begin();
   
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_JYDK_YC_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MOD_JYDK_YC_cmd(BYTE slave_adr)	//-��ʵ���ͱ��ĺܼ򵥾�����λ����Ҫ�ĸ�ʽ,���ѵ��Ǹ��ֲ�ͬ���ͱ��ĵ�,��С���䶯����	
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MOD_JYDK_PORT_NO]);	//-������е���Ϣ�����ڷ��͵�,���ǲ�һ�������һ��������
    temp_ptS_B[0] =slave_adr;
    temp_ptS_B[1] =0x03;
    temp_ptS_B[2] =0x01; //YC��ʼ��ַ
    temp_ptS_B[3] =0x20;
    temp_ptS_B[4] =0x00;  //����YC������ ,,��ʵ�������λ���в�û�д���
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
    temp_ptS_B[2] =0x00; //YX��ʼ��ַ
    temp_ptS_B[3] =0x80;
    temp_ptS_B[4] =0x00;  //����YX������
    temp_ptS_B[5] =0x02;
    the_ram_ax=MOD_JYDK_CRC16(temp_ptS_B,6);	//-����У��CRC
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
static void MOD_JYDK_TS_cmd(BYTE slave_adr)	//-��ô�о��Ƕ�ʱ����д��,,���������¼�����д,���ҵ���λ���ж�Ӧ���Ǳ����¼�
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MOD_JYDK_PORT_NO]);
    temp_ptS_B[0] =slave_adr;
    temp_ptS_B[1] =0x03;
    temp_ptS_B[2] =0x06; //TS��ʼ��ַ
    temp_ptS_B[3] =0x00;
    temp_ptS_B[4] =0x00;  //����TS������
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
static void MOD_JYDK_transmit_time_cmd(void)  	//-����λ������ʱ��,����Уʱ
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
    temp_ptS_B[7] =0x00;	//-���
    the_ram_ax=REG_Year % 100;
    temp_ptS_B[8] =byte0(the_ram_ax);	//-���
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
    port_send_len[MOD_JYDK_PORT_NO]=17;	//-��¼����Ҫ���ͼ����ֽ�����
    port_send_begin();	//-
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MOD_JYDK_scheduler      function               */
/************************************************/
//-ͨ����Ϥ�����ݸ��õ�����,Ȼ���ڴ󵨲²�,����һ���ܺõķ���
//-�����߼�:
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
      
    if((port_recv_pt[MOD_JYDK_PORT_NO]!=port_recv_dl[MOD_JYDK_PORT_NO])&&(MOD_JYDK_wait_replay==YES))	//-ֻ�д��ڵȴ��ظ�,���б�Ҫ���н��մ���
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
         	    if(port_recv[MOD_JYDK_PORT_NO][temp_loop]==port_send[MOD_JYDK_PORT_NO][0])	//-�����жϽ����ʺ����ֹ�Լ,������һ����
         	    {
         	        the_ram_ax=(port_recv_dl[temp_loop]+1)&0x1ff;
         	        if(port_recv[MOD_JYDK_PORT_NO][the_ram_ax]==port_send[MOD_JYDK_PORT_NO][1])	//-������ȶԿ����Ƿ���ȷ
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
            if(delta_len>=3)	//-����Լ�����ʾ�����,����վ��ַ�͹�������,��С����Ч����Ҳ���ٻ���3���ֽ�û�д���
            {	
            	the_ram_ax=(port_recv_dl[MOD_JYDK_PORT_NO]+1)&0x1ff;
                if(port_recv[MOD_JYDK_PORT_NO][the_ram_ax]==port_send[MOD_JYDK_PORT_NO][1])	//-��ȫ������ϱ������ݴ�����,,�ȽϹ�����
                {	//-�������Լ����,������γ���û�д�����,�������
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
    if(MOD_JYDK_rec_OK==YES)	//-��ʾ���պ���,����һ�ּ򵥵ı�־
    {
        MOD_JYDK_rec_OK=NO;
        MOD_JYDK_transmit_flag=YES;	//-��ʾ�����������ͱ�����
        MOD_JYDK_transmit_wait_time=Time_1ms_Counter;
        the_ram_bx=(port_recv_dl[MOD_JYDK_PORT_NO]+1)&0x1ff;;
        if(port_recv[MOD_JYDK_PORT_NO][the_ram_bx]!=0x05)	//-�����жϹ�����
        {
            the_ram_ax=(port_recv_dl[MOD_JYDK_PORT_NO]+2)&0x1ff;	//-��λ�������ֽڳ��ȵ�Ԫ��
            temp_int=port_recv[MOD_JYDK_PORT_NO][the_ram_ax]+5+port_recv_dl[MOD_JYDK_PORT_NO];  //-�������ĳ����ֽ���      
            for(temp_loop=port_recv_dl[MOD_JYDK_PORT_NO];temp_loop<temp_int;temp_loop++)
            {
            	if(temp_loop<=511)
              	    port_report[temp_loop-port_recv_dl[MOD_JYDK_PORT_NO]]=port_recv[MOD_JYDK_PORT_NO][temp_loop];	//-˫������
            	else
              	    port_report[temp_loop-port_recv_dl[MOD_JYDK_PORT_NO]]=port_recv[MOD_JYDK_PORT_NO][temp_loop-512];
            }
            port_recv_dl[MOD_JYDK_PORT_NO]+=delta_len;	//-���˽��յ������±����Ѿ�������
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
        {	//-�����������Ϊ�Ǵ����
            MOD_JYDK_comm_err_counter[port_report[0]]=0;
            temp_int=port_report[0]+MOD_JYDK_begin_addr-byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);
            YX_State[IED_TX_STATE_START_WORD+temp_int/16]&=(0xffff-(0x0001<<(temp_int%16)));
        }    
    }
     //-����Ĵ��������ǶԱ��ĵĴ���,��ת��inrda�Ĵ������Ƿ��ͱ�����,����Ĵ���ȴ�ǶԽ��յ��ľ������ݵĴ���,���м���һ��ʱ���
    switch(MOD_JYDK_rec_frame_type)	//-���ֵ�Ƿ��͵�ʱ�����,��ô�������ı��ľ�����λ������ʱ�Ĺ�����ͽ��յĹ�������һ�µ�
    {	//-���ﴦ�����ǽ��ձ���,���ڸ����ڲ����ݿ�
      case MOD_JYDK_YC_FRAME:       //yc
         Core_Src_Unit =port_report[0]+MOD_JYDK_begin_addr-byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);
         Core_Det_Pt=(WORD *)&(YC_State[unit_info[Core_Src_Unit].yc_val_start]);
         //��ң��ָ��ָ��0000λ��
         temp_int=port_report[2]/2;	//-��������Ŀ
         //����ң����� 
         if(temp_int>unit_info[Core_Src_Unit].yc_val_num)
            temp_int=unit_info[Core_Src_Unit].yc_val_num;	//-ʵ���ϴ�������������������Ŀ�Ļ�ֱ����������Ĳ���,��Ϊû�пռ��¼
         
         for(temp_loop=0;temp_loop<temp_int;temp_loop++)
         {
            Core_Det_Pt[temp_loop]=port_report[2*temp_loop+3]*0x100+port_report[2*temp_loop+4];
            //ȡֵ ��ֵ
            the_ram_bx=Core_Det_Pt[temp_loop];
            if(temp_loop<3)	//-��ͬλ�õ������в�ͬ�Ĵ�������
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
            //������ֵ����Ӧ���ݷŴ����С����ֹת�����
            Core_Det_Pt[temp_loop]=(the_ram_bx&0xfff);	//-����ط���ң��û��ʹ��ͳһģ��,����ֱ�Ӹ��������ݿ�������
         }         
         break;
      case MOD_JYDK_YX_FRAME:       //yx
         Core_Src_Unit=port_report[0]+MOD_JYDK_begin_addr-byte0(port_info[MOD_JYDK_PORT_NO].scan_panel); //-��Ȼ������Ŀ�������ݿ�,��Ȼ��ľ������ݿ��ƫ����
         Core_Src_Pt_B=(BYTE *)&port_deal_buf[MOD_JYDK_PORT_NO][0];
         the_ram_bx=(port_report[2]/2);

         for(temp_loop=0;temp_loop<unit_info[Core_Src_Unit].yx_num;temp_loop++)
         {
             Core_Src_Pt_B[2*temp_loop]=byte0(YX_State[unit_info[Core_Src_Unit].yx_start+temp_loop]);
             Core_Src_Pt_B[2*temp_loop+1]=byte1(YX_State[unit_info[Core_Src_Unit].yx_start+temp_loop]);
         }	//-����ܾ�����򵥵ļ��ܴ���
         Core_Src_Pt_B=(BYTE *)&port_deal_buf[MOD_JYDK_PORT_NO][0];
         for(temp_loop=0;temp_loop<(Core_Src_Len/2);temp_loop++)
         {
             Core_Src_Pt_B[2*temp_loop]=port_report[2*temp_loop+4];	//-�ر�Դ�
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
    {	//-�����Ǵ�����
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
inrda:	//-�Ѿ�У����CRC֮��͵����ﴦ����
    if(port_transmit_flag[MOD_JYDK_PORT_NO]==YES)	//-˵�������˿�����Ϣ���͵�����˿�,���ȴ���������Ϣ,��������֮����ٴ���������Ϣ
    {	//-�����Ǻ�̨������˿ڲ��ǽ���ͬһ�����������ϵ�,������ͨ��ʲô��������ʵ�ֵ���,ͨ�����ͻ�����
     	if(Judge_Time_In_MainLoop(MOD_JYDK_transmit_wait_time,200)==YES)	//-ΪYES˵�����͵ȴ�ʱ���Ѿ�����,������һ����������
     	{
     	 	if((MOD_JYDK_transmit_task_busy!=YES)||(MOD_JYDK_transmit_yk_host_port==port_transmit[MOD_JYDK_PORT_NO][1])) 
     	 	{//-���������,ǰ���ǿ��Է����жϻ�����ǶԷ��ͳ�ȥ����Ϣ���ر��ĵĴ���
     	 	    port_transmit_flag[MOD_JYDK_PORT_NO]=NO;	//-˵�����ݹ����������Ѿ���������,���Լ����������ݵ�����˿���,ָ���������˿ڶ�������λ��
     	 	    switch(port_transmit[MOD_JYDK_PORT_NO][8])	//-����Ǿ��崫�ݹ�������Ϣ,�������ָʾ��Ϣ���д���
     	 	    {
     	 	        case CORE_CODE_YK_CHOOSE:	//-����������������,��ô�������´���
     	 	        	
     	 	             MOD_JYDK_transmit_task_busy=NO;	//-��æ�Ƿ�˵�����ڷǹ���״̬,����Ҫ�ȴ�,���ش�����???
     	 	             MOD_JYDK_wait_replay=NO;
     	 	             MOD_JYDK_transmit_disable_begin_time=Time_2048ms_Counter;	//-��¼��ʼʱ��,���ʱ��֮��һ�ξͿ��Է�����,���ʱ�����ǽ�ֹ��
     	 	             MOD_JYDK_transmit_yk_host_port=port_transmit[MOD_JYDK_PORT_NO][1];	//-��¼���ĸ��˿ڽ��յĺ�̨�·���Ϣ
     	 	             MOD_JYDK_transmit_yk_cmd();	//-ͨ������²�,��������ϵͳ�����Զ���ʱ���ͳ������λ��,�����ܻش�,���ҿ���ʱ��,��Ҳ������λ��
				  										//-����˵��̨,���ȵȵȷ����ı���,��ʱ����Ϊ��λ��Ҳͬ����������Ӧ
				  	   //-��ǰ�����¾���һ��ì�ܵĵط�,�������ڿ���,�ǳ�����,����ʹﵽ�˶༶������Թ�Լ��˿�ת��									
     	 	             exchange_target_port=MOD_JYDK_transmit_yk_host_port;	//-�ĸ������Ĵ�����Ϣ,���б�Ҫ���Ǹ��ڷ������µ�����
     	 	             exchange_buf[1] = MOD_JYDK_PORT_NO;	//-��¼����װ�ö˿ں�
     	 	             exchange_buf[2] = PROTOCOL_MOD_JYDK%0x100;	//-��Լ����
     	 	             exchange_buf[3] = PROTOCOL_MOD_JYDK/0x100;	//-ǰ������ݾ���һ��������,û��ʲô��Լ����
     	 	             exchange_buf[5] =port_transmit[MOD_JYDK_PORT_NO][5]+MOD_JYDK_begin_addr;	//-�ڲ����ݿ��ַ
     	 	             exchange_buf[8] = CORE_CODE_YK_VERIFY;	//-������̨���Ӷ˿ڵ�,��֪�����Ѿ��·�,��Ҫ�ȴ���λ����ң�ط�У����
     	 	             exchange_buf[9] = 0x02;	//-��ʾ��16��ʼ�ٴ�������Ԫ��Ϣ��16��17
     	 	             exchange_buf[10]= 0x00;
     	 	             exchange_buf[16]=port_transmit[MOD_JYDK_PORT_NO][16]; 
     	 	             exchange_buf[17]=port_transmit[MOD_JYDK_PORT_NO][17];
     	 	             Ex_Produce_Transmit_Info();	//-��������Ϣ���ݸ�ָ���Ķ˿�,���Ǵ��ĸ��˿ڷ��ͳ�ȥ            
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
    {	//-����������������������ݵ��ж�
        if(Judge_LongTime_In_MainLoop(MOD_JYDK_begin_veriiedclk_time,TIME_MOD_JYDK_VERIFY_TIME_VALUE)==YES)//Уʱ
        {	//-��ʱУʱ,���ȼ��ܸ�
            MOD_JYDK_begin_veriiedclk_time=Time_2048ms_Counter;
            MOD_JYDK_transmit_time_cmd();	//-����λ��ATS������Уʱ����(����)
            MOD_JYDK_rec_frame_type=NO;
            MOD_JYDK_transmit_wait_time=Time_1ms_Counter;	//-��¼��ʼ����ʱ��,���
            MOD_JYDK_transmit_flag=YES;	//-���ڲ���Ҫ���ر���,���Կ��Է����µı���
        }
        else if(MOD_JYDK_transmit_flag==YES)	//-��ʾ���Է��ͱ���,���ڲ�ѯ����
        {
         	if(Judge_Time_In_MainLoop(MOD_JYDK_transmit_wait_time,80)==YES)
            	switch(MOD_JYDK_transmit_control)	//-�ڲ�������֮��,������ֵ,,���ֵ�ĸ������ڳ�ʼֵ�Ļ������߼��仯��
            	{	//-���ͱ��ĵĴ���
            	    case 1:
            	         MOD_JYDK_rec_frame_type=MOD_JYDK_YC_FRAME;		//-��ʵ������ȫ��Ϊ���մ����õ�
            	         MOD_JYDK_wait_replay=YES;	//-��ʾ��ʼ�ȴ����ر���
            	         MOD_JYDK_YC_cmd(MOD_JYDK_now_poll_addr);
            	         MOD_JYDK_now_poll_addr++;	//-��ʾ��Ҫ���ͱ��ĵĵ�ַ,���ж�װ���������ĵ�ַ,���������ݿ��е�ƫ����
            	         disable();
    	  		         port_recv_pt[MOD_JYDK_PORT_NO]=0;
    	  		     	 port_recv_dl[MOD_JYDK_PORT_NO]=0;	//-��ʼ���ͱ���,�ȴ��ش��ĵ�ʱ��,��ȻҪ�Ѵ�����������ʼ��
    	  		     	 enable();
            	         MOD_JYDK_transmit_flag=NO;	//-ΪNO��ʾ���ڲ��ܷ�������,�ڵȴ��ظ�����
            	         MOD_JYDK_rxd_head_flag=NO;
            	         MOD_JYDK_rxd_wait_time=Time_1ms_Counter;
            	         if(MOD_JYDK_now_poll_addr>byte1(port_info[MOD_JYDK_PORT_NO].scan_panel))
            	         {
            	             MOD_JYDK_now_poll_addr=byte0(port_info[MOD_JYDK_PORT_NO].scan_panel);	//-����һ�౨�ĵ�ʱ��,���Ǵӵ�һ��װ�ÿ�ʼ��ѯ
            	             MOD_JYDK_transmit_control=2;	//-�����˽�������Ҫ���ͱ��ĵ�����
            	         }
            	         break;
            	    case 2:
            	         MOD_JYDK_rec_frame_type=MOD_JYDK_YX_FRAME;
            	         MOD_JYDK_wait_replay=YES;	//-�������־��ԭ����,ϵͳ�������͵ı���,���������е����ݶ���Ҫ�ȴ��ظ����ĵ�,��Щ���ľ�����λ���лظ�,��Ҳ����Ҫ
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
            	    case 3:	//-��������ǰ���,����˵Ԥ����
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
    MOD_JYDK_transmit_flag=YES;	//-��ʾ���Է��ͱ���
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
  MOD_JYDK_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];	//-��������ʵ��ȫ����û��,������Ϊ�˷���,�ٴζ�Ӧ������ַ
  
}
