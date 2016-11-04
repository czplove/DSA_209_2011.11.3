/*****************************************************************************/
/*       FileName  :   COMUSE.C                                              */
/*       Content   :   DSA-208 COMMON_USE Module                             */
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

/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/






/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/

/************************************************/
/* Judge_LongTime_In_MainLoop function          */
/************************************************/
/*===========================================================================*/
BYTE Judge_LongTime_In_MainLoop(WORD start_time,WORD judge_value)	//-现在还没有看程序,但是我猜是这样的,倒计时程序
{
    UL time_value1;
    UL time_value2;

    time_value2=(UL)start_time+judge_value;
    
    if(Time_2048ms_Counter<start_time)
    {
        time_value1=Time_2048ms_Counter+0x10000;	//-小于的实际上顶多转了一圈
    }
    else
    {
        time_value1=Time_2048ms_Counter;	//-就是以他为时间轴的
    }
    
    if(time_value1>time_value2)
        return YES;
    else
        return NO;    
}


/************************************************/
/* Judge_LongTime_In_OtherInt function          */
/************************************************/
/*===========================================================================*/
BYTE Judge_LongTime_In_OtherInt(WORD start_time,WORD judge_value)
{
    UL time_value1;
    UL time_value2;

    time_value2=(UL)start_time+judge_value;
    
    if(Time_2048ms_Counter<start_time)
    {
        time_value1=Time_2048ms_Counter+0x10000;
    }
    else
    {
        time_value1=Time_2048ms_Counter;
    }
    
    if(time_value1>time_value2)
        return YES;
    else
        return NO;    
}


/************************************************/
/* Judge_Time_In_MainLoop function              */
/************************************************/
/*===========================================================================*/
BYTE Judge_Time_In_MainLoop(WORD start_time,WORD judge_value)
{
    UL time_value1;
    UL time_value2;

    time_value2=(UL)start_time+judge_value;
    
    if(Time_1ms_Counter<start_time)
    {
        time_value1=Time_1ms_Counter+0x10000;
    }
    else
    {
        time_value1=Time_1ms_Counter;
    }
    
    if(time_value1>time_value2)
        return YES;
    else
        return NO;    
}


/************************************************/
/* Judge_Time_In_OtherInt function              */
/************************************************/
/*===========================================================================*/
BYTE Judge_Time_In_OtherInt(WORD start_time,WORD judge_value)
{
    UL time_value1;
    UL time_value2;

    time_value2=(UL)start_time+judge_value;
    
    if(Time_1ms_Counter<start_time)
    {
        time_value1=Time_1ms_Counter+0x10000;
    }
    else
    {
        time_value1=Time_1ms_Counter;
    }
    
    if(time_value1>time_value2)
        return YES;
    else
        return NO;    
}



/***********************************************/
/* BCH_Calculate proc used in main_loop        */
/***********************************************/  
/*===========================================================================*/
void BCH_Calculate(void)
{
    BCH_Array[5]=0;

    BCH_Array[5]=BCH_Array[5] ^ BCH_Array[0];
    BCH_Array[5]=BCH_table[BCH_Array[5]];

    BCH_Array[5]=BCH_Array[5] ^ BCH_Array[1];
    BCH_Array[5]=BCH_table[BCH_Array[5]];

    BCH_Array[5]=BCH_Array[5] ^ BCH_Array[2];
    BCH_Array[5]=BCH_table[BCH_Array[5]];

    BCH_Array[5]=BCH_Array[5] ^ BCH_Array[3];
    BCH_Array[5]=BCH_table[BCH_Array[5]];

    BCH_Array[5]=BCH_Array[5] ^ BCH_Array[4];
    BCH_Array[5]=BCH_table[BCH_Array[5]];
    
    BCH_Array[5]=~(BCH_Array[5]);
}

/***********************************************/
/* clock proc used in soft_1ms_int             */
/***********************************************/  
/*===========================================================================*/
void Clock_Process(void)
{
/* for it is used in 1ms short ,it must disable interrupt in main_loop and other place */

    // other place needn't call the function,for REG_Surplus_Time changed only in 1ms_int.
    if(REG_1Msecond<1000) return;
    do
    {
        REG_1Msecond=REG_1Msecond-1000;
        REG_Second  =REG_Second+1;
    }
    while(REG_1Msecond>999);

    if(REG_Second<60) 
    {
        return;
    }   
    else
    {
        /* not consider clock_process not run 1 times in 60s */
        REG_Second=REG_Second-60;
        REG_Minute=REG_Minute+1;
    }

    if(REG_Minute<60) 
    {
        return;
    }   
    else
    {
        /* not consider clock_process not run 1 times in 60s */
        REG_Minute=REG_Minute-60;
        REG_Hour  =REG_Hour  +1;
    }

    if(REG_Hour<24) 
    {
        return;
    }   
    else
    {
        /* not consider clock_process not run 1 times in 60s */
        REG_Hour=REG_Hour-24;
        REG_Date=REG_Date+1;
    }


    if(REG_Date<29)
    {
        return;
    }   
    else
    {
        if(REG_Date==29)
        {
            if(REG_Month==2)
            {
                if((byte0(REG_Year) % 4)!=0)
                {
                    REG_Date=1;
                    REG_Month=3;
                }
            }
        }
        else
        {
            if(REG_Date==30)
            {
                if(REG_Month==2)
                {
                    REG_Date=1;
                    REG_Month=3;
                }
            }
            else
            {
                if(REG_Date==31)
                {
                    if((REG_Month==4)||(REG_Month==6)||(REG_Month==9)||(REG_Month==11))
                    {
                        REG_Date=1;
                        REG_Month=REG_Month+1;
                    }
                }
                else  // REG_Date==32
                {
                    REG_Date=1;
                    REG_Month=REG_Month+1;
                    if(REG_Month>12)
                    {
                        REG_Month=1;
                        REG_Year =REG_Year+1;
                    }                       
                }
            }
        }
    }
}


/***********************************************/
/*   Read_Time_From_Dallas                     */
/***********************************************/  
/*===========================================================================*/
void Read_Time_From_Dallas(void)
{
        BYTE *ram_base_addr;
        BYTE  ram_axl;

    ram_base_addr=(BYTE *)REAL_TIME_MEM_BASE;
    ram_base_addr[11]=0x86;

    ram_axl=ram_base_addr[9]; // DALLAS_YEAR
    REG_Year    =2000+ram_axl;

    ram_axl=ram_base_addr[8];
    REG_Month   =ram_axl;

    ram_axl=ram_base_addr[7];
    REG_Date    =ram_axl;

    ram_axl=ram_base_addr[4];
    REG_Hour    =ram_axl;

    ram_axl=ram_base_addr[2];
    REG_Minute  =ram_axl;

    ram_axl=ram_base_addr[0];
    REG_Second  =ram_axl;

    REG_1Msecond=500;
    
    ram_base_addr[11]=0x06;
}


/***********************************************/
/*   Write_Time_To_Dallas                      */
/***********************************************/  
/*===========================================================================*/
void Write_Time_To_Dallas(void)
{
        BYTE *ram_base_addr;
        WORD  ram_axl;

    ram_base_addr=(BYTE *)REAL_TIME_MEM_BASE;
    ram_base_addr[11]=0x86;

    ram_axl=REG_Year%100;
    ram_base_addr[9]=byte0(ram_axl);

    ram_base_addr[8]=REG_Month;

    ram_base_addr[7]=REG_Date;

    ram_base_addr[4]=REG_Hour;

    ram_base_addr[2]=REG_Minute;

    ram_base_addr[0]=REG_Second;
    ram_base_addr[11]=0x06;
}

/***********************************************/
/*   Store_Rcd_Info_Myself                     */
/***********************************************/  
/*===========================================================================*/
void Store_Rcd_Info_Myself(void)
{
    far BYTE *ram_base_addr;
    far BYTE *ram_base_addr1;

    ram_base_addr=(far BYTE *)DALLAS_RCD_MYSELF_SAV_PTR_ADDR;
    if( ((ram_base_addr[0]+1) & AND_RCD_INFO_PTR_LENGTH)==ram_base_addr[1] )
    {
        ram_base_addr[1]=(ram_base_addr[1]+1) & AND_RCD_INFO_PTR_LENGTH;
        ram_base_addr[5]=~ram_base_addr[1];
    }
    
    ram_base_addr1=(far BYTE *)DALLAS_RCD_INFO_MYSELF_ADDR + ram_base_addr[0]*16;
    ram_base_addr1[0x00]=byte1(REG_Year);
    ram_base_addr1[0x01]=byte0(REG_Year);
    ram_base_addr1[0x02]=REG_Month;
    ram_base_addr1[0x03]=REG_Date;
    ram_base_addr1[0x04]=REG_Hour;
    ram_base_addr1[0x05]=REG_Minute;
    ram_base_addr1[0x06]=REG_Second;
    ram_base_addr1[0x07]=byte1(REG_1Msecond);
    ram_base_addr1[0x08]=byte0(REG_1Msecond);
    ram_base_addr1[0x09]=Rcd_Info_Myself_Tmp[1];
    ram_base_addr1[0x0a]=Rcd_Info_Myself_Tmp[2];
    ram_base_addr1[0x0b]=Rcd_Info_Myself_Tmp[3];
    ram_base_addr1[0x0c]=Rcd_Info_Myself_Tmp[4];
    ram_base_addr1[0x0d]=Rcd_Info_Myself_Tmp[5];
    ram_base_addr1[0x0e]=Rcd_Info_Myself_Tmp[6];
    ram_base_addr1[0x0f]=Rcd_Info_Myself_Tmp[7];
    
    ram_base_addr[0]=(ram_base_addr[0]+1) & AND_RCD_INFO_PTR_LENGTH;
    ram_base_addr[4]=~ram_base_addr[0];
}


/***********************************************/
/*   Store_Rcd_Info_System                     */
/***********************************************/  
/*===========================================================================*/
void Store_Rcd_Info_System(void)
{
    far BYTE *ram_base_addr;
    far BYTE *ram_base_addr1;

    ram_base_addr=(far BYTE *)DALLAS_RCD_SYSTEM_SAV_PTR_ADDR;
    if( ((ram_base_addr[0]+1) & AND_RCD_INFO_PTR_LENGTH)==ram_base_addr[1] )
    {
        ram_base_addr[1]=(ram_base_addr[1]+1) & AND_RCD_INFO_PTR_LENGTH;
        ram_base_addr[5]=~ram_base_addr[1];
    }
    
    ram_base_addr1=(far BYTE *)DALLAS_RCD_INFO_SYSTEM_ADDR + ram_base_addr[0]*16;
    ram_base_addr1[0x00]=byte1(REG_Year);
    ram_base_addr1[0x01]=byte0(REG_Year);
    ram_base_addr1[0x02]=REG_Month;
    ram_base_addr1[0x03]=REG_Date;
    ram_base_addr1[0x04]=REG_Hour;
    ram_base_addr1[0x05]=REG_Minute;
    ram_base_addr1[0x06]=REG_Second;
    ram_base_addr1[0x07]=byte1(REG_1Msecond);
    ram_base_addr1[0x08]=byte0(REG_1Msecond);
    ram_base_addr1[0x09]=Rcd_Info_System_Tmp[1];
    ram_base_addr1[0x0a]=Rcd_Info_System_Tmp[2];
    ram_base_addr1[0x0b]=Rcd_Info_System_Tmp[3];
    ram_base_addr1[0x0c]=Rcd_Info_System_Tmp[4];
    ram_base_addr1[0x0d]=Rcd_Info_System_Tmp[5];
    ram_base_addr1[0x0e]=Rcd_Info_System_Tmp[6];
    ram_base_addr1[0x0f]=Rcd_Info_System_Tmp[7];
    
    ram_base_addr[0]=(ram_base_addr[0]+1) & AND_RCD_INFO_PTR_LENGTH;
    ram_base_addr[4]=~ram_base_addr[0];
}


