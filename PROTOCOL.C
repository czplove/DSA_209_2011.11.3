/*****************************************************************************/
/*       FileName  :   PROTOCOL.C                                            */
/*       Content   :   DSA-208 PROTOCOL Module                               */
/*       Date      :   Wed  11-05-2002                                       */
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



//CDT port
extern void CDT_Main();
extern void CDT_Init();
extern void CDT_Monitor();

//CDT_9702 port
extern void CDT9702_Main();
extern void CDT9702_Init();
extern void CDT9702_Monitor();

//HOSTZF port
extern void HOSTZF_Main();
extern void HOSTZF_Init();
extern void HOSTZF_Monitor();


//BUS port
extern void Bus_Main();
extern void Bus_Init();
extern void Bus_Monitor();

//LSA port
extern void LSA_Main();
extern void LSA_Init();
extern void LSA_Monitor();

//LFP port
extern void LFP_Main();
extern void LFP_Init();
extern void LFP_Monitor();
//LFP20 port
extern void LFP20_Main();
extern void LFP20_Init();
extern void LFP20_Monitor();

//IEC101DF port
extern void IEC101DF_Main();
extern void IEC101DF_Init();
extern void IEC101DF_Monitor();

//ZLP_CDT port
extern void ZLPCDT_Main();
extern void ZLPCDT_Init();
extern void ZLPCDT_Monitor();
//ZLP_CDT1 port
extern void ZLPCDT1_Main();
extern void ZLPCDT1_Init();
extern void ZLPCDT1_Monitor();

//C9302 port
extern void C9302_Main();
extern void C9302_Init();
extern void C9302_Monitor();

//GPS_NARI port
extern void Gps_Nari_Main();
extern void Gps_Nari_Init();
extern void Gps_Nari_Monitor();

//ZLPPSM port
extern void ZLPPSM_Main();
extern void ZLPPSM_Init();
extern void ZLPPSM_Monitor();

//DDB_BB port
extern void DDB_BB_Main();
extern void DDB_BB_Init();
extern void DDB_BB_Monitor();

//DDB_WS port
extern void DDB_WS_Main();
extern void DDB_WS_Init();
extern void DDB_WS_Monitor();

//JZPLC port
extern void JZPLC_Main();
extern void JZPLC_Init();
extern void JZPLC_Monitor();

//JZXJY port
extern void JZXJY_Main();
extern void JZXJY_Init();
extern void JZXJY_Monitor();

//DFP201 port
extern void DFP201_Main();
extern void DFP201_Init();
extern void DFP201_Monitor();

//ZLPMK port
extern void ZLPMK_Main();
extern void ZLPMK_Init();
extern void ZLPMK_Monitor();

//DSA301 port
extern void DSA301_Main();
extern void DSA301_Init();
extern void DSA301_Monitor();

//ZLP_DH port
extern void ZLP_DH_Main();
extern void ZLP_DH_Init();
extern void ZLP_DH_Monitor();

//ZLP_SHCY port
extern void ZLP_SHCY_Main();
extern void ZLP_SHCY_Init();
extern void ZLP_SHCY_Monitor();

//ZLP_JRK port
extern void ZLP_JRK_Main();
extern void ZLP_JRK_Init();
extern void ZLP_JRK_Monitor();

//ZLP_DPMZ port
extern void DPMZ_Main();
extern void DPMZ_Init();
extern void DPMZ_Monitor();
//ZLPDPMZ port
extern void ZLPDPMZ_Main();
extern void ZLPDPMZ_Init();
extern void ZLPDPMZ_Monitor();

//ZLP_LNJC port
extern void ZLP_LNJC_Main();
extern void ZLP_LNJC_Init();
extern void ZLP_LNJC_Monitor();

//CDT_D709 port
extern void CDTD709_Main();
extern void CDTD709_Init();
extern void CDTD709_Monitor();

//ZLP_ZWK port
extern void ZLP_ZWK_Main();
extern void ZLP_ZWK_Init();
extern void ZLP_ZWK_Monitor();

//ZLP_WCK3 port
extern void WCK3_Main();
extern void WCK3_Init();
extern void WCK3_Monitor();

//ZLP_ZWK port
extern void WZJK1_Main();
extern void WZJK1_Init();
extern void WZJK1_Monitor();

//JZDQ port
extern void JZDQ_Main();
extern void JZDQ_Init();
extern void JZDQ_Monitor();
//JZDQ1 port
extern void JZDQ1_Main();
extern void JZDQ1_Init();
extern void JZDQ1_Monitor();

//SC1801 port
extern void SC1801_Main();
extern void SC1801_Init();
extern void SC1801_Monitor();

//MODBUS port
extern void MODBUS_Main();
extern void MODBUS_Init();
extern void MODBUS_Monitor();

//MOD_HT port
extern void MOD_HT_Main();
extern void MOD_HT_Init();
extern void MOD_HT_Monitor();

//XJDSDX port
extern void SDX_Main();
extern void SDX_Init();
extern void SDX_Monitor();

//VQCDWK port
extern void VQCDWK_Main();
extern void VQCDWK_Init();
extern void VQCDWK_Monitor();

//PJD10 port
extern void PJD10_Main();
extern void PJD10_Init();
extern void PJD10_Monitor();
//NJKD port
extern void NJKD_Main();
extern void NJKD_Init();
extern void NJKD_Monitor();
//JK2001 port
extern void JK2001_Main();
extern void JK2001_Init();
extern void JK2001_Monitor();
//WDTTC310 port
extern void WD_TTC310_Main();
extern void WD_TTC310_Init();
extern void WD_TTC310_Monitor();
//RCS9000 port
extern void RCS9000_Main();
extern void RCS9000_Init();
extern void RCS9000_Monitor();
//LSA18X port
extern void LSA18X_Main();
extern void LSA18X_Init();
extern void LSA18X_Monitor();
//MODBUS port
extern void MODBUS_Main();
extern void MODBUS_Init();
extern void MODBUS_Monitor();

//RECHOST port
extern void RECHOST_Main();
extern void RECHOST_Init();
extern void RECHOST_Monitor();
//BJKR101 port
extern void BJKR101_Main();
extern void BJKR101_Init();
extern void BJKR101_Monitor();
//MODBUSABB port
extern void MODBUSABB_Main();
extern void MODBUSABB_Init();
extern void MODBUSABB_Monitor();
//HY103 port
extern void HY103_Main();
extern void HY103_Init();
extern void HY103_Monitor();
//ZLP_PSME10 port
extern void ZLP_PSME10_Main();
extern void ZLP_PSME10_Init();
extern void ZLP_PSME10_Monitor();
//FSA2000 port
extern void FSA2000_Main();
extern void FSA2000_Init();
extern void FSA2000_Monitor();
//ZLP_SHCY port
extern void ZLP_SHCY2_Main();
extern void ZLP_SHCY2_Init();
extern void ZLP_SHCY2_Monitor();
//SDJX VQC port
extern void SDJX_Main();
extern void SDJX_Init();
extern void SDJX_Monitor();
//DISA1 port
extern void DISA1_Main();
extern void DISA1_Init();
extern void DISA1_Monitor();
//XHXQ port
extern void XHXQ_KDXH_Main();
extern void XHXQ_KDXH_Init();
extern void XHXQ_KDXH_Monitor();
//WD port
extern void WD_FJLD_Main();
extern void WD_FJLD_Init();
extern void WD_FJLD_Monitor();
//GPS port
extern void GPS12_Main();
extern void GPS12_Init();
extern void GPS12_Monitor();
//IEC101LN port
extern void IEC101LN_Main();
extern void IEC101LN_Init();
extern void IEC101LN_Monitor();
//ZLP_SC port
extern void ZLP_SC_Main();
extern void ZLP_SC_Init();
extern void ZLP_SC_Monitor();
//ZLPNK port
extern void ZLPNK_Main();
extern void ZLPNK_Init();
extern void ZLPNK_Monitor();
//VQC_MCVQ1 port
extern void VQC_MCVQ1_Main();
extern void VQC_MCVQ1_Init();
extern void VQC_MCVQ1_Monitor();
//DDB_HX port
extern void DDB_HX_Main();
extern void DDB_HX_Init();
extern void DDB_HX_Monitor();
//XHXQ XH_SHSY port
extern void XH_SHSY_Main();
extern void XH_SHSY_Init();
extern void XH_SHSY_Monitor();
//ZLP_ATX port
extern void ZLP_ATX_Main();
extern void ZLP_ATX_Init();
extern void ZLP_ATX_Monitor();
//JLP_ATX port
extern void JLP_ATX_Main();
extern void JLP_ATX_Init();
extern void JLP_ATX_Monitor();
//XJBH_FCK port
extern void XJBH_Main();
extern void XJBH_Init();
extern void XJBH_Monitor();
//VQC208 port
extern void VQC208_Main();
extern void VQC208_Init();
extern void VQC208_Monitor();
//DDB_WS port
extern void DDB_BB_WSKZ_Main();
extern void DDB_BB_WSKZ_Init();
extern void DDB_BB_WSKZ_Monitor();
//zlp_easy port
extern void ZLP_EASY_Main();
extern void ZLP_EASY_Init();
extern void ZLP_EASY_Monitor();
//printf port
extern void PRINTF_Main();
extern void PRINTF_Init();
extern void PRINTF_Monitor();
//xhxqzgtd port
extern void XHXQ_ZGTD_Main();
extern void XHXQ_ZGTD_Init();
extern void XHXQ_ZGTD_Monitor();
//zlp_zjhy port
extern void ZLP_ZJHY_Main();
extern void ZLP_ZJHY_Init();
extern void ZLP_ZJHY_Monitor();
//DDB_XMZ port
extern void DDB_XMZ_Main();
extern void DDB_XMZ_Init();
extern void DDB_XMZ_Monitor();
//NAZI94 port
extern void NAZI94_Main();
extern void NAZI94_Init();
extern void NAZI94_Monitor();
//NAZI94JK port
extern void NAZI94JK_Main();
extern void NAZI94JK_Init();
extern void NAZI94JK_Monitor();
//NAZI103 port
extern void NAZI103_Main();
extern void NAZI103_Init();
extern void NAZI103_Monitor();
//SC1801_DF port
extern void SC1801_DF_Main();
extern void SC1801_DF_Init();
extern void SC1801_DF_Monitor();
//WXJNR port
extern void WXJNR_Main();
extern void WXJNR_Init();
extern void WXJNR_Monitor();
//MOD_HFHK port
extern void MOD_HFHK_Main();
extern void MOD_HFHK_Init();
extern void MOD_HFHK_Monitor();
//ZLP_ZJSC port
extern void ZLP_ZJSC_Main();
extern void ZLP_ZJSC_Init();
extern void ZLP_ZJSC_Monitor();
//CM320 port
extern void CM320_Main();
extern void CM320_Init();
extern void CM320_Monitor();
//BJHX port
extern void BJHX_Main();
extern void BJHX_Init();
extern void BJHX_Monitor();
//MOD_ST2 port
extern void MOD_ST2_Main();
extern void MOD_ST2_Init();
extern void MOD_ST2_Monitor();
//ZLPTG port
extern void Zlptg_Main();
extern void Zlptg_Init();
extern void Zlptg_Monitor();
//LC_ZYDL port
extern void LC_ZYDL_Main();
extern void LC_ZYDL_Init();
extern void LC_ZYDL_Monitor();
//xh_hdxh port
extern void xh_hdxh_Main();
extern void xh_hdxh_Init();
extern void xh_hdxh_Monitor();
//xxhyr12 port
extern void xxhyr12_Main();
extern void xxhyr12_Init();
extern void xxhyr12_Monitor();
//YZTY_HMK port
extern void YZTY_HMK_Main();
extern void YZTY_HMK_Init();
extern void YZTY_HMK_Monitor();
//BJSF103 port
extern void BJSF103_Main();
extern void BJSF103_Init();
extern void BJSF103_Monitor();
//VQC100 port
extern void VQC100_Main();
extern void VQC100_Init();
extern void VQC100_Monitor();
//ZLP_MK80 port
extern void ZLP_MK80_Main();
extern void ZLP_MK80_Init();
extern void ZLP_MK80_Monitor();
//DDB_CD194E port
extern void DDB_CD194E_Main();
extern void DDB_CD194E_Init();
extern void DDB_CD194E_Monitor();
//ZLP_POD port
extern void ZLP_POD_Main();
extern void ZLP_POD_Init();
extern void ZLP_POD_Monitor();
//ZLP_STD port
extern void ZLP_STD_Main();
extern void ZLP_STD_Init();
extern void ZLP_STD_Monitor();
//IEC101NRDW port
extern void IEC101NRDW_Main();
extern void IEC101NRDW_Init();
extern void IEC101NRDW_Monitor();
//ZLP_BJSF port
extern void ZLP_BJSF_Main();
extern void ZLP_BJSF_Init();
extern void ZLP_BJSF_Monitor();
//ISA300_CDT port
extern void ISA300_CDT_Main();
extern void ISA300_CDT_Init();
extern void ISA300_CDT_Monitor();
//DDB_WS1 port
extern void DDB_WS1_Main();
extern void DDB_WS1_Init();
extern void DDB_WS1_Monitor();
//XMZ103 port
extern void XMZ103_Main();
extern void XMZ103_Init();
extern void XMZ103_Monitor();
//XMZ600 port
extern void XMZ103_7SJ600_Main();
extern void XMZ103_7SJ600_Init();
extern void XMZ103_7SJ600_Monitor();
//zlp_gzdw port
extern void GZDW_Main();
extern void GZDW_Init();
extern void GZDW_Monitor();

//mod_hh port
extern void MOD_HH_Main();
extern void MOD_HH_Init();
extern void MOD_HH_Monitor();

//lsa601
extern void LSA601_Main();
extern void LSA601_Init();
extern void LSA601_Monitor();

//lfprtu
extern void LFP_RTU_Main();
extern void LFP_RTU_Init();
extern void LFP_RTU_Monitor();

//zlp_et
extern void zlp_et_sz5_Main();
extern void zlp_et_sz5_Init();
extern void zlp_et_sz5_Monitor();

//zlp_gzdw2
extern void ZLP_GZDW2_Main();
extern void ZLP_GZDW2_Init();
extern void ZLP_GZDW2_Monitor();

//zlpxy port
extern void ZLPXY_Main();
extern void ZLPXY_Init();
extern void ZLPXY_Monitor();

//xuji103 port
extern void XUJI103_Main();     
extern void XUJI103_Init();     
extern void XUJI103_Monitor();  

//mod_kd port
extern void mod_kd_Main();   
extern void mod_kd_Init();   
extern void mod_kd_Monitor();

//wd_tcm100
extern void WD_TCM_Main();    
extern void WD_TCM_Init();   
extern void WD_TCM_Monitor();

//ZLPGZG20 port
extern void ZLP_GZG20_Main();
extern void ZLP_GZG20_Init();
extern void ZLP_GZG20_Monitor();

//DL_SHKF port
extern void DR_SHKF_Main();         
extern void DR_SHKF_Init();         
extern void DR_SHKF_Monitor();      

//zlp_tep port
extern void ZLP_TEP_Main();
extern void ZLP_TEP_Init();
extern void ZLP_TEP_Monitor();

//dr_dmp port
extern void DR_DMP_Main();   
extern void DR_DMP_Init();   
extern void DR_DMP_Monitor();

//mod_mk80 port
extern void MOD_MK80_Main();            
extern void MOD_MK80_Init();
extern void MOD_MK80_Monitor();

//wrtu port
extern void WRTU_Main();
extern void WRTU_Init();
extern void WRTU_Monitor();

//DN_D2000 port
extern void DN_D2000_Main();
extern void DN_D2000_Init();
extern void DN_D2000_Monitor();

//MONI_LJ
extern void MONI_LJ_Main();
extern void MONI_LJ_Init();
extern void MONI_LJ_Monitor();

//dbj port
extern void DBJ_Main(); 
extern void DBJ_Init(); 
extern void DBJ_Monitor();

//mod_cw2 port
extern void MOD_CW2_Main();
extern void MOD_CW2_Init();
extern void MOD_CW2_Monitor();

//zh_wld port
extern void ZH_WLD_Main();
extern void ZH_WLD_Init();
extern void ZH_WLD_Monitor();

//ddb_rh port
extern void DDB_RH_Main();
extern void DDB_RH_Init();
extern void DDB_RH_Monitor();

//zlp_rhd port
extern void ZLP_RHD_Main();
extern void ZLP_RHD_Init();
extern void ZLP_RHD_Monitor();

//mod_bus port
extern void MOD_DDB_Main();
extern void MOD_DDB_Init();
extern void MOD_DDB_Monitor();

//mod_bus port
extern void MOD_KM_Main();
extern void MOD_KM_Init();
extern void MOD_KM_Monitor();

//mod_bus port
extern void MOD_WKLF_Main();
extern void MOD_WKLF_Init();
extern void MOD_WKLF_Monitor();

//HNBH port
extern void HNBH_Main();
extern void HNBH_Init();
extern void HNBH_Monitor();

//mod_bus port
extern void ZLP_XA_Main();
extern void ZLP_XA_Init();
extern void ZLP_XA_Monitor();

//mod_bus port
extern void LC_PP41_Main();
extern void LC_PP41_Init();
extern void LC_PP41_Monitor();

//mod_bus port
extern void MQR_Main();
extern void MQR_Init();
extern void MQR_Monitor();


//LC_HWJT port
extern void LC_HWJT_Main();
extern void LC_HWJT_Init();
extern void LC_HWJT_Monitor();

//TSQ_TY port
extern void TSQ_TY_Main();
extern void TSQ_TY_Init();
extern void TSQ_TY_Monitor();

//YD2000 port
extern void YD2000_Main();
extern void YD2000_Init();
extern void YD2000_Monitor();

//PLC_GE port
extern void PLC_GE_Main();
extern void PLC_GE_Init();
extern void PLC_GE_Monitor();

//TSQ_CJ port
extern void TSQ_CJ_Main();
extern void TSQ_CJ_Init();
extern void TSQ_CJ_Monitor();

//TSQ_CJ port
extern void SAVR2000_Main();
extern void SAVR2000_Init();
extern void SAVR2000_Monitor();

//NTS103 port
extern void NTS103_Main();
extern void NTS103_Init();
extern void NTS103_Monitor();

//NTS103 port
extern void MODBUS_LSA2000_Main();
extern void MODBUS_LSA2000_Init();
extern void MODBUS_LSA2000_Monitor();
//MODBUS_IPC port
extern void MODBUS_IPC_Main();
extern void MODBUS_IPC_Init();
extern void MODBUS_IPC_Monitor();
//MOD_JYDK port
extern void MOD_JYDK_Main();
extern void MOD_JYDK_Init();
extern void MOD_JYDK_Monitor();
//MOD_3S port
extern void MOD_3S_Main();
extern void MOD_3S_Init();
extern void MOD_3S_Monitor();
//MOD_KG port
extern void MOD_KG_Main();
extern void MOD_KG_Init();
extern void MOD_KG_Monitor();
//MOD_YD port
extern void MOD_YDYB_Main();
extern void MOD_YDYB_Init();
extern void MOD_YDYB_Monitor();

// all protocol define:
#define Protocol0                         PROTOCOL_NONE  
#define Protocol0_Main()                  None_Main()   
#define Protocol0_Init()                  None_Init()   
#define Protocol0_Monitor()               None_Monitor() 

#define Protocol1                         PROTOCOL_MODBUS
#define Protocol1_Main()                  MODBUS_Main()  
#define Protocol1_Init()                  MODBUS_Init()  
#define Protocol1_Monitor()               MODBUS_Monitor()

#define Protocol2                         PROTOCOL_MOD_JYDK        
#define Protocol2_Main()                  MOD_JYDK_Main()       
#define Protocol2_Init()                  MOD_JYDK_Init()       
#define Protocol2_Monitor()               MOD_JYDK_Monitor()    

#define Protocol3                         PROTOCOL_CDT_HOSTZF
#define Protocol3_Main()                  HOSTZF_Main()
#define Protocol3_Init()                  HOSTZF_Init()
#define Protocol3_Monitor()               HOSTZF_Monitor()

#define Protocol4                         PROTOCOL_INTERNAL_BUS
#define Protocol4_Main()                  Bus_Main()
#define Protocol4_Init()                  Bus_Init()
#define Protocol4_Monitor()               Bus_Monitor()

#define Protocol5                         PROTOCOL_CDT_XT9702
#define Protocol5_Main()                  CDT9702_Main()    
#define Protocol5_Init()                  CDT9702_Init()    
#define Protocol5_Monitor()               CDT9702_Monitor() 

#define Protocol6                         PROTOCOL_CDT_BB        
#define Protocol6_Main()                  CDT_Main()       
#define Protocol6_Init()                  CDT_Init()       
#define Protocol6_Monitor()               CDT_Monitor()     

#define Protocol7                         PROTOCOL_MODBUS_3S       
#define Protocol7_Main()                  MOD_3S_Main()        
#define Protocol7_Init()                  MOD_3S_Init()          
#define Protocol7_Monitor()               MOD_3S_Monitor()       

#define Protocol8                         PROTOCOL_MODBUS_KG         
#define Protocol8_Main()                  MOD_KG_Main()           
#define Protocol8_Init()                  MOD_KG_Init()           
#define Protocol8_Monitor()               MOD_KG_Monitor()        

#define Protocol9                         PROTOCOL_MOD_YDYB        
#define Protocol9_Main()                  MOD_YDYB_Main()   
#define Protocol9_Init()                  MOD_YDYB_Init()   
#define Protocol9_Monitor()               MOD_YDYB_Monitor()

#define Protocol10                        PROTOCOL_NONE
#define Protocol10_Main()                 None_Main()
#define Protocol10_Init()                 None_Init()
#define Protocol10_Monitor()              None_Monitor()

#define Protocol11                        PROTOCOL_NONE
#define Protocol11_Main()                 None_Main()
#define Protocol11_Init()                 None_Init()
#define Protocol11_Monitor()              None_Monitor()

#define Protocol12                        PROTOCOL_NONE
#define Protocol12_Main()                 None_Main()
#define Protocol12_Init()                 None_Init()
#define Protocol12_Monitor()              None_Monitor()




void None_Init()
{
    // do nothing
}

void None_Main()
{
    // do nothing
}

void None_Monitor()
{
    // do nothing
}

void Protocol_Init()
{
    for(port_no=0;port_no<12;port_no++)
    {
        if(port_info[port_no].protocal_type==Protocol0)
            Protocol0_Init();
        if(port_info[port_no].protocal_type==Protocol1)
            Protocol1_Init();
        if(port_info[port_no].protocal_type==Protocol2)
            Protocol2_Init();
        if(port_info[port_no].protocal_type==Protocol3)
            Protocol3_Init();
        if(port_info[port_no].protocal_type==Protocol4)
            Protocol4_Init();
        if(port_info[port_no].protocal_type==Protocol5)
            Protocol5_Init();
        if(port_info[port_no].protocal_type==Protocol6)
            Protocol6_Init();
        if(port_info[port_no].protocal_type==Protocol7)
            Protocol7_Init();
        if(port_info[port_no].protocal_type==Protocol8)
            Protocol8_Init();
        if(port_info[port_no].protocal_type==Protocol9)
            Protocol9_Init();
        if(port_info[port_no].protocal_type==Protocol10)
            Protocol10_Init();
        if(port_info[port_no].protocal_type==Protocol11)
            Protocol11_Init();
        if(port_info[port_no].protocal_type==Protocol12)
            Protocol12_Init();
    }    
}

void Protocol_Main()
{
    for(port_no=0;port_no<12;port_no++)
    {
        if(port_info[port_no].protocal_type==Protocol0)	//-寻找的主体是端口规约与可能的12种规约进行比较
            Protocol0_Main();
        if(port_info[port_no].protocal_type==Protocol1)	//-端口没有配置这样的规约的话,是不会进入规约处理子程序的
            Protocol1_Main();
        if(port_info[port_no].protocal_type==Protocol2) //-同样,端口配置的规约,处理子程序不存在的话也是无效的
            Protocol2_Main();
        if(port_info[port_no].protocal_type==Protocol3)
            Protocol3_Main();
        if(port_info[port_no].protocal_type==Protocol4)
            Protocol4_Main();
        if(port_info[port_no].protocal_type==Protocol5)
            Protocol5_Main();
        if(port_info[port_no].protocal_type==Protocol6)
            Protocol6_Main();
        if(port_info[port_no].protocal_type==Protocol7)
            Protocol7_Main();
        if(port_info[port_no].protocal_type==Protocol8)
            Protocol8_Main();
        if(port_info[port_no].protocal_type==Protocol9)
            Protocol9_Main();
        if(port_info[port_no].protocal_type==Protocol10)
            Protocol10_Main();
        if(port_info[port_no].protocal_type==Protocol11)
            Protocol11_Main();
        if(port_info[port_no].protocal_type==Protocol12)
            Protocol12_Main();
    }        
}

//-对规约的监测就是看是否正常,正常情况下是不起实际作用的
void Protocol_Monitor()
{
    for(port_no=0;port_no<12;port_no++)
    {
        if(port_info[port_no].protocal_type==Protocol0)
            Protocol0_Monitor();	//-查看有没有需要处理的规约或端口
        if(port_info[port_no].protocal_type==Protocol1)
            Protocol1_Monitor();
        if(port_info[port_no].protocal_type==Protocol2)
            Protocol2_Monitor();
        if(port_info[port_no].protocal_type==Protocol3)
            Protocol3_Monitor();
        if(port_info[port_no].protocal_type==Protocol4)
            Protocol4_Monitor();
        if(port_info[port_no].protocal_type==Protocol5)
            Protocol5_Monitor();
        if(port_info[port_no].protocal_type==Protocol6)
            Protocol6_Monitor();
        if(port_info[port_no].protocal_type==Protocol7)
            Protocol7_Monitor();
        if(port_info[port_no].protocal_type==Protocol8)
            Protocol8_Monitor();
        if(port_info[port_no].protocal_type==Protocol9)
            Protocol9_Monitor();
        if(port_info[port_no].protocal_type==Protocol10)
            Protocol10_Monitor();
        if(port_info[port_no].protocal_type==Protocol11)
            Protocol11_Monitor();
        if(port_info[port_no].protocal_type==Protocol12)
            Protocol12_Monitor();
    }    
}
