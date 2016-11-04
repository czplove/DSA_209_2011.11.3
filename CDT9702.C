/*****************************************************************************/
/*       FileName  :   CDT9702.C                                             */
/*       Content   :   DSA-208 CDT9702 Module                                */
/*       Date      :   Fri  06-22-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/
//-这个通讯规约是连接后台的,是很多东西的大脑,有点小复杂,但是需要突破
//-这个作为后台程序是与后台通讯的,但是对于端口主体而言是没有区别的
//-本规约是在端口8使用的,是连接后台的,而对于其中一个ATS的控制命令,却
//-是要通过端口0进行数据传输的,那么当我在通道8发送缓冲区准备好数据之后
//-是怎么映射到通道0进行发送的呢
//-这个是后台规约是和后台约定的通信协议

//-后台软件,也没有什么神秘的就是一个程序而以,他处理的数据也是通过接收信息而来的,有自己的逻辑
//-可能就是形式不同而已

#pragma  regconserve

#include _SFR_H_
#include _FUNCS_H_

#include "common.h"
#include "device.h"
#include "reg.h"
#include "ram.h"
#include "rom.h"
#include "comuse.h"

//-本规约作为后台规约的一种,应该具有主动上送能力,和接收命令能力
//-对于各种信息也有智能处理能力

//-组>页(N)=帧(N)>块(N+2)>字节(6)    :依次是包含关系



#define CDT9702_PORT     port_no
//-下面的这些定义仅仅是指定了全局变量,需要用到的时候才有实际数值意义
//send
#define cdt9702_yx_chg_send         port_flag[CDT9702_PORT][0]
#define cdt9702_yx_send             port_flag[CDT9702_PORT][1]
#define cdt9702_control_code        port_flag[CDT9702_PORT][2]
    // 0x61:YC  0xf4:YX  0x26:SOE  0xf8:Protect
#define cdt9702_send_info_turn      port_flag[CDT9702_PORT][3]
    // 0: EB90   1: control  2: info 
#define cdt9702_send_order          port_flag[CDT9702_PORT][4]
#define cdt9702_send_frame_len      port_flag[CDT9702_PORT][5]
#define cdt9702_send_stage          port_flag[CDT9702_PORT][6]
    // 0: 1: 2: YC           3: YX, SOE, YK, Protect
#define cdt9702_yk_verify_send      port_flag[CDT9702_PORT][7]


//receive
#define cdt9702_rec_syn_flag        port_flag[CDT9702_PORT][10]
#define cdt9702_rec_stage           port_flag[CDT9702_PORT][11]
#define cdt9702_rec_frame_len       port_flag[CDT9702_PORT][12]
#define cdt9702_rec_frame_type      port_flag[CDT9702_PORT][13]
    // 0x7161: YC,YK    0x71f4: YX  0x7126: SOE   0x7185: DD 
    // 0x71f8: protect   
#define cdt9702_rec_info_num        port_flag[CDT9702_PORT][14]
#define cdt9702_yx_fresh            port_flag[CDT9702_PORT][15]

//转发表号
#define CDT9702_TRAN_YC_TABLE_NO    port_flag[CDT9702_PORT][16]
#define CDT9702_TRAN_YX_TABLE_NO    port_flag[CDT9702_PORT][17]
#define CDT9702_TRAN_YM_TABLE_NO    port_flag[CDT9702_PORT][18]

#define CDT9702_RLY_REPORT          port_flag[CDT9702_PORT][19]
#define CDT9702_LSA_SELFCHK         port_flag[CDT9702_PORT][20]
#define CDT9702_PAGE_NUM            port_flag[CDT9702_PORT][21]
#define CDT9702_PAGE_ORDER          port_flag[CDT9702_PORT][22]
#define CDT9702_ZZZZ1               port_flag[CDT9702_PORT][23]
#define CDT9702_time_tmp_MS         port_flag[CDT9702_PORT][24]
#define CDT9702_time_tmp_MSL        byte0(port_flag[CDT9702_PORT][24])
#define CDT9702_time_tmp_MSH        byte1(port_flag[CDT9702_PORT][24])
#define CDT9702_time_tmp_SEC        byte0(port_flag[CDT9702_PORT][25])
#define CDT9702_time_tmp_MIN        byte1(port_flag[CDT9702_PORT][25])

#define CDT9702_soe_send_turn       (port_flag[CDT9702_PORT][26])

#define CDT9702_bh_bank_report_len  port_flag[CDT9702_PORT][27]
#define CDT9702_protect_report_len  port_flag[CDT9702_PORT][28]
#define CDT9702_YDZT                port_flag[CDT9702_PORT][29]

#define CDT9702_time_up_HOUR        byte0(port_flag[CDT9702_PORT][30])
#define CDT9702_time_up_DATE        byte1(port_flag[CDT9702_PORT][30])
#define CDT9702_time_up_MONTH       byte0(port_flag[CDT9702_PORT][31])
#define CDT9702_time_up_YEAR        byte1(port_flag[CDT9702_PORT][31])

#define CDT9702_YC_CHG_FLAG	    byte0(port_flag[CDT9702_PORT][32])
#define CDT9702_TRAN_YC_CHG_TABLE_NO   byte1(port_flag[CDT9702_PORT][32])

#define CDT9702_YC_CHG_SEND_NO	    port_flag[CDT9702_PORT][33]
#define CDT9702_YC_CHG_COUNT        byte0(port_flag[CDT9702_PORT][34])

#define CDT9702_TRAN_YC_NUM         port_info[CDT9702_PORT].scan_unit	//-定义了需要管理机转发遥测的个数
#define CDT9702_TRAN_YX_NUM         port_info[CDT9702_PORT].scan_panel	//-遥信上送的字数
#define CDT9702_TRAN_YM_NUM         port_info[CDT9702_PORT].rebound		//-配置的遥脉转发个数
#define CDT9702_TRAN_FRAME_IN_1PAGE port_info[CDT9702_PORT].YK_enable	//-表示每段中16个数据吗

#define CDT9702_CONTROL_HEAD   0xF1
#define CDT9702_YC_NORMAL      0x61	//-重要遥测（A帧）
#define CDT9702_YC_IMPORTANT   0xC3	//-次要遥测（B帧）
#define CDT9702_YC_LESSER      0xB2	//-一般遥测（C帧）
#define CDT9702_YX_NORMAL      0xF4	//-遥信状态（D1帧）
#define CDT9702_YX_EVENT       0x26	//-事件顺序记录（E帧）
#define CDT9702_YM_NORMAL      0x85	//-电能脉冲记数值（D2帧）
#define CDT9702_YM_CALCULATE   0xA8
#define CDT9702_YX_PREVENT     0xA9
#define CDT9702_YK_ERASE       0xB9
#define CDT9702_PROTECT_MSG    0x3D
#define CDT9702_RTU_STATUS     0x57
#define CDT9702_UPSET_CLK      0x7A

#define CDT9702_YK_SELECT      0x61	//-遥控选择
#define CDT9702_YK_REPLY       0xC2	//-遥控执行
#define CDT9702_YK_EXECUTE     0xB3	//-遥控撤消



void CDT9702_deal_YX_state(void);


void CDT9702_BCH_check()
{
  port_check = 0;
  temp_ptD_B = &port_send[CDT9702_PORT][0];
  for(temp_loop=0;temp_loop<5;temp_loop++)
  {
    temp_loop1 = port_check ^ (*temp_ptD_B);
    port_check = BCH_table[temp_loop1];
    temp_ptD_B++;                 //   set the BCH code   
  }
  port_check   = port_check ^ 0xff;
  *temp_ptD_B  = port_check;

}

//禁用 delta_len(temp_int) temp_ptS temp_ptD
unsigned char CDT9702_check_BCH()
{
  port_check = 0;
  for(temp_loop=0;temp_loop<5;temp_loop++)
  {
    temp_loop1 = port_check ^ (port_report[temp_loop]);
    port_check = BCH_table[temp_loop1];
  }
  port_check = 0xff ^ port_check;
  if(port_report[5] ==  port_check)
     return 0xaa;
  else
     return 0x55;

}

void CDT9702_send()
{
  CDT9702_BCH_check();

  port_send_len[CDT9702_PORT] = 6;
  port_send_begin();  
}

void CDT9702_send_EB90()
{
  temp_ptD_B = &port_send[CDT9702_PORT][0];
 
  for(temp_loop=0;temp_loop<6;temp_loop++)
  {
    *temp_ptD_B = SYN_WORD_CDT_EB90[temp_loop];
     temp_ptD_B ++;
  }

  port_send_len[CDT9702_PORT] = 6;
  port_send_begin();  
  
}

void CDT9702_send_control_byte()
{
   temp_ptD_B = &port_send[CDT9702_PORT][0];
  *temp_ptD_B = CDT9702_CONTROL_HEAD;	//-	固定为0F1H，表示本规约是“循环式远动规约DL451-91”的扩充
   temp_ptD_B ++;
  *temp_ptD_B = cdt9702_control_code;	//-帧类别码
   temp_ptD_B ++;
  *temp_ptD_B = cdt9702_send_frame_len;	//-信息字数
   temp_ptD_B ++;
  *temp_ptD_B = PROTOCOL_CDT9702_SOURCE_ADDR[CDT9702_PORT];	//-源站地址
   temp_ptD_B ++;
  *temp_ptD_B = PROTOCOL_CDT9702_DESTIN_ADDR[CDT9702_PORT];	//-目的站地址

  CDT9702_send();   
}

//-遥测量的安排是每连续的16的数据分为一段,每段内部再依次排列0~15序号
void CDT9702_send_YC()
{
  unsigned short temp_int,temp_short;
  
  temp_ptD_B = &port_send[CDT9702_PORT][0];
  temp_int   = (CDT9702_PAGE_ORDER*CDT9702_TRAN_FRAME_IN_1PAGE+cdt9702_send_order)*2;	//-	遥测序号=段*16+偏移量
 *temp_ptD_B = (temp_int+MON_FUN_NO[CDT9702_PORT][0])/16;	//-功能码,其实就是段数目,上面乘以2的原因是每个信息字传送2个数据

  temp_ptD_B ++;

  for(temp_loop=0;temp_loop<2;temp_loop++)
  {
     if(CDT9702_TRAN_YC_TABLE_NO<4) 
        temp_lp_int = YC_transmite_table[CDT9702_TRAN_YC_TABLE_NO][temp_int];
     else
        temp_lp_int = temp_int;
     temp_short = YC_State[temp_lp_int];	//-取得是实际值
     if(temp_lp_int<2048)	//-保证可靠,没有超出范围
     {
     	if(CDT9702_TRAN_YC_CHG_TABLE_NO<0x04)
	{
	    if(byte1(YC_Chg_FLAG[temp_lp_int])!=0xff)
	        YC_Chg_Last_Value[CDT9702_TRAN_YC_CHG_TABLE_NO][byte1(YC_Chg_FLAG[temp_lp_int])]=YC_State[temp_lp_int];
	}
     }	
     
     if(temp_short>0x7fff) 	//-对存储数据进行判断
     {
        temp_short = 0x10000 - temp_short;
        temp_short = 0x1000  - temp_short;
        temp_short&= 0x0fff;
     }
     else if(temp_short>0x0fff)
     {
        if(temp_short<5000)
            temp_short = 0x1000 - (5000 - temp_short);
        else
        {
            temp_short = temp_short - 5000;
            temp_short&= 0x0fff;
        }    
     }    
        
    *temp_ptD_B  = byte0(temp_short);
     temp_ptD_B  ++;
    *temp_ptD_B  = byte1(temp_short) 
                 + (( (CDT9702_PAGE_ORDER*CDT9702_TRAN_FRAME_IN_1PAGE+cdt9702_send_order) %8)*2+temp_loop)*16;	//-得遥测偏移量,并放入高4为
     temp_ptD_B  ++;
     temp_int    ++;	//-自然调整到奇数位
  }  

  CDT9702_send();   
}

void CDT9702_send_yc_change()
{
	WORD  the_ram_ax;
	WORD  the_ram_bx;
	BYTE  the_ram_axl;
	BYTE  the_ram_axh;
	
	if(CDT9702_TRAN_YC_CHG_TABLE_NO>0x03)	//-可能出错了,或者故意的
	{	//-作为配置值,本段程序仅仅执行到这里
	    CDT9702_YC_CHG_FLAG=0x55;
	    return;
	}
	the_ram_axl=1<<(CDT9702_TRAN_YC_CHG_TABLE_NO);
	the_ram_axh=0;
	temp_ptD_B = &port_send[CDT9702_PORT][0];
	
	for(temp_loop=CDT9702_YC_CHG_SEND_NO;temp_loop<CDT9702_TRAN_YC_NUM;temp_loop++)
	{	
	    if(CDT9702_TRAN_YC_TABLE_NO>0x03)
	    the_ram_ax=temp_loop;
	    else
	    the_ram_ax=YC_transmite_table[CDT9702_TRAN_YC_TABLE_NO][temp_loop];	
	    
	    if((byte0(YC_Chg_FLAG[the_ram_ax])&(the_ram_axl))==the_ram_axl)		//有突变YC
		{
			byte0(YC_Chg_FLAG[the_ram_ax])&=(0xff-the_ram_axl);
			if((temp_loop&0x01)==0x00)			//YC0
			{
				if(CDT9702_TRAN_YC_TABLE_NO>0x03)
				the_ram_bx=temp_loop+1;
				else
				the_ram_bx=YC_transmite_table[CDT9702_TRAN_YC_TABLE_NO][temp_loop+1];
				temp_ptD_B[the_ram_axh*6+6]=temp_loop/16;
				temp_ptD_B[the_ram_axh*6+7]=byte0(YC_State[the_ram_ax]);
				temp_ptD_B[the_ram_axh*6+8]=byte1(YC_State[the_ram_ax]);
				temp_ptD_B[the_ram_axh*6+8]&=0x0f;
				temp_ptD_B[the_ram_axh*6+8]+=(temp_loop%16)*0x10;
				temp_ptD_B[the_ram_axh*6+9]=byte0(YC_State[the_ram_bx]);
				temp_ptD_B[the_ram_axh*6+10]=byte1(YC_State[the_ram_bx]);
				temp_ptD_B[the_ram_axh*6+10]&=0x0f;
				temp_ptD_B[the_ram_axh*6+10]+=((temp_loop+1)%16)*0x10;
				BCH_Array[0]=temp_ptD_B[the_ram_axh*6+6];
	            		BCH_Array[1]=temp_ptD_B[the_ram_axh*6+7];
	            		BCH_Array[2]=temp_ptD_B[the_ram_axh*6+8];
	            		BCH_Array[3]=temp_ptD_B[the_ram_axh*6+9];
	            		BCH_Array[4]=temp_ptD_B[the_ram_axh*6+10];
	            		BCH_Calculate();
				temp_ptD_B[the_ram_axh*6+11]=BCH_Array[5];
				YC_Chg_Last_Value[CDT9702_TRAN_YC_CHG_TABLE_NO][byte1(YC_Chg_FLAG[the_ram_ax])]=YC_State[the_ram_ax];
				YC_Chg_Last_Value[CDT9702_TRAN_YC_CHG_TABLE_NO][byte1(YC_Chg_FLAG[the_ram_bx])]=YC_State[the_ram_bx];
				the_ram_axh++;
				
				if((byte0(YC_Chg_FLAG[the_ram_bx])&(the_ram_axl))==the_ram_axl)		//YC0 YC1均突变
				{
				temp_loop++;
				byte0(YC_Chg_FLAG[the_ram_bx])&=(0xff-the_ram_axl);
				}
			}
			else								//yc1
			{
				if(CDT9702_TRAN_YC_TABLE_NO>0x03)
				the_ram_bx=temp_loop-1;
				else
				the_ram_bx=YC_transmite_table[CDT9702_TRAN_YC_TABLE_NO][temp_loop-1];
				temp_ptD_B[the_ram_axh*6+6]=temp_loop/16;
				temp_ptD_B[the_ram_axh*6+7]=byte0(YC_State[the_ram_bx]);
				temp_ptD_B[the_ram_axh*6+8]=byte1(YC_State[the_ram_bx]);
				temp_ptD_B[the_ram_axh*6+8]&=0x0f;
				temp_ptD_B[the_ram_axh*6+8]+=((temp_loop-1)%16)*0x10;
				temp_ptD_B[the_ram_axh*6+9]=byte0(YC_State[the_ram_ax]);
				temp_ptD_B[the_ram_axh*6+10]=byte1(YC_State[the_ram_ax]);
				temp_ptD_B[the_ram_axh*6+10]&=0x0f;
				temp_ptD_B[the_ram_axh*6+10]+=(temp_loop%16)*0x10;
				BCH_Array[0]=temp_ptD_B[the_ram_axh*6+6];
	            		BCH_Array[1]=temp_ptD_B[the_ram_axh*6+7];
	            		BCH_Array[2]=temp_ptD_B[the_ram_axh*6+8];
	            		BCH_Array[3]=temp_ptD_B[the_ram_axh*6+9];
	            		BCH_Array[4]=temp_ptD_B[the_ram_axh*6+10];
	            		BCH_Calculate();
				temp_ptD_B[the_ram_axh*6+11]=BCH_Array[5];
				YC_Chg_Last_Value[CDT9702_TRAN_YC_CHG_TABLE_NO][byte1(YC_Chg_FLAG[the_ram_bx])]=YC_State[the_ram_bx];
				YC_Chg_Last_Value[CDT9702_TRAN_YC_CHG_TABLE_NO][byte1(YC_Chg_FLAG[the_ram_ax])]=YC_State[the_ram_ax];
				the_ram_axh++;
			}
		}
		if(the_ram_axh>31)
		{
		CDT9702_YC_CHG_SEND_NO=temp_loop;
		break;
		}
		
		
	}
	if(the_ram_axh==0x00)
	{
		CDT9702_YC_CHG_FLAG=0x55;
		CDT9702_YC_CHG_SEND_NO=0;
		return;
	}
	if(the_ram_axh<32)
	{
		CDT9702_YC_CHG_SEND_NO=0;
	}	
	if((the_ram_axh==32)&&(CDT9702_YC_CHG_SEND_NO==(CDT9702_TRAN_YC_NUM-1)))
	{
		CDT9702_YC_CHG_SEND_NO=0;
	}
	temp_ptD_B[0]=CDT9702_CONTROL_HEAD;
	temp_ptD_B[1]=CDT9702_YC_NORMAL;
	temp_ptD_B[2]=the_ram_axh;
	temp_ptD_B[3]=PROTOCOL_CDT9702_SOURCE_ADDR[CDT9702_PORT];
	temp_ptD_B[4]=PROTOCOL_CDT9702_DESTIN_ADDR[CDT9702_PORT];
	BCH_Array[0]=temp_ptD_B[0];
	BCH_Array[1]=temp_ptD_B[1];
	BCH_Array[2]=temp_ptD_B[2];
	BCH_Array[3]=temp_ptD_B[3];
	BCH_Array[4]=temp_ptD_B[4];
	BCH_Calculate();
	temp_ptD_B[5]=BCH_Array[5];	
	port_send_len[CDT9702_PORT]=(the_ram_axh+1)*6;
	CDT9702_YC_CHG_FLAG=0xaa;
	CDT9702_YC_CHG_COUNT++;
        port_send_begin();  
		
}


//-喜报喜报,哈哈,这里就实现了向后台传送数据的能力,后台就是接收到这些数据之后进行对应处理的
void CDT9702_send_YX()
{
   temp_ptD_B = &port_send[CDT9702_PORT][0];
  *temp_ptD_B = cdt9702_send_order+MON_FUN_NO[CDT9702_PORT][1];	//-每个信息字 都有自己的功能码
   temp_ptD_B ++;
   temp_int = cdt9702_send_order*2;
   
   if(CDT9702_TRAN_YX_TABLE_NO<4)	//-也许缓冲区只有4块,超过的就用实际值
      temp_lp_int = YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][temp_int];	//-处理之后的遥信值
   else
      temp_lp_int = YX_State[temp_int];	//-由于配置的时候配置的值为F,所以送上后台取的就是这个里面的数据,直接上送数据库的值
  *temp_ptD_B = byte0(temp_lp_int);
   temp_ptD_B ++;
  *temp_ptD_B = byte1(temp_lp_int);
   temp_ptD_B ++;
   temp_int ++;

   if(CDT9702_TRAN_YX_TABLE_NO<4)	//-这个数值是配置的并不会变化,在整个程序中,
      temp_lp_int = YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][temp_int];	//-表示上送的值来自转发表从新组织的数据
   else
      temp_lp_int = YX_State[temp_int];	//-没有配置转发表,那么就是直接来自数据库中
  *temp_ptD_B = byte0(temp_lp_int);
   temp_ptD_B ++;
  *temp_ptD_B = byte1(temp_lp_int);

  CDT9702_send();   
}

void CDT9702_send_YM()
{
   temp_ptD_B = &port_send[CDT9702_PORT][0];
   temp_int   = CDT9702_PAGE_ORDER*CDT9702_TRAN_FRAME_IN_1PAGE+cdt9702_send_order;	//-计算功能码,其实这个功能码也有偏移量的意思
  *temp_ptD_B = temp_int+MON_FUN_NO[CDT9702_PORT][3];
   temp_ptD_B ++;
   if(CDT9702_TRAN_YM_TABLE_NO<4)
      temp_ptL = (near UL *)&YM_State[YM_transmite_table[CDT9702_TRAN_YM_TABLE_NO][temp_int]]; 
   else
      temp_ptL = (near UL *)&YM_State[temp_int];    
  *temp_ptD_B = byte0(*temp_ptL);
   temp_ptD_B ++;
  *temp_ptD_B = byte1(*temp_ptL);
   temp_ptD_B ++;
  *temp_ptD_B = byte2(*temp_ptL);
   temp_ptD_B ++;
  *temp_ptD_B = byte3(*temp_ptL);

  if( (CDT9702_PAGE_ORDER+1)>=CDT9702_PAGE_NUM )
      cdt9702_yx_send = 0;
  CDT9702_send();   
}

void CDT9702_send_YK_verify_from_transmit()	//-下面组织的是信息字
{
    BYTE ram_axl;
    
    port_send[CDT9702_PORT][0]=0xFA;	//-功能码
    temp_ptS_B = &port_transmit[CDT9702_PORT][16];	//-应该是我想复杂了,并没有什么特别的规约可言,这个就是一个缓冲区,内容自知就行,而且作为缓冲区,他是最近有效原则
    if(*temp_ptS_B==CORE_CODE2_YK_CLOSE)
        port_send[CDT9702_PORT][1]=0xcc;	//-合
    else if(*temp_ptS_B==CORE_CODE2_YK_TRIP) 
        port_send[CDT9702_PORT][1]=0x33;	//-分
    else
        port_send[CDT9702_PORT][1]=0xff;	//-错
    port_send[CDT9702_PORT][2]=(*(temp_ptS_B+1))+MON_FUN_NO[CDT9702_PORT][2];	//-开关序号
    port_send[CDT9702_PORT][3]=port_send[CDT9702_PORT][1];	//-合/分/错(重复)
    port_send[CDT9702_PORT][4]=(*(temp_ptS_B+1))+MON_FUN_NO[CDT9702_PORT][2];	//-开关序号(重复)

    CDT9702_send();   //-这个里面统一计算校验码
}

void CDT9702_send_YX_chg()
{
   if(CDT9702_TRAN_YM_TABLE_NO < 4)
     temp_ptS = &yx_change_tr[CDT9702_TRAN_YX_TABLE_NO][yx_chg_out_pt[CDT9702_PORT]].offset_no;
   else
     temp_ptS = &yx_change[yx_chg_out_pt[CDT9702_PORT]].offset_no;    
   temp_ptD_B = &port_send[CDT9702_PORT][0];
  *temp_ptD_B = byte0(*temp_ptS) + 0x80+MON_FUN_NO[CDT9702_PORT][1];	//-	插入标志：全遥信取“0”，插入遥信取“1”
   temp_ptD_B ++;
   temp_ptS ++;
  *temp_ptD_B = byte0(*temp_ptS);
   temp_ptD_B ++;
  *temp_ptD_B = byte1(*temp_ptS);
   temp_ptS ++;
   temp_ptD_B ++;
  *temp_ptD_B = byte0(*temp_ptS);
   temp_ptD_B ++;
  *temp_ptD_B = byte1(*temp_ptS);

  CDT9702_send();   
}
//-先把具体发送的类型进行分类,然后才是具体的内容形式
void CDT9702_send_YX_SOE()
{
   if(CDT9702_TRAN_YX_TABLE_NO < 4)
     temp_ptS = &yx_event_tr[CDT9702_TRAN_YX_TABLE_NO][yx_soe_out_pt[CDT9702_PORT]].soe_ms;
   else
     temp_ptS = &yx_event[yx_soe_out_pt[CDT9702_PORT]].soe_ms;	//-取信息源的地址
   temp_ptD_B = &port_send[CDT9702_PORT][0];
   
   if(cdt9702_send_order==0)
   {
       *temp_ptD_B = cdt9702_send_order;	//-这些所谓的功能码,不如说编号更准确
        temp_ptD_B ++;
       *temp_ptD_B = byte0(*temp_ptS);
        temp_ptD_B ++;
       *temp_ptD_B = byte1(*temp_ptS);
        temp_ptD_B ++;
        temp_ptS ++;
       *temp_ptD_B = byte0(*temp_ptS);
        temp_ptD_B ++;
       *temp_ptD_B = byte1(*temp_ptS);
   }
   else
   {
        temp_ptS +=2;
       *temp_ptD_B = cdt9702_send_order;
        temp_ptD_B ++;
       *temp_ptD_B = byte0(*temp_ptS);
        temp_ptD_B ++;
       *temp_ptD_B = byte1(*temp_ptS);
        temp_ptD_B ++;
        temp_ptS ++;
       *temp_ptD_B = byte0((*temp_ptS)+MON_FUN_NO[CDT9702_PORT][1]*32);
        temp_ptD_B ++;
       *temp_ptD_B = byte1((*temp_ptS)+MON_FUN_NO[CDT9702_PORT][1]*32);    
   }

   if((CDT9702_soe_send_turn>=3)&&(cdt9702_send_order>=1))	//-本帧内容需要重复发送3遍才算,发送成功
   {
       CDT9702_soe_send_turn=0;
       yx_soe_out_pt[CDT9702_PORT] ++;	//-表示有成功的向后台发送了一个事件,并不是发送一个就是发送成功了
       yx_soe_out_pt[CDT9702_PORT] &= 0x3ff;
   }         
   CDT9702_send();    
}

void CDT9702_send_YDZT()	//-用的状态,,说白了就是遥信没有其它
{
   temp_ptD_B = &port_send[CDT9702_PORT][0];
  *temp_ptD_B = cdt9702_send_order;
   temp_ptD_B ++;

   temp_int   = cdt9702_send_order*2+IED_TX_STATE_START_WORD;
   temp_lp_int= YX_State[temp_int];
   
  *temp_ptD_B = byte0(temp_lp_int);
   temp_ptD_B ++;
  *temp_ptD_B = byte1(temp_lp_int);
   temp_ptD_B ++;
   temp_lp_int= YX_State[temp_int+1];
  *temp_ptD_B = byte0(temp_lp_int);
   temp_ptD_B ++;
  *temp_ptD_B = byte1(temp_lp_int);

  CDT9702_send();   
}

void CDT9702_send_clk()
{
   temp_ptD_B = &port_send[CDT9702_PORT][0];
  *temp_ptD_B = cdt9702_send_order;
   temp_ptD_B ++;
   if(cdt9702_send_order==0)
   {
       disable();
      *temp_ptD_B =byte0(REG_1Msecond);
       temp_ptD_B ++;
      *temp_ptD_B =byte1(REG_1Msecond);
       temp_ptD_B ++;
      *temp_ptD_B =REG_Second;
       temp_ptD_B ++;
      *temp_ptD_B =REG_Minute;
       CDT9702_time_up_HOUR =REG_Hour;
       CDT9702_time_up_DATE =REG_Date;
       CDT9702_time_up_MONTH=REG_Month;
       CDT9702_time_up_YEAR =(REG_Year-(1830+byte1(port_info[CDT9702_PORT].time_enable))) % 100;
       enable();
   }
   else
   {
      *temp_ptD_B =CDT9702_time_up_HOUR;
       temp_ptD_B ++;
      *temp_ptD_B =CDT9702_time_up_DATE;
       temp_ptD_B ++;
      *temp_ptD_B =CDT9702_time_up_MONTH;
       temp_ptD_B ++;
      *temp_ptD_B =CDT9702_time_up_YEAR;
   }
  
   CDT9702_send();    //-每6个字节一发送
}




void CDT9702_send_bh_bank()   //-发送上来的可以是保护帧,也可以用SOE表示保护事件,就看你使用哪种形式
{
  unsigned char *temp_ptsx;
  
  temp_ptD_B = &port_send[CDT9702_PORT][0];
 *temp_ptD_B =  cdt9702_send_order;
  temp_ptD_B ++;

  if(cdt9702_send_order==0)
  {
       *temp_ptD_B =  0;                                
       temp_ptD_B++;
       if( ((BH_Bank_Report[CDT9702_PORT][510]==(PROTOCOL_LSA%0x100))
        &&(BH_Bank_Report[CDT9702_PORT][511]==(PROTOCOL_LSA/0x100)))
        ||((BH_Bank_Report[CDT9702_PORT][510]==(PROTOCOL_REC_HOST%0x100))
        &&(BH_Bank_Report[CDT9702_PORT][511]==(PROTOCOL_REC_HOST/0x100))) )
       {
            *temp_ptD_B = 10+0x40;                           
            temp_ptD_B++;
            *temp_ptD_B = byte0(CDT9702_bh_bank_report_len); 
            temp_ptD_B++;
            *temp_ptD_B = byte1(CDT9702_bh_bank_report_len);
       }
       else
       {
            if((BH_Bank_Report[CDT9702_PORT][510]==(PROTOCOL_DFP201%0x100))   // old lfp
             &&(BH_Bank_Report[CDT9702_PORT][511]==(PROTOCOL_DFP201/0x100)))
            {
                *temp_ptD_B =  3+0x40;                           
                temp_ptD_B++;
                *temp_ptD_B = byte0(CDT9702_bh_bank_report_len); 
                temp_ptD_B++;
                *temp_ptD_B = byte1(CDT9702_bh_bank_report_len);
            }
            else
            {
	           if((BH_Bank_Report[CDT9702_PORT][510]==(PROTOCOL_LFP%0x100))   // new lfp
       	       &&(BH_Bank_Report[CDT9702_PORT][511]==(PROTOCOL_LFP/0x100)))
               {
                    *temp_ptD_B =  2+0x40;                           
                    temp_ptD_B++;
	                *temp_ptD_B = byte0(CDT9702_bh_bank_report_len); 
	                temp_ptD_B++;
       	            *temp_ptD_B = byte1(CDT9702_bh_bank_report_len);
               }
               else
               {
                    if((BH_Bank_Report[CDT9702_PORT][510]==(PROTOCOL_IEC103_LSA%0x100))   // lsa18x
       	              &&(BH_Bank_Report[CDT9702_PORT][511]==(PROTOCOL_IEC103_LSA/0x100)))
                    {
                         *temp_ptD_B =  14+0x40;                           
                         temp_ptD_B++;
	                     *temp_ptD_B = byte0(CDT9702_bh_bank_report_len); 
	                     temp_ptD_B++;
       	                 *temp_ptD_B = byte1(CDT9702_bh_bank_report_len);
                    }	
                    else
                    {
                     	if((BH_Bank_Report[CDT9702_PORT][510]==(PROTOCOL_ISA300_CDT%0x100))   // new Isa
       	                    &&(BH_Bank_Report[CDT9702_PORT][511]==(PROTOCOL_ISA300_CDT/0x100)))
              	        {
              	      	     *temp_ptD_B =  9+0x40;                           temp_ptD_B++;
	                     	 *temp_ptD_B = byte0(CDT9702_bh_bank_report_len); temp_ptD_B++;
       	                     *temp_ptD_B = byte1(CDT9702_bh_bank_report_len);
              	        }
              	        else
              	        {	
	        	             *temp_ptD_B = 0x3f;                              
	    	                 temp_ptD_B++;
       	    	             *temp_ptD_B = 0x00;                              
       	    	             temp_ptD_B++;
               	             *temp_ptD_B = 0x00;
                        }
                    }
               }   
            }
       }
  }
  else
  {
      temp_ptsx = &BH_Bank_Report[CDT9702_PORT][(cdt9702_send_order-1)*4];

      for(temp_loop=0;temp_loop<4;temp_loop++)
      {
         *temp_ptD_B    = *temp_ptsx;
          temp_ptsx     ++;
          temp_ptD_B    ++;
      }  
  }

  CDT9702_send();   
}


void CDT9702_send_bh_report()
{
  temp_ptD_B = &port_send[CDT9702_PORT][0];
 *temp_ptD_B =  cdt9702_send_order;
  temp_ptD_B ++;

  if(cdt9702_send_order==0)
  {
     *temp_ptD_B =  0;                                
     temp_ptD_B++;
     if( ((port_transmit[CDT9702_PORT][2]==(PROTOCOL_LSA%0x100))
        &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_LSA/0x100))) ||
         ((port_transmit[CDT9702_PORT][2]==(PROTOCOL_DSA301%0x100))
        &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_DSA301/0x100))) ||
         ((port_transmit[CDT9702_PORT][2]==(PROTOCOL_REC_HOST%0x100))
        &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_REC_HOST/0x100))) ) 
     {
         *temp_ptD_B = 10;    //-保护类型                            
         temp_ptD_B++;
         *temp_ptD_B = byte0(CDT9702_protect_report_len); 	//-	保护有效报文长度低8位
         temp_ptD_B++;
         *temp_ptD_B = byte1(CDT9702_protect_report_len);	//-	保护有效报文长度高8位
     }   
     else
     {
      	if((port_transmit[CDT9702_PORT][2]==(PROTOCOL_DFP201%0x100))
      	 &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_DFP201/0x100)))
      	 {
      	  	*temp_ptD_B = 3;                                 
      	  	temp_ptD_B++;
      	  	*temp_ptD_B = byte0(CDT9702_protect_report_len); 
      	  	temp_ptD_B++;
      	  	*temp_ptD_B = byte1(CDT9702_protect_report_len);
      	 }
      	else
      	 {
	    	if((port_transmit[CDT9702_PORT][2]==(PROTOCOL_LFP%0x100))
    			 &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_LFP/0x100)))
      		{ 
      		    *temp_ptD_B = 2;                                 
      		    temp_ptD_B++;
	  			*temp_ptD_B = byte0(CDT9702_protect_report_len); 
	  			temp_ptD_B++;
    			*temp_ptD_B = byte1(CDT9702_protect_report_len);
      		}
	    	else
	    	{	
	  			if((port_transmit[CDT9702_PORT][2]==(PROTOCOL_IEC103_LSA%0x100))
    				   &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_IEC103_LSA/0x100)))
      			{ 
      			      *temp_ptD_B = 14;                                 
      			      temp_ptD_B++;
	  			  	  *temp_ptD_B = byte0(CDT9702_protect_report_len); 
	  			      temp_ptD_B++;
    				  *temp_ptD_B = byte1(CDT9702_protect_report_len);
      			}
      			else	
      			{   	
	  				if((port_transmit[CDT9702_PORT][2]==(PROTOCOL_HNBH%0x100))
    					   &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_HNBH/0x100)))
      				{
      				       *temp_ptD_B = 0x10;                                 
      				       temp_ptD_B++;
	  				  	   *temp_ptD_B = byte0(CDT9702_protect_report_len); 
	  				       temp_ptD_B++;
    					   *temp_ptD_B = byte1(CDT9702_protect_report_len);           	
      				}
      				else			
   	  				{
   	  				 	if((port_transmit[CDT9702_PORT][2]==(PROTOCOL_ISA300_CDT%0x100))
    					      &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_ISA300_CDT/0x100)))
    					{
    					     *temp_ptD_B = 9;                                
    					     temp_ptD_B++;
	  				         *temp_ptD_B = byte0(CDT9702_protect_report_len); 
	  				         temp_ptD_B++;
    					     *temp_ptD_B = byte1(CDT9702_protect_report_len);
    					}
      				  	else
      				  	{		
      				      	*temp_ptD_B = 0x3f;
      				      	temp_ptD_B++;
	  				  		*temp_ptD_B = 0x00;                              
	  				  		temp_ptD_B++;
    					    *temp_ptD_B = 0x00;
    					}
    				}
    			} 
        	}    
      	 }
     }     
  }
  else
  {
      temp_ptS_B  = &port_transmit[CDT9702_PORT][16];    
      temp_ptS_B += (cdt9702_send_order-1)*4;
      for(temp_loop=0;temp_loop<4;temp_loop++)
      {
         *temp_ptD_B    = *temp_ptS_B;
          temp_ptS_B    ++;
          temp_ptD_B    ++;
      }  

      if(cdt9702_send_order >= (cdt9702_send_frame_len-1))
      {
         port_transmit_flag[CDT9702_PORT] = 0x55;
         CDT9702_RLY_REPORT = 0x00;
      }
  }
  CDT9702_send();   //-对于硬件而言,后台和其他通讯口的地位是一样的,仅仅是内部处理信息的时候对象不同而以
}

//-被插的帧若是A、B、C或D帧，则原信息字被取代，原帧长度不变，若是E帧则应在SOE完整字之间插入，帧长度相应加长，见附录A的A5例。
BYTE CDT9702_insert_frame_proc()	//-正常情况下从开机序列开始发送,并周而复始,当出现有些情况的时候,就进行插入发送,这个优先级高
{
    WORD ram_bx;
    
    //-干任何事情都存在一个优先级的问题,可以优先发送的是	遥信变位，遥控/调返校，遥控/调执行确认，遥控/调超时撤消时
    //-同级之间也是不能插入的
    // avoid invalid info STOP UP the transmit channal
    if(port_transmit_flag[CDT9702_PORT] == 0xaa)	//-优先查看是否有传递信息需要处理
    {
        port_transmit_flag[CDT9702_PORT] = 0x55;
        if(port_transmit[CDT9702_PORT][8]==CORE_CODE_YK_VERIFY) 	//-遥控返校报文,说明可以组织向后台发送执行还是取消命令了
        	port_transmit_flag[CDT9702_PORT]=0xaa;	//-这里不组织向后台发送报文,后能还在后面
        else
        {
            if( (port_transmit[CDT9702_PORT][2]==(PROTOCOL_LSA%0x100))
              &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_LSA/0x100)) )
            {  
                port_transmit_flag[CDT9702_PORT] = 0xaa;
            }    
            if( (port_transmit[CDT9702_PORT][2]==(PROTOCOL_DSA301%0x100))
              &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_DSA301/0x100)) )
            {  
                port_transmit_flag[CDT9702_PORT] = 0xaa;
            }    
            if( (port_transmit[CDT9702_PORT][2]==(PROTOCOL_DFP201%0x100))
              &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_DFP201/0x100)) )
            {
                port_transmit_flag[CDT9702_PORT] = 0xaa;
            }    
            if( (port_transmit[CDT9702_PORT][2]==(PROTOCOL_LFP%0x100))
              &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_LFP/0x100)) )
            {
                port_transmit_flag[CDT9702_PORT] = 0xaa;
            }    
            if( (port_transmit[CDT9702_PORT][2]==(PROTOCOL_REC_HOST%0x100))
              &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_REC_HOST/0x100)) )
            {  
                port_transmit_flag[CDT9702_PORT] = 0xaa;
            }       
            if( (port_transmit[CDT9702_PORT][2]==(PROTOCOL_IEC103_LSA%0x100))
              &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_IEC103_LSA/0x100)) )
            {  
                port_transmit_flag[CDT9702_PORT] = 0xaa;
            }   
            if( (port_transmit[CDT9702_PORT][2]==(PROTOCOL_ISA300_CDT%0x100))
              &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_ISA300_CDT/0x100)) )
            {  
                port_transmit_flag[CDT9702_PORT] = 0xaa;
            }   
            if( (port_transmit[CDT9702_PORT][2]==(PROTOCOL_HNBH%0x100))
              &&(port_transmit[CDT9702_PORT][3]==(PROTOCOL_HNBH/0x100)) )
            {  
                port_transmit_flag[CDT9702_PORT] = 0xaa;
            }                          
        }    
    }

    if(BH_Report_Bank_Tak_Ptr[CDT9702_PORT] != BH_Report_Bank_Sav_Ptr)	//-这里恒等于本程序没有使用到
    {
        Core_Src_Unit = BH_Report_Bank_Tak_Ptr[CDT9702_PORT];  //-前面的变量就是当全局变量用的没用实际意义
        core_get_bh_bank_report(CDT9702_PORT);
        BH_Report_Bank_Tak_Ptr[CDT9702_PORT]=Core_Src_Unit;
        cdt9702_control_code=CDT9702_PROTECT_MSG;
                       
        if(Core_Src_Len>2) Core_Src_Len-=2;
        CDT9702_bh_bank_report_len=Core_Src_Len;
        // if trans ACK,NAK len=2,not modified
        cdt9702_send_frame_len = (Core_Src_Len+3)/4+1;
    }
    else
    {
        if( (port_transmit_flag[CDT9702_PORT] == 0xaa)	//-表示本通道内有内容准备发送,但不一定是本规约准备的,但是不管是哪个规约准备的,都必须一个一个来
          &&(port_transmit[CDT9702_PORT][8]!=CORE_CODE_YK_VERIFY))	//-判断准备的内容是否是遥控返校,是的话就进行以下发送,但必须转化成本规约形式,
        {
            cdt9702_control_code   = CDT9702_PROTECT_MSG;
            CDT9702_RLY_REPORT     = 0x01;
            byte0(ram_bx)          = port_transmit[CDT9702_PORT][9];
            byte1(ram_bx)          = port_transmit[CDT9702_PORT][10];
            if(ram_bx>2)  ram_bx-=2;
            CDT9702_protect_report_len=ram_bx;
            cdt9702_send_frame_len = (ram_bx+3)/4+1;         
        }
        else
        {
            if(yx_soe_ex_pt[CDT9702_PORT] != yx_soe_out_pt[CDT9702_PORT])	//-实际改变的帧数与SOE帧已发送出去的帧数进行比较看看是否需要主动上送
            {  //-变位YX最先传送，如果本帧不够，则插入下一个YX帧,SOE可以在每帧前插入，每帧长度为6，一个变位SOE信息字80H、81H连传三遍。
                cdt9702_control_code   = CDT9702_YX_EVENT;	//-事项顺序记录安排在E帧以帧插入方式传送
                if(CDT9702_TRAN_YX_TABLE_NO<4)			//-SOE可以在每帧前插入，每帧长度为6，一个变位SOE信息字80H、81H连传三遍。
                {
                    cdt9702_send_frame_len = 2;
                    CDT9702_soe_send_turn++;
                }
                else
                {
                    if(((yx_event[yx_soe_out_pt[CDT9702_PORT]].soe_chn_sta)&0x0fff)<(CDT9702_TRAN_YX_NUM*16))	//-比较对象号
                    {
                        cdt9702_send_frame_len = 2;
                        CDT9702_soe_send_turn++;	//-需要设定一个指针以便后续的下次传送,计算已经发送的信息字数目
                    }
                    else
                    {
                        cdt9702_send_frame_len = 0;
                          
                        CDT9702_soe_send_turn  = 0;
                        yx_soe_out_pt[CDT9702_PORT] ++;
                        yx_soe_out_pt[CDT9702_PORT] &= 0x3ff;
                    }
                }    
            }
            else 
            {
                return NO;	//-返回这个值说明没有需要插入上送的内容
            }
        }
    }    
    return YES;
}

//-现在找到了一种假设,需要寻找证据来证明他的正确性
void CDT9702_assemble_send_rpt()	//-这个程序就是用来发送报文的
{
  BYTE ram_axl;
    
  if((port_send_pt[CDT9702_PORT]<=port_send_len[CDT9702_PORT]) && (port_send_len[CDT9702_PORT]!=0))	//-说明还有报文没有发送出去,所以暂停处理
     return;

  if(cdt9702_send_order>=cdt9702_send_frame_len)	//-发送的帧数大于需要发送的数目,就进行下次准备
  {
      if((CDT9702_PAGE_ORDER+2)>CDT9702_PAGE_NUM)	//-由于页是从0开始计数的,且里面是记录的刚发完的页号,而后面是共几页所以有一个基数2比较
      {		//-只不过下面的定时处理是一种相对的过程,不是严格意义上的定时
         cdt9702_send_stage++;	//-下一个发送类型
         if(cdt9702_send_stage>30)
            cdt9702_send_stage = 0;
         CDT9702_PAGE_NUM   = 0;
         CDT9702_PAGE_ORDER = 0;
      }
      else
      {
         CDT9702_PAGE_ORDER ++;	//-一组中没发完,继续下一页
      }
      cdt9702_send_info_turn = 0;	//-控制一帧中的进度,,几帧的内容可以称为一页,一页是由同一类型的几帧组成的
      cdt9702_send_order = 0;		//-已发送信息字的个数
      cdt9702_send_frame_len = 1;
  }
  
  switch(cdt9702_send_info_turn)	//-思想都是一样的就是进行,这样的操作,我现在需要痛苦的注释
  {
    case 0: //syn
       cdt9702_yx_chg_send = 0;
       cdt9702_yk_verify_send = 0;
       cdt9702_send_info_turn++;
       CDT9702_send_EB90();	//-仅仅起到一个同步的作用,没有什么特别的
       break;
    case 1: //control
       cdt9702_send_info_turn++;
       if(CDT9702_insert_frame_proc()!=YES)	//-这个程序段得解决是,规约的问题,不是硬件的问题.值为YES说明有东西需要插入上送,有得话就不需要下面的判断了
       {	//-下面程序处理的是正常周期发送的报文,若有需要插入发送的,下面就不执行了
       	   if(HOST_ZF_enable==YES)	//-这个仅仅是一个与后台通讯规约处理标志位之一,本程序没有使用到,所以无效
           {	//-初始状态是NO,但是当下位机都准备好了之后就变为了YES
       	   	if(CDT9702_YC_CHG_COUNT<3)
       	   		CDT9702_send_yc_change();
       	   	else
       	   	{
       	   		CDT9702_YC_CHG_COUNT=0;	
       	   		CDT9702_YC_CHG_FLAG=0x55;
       	   	}
       	   }
       	   
           if(CDT9702_YC_CHG_FLAG==0xaa)	//-在这个系统中用的是这个,与后台通讯的,他才有实际意义,当然无意义的东西留着也无妨,只要不影响功能
           {
           	cdt9702_send_info_turn=0x00;
           	return;
           }
           else
           	CDT9702_YC_CHG_COUNT=0;	//-很多扩展功能都没有使用,硬件是可以了,但是软件并没有使用
       	
       	   //-根据D1帧要求的周期决定A帧重复次数,本规约使用的就是这个最简单的方法进行通讯的,增加的另说
           if((cdt9702_send_stage%3)!=0)	//-每成功发送一帧内容才有可能加1,,这个逻辑也就决定了一些的不同点
           {	//-不是3的倍数的时候,向后台发送遥测值
               if(CDT9702_PAGE_NUM==0)
               {
                   cdt9702_control_code = CDT9702_YC_NORMAL;	//-这儿就是判断发送内容,并置标志
                   if((CDT9702_TRAN_FRAME_IN_1PAGE==0)||(CDT9702_TRAN_YC_NUM<=(CDT9702_TRAN_FRAME_IN_1PAGE*2)))
                       CDT9702_PAGE_NUM=1;
                   else    
                       CDT9702_PAGE_NUM=(CDT9702_TRAN_YC_NUM+CDT9702_TRAN_FRAME_IN_1PAGE*2-1)/(CDT9702_TRAN_FRAME_IN_1PAGE*2);	//-表示共需要几帧一个内容
                   CDT9702_PAGE_ORDER = 0;
                   if(CDT9702_PAGE_NUM>1)
                       cdt9702_send_frame_len = CDT9702_TRAN_FRAME_IN_1PAGE;	//-记录一帧中的信息字数,需要发送的
                   else
                       cdt9702_send_frame_len = (CDT9702_TRAN_YC_NUM+1)/2;
               }
               else    
               {
                   cdt9702_control_code = CDT9702_YC_NORMAL;
                   if((CDT9702_PAGE_ORDER+1)<CDT9702_PAGE_NUM)	//-当是最后一段得时候,可能这帧内容不满
                       cdt9702_send_frame_len = CDT9702_TRAN_FRAME_IN_1PAGE;
                   else
                       cdt9702_send_frame_len = (CDT9702_TRAN_YC_NUM-CDT9702_PAGE_ORDER*CDT9702_TRAN_FRAME_IN_1PAGE*2+1)/2;
               }    
           }
           else  
           {	//-是3的整数倍的时候,向后台发送另几种信息
               if((cdt9702_send_stage%30)!=0)  // add at 08/27/2002
               {    //-不是30的整数倍时,发送遥信和遥脉           	               	   
                   if((cdt9702_yx_send>2)&&(CDT9702_TRAN_YM_NUM !=0))	//-都是一个计时判断而已,发送完2个遥信就判断一下是否有遥脉需要发送
                   {	//-整遥信值连续发送3次之后,才会考虑发送遥脉值
                       if(CDT9702_PAGE_NUM==0)
                       {
                           cdt9702_control_code   = CDT9702_YM_NORMAL;
                           if((CDT9702_TRAN_FRAME_IN_1PAGE==0)||(CDT9702_TRAN_YM_NUM<=CDT9702_TRAN_FRAME_IN_1PAGE))	//-只要有遥脉需要转发至少为1段
                               CDT9702_PAGE_NUM=1;
                           else    
                               CDT9702_PAGE_NUM=(CDT9702_TRAN_YM_NUM+CDT9702_TRAN_FRAME_IN_1PAGE-1)/CDT9702_TRAN_FRAME_IN_1PAGE;	//-得段数

                           CDT9702_PAGE_ORDER = 0;
                           if(CDT9702_PAGE_NUM>1)
                               cdt9702_send_frame_len = CDT9702_TRAN_FRAME_IN_1PAGE;	//-一帧中最多发送一段
                           else
                           {
                               cdt9702_send_frame_len = CDT9702_TRAN_YM_NUM;	//-不满一段的有几个发送一个
                           }    
                       }
                       else
                       {
                           cdt9702_control_code   = CDT9702_YM_NORMAL;
                           if((CDT9702_PAGE_ORDER+1)<CDT9702_PAGE_NUM)	//-要发送的最后一段中很可能就不满一段,所以需要计算实际有几个,在最后一段中
                               cdt9702_send_frame_len = CDT9702_TRAN_FRAME_IN_1PAGE;	//-YC YM 帧信息字长度(每帧中最大信息字长)
                           else
                               cdt9702_send_frame_len = CDT9702_TRAN_YM_NUM-CDT9702_PAGE_ORDER*CDT9702_TRAN_FRAME_IN_1PAGE; //-总个数-有几页*每页最多数
                       }
                   }
                   else
                   {
                       cdt9702_yx_send ++;	//-记录已经发送的遥信数量
                       cdt9702_control_code   = CDT9702_YX_NORMAL;
                       if(yx_chg_ex_pt[CDT9702_PORT] != yx_chg_out_pt[CDT9702_PORT])	//-优先发送变化的遥信
                           cdt9702_send_frame_len = 0;	//-这个值赋0,说明需要从头开始重发,因为有变化的优先上送
                       else    
                           cdt9702_send_frame_len = (CDT9702_TRAN_YX_NUM+1)/2;	//-需要的信息字数目,不足1补1
                   }
               }                
               else  // 30    add at 08/27/2002	,,在这段程序里面是不可能执行的,没法达到30
               {	//-30时发送下面的信息	,,错了上面,这里执行的是30然后是0,然后再次循环,
                   if(CDT9702_YDZT==0xaa)
                   {
                       CDT9702_YDZT = 0x55;
                       cdt9702_control_code   = CDT9702_RTU_STATUS;	//-主动上送设备状态,并且是定时发送
                       cdt9702_send_frame_len = IED_TX_STATE_DWORD_NUM;
                   }
                   else
                   {
                       CDT9702_YDZT = 0xaa;
                       cdt9702_control_code   = CDT9702_UPSET_CLK;	//-时间
                       if(byte1(port_info[CDT9702_PORT].time_enable)!=0x55)	//-不等于55表示允许校时
                       {  
                           cdt9702_send_frame_len = 2;
                       }
                       else    
                       {  
                           cdt9702_send_frame_len = 0;
                       }
                   } 
               }
           }   
       }
       if(HOST_ZF_enable!=YES)	//-我猜还是后台转发的意思,为YES说明可以组织转发
          cdt9702_send_frame_len = 0;	//-这里是为了可靠,不等于YES说明不能发送,所以长度强制为0,应该不是没有配置,而是由于,下位机没有准备好所以,发的0
       CDT9702_send_control_byte();	//-有一部分是,仅仅执行了这个控制字就放弃的,可能本身就不对,或者,没有信息字.
       break;
    case 2:  
       switch(cdt9702_control_code)	//-由于至少这里用到的报文都是一问一答的,所以处理的报文都是发送的返回,这个值就是发送时的功能码
       {							//-这个好像有点不同,这里是发送而不是接收处理
          case CDT9702_YC_NORMAL:	//-这些报文很有可能就是主动上送的了
               ram_axl=0;	//-这个逻辑就是保证替代原来的信息字,而不是插入
               
               if(  (yx_chg_ex_pt[CDT9702_PORT] != yx_chg_out_pt[CDT9702_PORT])	//-遥测和遥信是循环发送的,但是变化的遥信优先级更高
                 &&((cdt9702_send_order+3-cdt9702_yx_chg_send)<cdt9702_send_frame_len)	//-需要重复3遍,如果本帧不够，则插入下一个YX帧
                 )
               {	//-被插的帧若是A、B、C或D帧，则原信息字被取代，原帧长度不变，若是E帧则应在SOE完整字之间插入，帧长度相应加长
                   if(CDT9702_TRAN_YX_TABLE_NO<4)
                   {
                       CDT9702_send_YX_chg();
                       cdt9702_yx_chg_send++;
                       ram_axl=1;
                   }
                   else
                   {
                       if((byte0(yx_change[yx_chg_out_pt[CDT9702_PORT]].offset_no))<((CDT9702_TRAN_YX_NUM+1)/2)) //-保证在偏移量内才去发送	
                       {
                           CDT9702_send_YX_chg();	//-每次发送的是一个信息字
                           cdt9702_yx_chg_send++;
                           ram_axl=1;
                       }
                       else
                       {
                           cdt9702_yx_chg_send=3;
                       }
                   }
                   
                   if(cdt9702_yx_chg_send>2)
                   {
                       yx_chg_out_pt[CDT9702_PORT] ++;	//-表示成功发送出去一个信息
                       yx_chg_out_pt[CDT9702_PORT] &= 0xff;
                       cdt9702_yx_chg_send = 0;
                       if(yx_chg_ex_pt[CDT9702_PORT] == yx_chg_out_pt[CDT9702_PORT])
                       {
                           CDT9702_deal_YX_state();
                       }
                   }
               }
               else
               {
                   if(port_transmit_flag[CDT9702_PORT] == 0xaa)	//-处理其他端口传递过来的信息
                   {
                       if(port_transmit[CDT9702_PORT][8]==CORE_CODE_YK_VERIFY)
                       {
                           if((cdt9702_send_order+3-cdt9702_yk_verify_send)<cdt9702_send_frame_len)	//-因为需要连续发送3遍一样的信息字,所以帧长度需要够
                           {
                               CDT9702_send_YK_verify_from_transmit();	//-这个发送的内容是发向后台的,组织的内容并不一定都是这个文件组织的,也有可能是其他规约口
                               cdt9702_yk_verify_send++;
                               if(cdt9702_yk_verify_send>2)	//-控制连续发送三遍同样信息字的目的,,这个再一步说明了,必须知道实际的目的否则没法理解,到了应用领域
                               {
                                   port_transmit_flag[CDT9702_PORT] = 0x55;
                                   cdt9702_yk_verify_send=0;
                                   HOST_YK_Doing_Begin_Time=Time_1ms_Counter;
                                   HOST_YK_Doing=2;	//-表示遥控返校命令已经发送给后台了,在一定的有效时间内,只要后台发来执行命令就可以了
                               }
                               ram_axl=1;
                           }
                       }
                   }
               }
               
               if(ram_axl==0)	//-值为1说明有东西发送了,如果没有东西的话,发送正常的遥测值
               {
                   CDT9702_send_YC();	//-当变位遥信替代空间还有多余的话,继续发送遥测值
               }
               break;
          case CDT9702_PROTECT_MSG:
               if(CDT9702_RLY_REPORT==0x01)	//-也是准备的时候设定的一个标识位
               {
                   CDT9702_send_bh_report();
               }
               else
               {
                   CDT9702_send_bh_bank();
               }
               break;
          case CDT9702_YX_EVENT:
               CDT9702_send_YX_SOE();	//-这里才是真正的对信息字的发送
               break;   
          case CDT9702_YM_NORMAL:
               CDT9702_send_YM();
               break;   
          case CDT9702_YX_NORMAL:
               CDT9702_send_YX();
               break;
          case CDT9702_RTU_STATUS:
               CDT9702_send_YDZT();	//-发送用的状态
               break;
          case CDT9702_UPSET_CLK:
               CDT9702_send_clk();
               break;   
               
          default:
               break;     
       }
       cdt9702_send_order ++;	//-信息字计数,是单位最小的,上面还有帧,还有页
       break;
    default:
       break;
  }
       
}

void CDT9702_transmit_YK()	//-这里是进行信息的传递,根据本端口收到的报文,进行信息的传递
{
  BYTE *temp_pt;
  
  temp_pt = &port_deal_buf[CDT9702_PORT][0];	//-需要处理的信息字已经提取到了这个里面
  for(temp_loop=0;temp_loop<5;temp_loop++)
  {	//-检查接收到的内容是否正确
     if( (*temp_pt!=*(temp_pt+5))||(*temp_pt!=*(temp_pt+10)) )	//-确保三个信息字都一样
         temp_loop = 100;    
     temp_pt++;      
  }
  
  if(temp_loop<6)
  {	//-若正常的话就应该小于6值为5
      exchange_buf[1] = CDT9702_PORT;         // source port	,组织内容的端口号
      exchange_buf[2] = PROTOCOL_CDT_XT9702%0x100;  // source protocol	,,前面缓冲区没有特殊定义,只要互异就行
      exchange_buf[3] = PROTOCOL_CDT_XT9702/0x100;  // source protocol
      
      temp_pt = &port_deal_buf[CDT9702_PORT][0];	//-功能码
      if(*temp_pt==0x7A)
              exchange_buf[8] = CORE_CODE_YK_CHOOSE;  // CMD    operate type
      else if(*temp_pt==0x7B)
              exchange_buf[8] = CORE_CODE_YK_EXECUTE;  // CMD    operate type
      else
              exchange_buf[8] = CORE_CODE_YK_CANCEL;  // CMD    operate type
      
      exchange_buf[9] = 0x02;             // LEN_L	,,还有16和17俩个单元的内容需要传递
      exchange_buf[10]= 0x00;             // LEN_H
      
      temp_pt = &port_deal_buf[CDT9702_PORT][1];	//-存放的是动作码
      if(exchange_buf[8]==CORE_CODE_YK_CHOOSE)	
      {
          if(*temp_pt==0xcc)
              exchange_buf[16] = CORE_CODE2_YK_CLOSE;  // CMD    action type  合
          else
              exchange_buf[16] = CORE_CODE2_YK_TRIP;   // CMD    action type  分
      }
      else
      {
          if(exchange_buf[8]==CORE_CODE_YK_EXECUTE)
              exchange_buf[16] = CORE_CODE2_YK_EXECUTE;  // CMD    action type
          else
              exchange_buf[16] = CORE_CODE2_YK_CANCEL;   // CMD    action type
      }
      
      exchange_buf[5] =port_deal_buf[CDT9702_PORT][2]-MON_FUN_NO[CDT9702_PORT][2];  // YK   object No.	对象号
      exchange_buf[17]=0xff;  
//      Core_YK_CDTBB_ObjectNo_To_UnitAddr();  // input  [5] YK  ObjNo, [10] 0xff;
                                               // output [5] Unit Addr, [10] YK No.
      
      if(Ex_YK_CDTBB_ObjectNo_To_UnitAddr()==YES)  // if  YK object No. not exist do not transmit_info	,这里获得了把信息传递给的端口对象号
      {
          port_transmit[exchange_target_port][0]=PORT_EXCHANGE_STA_START;	//-也是标志位的一种
          Ex_Produce_Transmit_Info();	//-向需要的端口传送信息
 		  //-上面对从端口(后台)接收到的信息进行处理,之后准备的内容存放到了,需要的其他端口,但实际启动发送需要根据其他端口特定规约自己再组织	
      }
  }
}

void CDT9702_exchange_lsa_report()
{
  #define   LSA_CODE_ACK                    0x06
  #define   LSA_CODE_NAK                    0x15
  unsigned char  *temp_pt,*temp_pts;
  WORD            temp_port,temp_word,sumword;
  
  temp_pt = &port_deal_buf[CDT9702_PORT][0];
  temp_word = *(temp_pt+4);
  for(temp_port=0;temp_port<14;temp_port++)
  {
      if((port_info[temp_port].protocal_type==PROTOCOL_LSA)
       ||(port_info[temp_port].protocal_type==PROTOCOL_CAN_DSA)
       ||(port_info[temp_port].protocal_type==PROTOCOL_DSA301)
       ||(port_info[temp_port].protocal_type==PROTOCOL_REC_HOST))
      {
          if(   (temp_word>=byte0(port_info[temp_port].scan_panel))
             && (temp_word<=byte1(port_info[temp_port].scan_panel)) )
              temp_port += 100;
      }        
  }
  
  if(temp_port<90)
     return;
     
  temp_port -= 101;
  if(port_transmit_flag[temp_port] == 0xaa)
     return;
  port_transmit_flag[temp_port] = 0xaa;

  temp_pts = &port_transmit[temp_port][0];

  *temp_pts = 0x00;          temp_pts++; //0
  *temp_pts = CDT9702_PORT;  temp_pts++; //1
  *temp_pts = PROTOCOL_CDT_XT9702%0x100;   temp_pts++; //2
//  if(temp_port<12)
//     *temp_pts = PROTOCOL_CAN_DSA;
//  else   
//     *temp_pts = PROTOCOL_CAN_DSA;
  *temp_pts = PROTOCOL_CDT_XT9702/0x100;   temp_pts++; //3
  *temp_pts = *(temp_pt+4);  temp_pts++; //4
  *temp_pts = 0x00;          temp_pts++; //5
  *temp_pts = 0x72;          temp_pts++; //6
  *temp_pts = 0x00;          temp_pts++; //7
  *temp_pts = *(temp_pt+5);  temp_pts++; //8

   if(((*(temp_pt+5))==LSA_CODE_ACK)||((*(temp_pt+5))==LSA_CODE_NAK))
   {
      *temp_pts = 0x02;          temp_pts++;   //9
      *temp_pts = 0x00;          temp_pts+=6;  //10
      *temp_pts = *(temp_pt+4);  temp_pts++;   //16
      *temp_pts = *(temp_pt+5);                //17
   }
   else
   {
       byte0(temp_word)=*(temp_pt+2);
       byte1(temp_word)=*(temp_pt+3);
       if(temp_word>500) temp_word=500;
       sumword=temp_word+2;
      *temp_pts = byte0(sumword);  temp_pts++; //9
      *temp_pts = byte1(sumword);              //10
       temp_pt += 4;
       temp_pts+= 6;
       sumword=0;
       for(temp_port=0;temp_port<temp_word;temp_port++)
       { 
           if(temp_port!=0) sumword += *temp_pt;
          *temp_pts = *temp_pt;
           temp_pts++; 
           temp_pt++;
       }
      
      *temp_pts=byte0(sumword);
       temp_pts++; 
      *temp_pts=byte1(sumword);
  }    
}

void CDT9702_exchange_dfp201_report()
{
    BYTE  *temp_pt,*temp_pts;
    WORD   temp_port,temp_word;
  
    temp_pt   = &port_deal_buf[CDT9702_PORT][0];
    for(temp_port=0;temp_port<12;temp_port++)
    {
        if(port_info[temp_port].protocal_type==PROTOCOL_DFP201)
        {
           if(   (temp_pt[4]>=byte0(port_info[temp_port].scan_panel))
              && (temp_pt[4]<=byte1(port_info[temp_port].scan_panel))
             )
               temp_port += 100;
        }        
    }
  
    if(temp_port<90)
       return;
     
    temp_port -= 101;
    if(port_transmit_flag[temp_port] != 0xaa)
    {
        port_transmit_flag[temp_port] = 0xaa;

        temp_pts = &port_transmit[temp_port][0];

        temp_pts[0] = 0x00;                       
        temp_pts[1] = CDT9702_PORT;               
        temp_pts[2] = PROTOCOL_CDT_XT9702%0x100;  
        temp_pts[3] = PROTOCOL_CDT_XT9702/0x100; 

        temp_pts[4] = 0x00;
        temp_pts[5] = 0x00;
        temp_pts[6] = 0x73;
        temp_pts[7] = 0x00;
        temp_pts[8] = temp_pt[6];
        byte0(temp_word)=temp_pt[2];
        byte1(temp_word)=temp_pt[3];
        if(temp_word>500) temp_word=500;
        temp_pts[9] = byte0(temp_word); 
        temp_pts[10]= byte1(temp_word); 
        temp_pts = &port_transmit[temp_port][16];
        for(temp_port=0;temp_port<temp_word;temp_port++)
        {
            temp_pts[temp_port]=temp_pt[temp_port+4];
        }
    }    
} 

void CDT9702_exchange_lfp_report()
{
    BYTE  *temp_pt,*temp_pts;
    WORD   temp_port,temp_word;
  
    temp_pt   = &port_deal_buf[CDT9702_PORT][0];
    for(temp_port=0;temp_port<12;temp_port++)
    {
        if(port_info[temp_port].protocal_type==PROTOCOL_LFP)	//-匹配端口规约
        {
           if(   (temp_pt[4]>=byte0(port_info[temp_port].scan_panel))
              && (temp_pt[4]<=byte1(port_info[temp_port].scan_panel))	//-所管理设备的地址范围
             )
               temp_port += 100;	//-呵呵,这种方法已经不是第一次看到了
        }        
    }
  
    if(temp_port<90)
       return;
     
    temp_port -= 101;	//-规约匹配端口号,即找到了特定规约的端口
    if(port_transmit_flag[temp_port] != 0xaa)
    {
        port_transmit_flag[temp_port] = 0xaa;	//-表示有东西需要发送了

        temp_pts = &port_transmit[temp_port][0];	//-这个端口处理的数据,直接保存到需要发送数据的端口缓冲区进行发送

        temp_pts[0] = 0x00;                       
        temp_pts[1] = CDT9702_PORT;               
        temp_pts[2] = PROTOCOL_CDT_XT9702%0x100;  
        temp_pts[3] = PROTOCOL_CDT_XT9702/0x100; 

        temp_pts[4] = 0x00;
        temp_pts[5] = 0x00;
        temp_pts[6] = 0x70;
        temp_pts[7] = 0x00;
        temp_pts[8] = temp_pt[5];
        byte0(temp_word)=temp_pt[2];
        byte1(temp_word)=temp_pt[3];
        if(temp_word>500) temp_word=500;
        temp_pts[9] = byte0(temp_word); 
        temp_pts[10]= byte1(temp_word); 
        temp_pts = &port_transmit[temp_port][16];
        for(temp_port=0;temp_port<temp_word;temp_port++)
        {
            temp_pts[temp_port]=temp_pt[temp_port+4];	//-其实这些地方对于我来说已经没有不可解决的东西了,但是还是看不懂,原因就是实际含义不明白
        }												//-语言会组织,但是不知道怎么去组织
    }    
}
/******************************************************************************/
/* HOSTZF_exchange_hnbh_report 函数 从HOSTZF规约收到调定值命令向其他口转发  */
/******************************************************************************/
void CDT9702_exchange_hnbh_report()
{
    BYTE  *temp_pt,*temp_pts;
    WORD  temp_port,temp_word,sumword;
    
    temp_pt = &port_deal_buf[CDT9702_PORT][0];
    temp_word = temp_pt[7];
    for (temp_port = 0; temp_port < 12; temp_port ++)   //在串口中找规约
    {
        if (port_info[temp_port].protocal_type == PROTOCOL_HNBH)
        {
            if ((temp_word >= byte0(port_info[temp_port].scan_panel))   //判柜号
            && (temp_word <= byte1(port_info[temp_port].scan_panel)))
            {
                temp_port += 100;   //找到跳出循环
            }       
        }        
    }
    if (temp_port < 90)
    {
        return;
    }   
    temp_port -= 101;   //实际串口号
    if (port_transmit_flag[temp_port] == 0xaa)   //正忙
    {
        return;
    }
    port_transmit_flag[temp_port] = 0xaa;   
    temp_pts = &port_transmit[temp_port][0];    
    *temp_pts = 0x00;                            temp_pts ++;  //0
    *temp_pts = CDT9702_PORT;                     temp_pts ++;  //1
    *temp_pts = PROTOCOL_CDT_XT9702 % 0x100;     temp_pts ++;  //2
    *temp_pts = PROTOCOL_CDT_XT9702 / 0x100;     temp_pts ++;  //3
    *temp_pts = temp_pt[7];                      temp_pts ++;  //4柜号
    *temp_pts = 0x00;                            temp_pts ++;  //5
    *temp_pts = 0x73;                            temp_pts ++;  //6
    *temp_pts = 0x01;                            temp_pts ++;  //7
    *temp_pts = temp_pt[9];                      temp_pts ++;  //8特征码                                                              
    temp_word = (WORD)temp_pt[6] + 2;                          //长度不加两位校验
    *temp_pts = byte0(temp_word);                temp_pts ++;  //9
    *temp_pts = byte1(temp_word);                temp_pts += 6;//10
    temp_pt = &port_deal_buf[CDT9702_PORT][0];    
    for (temp_port = 0; temp_port < temp_word; temp_port ++)
    { 
        *temp_pts = temp_pt[4];
        temp_pts ++; 
        temp_pt ++;
    }
} 
void CDT9702_exchange_lsa18x_report()
{
    BYTE  *temp_pt,*temp_pts;
    WORD   temp_port,temp_word;
  
    temp_pt   = &port_deal_buf[CDT9702_PORT][0];
    for(temp_port=0;temp_port<12;temp_port++)
    {
        if(port_info[temp_port].protocal_type==PROTOCOL_IEC103_LSA)
        {
           temp_port += 100;
        }        
    }
  
    if(temp_port<90)
       return;
     
    temp_port -= 101;
    if(port_transmit_flag[temp_port] != 0xaa)
    {
        port_transmit_flag[temp_port] = 0xaa;

        temp_pts = &port_transmit[temp_port][0];

        temp_pts[0] = 0x00;                       
        temp_pts[1] = CDT9702_PORT;               
        temp_pts[2] = PROTOCOL_CDT_XT9702%0x100;  
        temp_pts[3] = PROTOCOL_CDT_XT9702/0x100; 

        temp_pts[4] = 0x00;
        temp_pts[5] = 0x00;
        temp_pts[6] = 0x74;
        temp_pts[7] = 0x00;
        temp_pts[8] = temp_pt[10];
        byte0(temp_word)=temp_pt[2];
        byte1(temp_word)=temp_pt[3];
        if(temp_word>500) temp_word=500;
        temp_pts[9] = byte0(temp_word); 
        temp_pts[10]= byte1(temp_word); 
        temp_pts = &port_transmit[temp_port][16];
        for(temp_port=0;temp_port<temp_word;temp_port++)
        {
            temp_pts[temp_port]=temp_pt[temp_port+4];
        }
    }    
} 

void CDT9702_exchange_Isa2_report()
{
  #define   ISA_CODE_ACK                    0x06
  #define   ISA_CODE_NAK                    0x15
  unsigned char  *temp_pt,*temp_pts;
  WORD            temp_port,temp_word,sumword;
  
  temp_pt = &port_deal_buf[CDT9702_PORT][0];
  temp_word = *(temp_pt+4);
  for(temp_port=0;temp_port<14;temp_port++)
  {
      if((port_info[temp_port].protocal_type==PROTOCOL_ISA300_CDT))
       
      {
          if(   (temp_word>=byte0(port_info[temp_port].scan_panel))
             && (temp_word<=byte1(port_info[temp_port].scan_panel)) )
              temp_port += 100;
      }        
  }
  
  if(temp_port<90)
     return;
     
  temp_port -= 101;
  if(port_transmit_flag[temp_port] == 0xaa)
     return;
  port_transmit_flag[temp_port] = 0xaa;

  temp_pts = &port_transmit[temp_port][0];

  *temp_pts = 0x00;          temp_pts++; //0
  *temp_pts = CDT9702_PORT;  temp_pts++; //1
  *temp_pts = PROTOCOL_CDT_XT9702%0x100;   temp_pts++; //2
//  if(temp_port<12)
//     *temp_pts = PROTOCOL_CAN_DSA;
//  else   
//     *temp_pts = PROTOCOL_CAN_DSA;
  *temp_pts = PROTOCOL_CDT_XT9702/0x100;   temp_pts++; //3
  *temp_pts = *(temp_pt+4);  temp_pts++; //4
  *temp_pts = 0x00;          temp_pts++; //5
  *temp_pts = 0x00;          temp_pts++; //6
  *temp_pts = 0x00;          temp_pts++; //7
  *temp_pts = *(temp_pt+8);  temp_pts++; //8

  
       byte0(temp_word)=*(temp_pt+2);
       byte1(temp_word)=*(temp_pt+3);
       if(temp_word>500) temp_word=500;
       sumword=temp_word+2;
      *temp_pts = byte0(sumword);  temp_pts++; //9
      *temp_pts = byte1(sumword);              //10
       temp_pt += 4;
       temp_pts+= 6;
       sumword=0;
       for(temp_port=0;temp_port<temp_word;temp_port++)
       { 
           if(temp_port!=0) sumword += *temp_pt;
          *temp_pts = *temp_pt;
           temp_pts++; 
           temp_pt++;
       }
      
      *temp_pts=byte0(sumword);
       temp_pts++; 
      *temp_pts=byte1(sumword);
      
}


void CDT9702_exchange_jz_report()
{
    BYTE  *temp_pt,*temp_pts;
    WORD   temp_port,temp_word;
  
    temp_pt   = &port_deal_buf[CDT9702_PORT][0];
    for(temp_port=0;temp_port<12;temp_port++)
    {
        if( (port_info[temp_port].protocal_type==PROTOCOL_JZPLC)
          &&(temp_pt[4]==byte0(port_info[temp_port].scan_panel)) )
        {
            temp_port += 100;
        }        
    }
  
    if(temp_port<90)
       return;
     
    temp_port -= 101;
    if(port_transmit_flag[temp_port] != 0xaa)
    {
        port_transmit_flag[temp_port] = 0xaa;

        temp_pts = &port_transmit[temp_port][0];

        temp_pts[0] = 0x00;                       
        temp_pts[1] = CDT9702_PORT;               
        temp_pts[2] = PROTOCOL_CDT_XT9702%0x100;  
        temp_pts[3] = PROTOCOL_CDT_XT9702/0x100; 

        byte0(temp_word)=*(temp_pt+2);
        byte1(temp_word)=*(temp_pt+3);
        if(temp_word>500) temp_word=500;
        if(temp_word>2) temp_word-=2;
        temp_pts[9] = byte0(temp_word); 
        temp_pts[10]= byte1(temp_word); 
        temp_pts = &port_transmit[temp_port][16];
        for(temp_port=0;temp_port<temp_word;temp_port++)
        {
            temp_pts[temp_port]=temp_pt[temp_port+6];
        }
    }    
} 
//-控制字节		E	L	S	D	0	0	0	1
//-E: 扩展位。当E=0 时使用表2已定义的帧类别；当E=1 时帧类别可另行定义，以便扩展功能
//-L: 帧长度定义位。当L=0时表示本帧信息字数n为0，既本帧没有信息字；当L=1 时表示本帧有信息
//-S：源站址定义位
//-D：目的站址定义位 
//-所谓的下行报文,是根据什么来的啊,谁向谁发送报文,,高位机向下位机发送报文叫下行,这个是相对的
//禁用 delta_len(temp_int) temp_ptS temp_ptD
void CDT9702_analysis_control()	//-控制字的处理
{
  cdt9702_rec_frame_len = port_report[2];	//-信息字 数目
  cdt9702_rec_stage     = 2;	//-可以进入阶段2,信息字处理了
  byte0(cdt9702_rec_frame_type) = port_report[1];	//-帧类别
  byte1(cdt9702_rec_frame_type) = port_report[0];	//-控制字节
  cdt9702_rec_info_num = 0;		//-记录已处理的信息字个数
}

//禁用 delta_len(temp_int) temp_ptS temp_ptD
void CDT9702_analysis_info()	//-最终接受的信息字的处理程序
{
  BYTE   *temp_pt;
  
  switch(cdt9702_rec_frame_type)	//-这里的下行报文是正对后台而言的,只有后台电脑能够发送这些命令
  {
     case 0xF161:	//-遥控选择,,控制字 	固定为0F1H，表示本规约是“循环式远动规约DL451-91”的扩充。
     	if((port_report[0]> 0x79)&&(port_report[0]< 0x7D))//YK	,功能码
          {
              temp_pt = &port_deal_buf[CDT9702_PORT][cdt9702_rec_info_num*5];	//-去掉一个校验字节,有用信息就是5个一块
              for(temp_loop=0;temp_loop<5;temp_loop++)
              {
                 *temp_pt = port_report[temp_loop];
                  temp_pt ++;
              }
              
              if(cdt9702_rec_info_num == 2)	//-只有接收完所有的信息字内容,之后才开始处理,但是必须每个校验都通过
              {
                  if((HOST_YK_Doing==0)&&(port_report[0]==0x7A))	//-下行报文的时候7A功能码表示遥控选择,前面的0表示什么还没有做,可能就是进程的一种标识
                  {
                      HOST_YK_Doing=1;	//-表示现在开始等待遥控返校,或选择已经进行了遥控选择
                      HOST_YK_Port_No=CDT9702_PORT;	//-记录哪个口收到遥控选择命令报文
                      HOST_YK_Doing_Begin_Time=Time_1ms_Counter;	//-开始收到报文的时间,可能用于判断有效时间

                      CDT9702_transmit_YK();	//-这个是对上位机报文做出的回应
                  }
                  else
                  {
                      if( (HOST_YK_Doing==2) && (port_report[0]!=0x7A) && (HOST_YK_Port_No==CDT9702_PORT) )	//-后台发来遥控执行或遥控臭小撤销命令,
                      {
                          HOST_YK_Doing=0;	//-表示现在处于遥控进程的未选择状态

                          CDT9702_transmit_YK();	//-组织传递的信息
                      }    
                  }        
              }        
          }
          break;
     //-上下是不同的帧类型,但是其中任意一个帧类型都可以实现遥控的所有命令,也许这就是程序的缺陷所在,或者说目的所在	
     case 0xF1c2:	//-遥控执行
     case 0xF1b3:	//-遥控撤消
          if((port_report[0]> 0x79)&&(port_report[0]< 0x7D))//YK
          {
              temp_pt = &port_deal_buf[CDT9702_PORT][cdt9702_rec_info_num*5];
              for(temp_loop=0;temp_loop<5;temp_loop++)
              {
                 *temp_pt = port_report[temp_loop];
                  temp_pt ++;
              }
              if(cdt9702_rec_info_num == 2)
              {
                  if((HOST_YK_Doing==0)&&(port_report[0]==0x7A))
                  {
                      HOST_YK_Doing=1;
                      HOST_YK_Port_No=CDT9702_PORT;
                      HOST_YK_Doing_Begin_Time=Time_1ms_Counter;

                      CDT9702_transmit_YK();
                  }
                  else
                  {
                      if( (HOST_YK_Doing==2) && (port_report[0]!=0x7A) && (HOST_YK_Port_No==CDT9702_PORT) )
                      {
                          HOST_YK_Doing=0;

                          CDT9702_transmit_YK();	//-有些地方解释不通,很正常,需要调整思路,这个是精简过的程序
                      }    
                  }        
              }        
          }
          break;
     case 0xF17a:  //time	,设置时钟
          mon_ok_flag[CDT9702_PORT]=YES;
          mon_err_start_time[CDT9702_PORT]=Time_2048ms_Counter;
          if(unit_info[255].yx_num!=0)
       	      YX_State[unit_info[255].yx_start]&=(~(0x0001<<CDT9702_PORT));	//-留下了这么多的接口,想想没有使用的完整程序是多么强大
          if(port_report[0]==0x00)	//-对功能码的判断
          {
              CDT9702_time_tmp_MSL=port_report[1];	//-毫秒
              CDT9702_time_tmp_MSH=port_report[2];
              CDT9702_time_tmp_SEC=port_report[3];	//-秒
              CDT9702_time_tmp_MIN=port_report[4];	//-分
          }
          else
          {
              if(port_report[0]==0x01)	//-功能码,决定程序走向,有些东西很自由,千万不要死
              {
                  if(byte0(port_info[CDT9702_PORT].time_enable)!=0x55)	//-值为AA表示允许接收校时
                  {
                      disable();
                  
                      REG_1Msecond=(CDT9702_time_tmp_MS % 1000) + P554_XTAL16M_CLOSE_RTS_DELAY[port_info[CDT9702_PORT].bauds]*20;
                      REG_Second=CDT9702_time_tmp_SEC % 60;
                      REG_Minute=CDT9702_time_tmp_MIN % 60;
                      REG_Hour  =port_report[1] % 24;	//-时
                      REG_Date  =port_report[2] % 32;	//-日
                      REG_Month =port_report[3] % 13;	//-月
                      REG_Year  =(port_report[4] % 100)+1830+byte0(port_info[CDT9702_PORT].time_enable);	//-年
                  
                      Clock_Process();
                  
                      Write_Time_To_Dallas();	//-把接收到的值向时钟芯片中写入
                  
                      enable();
                  }
              }
          }
        
          break;
     case 0xF13D:	//-	保护命令
           temp_pt = &port_deal_buf[CDT9702_PORT][port_report[0]*4];
          *temp_pt = port_report[1];temp_pt++;	//-这些可能不仅仅与标准规约有关,还与实际装置通讯程序有关
          *temp_pt = port_report[2];temp_pt++;	//-	通信标志		主动标志		保护类型
          *temp_pt = port_report[3];temp_pt++;	//-	保护有效报文长度低8位
          *temp_pt = port_report[4];			//-	保护有效报文长度高8位
          if(port_report[0]>=(cdt9702_rec_frame_len-1))	//-我现在看的规约内容和编程依据的规约有偏差
          {
            switch(port_deal_buf[CDT9702_PORT][1] & 0x3f)	//-取出保护类型的值
             {
              case 10:         // LSA
               CDT9702_exchange_lsa_report();
               break;
              case 13:         // JZ
               CDT9702_exchange_jz_report();
               break;
              case 3:         // DFP201
               CDT9702_exchange_dfp201_report();
               break;
              case 2:         // LFP
               CDT9702_exchange_lfp_report();
               break;
              case 14:         // LSA18X
               CDT9702_exchange_lsa18x_report();
               break;
              case 9:
               CDT9702_exchange_Isa2_report();
               break;
              case 0x10:
              	CDT9702_exchange_hnbh_report();
              break;	
              default:
               break;
             }
              
              cdt9702_rec_frame_len = 0;
          }
          break;
     default:
          break;
  }
  cdt9702_rec_info_num ++;	//-数值表示已经接收到的信息字的个数
  if(cdt9702_rec_info_num >= cdt9702_rec_frame_len)
     cdt9702_rec_syn_flag = 0x55;	//-达到或超过信息字个数后同步失效
}

//-这个处理报文和其他的有点不同,其他的有个问答的形式,如果没有问,即使收到报文也不处理
void CDT9702_deal_rpt()
{
//temp_loop   同步字数
//temp_int    待查字长
//temp_ptS    当前位置
//temp_ptD    回零位置
//temp_lp_int 同步字
//port_check  已过滤字数
#define synword_num  temp_loop  
#define delta_len    temp_int
#define synword      temp_lp_int  
#define filter_num   port_check

  synword_num   = 0;
  synword       = SYN_WORD_CDT_EB90[0];
  filter_num    = 0;
  //-作为一个端口,他处理的信息也是来自本端口接收到的缓冲区的
  disable();
  if(port_recv_dl[CDT9702_PORT]>port_recv_pt[CDT9702_PORT])	//-如果要谈到接收报文,肯定就是后台发来的
     delta_len = (port_recv_pt[CDT9702_PORT]+512) - port_recv_dl[CDT9702_PORT];
  else
     delta_len = port_recv_pt[CDT9702_PORT] - port_recv_dl[CDT9702_PORT];	//-计算需要处理的长度
  enable();

  if(delta_len<6)	//-长度至少为6才有处理的必要.
     return;

  temp_ptS_B = &port_recv[CDT9702_PORT][port_recv_dl[CDT9702_PORT]];	//-实际接收的内容存储在这个里面
  temp_ptD_B = &port_recv[CDT9702_PORT][511];	//-缓冲区边界值,回零位置

  if(cdt9702_rec_syn_flag != 0xaa)	//-不等于aa说明还没有同步,需要这样处理的原因是,一个同步头可能跟若干个信息字,单接收
  {// find EB				,,处理不是在一个循环层次之间的
    while(synword_num<6)	//-同步字数
    {
      while(*temp_ptS_B != synword)	//-同步字
      {
         synword_num = 0;	//-这里说明同步必须连续才有效
         temp_ptS_B   ++;
         filter_num ++;		//-这个表示过滤字节的总数,同步成功才会舍弃一个字节报文
         if(temp_ptS_B>temp_ptD_B)
            temp_ptS_B -= 512;
         delta_len  --;
         if(delta_len<6)
         {
            port_recv_dl[CDT9702_PORT] += filter_num;
            if(port_recv_dl[CDT9702_PORT]>511)
                port_recv_dl[CDT9702_PORT] -= 512;
            return;
         }
         synword = SYN_WORD_CDT_EB90[0];
      }
      synword_num++;
      synword = SYN_WORD_CDT_EB90[synword_num];
      temp_ptS_B++;
      filter_num++;	//-表示到目前为止总共过滤了的字节数
      if(temp_ptS_B>temp_ptD_B)
         temp_ptS_B -= 512;	//-循环处理的目的
      delta_len--;	//-去掉一个失效字节
      if(delta_len<(unsigned short)(6-synword_num))	//-猜,当小可以同步的字数都小于需要的字数的时候肯定是不同步了
      {
         filter_num -= synword_num;
         port_recv_dl[CDT9702_PORT] += filter_num;	//-过滤过的内容都去掉,没有的保留
         if(port_recv_dl[CDT9702_PORT]>511)
            port_recv_dl[CDT9702_PORT] -= 512;
         return;
      }
    }  // <6
    port_recv_dl[CDT9702_PORT] += filter_num;
    if(port_recv_dl[CDT9702_PORT]>511)
       port_recv_dl[CDT9702_PORT] -= 512;
    cdt9702_rec_stage = 1; 		//-0阶段已经结束了,接下来要处理的是1阶段
    cdt9702_rec_syn_flag = 0xaa;	//-到这里说明了同步成功
  }

  while(delta_len>=6)	//-由于这个特定规约的缘故,后续处理也必须是至少有6个字节内容,而且绝大多数情况下不是同次受到的
  {
//禁用 delta_len(temp_int) temp_ptS temp_ptD
     for(temp_loop1=0;temp_loop1<6;temp_loop1++)
     {	//-把需要处理的值取到处理缓冲区内,这样可能便于模块化处理
       port_report[temp_loop1] = *temp_ptS_B;	//-一次性取出6字节的内容供处理
       temp_ptS_B++;
       if(temp_ptS_B>temp_ptD_B)	//-始终是循环的思想,,到达缓冲区末尾时,返回到0起点
          temp_ptS_B -= 512;
     }
     //-下面检查BCH就是模块化处理的最好佐证
     if(CDT9702_check_BCH() == 0xaa)	//-本规约的特点就是每6个字节就有一个校验,非常可靠,,校验通过了,才有处理的必要
     {
         port_recv_dl[CDT9702_PORT] += 6;	//-认为处理成功,也记录下来了,所以释放空间
         if(port_recv_dl[CDT9702_PORT]>511)	//-这个是循环转圈的所以不一定就是回到0位置
            port_recv_dl[CDT9702_PORT] -= 512;
         delta_len -= 6;	//-又去掉6个无效字节
         
         if(cdt9702_rec_stage==1)
             CDT9702_analysis_control();	//-分析接收到的报文的控制字
         else if(cdt9702_rec_stage==2)
             CDT9702_analysis_info();	//-分析信息字,这些内容肯定是对应端口发送来的
         else
         {
           cdt9702_rec_syn_flag = 0x55;
           return;
         }
         
         if(cdt9702_rec_syn_flag != 0xaa)
           return;	//-出现了不同步就返回,若处于同步状态,可以联系处理有效的块
     }
     else
     {
       cdt9702_rec_syn_flag = 0x55; //-当阅读程序到了,可以猜测下面逻辑的时候,就是非常开心的事情了
       return;
     }
  }
}

//-这个遥信状态怎么初始化的时候就进行这里的处理了呢,难道是说不仅仅是实际的遥信
//-还有其他的东西,仅仅是利用了遥信的形式
void CDT9702_deal_YX_state()
{
  if(CDT9702_TRAN_YX_TABLE_NO>3)
     return;
  if(yx_chg_tr_ex_pt[CDT9702_TRAN_YX_TABLE_NO]!=yx_chg_in_pt)
     return;
 
  temp_ptD = (unsigned short*)&YX_transmite_table[CDT9702_TRAN_YX_TABLE_NO][0];	//-这个表格代表的属性可能就是一个数据代表一位遥信状态
  temp_int = 0;
//  while(*temp_ptD < 2048)
  while(*temp_ptD < 4096)	//-最多检查4096个单元属性
  {
     if((YX_State[(*temp_ptD)/16] & (1<<((*temp_ptD)&0x0f))) != 0) //-YX_State存储区的值是一位为一个遥信状态
     {
        temp_lp_int = YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][temp_int/16];	//-从转发表中依次取数,取遥信数据库中的值,放入发送缓冲区
        temp_lp_int = temp_lp_int | (1 << (temp_int&0x0f));
        YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][temp_int/16] = temp_lp_int;	//-其实,这样就实现了从新排列遥信位
     }
     else
     {
        temp_lp_int = YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][temp_int/16];
        temp_lp_int = temp_lp_int & (0xffff ^ (1 << (temp_int&0x0f)));
        YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][temp_int/16] = temp_lp_int;	//-检查之后处理的值,又从位赋值变为了字赋值
     }
     temp_ptD ++;
     temp_int ++;
     if(temp_int>2047)	//-最多检查2048个遥信位
        break;
  }

}

void CDT9702_deal_YX_chg()
{
  unsigned short temp_chg;
  unsigned short old_chg_channel;
  unsigned short *temp_chg_pt;
  unsigned short  chg_bit_buf;   
  
  if(CDT9702_TRAN_YX_TABLE_NO>3)	//-配置的转发表号
  {	//-下面的处理把变化遥信优先上送给
     yx_chg_ex_pt[CDT9702_PORT] = yx_chg_in_pt;	//-由于不需要转发直接跟进了,说明不需要处理?错,不需要从新组织转发,所以直接更新数据库
     return;
  }

  old_chg_channel = 0xff;	//-这个可能就是局部程序进度标志
  while(yx_chg_tr_ex_pt[CDT9702_TRAN_YX_TABLE_NO]!=yx_chg_in_pt)	//-难道交给后台的数据不是直接来自数据库,而是来自转发表组织的数据吗
  {
     temp_ptS = &yx_change[yx_chg_tr_ex_pt[CDT9702_TRAN_YX_TABLE_NO]].offset_no;
     temp_lp_int = 1;
     temp_loop = 0;
     chg_bit_buf = *(temp_ptS+3);	//-取出,遥信变位信息,1表示变化
     while(chg_bit_buf!=0)
     {	//-不等于0说明有变化遥信存在
       if((chg_bit_buf & temp_lp_int)!=0)	//-呵呵,有什么东西啊,只要没有发送质的变化,就是这么回事
       {
         chg_bit_buf ^= temp_lp_int;	//-不是0的位再获得了这个信息之后,就清零了
         temp_ptD = (unsigned short*)&YX_transmite_table[CDT9702_TRAN_YX_TABLE_NO][0];
         temp_int = *temp_ptS*32 + temp_loop;	//-把变位遥信的偏移量字和位整合到一个数字内,由于一个结构体记录32位遥信所以*32
         temp_chg =  0;
         while((*temp_ptD != temp_int) && (temp_chg<2048))	//-匹配遥信转发表中的偏移量
         {
             temp_ptD ++;
             if(*temp_ptD > 0x0fff)
                 temp_chg = 10000;
             temp_chg++;
         }
         if(temp_chg<2048)	//-表示经转发表重新排列顺序之后的位数
         {
             if((*(temp_ptS+1) & temp_lp_int) != 0)	//-取出发生变位遥信对应位的变化后的值
                 YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][temp_chg/16] |= (1<<(temp_chg&0x0f));	//-变化之后的遥信值
             else
                 YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][temp_chg/16] &= ( 0xffff - (1<<(temp_chg&0x0f)));	//-属于转发遥信的排列顺序
             if(old_chg_channel != (temp_chg/32))	//-后面的数值是转发表中代表的字数
             {
                 if(old_chg_channel != 0xff)
                 {	//-在同一次更新中,同样的遥信发生一次以上变化,下面的处理
                     temp_chg_pt    =&yx_change_tr[CDT9702_TRAN_YX_TABLE_NO][yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO]].offset_no;
                    *temp_chg_pt    = old_chg_channel;
                     temp_chg_pt++;
                    *temp_chg_pt    = YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][old_chg_channel*2];
                     temp_chg_pt++;
                    *temp_chg_pt    = YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][old_chg_channel*2+1];
                     yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO] ++;	//-统计转发表中变位遥信的个数,主动自加
                     yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO] &= 0xff;
                 }
                 
                 old_chg_channel = temp_chg/32;	//-以32位为块记录的
             }
         } 
       }
       temp_lp_int <<= 1;
       temp_loop++;
       if(temp_loop>16)	//-我刚才担心的死循环,在这里解决了
          break;
     }

     temp_lp_int = 1;
     temp_loop = 0;
     chg_bit_buf = *(temp_ptS+4);
     while(chg_bit_buf != 0)
     {
       if((chg_bit_buf & temp_lp_int)!=0)
       {
         chg_bit_buf ^= temp_lp_int;
         temp_ptD = (unsigned short *)&YX_transmite_table[CDT9702_TRAN_YX_TABLE_NO][0];	//-猜测转发表的意思就是重新排列然后发送即原来是1234,现在可2341
         temp_int = *temp_ptS*32 + 16 + temp_loop;
         temp_chg = 0;
         while((*temp_ptD != temp_int)&&(temp_chg<2048))
         {
             temp_ptD ++;
             if(*temp_ptD > 0x0fff)
                 temp_chg = 10000;
             temp_chg++;
         }
         if(temp_chg<2048)
         {
             if((*(temp_ptS+2) & temp_lp_int) != 0)
                 YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][temp_chg/16] |= (1<<(temp_chg&0x0f));
             else
                 YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][temp_chg/16] &= ( 0xffff - (1<<(temp_chg&0x0f)));
             if(old_chg_channel != (temp_chg/32))
             {
                 if(old_chg_channel != 0xff)
                 {
                     temp_chg_pt    =&yx_change_tr[CDT9702_TRAN_YX_TABLE_NO][yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO]].offset_no;
                    *temp_chg_pt    = old_chg_channel;
                     temp_chg_pt++;
                    *temp_chg_pt    = YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][old_chg_channel*2];
                     temp_chg_pt++;
                    *temp_chg_pt    = YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][old_chg_channel*2+1];
                     yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO] ++;
                     yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO] &= 0xff;
                 }
                 
                 old_chg_channel = temp_chg/32;
             }
         } 
       }
       temp_lp_int<<= 1;
       temp_loop++;
       if(temp_loop>16)
          break;
     }  
     
     yx_chg_tr_ex_pt[CDT9702_TRAN_YX_TABLE_NO] ++;	//-这个和yx_chg_in_pt都是标志遥信变化帧的个数,只不过这个追yx_chg_in_pt的数值
     yx_chg_tr_ex_pt[CDT9702_TRAN_YX_TABLE_NO] &= 0xff;        
                
  }

  if(old_chg_channel != 0xff)	//-保证最后一次也记录了,转发表变化信息
  {
     temp_chg_pt     =&yx_change_tr[CDT9702_TRAN_YX_TABLE_NO][yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO]].offset_no;
     *temp_chg_pt    = old_chg_channel;
     temp_chg_pt++;
     *temp_chg_pt    = YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][old_chg_channel*2];
     temp_chg_pt++;
     *temp_chg_pt    = YX_state_tr[CDT9702_TRAN_YX_TABLE_NO][old_chg_channel*2+1];
     yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO] ++;	//-统计每个yx_transmite_table中变位YX个数
     yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO] &= 0xff;
  }


  yx_chg_ex_pt[CDT9702_PORT] = yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO];	//-有一种可能性就是,防止中断产生不可预知的逻辑,所以多了一个标识位

}

//-我想这里的处理可能就是有一个大小的包含关系,首先实际的变化了,就全部记录了
//-转发表中并不是所有的量都进行转发,所以记录的变化量同样是小的
void CDT9702_deal_YX_soe()
{
  if(CDT9702_TRAN_YX_TABLE_NO>3)	//-在不使用转发表的时候,下面就更新了数据库的指示标志
  {
     yx_soe_ex_pt[CDT9702_PORT] = yx_soe_in_pt;	//-大于3说明转发表无效,因为一共仅仅有4个转发表
     return;
  }

  while(yx_soe_tr_ex_pt[CDT9702_TRAN_YX_TABLE_NO]!=yx_soe_in_pt)	//-ex---examine检查的意思
  {
     temp_ptS = &yx_event[yx_soe_tr_ex_pt[CDT9702_TRAN_YX_TABLE_NO]].soe_ms;	//-不是每个记录的事件都需要转发的
     temp_int = *(temp_ptS + 3) & 0x7fff;	//-SOE对象号和性质（合/分） 
     temp_ptD = (unsigned short *)&YX_transmite_table[CDT9702_TRAN_YX_TABLE_NO][0]; //-靠,怎么也是用的遥信转发表啊,,这个应该就是一个属性列表
     temp_lp_int = 0;
     while(*temp_ptD != temp_int)	//-查询匹配对象
     {
        temp_ptD ++;
        if(*temp_ptD > 0x0fff)    // now max YX num 4096 in Dbase
           temp_lp_int = 10000;
        temp_lp_int++;
        if(temp_lp_int>2047)	//-最大查询属性表中的前2048个单元
           break;
     }
     if(temp_lp_int<2048)	//-值表示在转发表中的位数
     {
        temp_ptD = &yx_event_tr[CDT9702_TRAN_YX_TABLE_NO][yx_soe_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO]].soe_ms;	//-有变化的更新值放在这个里面
       *temp_ptD = *temp_ptS; temp_ptD++; temp_ptS++;
       *temp_ptD = *temp_ptS; temp_ptD++; temp_ptS++;
       *temp_ptD = *temp_ptS; temp_ptD++; temp_ptS++;
       *temp_ptD = *temp_ptS; *temp_ptD = (*temp_ptD & 0x8000) + temp_lp_int;
        yx_soe_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO]++;	//-检查指针是每一个都要查的,而内部指针是实际指向的位置
        yx_soe_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO] &= 0x3ff;
     }
     yx_soe_tr_ex_pt[CDT9702_TRAN_YX_TABLE_NO] ++;	//-从这里可以看的出来,yx_soe_in_pt是主动变化的,而这个是跟进处理
     yx_soe_tr_ex_pt[CDT9702_TRAN_YX_TABLE_NO] &= 0x3ff;
  }

  yx_soe_ex_pt[CDT9702_PORT] = yx_soe_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO];	//-tr转发表的意思,,最终查询符合要求的个数

}
//-这样处理的目的也许就是不需要后台参与的自动更新
void CDT9702_deal_info()	//-靠,搞了半天原来不需要转发表的程序这个一段是不用的
{
  CDT9702_deal_YX_chg();
  CDT9702_deal_YX_soe();
}


    
void CDT9702_Init()
{
  cdt9702_send_order      = 0;
  cdt9702_send_frame_len  = 0;
  cdt9702_send_info_turn  = 0; 
  cdt9702_send_stage      = 0;
  cdt9702_yx_send         = 0;
  cdt9702_yx_chg_send     = 0;
  cdt9702_yk_verify_send  = 0;
  CDT9702_soe_send_turn   = 0;

  cdt9702_rec_syn_flag    = 0x55;
  cdt9702_rec_stage       = 0;
  cdt9702_rec_frame_len   = 0;

  CDT9702_TRAN_YC_TABLE_NO = byte0(port_info[CDT9702_PORT].table_no);	//-YC 转发表号  也许就没有使用
  CDT9702_TRAN_YX_TABLE_NO = byte1(port_info[CDT9702_PORT].table_no);	//-YX/YM 转发表号,,配置的属性值
  CDT9702_TRAN_YM_TABLE_NO = byte1(port_info[CDT9702_PORT].table_no);
  CDT9702_TRAN_YC_CHG_TABLE_NO=byte1(port_info[CDT9702_PORT].semiduplex_parity);	//-是表示半双工吗

  if(CDT9702_TRAN_YX_TABLE_NO<4)	//-可以人为设定转发表,用于从数据库中提炼出自己需要的信息
  {
     yx_chg_out_pt[CDT9702_PORT] = yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO];
     yx_chg_ex_pt[CDT9702_PORT]  = yx_chg_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO];
     yx_soe_out_pt[CDT9702_PORT] = yx_soe_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO];
     yx_soe_ex_pt[CDT9702_PORT]  = yx_soe_tr_in_pt[CDT9702_TRAN_YX_TABLE_NO];
  }
  else
  {
     yx_chg_out_pt[CDT9702_PORT] = yx_chg_in_pt;	//-是仅仅表示变化个数吗
     yx_chg_ex_pt[CDT9702_PORT]  = yx_chg_in_pt;
     yx_soe_out_pt[CDT9702_PORT] = yx_soe_in_pt;
     yx_soe_ex_pt[CDT9702_PORT]  = yx_soe_in_pt;
  }

  cdt9702_yx_fresh   = 0;
  CDT9702_deal_YX_state();	//-如果不需要从新组织的话,那么就不需要这个里面的处理
  
  CDT9702_RLY_REPORT = 0x00;
  CDT9702_PAGE_NUM   = 0;
  CDT9702_PAGE_ORDER = 0;
}

//-这个是老得程序,兼容性是很好的,值得好好研究
void CDT9702_Main()
{
  CDT9702_deal_info();	//-处理信息,数据库中的,刷新数据库指示信息
  CDT9702_deal_rpt();	//-处理报告,是后台发来的报文,若后台没有发送来报文这里也是没有处理的.并进行了信息端口间的传递
  CDT9702_assemble_send_rpt();	//-集中发送报告,根据内部各个各样的标志位,进行判断并组织发送内容
}

void CDT9702_Monitor()	//-监视就是最简单的一种排错
{
  if(port_send_pt[CDT9702_PORT]<port_send_len[CDT9702_PORT])
     port_mon[CDT9702_PORT]++;
  else
     port_mon[CDT9702_PORT] = 0;	//-有点像循环的意思
     
  if(port_mon[CDT9702_PORT]>200)	//-累加到200就说明出现了错误,需要重新初始化
  {
    (*((far BYTE *)(SOFT_ERR_FLAG+0x0080+CDT9702_PORT*4)))++;
    disable();
    init_port();
    enable();
    port_mon[CDT9702_PORT] = 0;
  }

  cdt9702_yx_fresh++;
  if(cdt9702_yx_fresh>40)
  {
     CDT9702_deal_YX_state();	//-为什么需要定时刷新这个值的内容
     cdt9702_yx_fresh = 0;
  }
}
