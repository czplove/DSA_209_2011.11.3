/*****************************************************************************/
/*       FileName  :   MAIN.C                                                */
/*       Content   :   DSA-208 MAIN Module                                   */
/*       Date      :   Fri  02-22-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/
//-本系统作为一个通讯管理机,就是负责集中通讯用的,而通讯就是信息的交流的过程,就是
//-大家交换的一个过程
//-还有一个标识有效性的问题,这个地方标识位是多,但是谈不上复杂,其实就是连续变化,不能乱

//-现在已经可以肯定的是后台也是通过12个通道之一和通讯管理机进行数据交换的,问题是
//-这个上位机程序需要我去编写吗,或者仅仅我和上位机程序编写人员协调好,规约内容即可???
//-就目前而言上位机的程序肯定是不需要去学习的,应该是我和做上位机得人协调好规约内容
//-然后各司其职,某块化处理


#pragma  regconserve

#include _SFR_H_
#include _FUNCS_H_

#include "common.h"
#include "device.h"
#include "reg.h"
#include "ram.h"
#include "rom.h"
#include "comuse.h"

//#pragma  ccb(0xF8D8)










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
//extern void CAN0_Main(void);
//extern void CAN0_Init(void);
//extern void CAN0_Monitor(void);

//Set port
extern void set_main();
extern void set_init();
extern void set_announce_rpt();

// Protocols
extern void Protocol_Main(void);
extern void Protocol_Init(void);
extern void Protocol_Monitor(void);

//bus
extern void Bus_Main();

//Exchange
extern void Ex_init();

 





/*---------------------------------------------------------------------------*/
/*                      predefintion        functions                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/

/***********************************************/
/*   initial_dallas  for   Main  procedure     */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void initial_dallas(void)
{
    BYTE *Temp_Pt_S_B;

    Temp_Pt_S_B=(BYTE *)REAL_TIME_MEM_BASE;
    Temp_Pt_S_B[0x0b]=0x86;

	    Rcd_Info_Myself_Tmp[0]=0x0d;  // DALLAS INI    Xor Err
    	Rcd_Info_Myself_Tmp[1]=0x0a;  
	    Rcd_Info_Myself_Tmp[2]=0x15;
	    Rcd_Info_Myself_Tmp[3]=0x15;
    	Rcd_Info_Myself_Tmp[4]=0x0a;
	    Rcd_Info_Myself_Tmp[5]=0x1c;
    	Rcd_Info_Myself_Tmp[6]=0x3e;
	    Rcd_Info_Myself_Tmp[7]=0x12;
    	Rcd_Info_Myself_Tmp[8]=0x17;
    	Rcd_Info_Myself_Tmp[9]=0x12;
    	Rcd_Info_Myself_Tmp[10]=0x3e;
    	Rcd_Info_Myself_Tmp[11]=0x3e;

    	Rcd_Info_Myself_Tmp[21]=0x3e;
    	Rcd_Info_Myself_Tmp[22]=0x3e;

    if(Temp_Pt_S_B[9]>99)  // DALLAS_YEAR
    {
    	Rcd_Info_Myself_Tmp[12]=0x22;
    	Rcd_Info_Myself_Tmp[13]=0x0e;
    	Rcd_Info_Myself_Tmp[14]=0x0a;
    	Rcd_Info_Myself_Tmp[15]=0x1b;
    	Rcd_Info_Myself_Tmp[16]=0x42;
    	Rcd_Info_Myself_Tmp[17]=Temp_Pt_S_B[9]/0x10;
    	Rcd_Info_Myself_Tmp[18]=Temp_Pt_S_B[9]&0x0f;
    	Rcd_Info_Myself_Tmp[19]=0x3e;
    	Rcd_Info_Myself_Tmp[20]=0x3e;
        Store_Rcd_Info_Myself();

        Temp_Pt_S_B[9]=0;
    }

    if( (Temp_Pt_S_B[8]<1) || (Temp_Pt_S_B[8]>12) )  // DALLAS_MONTH;
    {
    	Rcd_Info_Myself_Tmp[12]=0x16;
    	Rcd_Info_Myself_Tmp[13]=0x18;
    	Rcd_Info_Myself_Tmp[14]=0x17;
    	Rcd_Info_Myself_Tmp[15]=0x1d;
    	Rcd_Info_Myself_Tmp[16]=0x11;
    	Rcd_Info_Myself_Tmp[17]=0x42;
    	Rcd_Info_Myself_Tmp[18]=Temp_Pt_S_B[8]/0x10;
    	Rcd_Info_Myself_Tmp[19]=Temp_Pt_S_B[8]&0x0f;
    	Rcd_Info_Myself_Tmp[20]=0x3e;
        Store_Rcd_Info_Myself();
        
        Temp_Pt_S_B[8]=1;
    }

    if( (Temp_Pt_S_B[7]<1) || (Temp_Pt_S_B[7]>31) )  // DALLAS_DATE;
    {
    	Rcd_Info_Myself_Tmp[12]=0x0d;
    	Rcd_Info_Myself_Tmp[13]=0x0a;
    	Rcd_Info_Myself_Tmp[14]=0x1d;
    	Rcd_Info_Myself_Tmp[15]=0x0e;
    	Rcd_Info_Myself_Tmp[16]=0x42;
    	Rcd_Info_Myself_Tmp[17]=Temp_Pt_S_B[7]/0x10;
    	Rcd_Info_Myself_Tmp[18]=Temp_Pt_S_B[7]&0x0f;
    	Rcd_Info_Myself_Tmp[19]=0x3e;
    	Rcd_Info_Myself_Tmp[20]=0x3e;
        Store_Rcd_Info_Myself();
        
        Temp_Pt_S_B[7]=1;
    }

    if(Temp_Pt_S_B[4]>23)  // DALLAS_HOUR;
    {
    	Rcd_Info_Myself_Tmp[12]=0x11;
    	Rcd_Info_Myself_Tmp[13]=0x18;
    	Rcd_Info_Myself_Tmp[14]=0x1e;
    	Rcd_Info_Myself_Tmp[15]=0x1b;
    	Rcd_Info_Myself_Tmp[16]=0x42;
    	Rcd_Info_Myself_Tmp[17]=Temp_Pt_S_B[4]/0x10;
    	Rcd_Info_Myself_Tmp[18]=Temp_Pt_S_B[4]&0x0f;
    	Rcd_Info_Myself_Tmp[19]=0x3e;
    	Rcd_Info_Myself_Tmp[20]=0x3e;
        Store_Rcd_Info_Myself();
        
        Temp_Pt_S_B[4]=0;
    }

    if(Temp_Pt_S_B[2]>59)  // DALLAS_MINUTE;
    {
    	Rcd_Info_Myself_Tmp[12]=0x16;
    	Rcd_Info_Myself_Tmp[13]=0x12;
    	Rcd_Info_Myself_Tmp[14]=0x17;
    	Rcd_Info_Myself_Tmp[15]=0x1e;
    	Rcd_Info_Myself_Tmp[16]=0x1d;
    	Rcd_Info_Myself_Tmp[17]=0x0e;
    	Rcd_Info_Myself_Tmp[18]=0x42;
    	Rcd_Info_Myself_Tmp[19]=Temp_Pt_S_B[2]/0x10;
    	Rcd_Info_Myself_Tmp[20]=Temp_Pt_S_B[2]&0x0f;
        Store_Rcd_Info_Myself();
        
        Temp_Pt_S_B[2]=0;
    }

    if(Temp_Pt_S_B[0]>59)  // DALLAS_SECOND;
    {
    	Rcd_Info_Myself_Tmp[12]=0x1c;
    	Rcd_Info_Myself_Tmp[13]=0x0e;
    	Rcd_Info_Myself_Tmp[14]=0x0c;
    	Rcd_Info_Myself_Tmp[15]=0x18;
    	Rcd_Info_Myself_Tmp[16]=0x17;
    	Rcd_Info_Myself_Tmp[17]=0x0d;
    	Rcd_Info_Myself_Tmp[18]=0x42;
    	Rcd_Info_Myself_Tmp[19]=Temp_Pt_S_B[0]/0x10;
    	Rcd_Info_Myself_Tmp[20]=Temp_Pt_S_B[0]&0x0f;
        Store_Rcd_Info_Myself();
        
        Temp_Pt_S_B[0]=0;
    }
    
    Temp_Pt_S_B[0x0b]=0x06;
    Temp_Pt_S_B[0x0a]=0x20;

    Read_Time_From_Dallas();

}  // initial_dallas()


/***********************************************/
/*   judge_mainloop_int_dead                   */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void judge_mainloop_int_dead(void)
{
    BYTE  ram_axl;
    
    
    RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER=0;

    // EPA1  soft_time_1ms
    RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER++;	//-这个是很显然的看门狗处理程序标志,若正常的话会定时清零,否则溢出就是错误的
    if(RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER>50000)
    {
        disable();
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_EPA1_INT;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();
        enable();
        
        int_pend1=int_pend1 | 0x01;		//-是软件标志产生中断试试恢复
        RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER=0;
        RAM_CPU_INT_Rst_Cn[5]++;
    }
    
    // EPA0  INT_COM0
    ram_axl=p1_pin | 0xfe;
    if(RAM_CPU_Int_Moni.EPA0_INT_DEAD_COUNTER>500)
    {
        disable();
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_EPA0_INT;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();
        enable();
        
        int_pend=int_pend | 0x80;	//-这里也是软件产生中断,554_0

        RAM_CPU_Int_Moni.EPA0_INT_DEAD_COUNTER=0;
        RAM_CPU_INT_Rst_Cn[4]++;
    }
    else
    {
        if(ram_axl!=0xff)  //-长时间中断输入引脚上,没有出现需要的电平,才会判断错误
           RAM_CPU_Int_Moni.EPA0_INT_DEAD_COUNTER++;
    }
    
    // EXINT2  INT_COM1
    ram_axl=p3_pin | 0xbf;
    if(RAM_CPU_Int_Moni.EXINT2_INT_DEAD_COUNTER>5)
    {
        disable();
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_EXINT2_INT;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();
        enable();
        
        int_pend1=int_pend1 | 0x20;	//-554_1
        RAM_CPU_Int_Moni.EXINT2_INT_DEAD_COUNTER=0;
        RAM_CPU_INT_Rst_Cn[2]++;
    }
    else
    {
        if(ram_axl!=0xff)  
           RAM_CPU_Int_Moni.EXINT2_INT_DEAD_COUNTER++;
    }
    
    // EXINT3  INT_COM2
    ram_axl=p3_pin | 0x7f;
    if(RAM_CPU_Int_Moni.EXINT3_INT_DEAD_COUNTER>5)
    {
        disable();
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_EXINT3_INT;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();
        enable();
        
        int_pend1=int_pend1 | 0x40;	//-554_2
        RAM_CPU_Int_Moni.EXINT3_INT_DEAD_COUNTER=0;
        RAM_CPU_INT_Rst_Cn[3]++;
    }
    else
    {
        if(ram_axl!=0xff)  
           RAM_CPU_Int_Moni.EXINT3_INT_DEAD_COUNTER++;
    }

}


/***********************************************/
/*   judge_host_zf_enable                      */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void judge_host_zf_enable(void)
{
    BYTE  ram_axl;
    BYTE  ram_axh;
    
    ram_axh=HOST_ZF_enable;
    for(ram_axl=0;ram_axl<14;ram_axl++)
    {
        if((port_info[ram_axl].protocal_type!=PROTOCOL_NONE)&&(byte1(port_info[ram_axl].protocal_type)<0x80))	//-检测下行规约
        {	//-凡是配置了规约的端口,都必须初始化一段时间后,方口通讯
            if(Portx_Poll_First[ram_axl]!=YES)
                break;
        }
        
    }
    if(ram_axl>=14)	//-大于14说明所有的下位机都准备好向上位机发送内容了
        HOST_ZF_enable=YES;	//-说明没有检测到
    else
        HOST_ZF_enable=NO; 	//-只要出现一个端口延时不到即没有稳定,向后台发送数据就禁止
    
    if((ram_axh!=YES)&&(HOST_ZF_enable==YES))	//-表示所有的下位机都达到了一定时间,可以通过管理机向后台转发消息了
    {	//-只有第一次需要这样的处理,即从NO变为YES,,也是一种更新
        for(ram_axl=0;ram_axl<4;ram_axl++)
        {
            temp_ptD = (unsigned short*)&YX_transmite_table[ram_axl][0];
            temp_int = 0;
            //while(*temp_ptD < 2048)
            while(*temp_ptD < 4096)	//-这个也许没有任何意义,仅仅就是一个结束符而已
            {
                if((YX_State[(*temp_ptD)/16] & (1<<((*temp_ptD)&0x0f))) != 0)	//-转发表中的数值又是一个数据表示一位
                {	//-这个地方是一位一位的检查状态
                    temp_lp_int = YX_state_tr[ram_axl][temp_int/16];
                    temp_lp_int = temp_lp_int | (1 << (temp_int&0x0f));
                    YX_state_tr[ram_axl][temp_int/16] = temp_lp_int;
                }
                else
                {
                    temp_lp_int = YX_state_tr[ram_axl][temp_int/16];	//-取出一个字的数据
                    temp_lp_int = temp_lp_int & (0xffff ^ (1 << (temp_int&0x0f)));	//-对对应位赋0值
                    YX_state_tr[ram_axl][temp_int/16] = temp_lp_int;	//-放回新数据
                }
                temp_ptD ++;
                temp_int ++;
                if(temp_int>1023)
                break;
            }    
        }
    }
    
    if(HOST_YK_Doing==2)
    {
    	if(Judge_Time_In_MainLoop(HOST_YK_Doing_Begin_Time,40000)==YES)
    	{
    		HOST_YK_Doing=0;
    	}
    }
    else
    {
    	if(HOST_YK_Doing!=0)  // && !=2
    	{
	    	if(Judge_Time_In_MainLoop(HOST_YK_Doing_Begin_Time,3000)==YES)
    		{
    			HOST_YK_Doing=0;
    		}
    	}	
    }
    
    
}
    
/***********************************************/
/*   byq_dw_yx_translate_to_yc                 */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void byq_dw_yx_translate_to_yc(void)		//-变压器档位
{
    BYTE  ram_axl;
    BYTE  ram_axh;
    WORD  ram_bx;
    
    for(ram_axl=0;ram_axl<8;ram_axl++)
    {
        if(Byq_Dw[ram_axl].yc_val_no<=(sizeof(YC_State)/sizeof(YC_State[0])))
        {
            ram_bx=0;
            for(ram_axh=0;ram_axh<32;ram_axh++)
            {
                if( (YX_State[(Byq_Dw[ram_axl].yx_no[ram_axh])/16] & (1<<(Byq_Dw[ram_axl].yx_no[ram_axh] & 0x000f)))!=0)	//-检查特定遥信位,这些位肯定是定义给了变压器档位用的
                    ram_bx=ram_bx+Byq_Dw[ram_axl].yx_weight[ram_axh];	//-累加和的目的是什么???
            }
            //-可能就是通过遥信状态,来判断变压器档位
            YC_State[Byq_Dw[ram_axl].yc_val_no]=ram_bx;	//-遥测量记录了变压器档位,可能意义不清楚,但是可以交流就行
        }
        
    }
}

/***********************************************/
/*   yx_or_calculate                           */
/***********************************************/
//-就是把定义的8个遥信位的值进行或运算之后值赋给目的遥信位
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void yx_or_calculate(void)	//-这段程序也许有他的功能,但是不使用并不影响他的作用
{
    BYTE  ram_axl;
    BYTE  ram_axh;
    WORD  ram_ax;
    for(ram_axl=0;ram_axl<(sizeof(YX_OR_Table)/sizeof(YX_OR_Table[0]));ram_axl++)
    {
        if(YX_OR_Table[ram_axl][0]<0x1000)
        {
            ram_ax=YX_State[(YX_OR_Table[ram_axl][0])/16];	//-*/16 在这个里面的程序表示把位的作用转化为单位,这个遥信号是以位为单位数的,除16后得字单位
            for(ram_axh=1;ram_axh<9;ram_axh++)
            {	//-检查表格中配置的数值,也是以位为单位数的
                if( (YX_State[(YX_OR_Table[ram_axl][ram_axh])/16] & (1<<(YX_OR_Table[ram_axl][ram_axh] & 0x000f)))!=0)	//-检测位值
                {
                  if((YX_State[(YX_OR_Table[ram_axl][0])/16]&(1<<(YX_OR_Table[ram_axl][0] & 0x000f)))==0)
                  {
                    YX_State[(YX_OR_Table[ram_axl][0])/16]|=(1<<(YX_OR_Table[ram_axl][0] & 0x000f));	//-这就是对对应数据库的一个处理
                    
                  }
                   ram_axh=100;	//-为这个值说明目的遥信位肯定为1
                }    
            }
            if(ram_axh<80)
            {
            	YX_State[(YX_OR_Table[ram_axl][0])/16]&=(0xffff-(1<<(YX_OR_Table[ram_axl][0] & 0x000f)));	//-全为0之后,对目的遥信位赋0
            }
            if(YX_State[(YX_OR_Table[ram_axl][0])/16] != ram_ax)	//-不等说明有变化
            {            	
                Core_Temp_ShortInt=YX_OR_Table[ram_axl][0];	//-遥信位数
				Core_Temp_Loop=Core_Temp_ShortInt/16;	//-遥信字数
		    	Core_Det_Pt = (WORD *)&YX_State[Core_Temp_Loop];	//-目标遥信值
				Core_Src_Pt = &yx_change[yx_chg_in_pt].offset_no;
				*Core_Src_Pt = Core_Temp_Loop/2;       //-确定变位YX的偏移量
			    Core_Src_Pt++; // offset
		    	if(((Core_Temp_Loop)& 0x01)==0)	//-由于一个结构体中存放两个字的遥信值所以需要判断奇偶数 
				{	
				   *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX0
		    	   *Core_Src_Pt = *(Core_Det_Pt+0x01);   Core_Src_Pt++; // YX1
				   *Core_Src_Pt = (WORD)(1<<(Core_Temp_ShortInt%16));    Core_Src_Pt++;   // CHG0	,,记录哪一位发生了遥信变化,1表示变化了
				   *Core_Src_Pt = 0x00;                                 // CHG1
		    	} 
				else
				{
		    	   *Core_Src_Pt = *(Core_Det_Pt-0x01);   Core_Src_Pt++; // YX0
				   *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX1
				   *Core_Src_Pt = 0x00;                  Core_Src_Pt++; // CHG0
		    	   *Core_Src_Pt = (WORD)(1<<(Core_Temp_ShortInt%16));   // CHG1
				}
   
				yx_chg_in_pt ++;	//-遥信变化数组中增加了一个值
		    	yx_chg_in_pt &= 0xff;         

		    	// insert SOE
		    	
				Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;	//-通讯管理机自己产生了SOE事件记录,不是下位机上送上来的
				//ms
				*Core_Det_Pt = REG_1Msecond;
				Core_Det_Pt++;
				//sec min
				*Core_Det_Pt = (WORD)REG_Minute*0x100 + REG_Second;
				Core_Det_Pt++;
				//hour day
				*Core_Det_Pt = (WORD)REG_Date*256 + REG_Hour;
				Core_Det_Pt++;
				// channel state
				*Core_Det_Pt = YX_OR_Table[ram_axl][0];
				if(ram_axh>100)
				*Core_Det_Pt += 0x8000;	//-说明状态为1,即SOE事件为合状态

				yx_soe_in_pt++;
				if(yx_soe_in_pt>1023)
				    yx_soe_in_pt = 0;       

            }
        }
    }       
}           

/***********************************************/
/*   yx_and_calculate                           */
/***********************************************/
/-这样做的程序的目的,可能就是一个逻辑功能程序片段,但是并没有使用,需要根据实际
//-情况进行运用
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void yx_and_calculate(void)	//-这个和上面有区别学习的地方吗,可以找到突破口吗,是表示遥信与计算吗
{
    BYTE  ram_axl;
    BYTE  ram_axh;
    WORD  ram_ax,ram_bx;
    for(ram_axl=0;ram_axl<64;ram_axl++)
    {
        if(yx_and_table[ram_axl].yx_destion_no!=0xffff)	//-与遥信设置表,这样的表格其实就是组织了属性值
        {
            ram_ax=YX_State[yx_and_table[ram_axl].yx_destion_no/16];	//-取出遥信值(字为单位)
            for(ram_axh=0;ram_axh<8;ram_axh++)
            {
                ram_bx=YX_State[yx_and_table[ram_axl].yx_source_no[ram_axh]/16]&(1<<(yx_and_table[ram_axl].yx_source_no[ram_axh] & 0x000f));
                if(((ram_bx==0)&&(yx_and_table[ram_axl].yx_source_not_flag[ram_axh]==0x55))||
                	((ram_bx!=0)&&(yx_and_table[ram_axl].yx_source_not_flag[ram_axh]==0xaa)))
                {
                  if((YX_State[yx_and_table[ram_axl].yx_destion_no/16]&(1<<(yx_and_table[ram_axl].yx_destion_no& 0x000f)))!=0)
                   {
                    YX_State[yx_and_table[ram_axl].yx_destion_no/16]&=(0xffff-(1<<(yx_and_table[ram_axl].yx_destion_no& 0x000f)));
                   }
                  ram_axh=100;
                }

            }
            if(ram_axh<80)
            {
            	YX_State[yx_and_table[ram_axl].yx_destion_no/16]|=(1<<(yx_and_table[ram_axl].yx_destion_no & 0x000f));
            }
            if(YX_State[yx_and_table[ram_axl].yx_destion_no/16]!=ram_ax)
            {
            	
                		Core_Temp_ShortInt=yx_and_table[ram_axl].yx_destion_no;
				    Core_Temp_Loop=Core_Temp_ShortInt/16;
		    		Core_Det_Pt = (WORD *)&YX_State[Core_Temp_Loop];
				    Core_Src_Pt = &yx_change[yx_chg_in_pt].offset_no;
				   *Core_Src_Pt = Core_Temp_Loop/2;          Core_Src_Pt++; // offset
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

		    		// insert SOE
		    		
				    Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;
					//ms
				   *Core_Det_Pt = REG_1Msecond;
				    Core_Det_Pt++;
					//sec min
				   *Core_Det_Pt = (WORD)REG_Minute*0x100 + REG_Second;
				    Core_Det_Pt++;
				    //hour day
				   *Core_Det_Pt = (WORD)REG_Date*256 + REG_Hour;
				    Core_Det_Pt++;
				    // channel state
				   *Core_Det_Pt = yx_and_table[ram_axl].yx_destion_no;
				   if(ram_axh>100)
				   *Core_Det_Pt += 0x8000;

					yx_soe_in_pt++;
				    if(yx_soe_in_pt>1023)
					    yx_soe_in_pt = 0;       

            }
        }   
    }       
}           

/***********************************************/
/*   ylb_out_yx                           */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void ylb_out_yx(void)	//-没有绝对的东西,yx在也不仅仅是通断了,就是一个标志位而已,如果定义的好的话
{
	BYTE  ram_axl;
	WORD  ram_ax,ram_bx;
	
	for(ram_axl=0;ram_axl<64;ram_axl++)
	{
	  if((log_out[ram_axl].yk_add!=0xff)&&(log_out[ram_axl].yk_add>=0x30)&&(log_out[ram_axl].yk_add<=0x3f)&&(unit_info[log_out[ram_axl].yk_add].yk_num==8))
	  {
	    if((log_out[ram_axl].yx_no!=0xffff)&&(log_out[ram_axl].yk_no<8))
	    {
	      ram_ax=log_out[ram_axl].yx_no/16;
	      ram_bx=log_out[ram_axl].yx_no%16;	//-位地址转化为字地址
	      if((YX_State[ram_ax]&(0x0001<<ram_bx))!=0)	//-数据库中的遥信值,再也不仅仅是表面上的遥信通断了
	      	log_out_yx_state[log_out[ram_axl].yk_add-0x30]|=(0x01<<log_out[ram_axl].yk_no);
	      else
	      	log_out_yx_state[log_out[ram_axl].yk_add-0x30]&=(~(0x01<<log_out[ram_axl].yk_no));	
	    }
	    
	  }
	}

}                        
/***********************************************/
/*   judge_yc_change                           */
/***********************************************/
//-周期的判断数据库中的遥测值是否发生变化,变化的填1,有4个数值
/*~~~~~~~~~~    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void judge_yc_change(void)	//-这个程序中没有处理就把0变为了ffff那么直接跳出这个程序
{
	WORD  the_ram_ax;
	BYTE  the_ram_bx;
	for(the_ram_bx=0;the_ram_bx<255;the_ram_bx++)
	{
		if(IEC101_YC_Chg_Out_Table[the_ram_bx*2]>0x7ff)	//-偶数是序号,奇数单元里面存放的是分辨率
			the_ram_bx=254;	//-老一套跳出循环
		else
		{
			the_ram_ax=IEC101_YC_Chg_Out_Table[the_ram_bx*2];	//-记录的是遥测序号
			for(temp_loop=0;temp_loop<4;temp_loop++)
			{
				if(YC_State[the_ram_ax]>YC_Chg_Last_Value[temp_loop][the_ram_bx])	//-我觉的这里就有变化输入顺序的值
				{
					if((YC_State[the_ram_ax]-YC_Chg_Last_Value[temp_loop][the_ram_bx])>=IEC101_YC_Chg_Out_Table[the_ram_bx*2+1])
						YC_Chg_FLAG[the_ram_ax]|=1<<temp_loop;	//-怎样叫做遥测量发生了变化呢,变化值大于设定的变动量
				}
				else
				{
			 		if((YC_Chg_Last_Value[temp_loop][the_ram_bx]-YC_State[the_ram_ax])>=IEC101_YC_Chg_Out_Table[the_ram_bx*2+1])
						YC_Chg_FLAG[the_ram_ax]|=1<<temp_loop;
				}
			}
		}
	}
		
}

/***********************************************/
/*   judge_led_yc                           */
/***********************************************/
/*~~~~~~~~~~    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void judge_led_yc(void)	//-这个地方其实实现了,数据的从新映射,即把数据库的量,按照自己的需要从新排列到我需要的空间
{
	WORD  the_ram_ax;
	BYTE  the_ram_bx;

	for(the_ram_ax=0;the_ram_ax<32;the_ram_ax++)
	 {
		if(LED_mea_info[the_ram_ax].xianlu_no==0xffff)	//-从这里得到的大量信息,可以使得猜测范围更广
			break;	//-其实这里就是和那个一样做的非常详细,非常自由,就一ffff为结束符
		else
		{
			LED_mea_buf.xianlu_no[the_ram_ax]=LED_mea_info[the_ram_ax].xianlu_no;
			for(the_ram_bx=0;the_ram_bx<16;the_ram_bx++)
			{
			    LED_mea_buf.yc_state[the_ram_ax*16+the_ram_bx]=YC_State[LED_mea_info[the_ram_ax].YC_NO[the_ram_bx]];	//-从这个缓冲区调入宁一个缓冲区
			}	//-上面是把数据库的量怎么着吗,,还有一种可能性就是上送上来的数据可能是乱的,需要整理一下,以便数据入库
		}
	 }
	LED_mea_buf.xianlu_nums=the_ram_ax;	//-记录几个线路号
	
}

/***********************************************/
/*   judge_led_GJ                           */
/***********************************************/
/*~~~~~~~~~~    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void judge_led_GJ(void)	//-最多可在整个遥信中提取32位遥信值
{
	WORD  the_ram_ax,the_ram_bx,the_ram_cx;

	for(the_ram_ax=0;the_ram_ax<32;the_ram_ax++)
	{
     	 if(LED_GJ_info[the_ram_ax].xianlu_no==0xffff)
			break;
		 else
		 {
		 	LED_GJ_buf.xianlu_no[the_ram_ax]=LED_GJ_info[the_ram_ax].xianlu_no;
		 	the_ram_bx=LED_GJ_info[the_ram_ax].YX_NO/16;	//-字数
		 	the_ram_cx=LED_GJ_info[the_ram_ax].YX_NO%16;	//-对应字数里面的位数
		 	if((YX_State[the_ram_bx]&(0x0001<<the_ram_cx))!=0)	//-这里是对位值的检测
		 	{
		 	    LED_GJ_buf.yx_state[the_ram_ax/16]|=(0x0001<<(the_ram_ax%16));	//-而这里每个元素是16位,所以要把一位的组织成16位
		 	}
		 	else
		 	{
		 		LED_GJ_buf.yx_state[the_ram_ax/16]&=(~(0x0001<<(the_ram_ax%16)));
		 			
		 	}
		 }	  
	}
	LED_GJ_buf.xianlu_nums=the_ram_ax;	//-记录一共有几个线路号
	
}

/***********************************************/
/*   MAIN_DSA_208_VER_INFO_SET    程序版本      */
/***********************************************/
/*~~~~~~~~~~    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MAIN_DSA_208_VER_INFO_SET(void)
{
	MAIN_DSA_208_VER_INFO[0]=0xff;
	MAIN_DSA_208_VER_INFO[1]=0x01;		
	MAIN_DSA_208_VER_INFO[2]=0x00;	
	MAIN_DSA_208_VER_INFO[3]=0x01;		//版本101
	MAIN_DSA_208_VER_INFO[4]=0x20;		//年高
	MAIN_DSA_208_VER_INFO[5]=0x05;		//年低
	MAIN_DSA_208_VER_INFO[6]=0x08;		//月
	MAIN_DSA_208_VER_INFO[7]=0x11;		//日
	
}	

/***********************************************/
/*   judge_yc_up_to_yx                         */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void judge_yc_up_to_yx(void)	//-大胆猜测遥测转到遥信的意思就是遥测的数值信息,直接使用遥信信息直观表达
{
	WORD	ram_ax;
	WORD    ram_bx;
	BYTE    ram_axh;
	BYTE    ram_axl;
	WORD    ram_cx;
	
	for(temp_loop=0;temp_loop<32;temp_loop++)
	{
		if(ycup_to_yx[temp_loop].yc_no>0x0fff)	//-草,怎么整这么多的属性表啊,烦不烦啊
		{
			temp_loop=100;	//-呵呵,无语,一个人一个想法,无所谓好与坏
		}
		else	
		{
			if(ycup_to_yx[temp_loop].yx_no<0x1000)	//-配置的属性值在有效值范围内才有意义
			{
				ram_axh=ycup_to_yx[temp_loop].yx_no/16;
				ram_axl=ycup_to_yx[temp_loop].yx_no%16;
				ram_ax=YX_State[ram_axh];
				ram_cx=ram_ax;
				switch(ycup_to_yx[temp_loop].yc_up_down)	//-AA上限告警，55下限告警
				{
					case 0xaa:	
						if(ycup_to_yx[temp_loop].yc_up_value<0x800)	//-越限值  >2048为负值
						{    //-以下是正数
						 	if((YC_State[ycup_to_yx[temp_loop].yc_no]<0x800)&&(YC_State[ycup_to_yx[temp_loop].yc_no]>ycup_to_yx[temp_loop].yc_up_value))
								ram_cx =ram_ax | (1<<ram_axl);	//-到这里就表示越限了,1表示越限
							else
							{
								if(ycup_to_yx[temp_loop].yc_hc_value<ycup_to_yx[temp_loop].yc_up_value)	//-回差
								{
									ram_bx=ycup_to_yx[temp_loop].yc_up_value-ycup_to_yx[temp_loop].yc_hc_value;
									if((YC_State[ycup_to_yx[temp_loop].yc_no]<ram_bx)||(YC_State[ycup_to_yx[temp_loop].yc_no]>0x7ff))
										ram_cx =ram_ax & (0xffff-(1<<ram_axl));	//-给0值,取消越限
								}
							}
						}		
						else
						{	//-以下是负数
							if((YC_State[ycup_to_yx[temp_loop].yc_no]>ycup_to_yx[temp_loop].yc_up_value)||(YC_State[ycup_to_yx[temp_loop].yc_no]<0x800))
								ram_cx =ram_ax | (1<<ram_axl);
							else
							{
								if(ycup_to_yx[temp_loop].yc_hc_value<(ycup_to_yx[temp_loop].yc_up_value-0x7ff))
								{
									ram_bx=ycup_to_yx[temp_loop].yc_up_value-ycup_to_yx[temp_loop].yc_hc_value;
									if(YC_State[ycup_to_yx[temp_loop].yc_no]<ram_bx)
										ram_cx =ram_ax & (0xffff-(1<<ram_axl));
								}
							}	
		        			
		        			
						}
						break;
					case 0x55:	
						if(ycup_to_yx[temp_loop].yc_up_value<0x800)
						{    
						 	if((YC_State[ycup_to_yx[temp_loop].yc_no]<ycup_to_yx[temp_loop].yc_up_value)||(YC_State[ycup_to_yx[temp_loop].yc_no]>0x800))
								ram_cx =ram_ax | (1<<ram_axl);
							else
							{
								if((ycup_to_yx[temp_loop].yc_hc_value+ycup_to_yx[temp_loop].yc_up_value)<0x800)
								{
									ram_bx=ycup_to_yx[temp_loop].yc_up_value+ycup_to_yx[temp_loop].yc_hc_value;
									if(YC_State[ycup_to_yx[temp_loop].yc_no]>ram_bx)
										ram_cx =ram_ax & (0xffff-(1<<ram_axl));
								}
							}
						}		
						else
						{
							if((YC_State[ycup_to_yx[temp_loop].yc_no]<ycup_to_yx[temp_loop].yc_up_value)&&(YC_State[ycup_to_yx[temp_loop].yc_no]>0x7ff))
							ram_cx =ram_ax | (1<<ram_axl);
							else
							{
								if(ycup_to_yx[temp_loop].yc_up_value<0x1000-ycup_to_yx[temp_loop].yc_hc_value)
								{
								ram_bx=ycup_to_yx[temp_loop].yc_up_value+ycup_to_yx[temp_loop].yc_hc_value;
									if((YC_State[ycup_to_yx[temp_loop].yc_no]>ram_bx)||(YC_State[ycup_to_yx[temp_loop].yc_no]<0x800))
									ram_cx =ram_ax & (0xffff-(1<<ram_axl));
								}
							}	
		        			
		        			
						}
						break;
					default:
						break;
				}
				if(ram_cx!=ram_ax)	//-若经过判断有变化的话就进行如下处理
				{	                	
					YX_State[ram_axh]=ram_cx;	//-记录变化之后的值
		    		Core_Det_Pt = (WORD *)&YX_State[ram_axh];
		    		    
					Core_Src_Pt = &yx_change[yx_chg_in_pt].offset_no;
					*Core_Src_Pt = ram_axh/2;          Core_Src_Pt++; // offset
		    		if(((ram_axh)& 0x01)==0)
					{
					    *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX0
		    		    *Core_Src_Pt = *(Core_Det_Pt+0x01);   Core_Src_Pt++; // YX1
					    *Core_Src_Pt = (WORD)(1<<(ram_axl));  Core_Src_Pt++;   // CHG0
					    *Core_Src_Pt = 0x00;                                 // CHG1
		    		} 
					else
					{
		    		    *Core_Src_Pt = *(Core_Det_Pt-0x01);   Core_Src_Pt++; // YX0
					    *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX1
					    *Core_Src_Pt = 0x00;                  Core_Src_Pt++; // CHG0
		    		    *Core_Src_Pt = (WORD)(1<<(ram_axl));   // CHG1
					}
                     	
					yx_chg_in_pt ++;	//-这个系统中的yx已经不是简单意义上的普通遥信了,已经转化为一个标志位集合了
		    		yx_chg_in_pt &= 0xff;         
					                
                }
            }
		}		
	}	
	
}	

/***********************************************/
/*   judge_bh_to_yx_delay_time                 */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void judge_bh_to_yx_delay_time(void)
{
    WORD  ram_ax;
    WORD  ram_bx;
    WORD  ram_cx;
    WORD  ram_dx;
    
    for(ram_bx=0;ram_bx<512;ram_bx++)
    {
        ram_ax=BH_Event_To_SOE[ram_bx].YX_State_No;
        if(ram_ax<4096)
        {
		    for(ram_dx=0;ram_dx<512;ram_dx++)
		    {
        		ram_cx=BH_Event_To_SOE_Ret[ram_dx].YX_State_No;
		        if(ram_cx<4096)
        		{
        			if(ram_cx==ram_ax)  goto judge_next;	//-为什么出现相等的就结束呢,可能就是一种属性配置的需要,只是我没有做而已
        		}
        		else
        			break;
        	}
            if(PROTOCOL_CDT_YKFG==NO)	//-在返回列表中没有这样的号就进行以下判断?  复归判断
            {
              if(Judge_LongTime_In_MainLoop(BH_To_YX_Delay_Begin_Time[ram_bx],BH_EVENT_TO_SOE_YX_RETURN_TIME)==YES)  // 150*2s=5min
               	  YX_State[ram_ax/16]=YX_State[ram_ax/16] & (0xffff-(1<<(ram_ax%16)));	//-自动复归就是对对应意思位清零
            }
            else if((YX_FG_FLAG==YES)&&(PROTOCOL_CDT_YKFG==YES))
            {	//-遥控复归
             	if((YX_State[ram_ax/16]&(0x0001<<(ram_ax%16)))!=0)	//-为了防止重复记录
             	{
             	  Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;	//-遥信事件的记录
             	  //ms
             	  *Core_Det_Pt =REG_1Msecond;
             	  Core_Det_Pt++;
             	  //sec min
             	  *Core_Det_Pt =REG_Second+REG_Minute*256;
             	  Core_Det_Pt++;
             	  //hour day
             	  *Core_Det_Pt =REG_Hour+REG_Date*256;
             	  Core_Det_Pt++;
             	  // channel state
             	  *Core_Det_Pt =ram_ax;
             	  yx_soe_in_pt++;
             	  if(yx_soe_in_pt>1023)
             	   yx_soe_in_pt = 0;
             	}
             	YX_State[ram_ax/16]=YX_State[ram_ax/16] & (0xffff-(1<<(ram_ax%16)));	//-可靠复归
            }
judge_next:
			;
        }
        else
        	break;	//-必须连续配置
    }
    YX_FG_FLAG=NO; 	//-这个值说明已经复归了,不需要了.
}

/***********************************************/
/*   initial_bh_to_yx                          */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void initial_bh_to_yx(void)
{
    WORD  ram_ax;
    WORD  ram_bx;
    
    for(ram_bx=0;ram_bx<512;ram_bx++)
    {
        ram_ax=BH_Event_To_SOE[ram_bx].YX_State_No;
        if(ram_ax<4096)
        {
            YX_State[ram_ax/16]=YX_State[ram_ax/16] & (0xffff - (1<<(ram_ax%16)));
        }
    }
    
}

/***********************************************/
/*     initial       for   Main  procedure     */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void initial(void)      // static mean inside use function
{
        WORD  ram_bx;
    far BYTE *ram_base_addr;
    far WORD *ram_base_addr_w;


//............................ initial 296CPU ...............................//
    p1_dir =0x0f;//0x81;	,,配置引脚方向,1表示输入  0代表输出
    p1_mode=0x01;	//-0 一般目的的IO信号    1特殊功能引脚
    p1_reg =0x7f;	//-这里还复位了554
    
    p2_dir =0x1e;
    p2_mode=0x17;	//-这个地方配置了一个串口接收发送引脚  两个中断
    p2_reg =0xff;//0x9f;	//-作为IO时引脚是什么就是什么数据
    
    p3_dir =0xc0;
    p3_mode=0xff;
    p3_reg =0xff;	//-作为特殊功能引脚的时候,这个值是无意义的
    

    extint_con=0x0f;//0xcf;   // low level to fall edge ???		,,定义了外部信号的事件
  	//-下面定义的时钟计数,内部时钟计数是12MHz,即每增加一计时是1/12uS
    t1control=0xc0;      // 1100$0000 B count enable       : enable timer1
                         //             count direction    : count up
                         //             EPA clk direct&mode: clk source-f/4,direct source-UD(bit 6)
                         //             EPA clk prescale   : divide by 1

    epa1_con=0x0048;     // 0000$0000$0100$1000 B
                         // disable remap
                         // timer1 is the reference timer
                         // compare mode	,,选择比较模式
                         // toggle output pin
                         // re-enable
                         // disable reset timer
    timer1   =0;
    epa1_time=timer1+EPA1_PERIOD;	//-设定了比较时间
    
    epa0_con=0x10;	//-定时器1为基准,采用扑捉模式
    epa2_con=0x00;	//-其实就是没有使用这个扑捉
    epa3_con=0x0000;
    

    REG_TimeTemp     =0;
    REG_Surplus_Time =0;


    Time_1ms_Counter   =0;
    Time_2048ms_Counter=0;


//............................ initial DALLAS ...............................//
    Read_Time_From_Dallas();
    
    ram_base_addr=(far BYTE *)DALLAS_RCD_MYSELF_SAV_PTR_ADDR;
    if((ram_base_addr[0] ^ ram_base_addr[4])!=0xff) 
    	goto rcd_info_myself_ptr_err;
    ram_base_addr=(far BYTE *)DALLAS_RCD_MYSELF_TAK_PTR_ADDR;
    if((ram_base_addr[0] ^ ram_base_addr[4])!=0xff) 
    	goto rcd_info_myself_ptr_err;
    goto rcd_info_myself_ptr_good;

rcd_info_myself_ptr_err:
    ram_base_addr=(far BYTE *)DALLAS_RCD_MYSELF_SAV_PTR_ADDR;
    ram_base_addr[0]=0x00;
    ram_base_addr[4]=0xff;
    ram_base_addr=(far BYTE *)DALLAS_RCD_MYSELF_TAK_PTR_ADDR;
    ram_base_addr[0]=0x00;
    ram_base_addr[4]=0xff;

    Rcd_Info_Myself_Tmp[0]=0;
    Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_DALLAS;
    Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_RCD_MYSELF_PTR_ADDR;
    Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_PTR_ADDR_XOR_ERR;
    Rcd_Info_Myself_Tmp[4]=0;
    Rcd_Info_Myself_Tmp[5]=0;
    Rcd_Info_Myself_Tmp[6]=0;
    Rcd_Info_Myself_Tmp[7]=0;
    Store_Rcd_Info_Myself();
    
rcd_info_myself_ptr_good:
    ram_base_addr=(far BYTE *)DALLAS_RCD_SYSTEM_SAV_PTR_ADDR;
    if((ram_base_addr[0] ^ ram_base_addr[4])!=0xff) 
    	goto rcd_info_system_ptr_err;
    ram_base_addr=(far BYTE *)DALLAS_RCD_SYSTEM_TAK_PTR_ADDR;
    if((ram_base_addr[0] ^ ram_base_addr[4])!=0xff) 
    	goto rcd_info_system_ptr_err;

    goto rcd_info_system_ptr_good;

rcd_info_system_ptr_err:
    ram_base_addr=(far BYTE *)DALLAS_RCD_SYSTEM_SAV_PTR_ADDR;
    ram_base_addr[0]=0x00;
    ram_base_addr[4]=0xff;
    ram_base_addr=(far BYTE *)DALLAS_RCD_SYSTEM_TAK_PTR_ADDR;
    ram_base_addr[0]=0x00;
    ram_base_addr[4]=0xff;

    Rcd_Info_Myself_Tmp[0]=0;
    Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_DALLAS;
    Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_RCD_SYSTEM_PTR_ADDR;
    Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_PTR_ADDR_XOR_ERR;
    Rcd_Info_Myself_Tmp[4]=0;
    Rcd_Info_Myself_Tmp[5]=0;
    Rcd_Info_Myself_Tmp[6]=0;
    Rcd_Info_Myself_Tmp[7]=0;
    Store_Rcd_Info_Myself();
    
rcd_info_system_ptr_good:
    
    initial_dallas();
    
    if(RAM_CPU_RST_TAG1!=0x55aa) 
    	goto ColdRst;
    if(RAM_CPU_RST_TAG2!=0xaa55) 
    	goto ColdRst;
    ram_base_addr_w=(far WORD *)CN_CPU_RST_HOT;

    if(ram_base_addr_w[0]<0xffff)
    {
        ram_base_addr_w[0]++;
    }
    Rcd_Info_Myself_Tmp[0]=0;
    Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
    Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_RST;
    Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPURST_HOT;
    Rcd_Info_Myself_Tmp[4]=0;
    Rcd_Info_Myself_Tmp[5]=0;
    Rcd_Info_Myself_Tmp[6]=byte1(ram_base_addr_w[0]);
    Rcd_Info_Myself_Tmp[7]=byte0(ram_base_addr_w[0]);
    Store_Rcd_Info_Myself();
    goto RstNoInc;
ColdRst:
    ram_base_addr_w=(far WORD *)CN_CPU_RST_COLD;
    if(ram_base_addr_w[0]<0xffff)
    {
        ram_base_addr_w[0]++;
    }
    Rcd_Info_Myself_Tmp[0]=0;
    Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
    Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_RST;
    Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPURST_COLD;
    Rcd_Info_Myself_Tmp[4]=0;
    Rcd_Info_Myself_Tmp[5]=0;
    Rcd_Info_Myself_Tmp[6]=byte1(ram_base_addr_w[0]);
    Rcd_Info_Myself_Tmp[7]=byte0(ram_base_addr_w[0]);
    Store_Rcd_Info_Myself();

    RAM_CPU_RST_TAG1=0x55aa;
    RAM_CPU_RST_TAG2=0xaa55;
RstNoInc:


    p1_reg=0x8f;//0xf1;	//-作为IO引脚时,将一直保持设定状态
    p2_reg=0x1f;//0xff;      // let CAN return from hardware reset, p2 pins as output is only rst_CANx 
	//-关系到CAN网的设置就不一样了,因为这里并没有这样的网络
//.............................. initial  comuse ...............................//
  
    main_loop=0;

    RAM_CPU_Int_Moni.EXINT0_INT_DEAD_COUNTER=0;
    RAM_CPU_Int_Moni.EXINT1_INT_DEAD_COUNTER=0;
    RAM_CPU_Int_Moni.EXINT2_INT_DEAD_COUNTER=0;
    RAM_CPU_Int_Moni.EXINT3_INT_DEAD_COUNTER=0;
    RAM_CPU_Int_Moni.EPA0_INT_DEAD_COUNTER  =0;
    RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER  =0;
    RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER  =0;
    
    RAM_CPU_INT_Rst_Cn[0]=0;  // EXINT0_INT
    RAM_CPU_INT_Rst_Cn[1]=0;  // EXINT1_INT
    RAM_CPU_INT_Rst_Cn[2]=0;  // EXINT2_INT
    RAM_CPU_INT_Rst_Cn[3]=0;  // EXINT3_INT
    RAM_CPU_INT_Rst_Cn[4]=0;  // EPA0_INT
    RAM_CPU_INT_Rst_Cn[5]=0;  // EPA1_INT
    RAM_CPU_INT_Rst_Cn[6]=0;  // MAINLOOP
    RAM_CPU_INT_Rst_Cn[7]=0;  // Reserved
    
    SIO_CAN_Need_Reset=0;   // PORTs need reset flag
    

//.............................. initial   PORTs ...............................//
    Initial_CPUCOM();	//-这个虽然编译没有报错,但是实际却不存在,可能是编译系统的问题,需要注意
    
//    Initial_CAN(0);
//    Initial_CAN(1);
//-整个程序并没有变化仅仅是把CAN给去掉了
    init_all_port();	//-对554进行了初始化

    ram_base_addr_w=(far WORD *)(&YX_State);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(YX_State)/sizeof(YX_State[0]);
        ram_bx++)
    {
        ram_base_addr_w[ram_bx]=0;
    }

    ram_base_addr_w=(far WORD *)(&YC_State);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(YC_State)/sizeof(YC_State[0]);
        ram_bx++)
    {
        ram_base_addr_w[ram_bx]=0;
    }

    ram_base_addr_w=(far WORD *)(&YM_State);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(YM_State)/sizeof(YM_State[0]);
        ram_bx++)
    {
        ram_base_addr_w[ram_bx*2+0x00]=0;
        ram_base_addr_w[ram_bx*2+0x01]=0;
    }
	 //变化YC初始化
    for(ram_bx=0;ram_bx<2048;ram_bx++)									
    {
   	   YC_Chg_FLAG[ram_bx]=0xff00;
    }
    for(ram_bx=0;ram_bx<256;ram_bx++)									
    {
	 byte1(YC_Chg_FLAG[IEC101_YC_Chg_Out_Table[ram_bx*2]])=byte0(ram_bx);
	 YC_Chg_Last_Value[0][ram_bx]=0x0000;
    	 YC_Chg_Last_Value[1][ram_bx]=0x0000;
   	 YC_Chg_Last_Value[2][ram_bx]=0x0000;
   	 YC_Chg_Last_Value[3][ram_bx]=0x0000;
    }

    ram_base_addr_w=(far WORD *)(&Cn_SIO_CAN_Reset);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(Cn_SIO_CAN_Reset)/sizeof(Cn_SIO_CAN_Reset[0]);
        ram_bx++)
    {
        ram_base_addr_w[ram_bx]=0;
    }
    
    for(ram_bx=0;ram_bx<12;ram_bx++)
    {
      port_recv_pt[ram_bx]=0;
      port_send_pt[ram_bx]=0;
      port_recv_dl[ram_bx]=0;
      port_send_len[ram_bx]=0;
    }

//............................ initial protocols ............................//
    Ex_init();                // must initial before all Ports !

    set_init();               // include set's port_initial & protocol_initial
    p2_img = p2_pin & 0x08;               
    if(p2_img == 0)
        sp_baud = CPU_SIO_BAUD_TO_LED;

    Protocol_Init();
    
    for(port_no=0;port_no<12;port_no++)
    {
        P554_Port_Transing_Lastbyte[port_no]=NO;
    }    

    for(port_no=0;port_no<16;port_no++)
    {
        port_mirror[port_no] = 0x0f;	//-初始状态映射是无效的
        log_out_yx_state[port_no]=0;
    }
//    if need monitor any PORT info , initial 'port_mirror' here
//    port_mirror[11]=0x3a;
//    port_mirror[14]=0x3a;

    for(main_loop=0;main_loop<256;main_loop++)
        unit_set_flag[main_loop] = 0x55;

    for(port_no=0;port_no<14;port_no++)
        Portx_Poll_First[port_no] = NO;
    
    initial_bh_to_yx();
    YX_FG_FLAG=NO;
    HOST_ZF_enable=NO;	//-这个仅仅是后台规约的一种,与整体并没有联系
    HOST_YK_Doing =0;
    
//.............................. initial INTs ...............................//
//    int_mask =0xf8;  // bit 7-0: EPA0  , SIORec , SIOTran, Exint1,
//    int_pend =0x00;  //          Exint0, Reserve, T2_Ovr , T1_Ovr.
    int_mask =0xe0;  // bit 7-0: EPA0  , SIORec , SIOTran, Reserve,
    int_pend =0x00;  //          Reserve, Reserve, T2_Ovr , T1_Ovr.
    //-他可以修改这个程序不简单,有用的东西不能丢到他的手上
    int_mask1=0x61;  // bit 7-0: NMI      , Exint3 , Exint2 , EPAOvr2_3,
    int_pend1=0x00;  //          EPAOvr1_0, EPA3   , EPA2   , EPA1     .
}  // initial()





/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/

/***********************************************/
/*   C main function       Program start here  */
/***********************************************/
/*===========================================================================*/
void main(void)
{
main_start:
    disable();
    p4_reg = 0xfc;     // let CS0->FLASH
     //-总线是多元的,都是16位宽的
    //prepare_status!!!!
    buscon0=0xc1;      // 0xc1
    buscon1=0xc1;      // f80000 fbffff	,,0和1上挂接的设备操作时间长一点,需要插入一个等待状态
    buscon2=0xc0;      // RAM         
    sp  = 0xff00;


    for(main_loop=0;main_loop<120;main_loop++)
    {
        *((far WORD *)(SOFT_ERR_FLAG+main_loop*2))=0;
    }


    initial();
    enable();  
    
    int_pend =0x80;	//-为什么需要对三个554进行一次人为中断处理呢??
    int_pend1=0x60;
    
    set_announce_rpt();
	//-以下的仅仅是一个循环,实际主要的功能还是在中断内处理的
    while(1)
    {
        if((main_loop % 4)==0) 
        	p1_reg = p1_reg ^ 0x80;  // for QH	?为了切换吗	,,直接改变引脚的输出状态,目的先保留,就是向外输出一个遥信量,可能就是便于检测系统正在运行

        RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER=0;

        main_loop++;

        //设置口
        set_main();	//-怎么总觉得这个口判断有点像下载程序的端口呢,不是的
		//-从这里也可以看的出是208的简化版
        Protocol_Main();
        judge_led_yc();
        judge_led_GJ();
        if(main_loop>180)
        {
            MAIN_DSA_208_VER_INFO_SET();	//-这些对于这个程序完全可以不理会的,至少在我这个层次
            main_loop = 0;
        
            p4_reg   =p4_reg ^ 0x01;	//-这个应该是看门狗周期的喂狗,,和直接控制运行灯的

            Protocol_Monitor();
            
            if(p2_img != (p2_pin & 0x08))	//-通过判断引脚状态改变波特率,目的何在,意义何在???,引脚2.3是一般目的的IO引脚配置为输入
            {	//-常态应该是为0的
                p2_img = p2_pin & 0x08;	//-从新取值
                if((p2_img & 0x08) == 0x00)
                    sp_baud = CPU_SIO_BAUD_TO_LED;	//-正常情况
                else
                    sp_baud = CPU_SIO_BAUD_TO_PC;	//-这个是下载程序时用得波特率,不对啊,下载程序的时候这个程序肯定不运行的啊
            }
            judge_yc_change();
            judge_host_zf_enable();
            byq_dw_yx_translate_to_yc();
            yx_or_calculate();
            yx_and_calculate();
            ylb_out_yx();	//-都是有的但是没有使用
            judge_bh_to_yx_delay_time();
            judge_yc_up_to_yx();
        }

        // use port_no
        Judge_P554_CAN_Reset();
        judge_mainloop_int_dead();
    }
}

