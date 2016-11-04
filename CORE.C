/*****************************************************************************/
/*       FileName  :   CORE.C                                                */
/*       Content   :   DSA-208 CORE Module                                   */
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
#include "comuse.h"





/************************************************/
/* Core_ObjectNo_To_UnitAddr      function      */
/************************************************/
//-判断遥控对象号是否存在,遥控号配置的原则是:所有挂在通讯管理机上的设备都有唯一的
//-遥控号,通过这个对象号就可以锁定属性和端口号
/*===========================================================================*/
BYTE Ex_YK_CDTBB_ObjectNo_To_UnitAddr(void)
{
  unsigned short temp_rnd,temp_rnd1;

  for(temp_rnd=0;temp_rnd<14;temp_rnd++)	//-这样查找的原因就是某块化处理
  {
      if(  (exchange_buf[5] >= Ex_YK_start[temp_rnd])
         &&(exchange_buf[5] <= Ex_YK_end[temp_rnd]) )	//-可能这里要求的配置遥控号,必须互易,在每个属性表中
      {	//-如果全部互易的话,这里就应该已经找到被遥控的设备挂在的端口号
          for(temp_rnd1  = byte0(port_info[temp_rnd].mirror_unit);
              temp_rnd1 <= byte1(port_info[temp_rnd].mirror_unit);
              temp_rnd1++ )
          {	//-在找到的端口内,再一次的检查这个端口的所有属性配置单元
              if(   (exchange_buf[5] >= unit_info[temp_rnd1].yk_start)
                  &&(exchange_buf[5] < (unit_info[temp_rnd1].yk_start+unit_info[temp_rnd1].yk_num))
                )
              {	//-只有得到的对象号,在其中一个元素范围内,才会最终确定遥控号和终端装置整定的实际地址
                  exchange_buf[17] = exchange_buf[5] - unit_info[temp_rnd1].yk_start;     // yk_no in unit
                  exchange_buf[5]  = temp_rnd1-byte0(port_info[temp_rnd].mirror_unit);    // unit addr
                  exchange_target_port=temp_rnd;	//-这个地方使用了一个较为简单的方法,进行了映射报文发送,他是直接把报文存储到映射口单元,没有先放在本通道单元内
                  return YES;
              }
          }
      }
  }
  
  return NO;
}



/************************************************/
/* Ex_Produce_Transmit_Info     function        */
/* Input :    exchange_buf[xxx]                 */
/* Output:    port_transmit[port][xxx]          */
/************************************************/
/*===========================================================================*/
BYTE Ex_Produce_Transmit_Info(void)	//-由于每个端口固定一种规约,这个端口下挂接的所有信息都是在各个规约文件中处理,这个程序段的作用就是把报文换口发送
{
    unsigned short temp_rnd;
    unsigned short temp_rnd1;
    unsigned char *temp_pt;

    if(exchange_target_port<14)
    {
        if(port_transmit_flag[exchange_target_port] == 0x55)	//-值为55说明没有数据报文已经发送,所以可以发送,,不能穿插发送报文
        {
            temp_pt = &port_transmit[exchange_target_port][0]; 
            for(temp_rnd=1;temp_rnd<16;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];	//-把传递的信息已经放入到了目的端口缓冲区中了
            }

            byte0(temp_rnd1)=exchange_buf[9];	//-表示从16单元格开始还有几个单元格的内容需要传递
            byte1(temp_rnd1)=exchange_buf[10];
            temp_rnd1=temp_rnd1+16;
            for(temp_rnd=16;temp_rnd<temp_rnd1;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];	//-也许这个就没有任何规约可言就是一个交换缓冲区
            }
            port_transmit_flag[exchange_target_port] = 0xaa;	//-表示本端口接收到了传递信息,需要处理
            return YES;
        }   
        else
            return NO;
    }
    else
    {
        if(set_transmit_flag == 0x55)
        {
            temp_pt = &set_transmit[0]; 
            for(temp_rnd=1;temp_rnd<16;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];
            }

            byte0(temp_rnd1)=exchange_buf[9];
            byte1(temp_rnd1)=exchange_buf[10];
            if(temp_rnd1>250) 
                return NO;
            temp_rnd1=temp_rnd1+16;
            for(temp_rnd=16;temp_rnd<temp_rnd1;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];
            }
            set_transmit_flag = 0xaa;
            return YES;
        }   
        else
            return NO;
    }
}





void Ex_scan_yk_range_for_port()
{
  unsigned short temp_rnd,temp_rnd1;
  unsigned char  the_start_flag[14],the_end_flag[14];
  
  for(temp_rnd=0;temp_rnd<14;temp_rnd++)
  { 
     the_start_flag[temp_rnd] = 0;
     the_end_flag[temp_rnd]   = 0;
     
     Ex_YK_start[temp_rnd]   = 1;
     Ex_YK_end[temp_rnd]     = 0;
  }
  //-一台装置对应数据库中的唯一属性,而同一规约的装置可以接到一个端口上
  for(temp_rnd=0;temp_rnd<256;temp_rnd++)	//-检查数据库属性表的配置值
  {
      if(unit_info[temp_rnd].yk_num!=0)	//-若配置有遥控功能就进行如下判断
      {
          for(temp_rnd1=0;temp_rnd1<14;temp_rnd1++)	//-怎么又找到端口了呢
          {
              if(  (temp_rnd>=byte0(port_info[temp_rnd1].mirror_unit))
                 &&(temp_rnd<=byte1(port_info[temp_rnd1].mirror_unit)) )	//-确保数据库元素在端口配置内,因为每个数据库元素是先在端口配置属性下的
              {	//-每个端口对应的内部数据库单元的值肯定是不同的
                  if(the_start_flag[temp_rnd1]==0)	//-找打了端口号
                  {
                      the_start_flag[temp_rnd1]=0xff;	//-是表示这个端口有遥控配置存在吗
                      Ex_YK_start[temp_rnd1]=unit_info[temp_rnd].yk_start;	//-一个端口上所有设备遥控开始的号
                  }
                  
                  if(unit_info[temp_rnd].yk_start<Ex_YK_start[temp_rnd1])
                  {
                      Ex_YK_start[temp_rnd1]=unit_info[temp_rnd].yk_start;	//-确保记录的是一个端口上所有有遥控设备的号的最小值
                  }

                  if(the_end_flag[temp_rnd1]==0)	//-0就表示还没有发生
                  {
                      the_end_flag[temp_rnd1]=0xff;	//-表示结束标识有效
                      Ex_YK_end[temp_rnd1] = unit_info[temp_rnd].yk_start + unit_info[temp_rnd].yk_num-1;	//-记录的是端口中遥控结束号
                  }
                  if((unit_info[temp_rnd].yk_start+unit_info[temp_rnd].yk_num)>Ex_YK_end[temp_rnd1]) 
                      Ex_YK_end[temp_rnd1] = unit_info[temp_rnd].yk_start + unit_info[temp_rnd].yk_num-1;    //-可能有些不是按照顺序配置的,所以必须找到其中最大的
              }
          }
      }
  }
}

void core_init()
{
  yx_chg_in_pt = 0;
  yx_soe_in_pt = 0;
  for(temp_loop=0;temp_loop<4;temp_loop++)
  {
     yx_chg_tr_ex_pt[temp_loop] = 0;
     yx_soe_tr_ex_pt[temp_loop] = 0;
     yx_chg_tr_in_pt[temp_loop] = 0;
     yx_soe_tr_in_pt[temp_loop] = 0;
  }

  BH_Report_Bank_Sav_Ptr=0;
  
  for(temp_loop=0;temp_loop<12;temp_loop++)
  {
      BH_Report_Bank_Tak_Ptr[temp_loop]=0;
  } 
}

void Ex_init()	//-为交换信息做的初始化准备
{
  unsigned char temp_rnd;

  core_init();
  Ex_scan_yk_range_for_port();

  for(temp_rnd=0;temp_rnd<14;temp_rnd++)
  {
     port_transmit_flag[temp_rnd] = 0x55;
     port_transmit[temp_rnd][0]   = PORT_EXCHANGE_STA_IDLE;
  }  
}


//应用 core  前提已释放 temp_loop,temp_loop1,temp_int,temp_lp_int 

void core_get_bh_bank_report(BYTE the_port)
{
	BYTE need_refind;
	if(Core_Src_Unit>=BH_REPORT_BANK_SIZE)  Core_Src_Unit=0;
	if(the_port>11)  
	{
		Core_Src_Unit++;
		Core_Src_Len=0;
          	return;
		
	}	
    Core_Src_Pt_B=&BH_Saved_Report_Bank[Core_Src_Unit][0];
    need_refind=NO;
re_find:
    while((Core_Src_Pt_B[4]!=( (Core_Src_Pt_B[0]+Core_Src_Pt_B[1]+Core_Src_Pt_B[2]+Core_Src_Pt_B[3])&0xff ))
        ||(Core_Src_Pt_B[2]!=0)||(need_refind==YES))
    {//-检查内容是否错了
    	need_refind=NO;
        Core_Src_Unit++;
        if(Core_Src_Unit>=BH_REPORT_BANK_SIZE) Core_Src_Unit=0;
        if(Core_Src_Unit==BH_Report_Bank_Sav_Ptr)
        {
            Core_Src_Len=0;
            return;
        }
        Core_Src_Pt_B=&BH_Saved_Report_Bank[Core_Src_Unit][0];
    }
    
    byte1(Core_Temp_Loop)=Core_Src_Pt_B[6];
    if(byte1(Core_Temp_Loop)>10) 
    {
    	need_refind=YES;
    	goto re_find;
    }	
    for(byte0(Core_Temp_Loop)=0;byte0(Core_Temp_Loop)<byte1(Core_Temp_Loop);byte0(Core_Temp_Loop)++)
    {
    	if((Core_Src_Pt_B[2]!=byte0(Core_Temp_Loop))||(Core_Src_Pt_B[3]>26)
    	 ||(Core_Src_Pt_B[4]!=( (Core_Src_Pt_B[0]+Core_Src_Pt_B[1]+Core_Src_Pt_B[2]+Core_Src_Pt_B[3])&0xff )))  // frame_NO err
    	{//-帧出错了,就重找
	    	need_refind=YES;
    		goto re_find;
    	}

		
   		if(byte0(Core_Temp_Loop)==0)
   		{
   			if(Core_Src_Pt_B[3]>2)  //-报文长度
   				byte1(Core_Temp_ShortInt)=Core_Src_Pt_B[3]-2;
   			else
   				byte1(Core_Temp_ShortInt)=0;
   			for(byte0(Core_Temp_ShortInt)=0;byte0(Core_Temp_ShortInt)<byte1(Core_Temp_ShortInt);byte0(Core_Temp_ShortInt)++)
   			{//-从总库里把保护内容复制到对应通道表内,暂存内容供发送用
   				BH_Bank_Report[the_port][byte0(Core_Temp_ShortInt)]=Core_Src_Pt_B[byte0(Core_Temp_ShortInt)+8];
   			}
   			Core_Src_Len=byte1(Core_Temp_ShortInt);
   		    BH_Bank_Report[the_port][509]=Core_Src_Pt_B[1];
   		    BH_Bank_Report[the_port][510]=Core_Src_Pt_B[0];
   		    BH_Bank_Report[the_port][511]=Core_Src_Pt_B[5];
   		}
   		else
   		{
   			if((Core_Src_Pt_B[0]!=BH_Bank_Report[the_port][510])||(Core_Src_Pt_B[5]!=BH_Bank_Report[the_port][511])
   			 ||(Core_Src_Pt_B[1]!=BH_Bank_Report[the_port][509]))
   			{
   		    	need_refind=YES;
       			goto re_find;
   			}
   			
   			byte1(Core_Temp_ShortInt)=Core_Src_Pt_B[3];
   			for(byte0(Core_Temp_ShortInt)=0;byte0(Core_Temp_ShortInt)<byte1(Core_Temp_ShortInt);byte0(Core_Temp_ShortInt)++)
   			{
   				BH_Bank_Report[the_port][Core_Src_Len+byte0(Core_Temp_ShortInt)]=Core_Src_Pt_B[byte0(Core_Temp_ShortInt)+6];
   			}
   			Core_Src_Len+=byte1(Core_Temp_ShortInt);
   		}	

        Core_Src_Unit++;
        if(Core_Src_Unit>=BH_REPORT_BANK_SIZE) Core_Src_Unit=0;
        Core_Src_Pt_B=&BH_Saved_Report_Bank[Core_Src_Unit][0];
    }
    
//    if((Core_Src_Pt_B[0]==(PROTOCOL_LSA%0x100))&&(Core_Src_Pt_B[5]==(PROTOCOL_LSA/0x100)))
//    {
//        if((Core_Src_Pt_B[2]==0)&&(Core_Src_Pt_B[6]==1)&&(Core_Src_Pt_B[3]<=24))
//        {
//            Core_Src_Len=Core_Src_Pt_B[3];
//            for(Core_Temp_Loop=0;Core_Temp_Loop<Core_Src_Len;Core_Temp_Loop++)
//            {
//                BH_Bank_Report[the_port][Core_Temp_Loop]=Core_Src_Pt_B[Core_Temp_Loop+8];
//            }
//            BH_Bank_Report[the_port][510]=(PROTOCOL_LSA%0x100);
//            BH_Bank_Report[the_port][511]=(PROTOCOL_LSA/0x100);
//        }
//        else
//        {
//           Core_Src_Len=0;
//        }
//    }
//    else  // other protocol
//    {
//        Core_Src_Len=0;
//    }
    
//    Core_Src_Unit++;
//    if(Core_Src_Unit>=BH_REPORT_BANK_SIZE) Core_Src_Unit=0;
}

void core_get_yx_set_unit()
{
  if((Core_Src_Unit>0x1f)&&(Core_Src_Unit<0x30))
     Core_Temp_Loop = 4;
  else
     Core_Temp_Loop = unit_info[Core_Src_Unit].yx_num;
  //Core_Src_Unit Core_Src_Pt_B
  Core_Temp_ShortInt= unit_info[Core_Src_Unit].yx_start;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Src_Pt_B =  byte0(YX_property[Core_Temp_ShortInt + Core_Temp_Loop1]);
      Core_Src_Pt_B ++;
     *Core_Src_Pt_B = byte1(YX_property[Core_Temp_ShortInt + Core_Temp_Loop1]);
      Core_Src_Pt_B ++;    
  }
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Src_Pt_B =  byte0(YX_YM[Core_Temp_ShortInt+Core_Temp_Loop1]);
      Core_Src_Pt_B ++;
     *Core_Src_Pt_B = byte1(YX_YM[Core_Temp_ShortInt+Core_Temp_Loop1]);
      Core_Src_Pt_B ++;    
  }

  Core_Temp_ShortInt = Core_Temp_ShortInt*8;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<(Core_Temp_Loop*8);Core_Temp_Loop1++)
  {
     if((Core_Temp_ShortInt + Core_Temp_Loop1)<256)
     {
        *Core_Src_Pt_B =  byte0(YX_double[Core_Temp_ShortInt + Core_Temp_Loop1]);
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B = byte1(YX_double[Core_Temp_ShortInt + Core_Temp_Loop1]);
         Core_Src_Pt_B ++;    
     }
     else
     {
        *Core_Src_Pt_B =  0xc8;
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B =  0xc8;
         Core_Src_Pt_B ++;    
     }
  }

  Core_Temp_ShortInt = Core_Temp_ShortInt*2;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<(Core_Temp_Loop*16);Core_Temp_Loop1++)
  {
     if((Core_Temp_ShortInt + Core_Temp_Loop1)<512)
     {
        *Core_Src_Pt_B =  byte0(YX_delay[Core_Temp_ShortInt + Core_Temp_Loop1]);
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B = byte1(YX_delay[Core_Temp_ShortInt + Core_Temp_Loop1]);    
         Core_Src_Pt_B ++;
     }
     else
     {
        *Core_Src_Pt_B =  0xc8;
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B =  0x00;    
         Core_Src_Pt_B ++;
     }
  }
}

void core_get_yc_set_unit()
{
  if((Core_Src_Unit>0x0f)&&(Core_Src_Unit<0x20))
      Core_Temp_Loop = 6;
  else
      Core_Temp_Loop = unit_info[Core_Src_Unit].yc_line_num;

  //Core_Src_Unit Core_Src_Pt_B
  Core_Temp_ShortInt = unit_info[Core_Src_Unit].yc_line_start;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     if(Core_Temp_Loop1<unit_info[Core_Src_Unit].yc_line_num)
     {
       *Core_Src_Pt_B =  YC_coef_table[(Core_Temp_ShortInt + Core_Temp_Loop1)*4];
        Core_Src_Pt_B ++;
       *Core_Src_Pt_B =  YC_coef_table[(Core_Temp_ShortInt + Core_Temp_Loop1)*4 + 1];
        Core_Src_Pt_B ++;    
     }
     else
     {
       *Core_Src_Pt_B =  0;
        Core_Src_Pt_B ++;
       *Core_Src_Pt_B =  0;
        Core_Src_Pt_B ++;    
     }
  }
}

void core_update_YC()
{
  Core_Temp_Loop = unit_info[Core_Src_Unit].yc_val_num;
  Core_Det_Pt = (unsigned short *)&(YC_State[unit_info[Core_Src_Unit].yc_val_start]);
  if(Core_Temp_Loop>Core_Src_Len)
     Core_Temp_Loop = Core_Src_Len;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Det_Pt   =  (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
     Core_Src_Pt_B += 2;
     Core_Det_Pt    ++;
  }
    
}

void core_update_DC()
{
  Core_Temp_Loop = unit_info[Core_Src_Unit].dc_num;
  Core_Det_Pt = (unsigned short *)&(YC_State[unit_info[Core_Src_Unit].dc_start]);
  if(Core_Temp_Loop>Core_Src_Len)
     Core_Temp_Loop = Core_Src_Len;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Det_Pt   =  (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
     Core_Src_Pt_B += 2;
     Core_Det_Pt    ++;
  }
    
}

/************************************************/
/* core_update_YX     function                  */
/* Input :  Core_Src_Len                        */
/*          Core_Src_Pt_B                       */
/*          Core_Src_Unit                       */
/* Output:                                      */
/* REG varible used: Core_Det_Pt                */
/*                   Core_Src_Pt                */
/*                   Core_Temp_Loop             */
/*                   Core_Temp_Loop1            */
/*                   Core_Temp_ShortInt         */
/************************************************/
//-更新遥信值似乎不是简单的遥信状态,而是变化量.其实主要是看后台是怎么处理的
//-现在看明白了,数据库的值全部更新了,只是有变化的话还在变化空间内记录变化信息
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void core_update_YX()
{
  unsigned short chg_buf0,chg_buf1;
  unsigned short  yx_buf0;

  Core_Det_Pt = (unsigned short *)&(YX_State[unit_info[Core_Src_Unit].yx_start]);	//-取出实际值,其实就是属性所指内容

  Core_Temp_Loop   = unit_info[Core_Src_Unit].yx_num;	//-单位是字
  Core_Temp_Loop   = Core_Temp_Loop * 2;
  if(Core_Temp_Loop>Core_Src_Len)	//-其实,来源一样,只是可靠,,或者是由于模块化编程思想的作用,不在乎重复这样一步
      Core_Temp_Loop = Core_Src_Len;
  Core_Temp_Loop &= 0xfe;  //avoid odd
  
  if(HOST_ZF_enable!=YES)	//-在还没有可以向后台转发的时候直接更新数据库
  {
     Core_Temp_Loop = Core_Temp_Loop/2;
     for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
     {
        *Core_Det_Pt = (*(Core_Src_Pt_B+1))*256 + (*Core_Src_Pt_B);
         Core_Det_Pt ++;
         Core_Src_Pt_B +=2;
     }    
     return;	//-结束这段程序了
  }
  //-在所有的配置下位机开机稳定时间没有到达之前,直接更新数据库,当延时到达之后,增加了一个记录遥信变化的功能
  Core_Src_Len = 0;
  while(Core_Temp_Loop>0)	//-遥信字节数
  {
      Core_Src_Pt = &yx_change[yx_chg_in_pt].offset_no;	//-这个存储空间自带属性,所以不需要特殊定位,依次存储就行
     *Core_Src_Pt =  unit_info[Core_Src_Unit].yx_start + Core_Src_Len;	//-记录遥信的开始地址,说白了还是偏移量,以字为单位

      Core_Temp_ShortInt = 0x5555;	//-表示没有出现变化遥信

      chg_buf0  = 0;
      chg_buf1  = 0;

      yx_buf0   = (*(Core_Src_Pt_B+1))*256 + (*Core_Src_Pt_B);	//-刚上送上来的新值
      if(yx_buf0 != *Core_Det_Pt)	//-与原来的老值进行比较
      {
          Core_Temp_ShortInt = 0xaaaa;
          chg_buf0           = yx_buf0 ^ (*Core_Det_Pt);
      }
     *Core_Det_Pt     = yx_buf0;	//-记录变化后的值,这里就更新了数据库
      Core_Src_Pt_B   += 2;
      Core_Det_Pt     ++;
      Core_Temp_Loop  -= 2;
      Core_Src_Len    ++;	//-确定变位YX的偏移量,是以字为单位的,但是每个记录单元又可以记录两个字的量

      if((*Core_Src_Pt & 1) != 0)	//-这个肯定是判断奇偶的,
      {	//-到这里表示是奇数,只更新一次的
          if(Core_Temp_ShortInt == 0xaaaa)	//-不等于这个值表示没有发送变化,当然就不需要记录
          {
             *Core_Src_Pt = *Core_Src_Pt / 2;
             Core_Src_Pt++;
             *Core_Src_Pt = *(Core_Det_Pt-2);	//-在这里就是原来的数值
             Core_Src_Pt++;
             *Core_Src_Pt = *(Core_Det_Pt-1);	//-由于一个的有效信息是16为,但是这里的元素一个有32为,所以每次仅仅使用一般,宁一半无效
             Core_Src_Pt++;
             *Core_Src_Pt = 0;               //-这里说明了,这个遥信没有使用
             Core_Src_Pt++; 
             *Core_Src_Pt = chg_buf0;	//-1表示有变化
             yx_chg_in_pt ++;	//-这个数字其实就可以作为自动上送的一个依据
             yx_chg_in_pt &= 0xff;         
             Core_Temp_ShortInt = 0x5555;
          }
      }
      else
      {	//-到这里是偶数,,由于记录的是一个偶数+一个奇数,所以当判断为偶数的的时候,如果还有量的话再判断一下下一个没有就算了
          if(Core_Temp_Loop>0)
          {
               yx_buf0 = ((*(Core_Src_Pt_B+1)))*256 + (*Core_Src_Pt_B);
               if(yx_buf0 != (*Core_Det_Pt))
               {
                   Core_Temp_ShortInt = 0xaaaa;
                   chg_buf1           = yx_buf0 ^ (*Core_Det_Pt);
               }
              *Core_Det_Pt     = yx_buf0;	//-记录的是新值,更新了数据库
               Core_Src_Pt_B   = Core_Src_Pt_B + 2;
               Core_Temp_Loop -= 2;
               Core_Src_Len    ++;
          }

          Core_Det_Pt++;	//-为了配合模块程序,前面固定为偶数后面是奇数

          if(Core_Temp_ShortInt == 0xaaaa)
          {
             *Core_Src_Pt = *Core_Src_Pt / 2;	//-除以2的目的是两个一组,所以偏移量要除以2
             Core_Src_Pt++;
             *Core_Src_Pt = *(Core_Det_Pt-2);	//-遥信刚刚更新的值
             Core_Src_Pt++;
             *Core_Src_Pt = *(Core_Det_Pt-1);	//-遥信刚刚更新的值,即变化之后的值
             Core_Src_Pt++;
             *Core_Src_Pt = chg_buf0;           //-标志哪些位遥信发生了变化
             Core_Src_Pt++; 
             *Core_Src_Pt = chg_buf1;			//-变位信息
             yx_chg_in_pt ++;	//-变化记忆空间增加了量
             yx_chg_in_pt &= 0xff;         
             Core_Temp_ShortInt = 0x5555;
          }
      }
  }
}

void core_insert_SOECOS()
{
   // before insert SOE & COS, must fill YX_State with New YX value to generate COS
   
   Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;
  //ms
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //sec min
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //hour day
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  // channel state

   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Temp_ShortInt=((*Core_Det_Pt) & 0x8fff) + (unit_info[Core_Src_Unit].yx_start*16 & 0x0fff);
   *Core_Det_Pt = Core_Temp_ShortInt;

   yx_soe_in_pt++;
   if(yx_soe_in_pt>1023)
     yx_soe_in_pt = 0;

   Core_Temp_ShortInt&=0x0fff;
   Core_Temp_Loop=Core_Temp_ShortInt/16;
   Core_Det_Pt = (WORD *)&YX_State[Core_Temp_Loop];
   Core_Src_Pt = &yx_change[yx_chg_in_pt].offset_no;
  *Core_Src_Pt = Core_Temp_Loop/2;           Core_Src_Pt++; // offset
   if(((Core_Temp_Loop)& 0x01)==0)
   {
       *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX0
       *Core_Src_Pt = *(Core_Det_Pt+0x01);   Core_Src_Pt++; // YX1
       *Core_Src_Pt = (WORD)(1<<(Core_Temp_ShortInt%16));    Core_Src_Pt++;   // CHG0
       *Core_Src_Pt = 0x00;                                 // CHG1
   } 
   else
   {
       *Core_Src_Pt = *(Core_Det_Pt-0x01);   Core_Src_Pt++; // YX0
       *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX1
       *Core_Src_Pt = 0x00;                  Core_Src_Pt++; // CHG0
       *Core_Src_Pt = (WORD)(1<<(Core_Temp_ShortInt%16));   // CHG1
   }
   
   yx_chg_in_pt ++;
   yx_chg_in_pt &= 0xff;         
}

void core_insert_SOE()
{
/*   unsigned short	the_ram_ax;	
   the_ram_ax=Core_Src_Pt_B[6]+(Core_Src_Pt_B[7]&0x0f)*256;
   if(the_ram_ax>=(unit_info[Core_Src_Unit].yx_num*16))
   return;
   the_ram_ax+=unit_info[Core_Src_Unit].yx_start*16;
   if(the_ram_ax>0xfff)
   return;*/
   Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;
  //ms
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //sec min
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //hour day
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  // channel state

   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   *Core_Det_Pt = ((*Core_Det_Pt) & 0x8fff) + (unit_info[Core_Src_Unit].yx_start*16 & 0x0fff);

   yx_soe_in_pt++;
   if(yx_soe_in_pt>1023)
     yx_soe_in_pt = 0;
}

void core_update_YM()
{
  Core_Temp_Loop = unit_info[Core_Src_Unit].ym_num;
  if(*Core_Src_Pt_B>Core_Temp_Loop)
     return;
  
  Core_Det_Pt = (unsigned short *)&(YM_State[unit_info[Core_Src_Unit].ym_start+(*Core_Src_Pt_B)]);
  if(Core_Temp_Loop<(Core_Src_Len+(*Core_Src_Pt_B)))
     Core_Temp_Loop = Core_Temp_Loop - *Core_Src_Pt_B;
  else
     Core_Temp_Loop = Core_Src_Len;

  Core_Temp_Loop = Core_Temp_Loop*2;
  Core_Src_Pt_B  ++;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Det_Pt    = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
      Core_Src_Pt_B += 2;
      Core_Det_Pt   ++;
  }
    
}
