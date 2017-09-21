/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#define  _RTW_IOCTL_RTL_C_

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>
#include <wlan_bssdef.h>
#include <wifi.h>
#include <rtw_ioctl.h>
#include <rtw_ioctl_set.h>
#include <rtw_ioctl_query.h>
#include <rtw_ioctl_rtl.h>
#include <mp_custom_oid.h>
#ifdef CONFIG_MP_INCLUDED
#include <rtw_mp.h>
#include <rtw_mp_ioctl.h>
#endif

struct oid_obj_priv oid_rtl_seg_01_01[] =
{
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_rt_get_signal_quality_hdl},						
	{1, &oid_rt_get_small_packet_crc_hdl},					
	{1, &oid_rt_get_middle_packet_crc_hdl},					
	{1, &oid_rt_get_large_packet_crc_hdl},					
	{1, &oid_rt_get_tx_retry_hdl},							
	{1, &oid_rt_get_rx_retry_hdl},							
	{1, &oid_rt_pro_set_fw_dig_state_hdl},					
	{1, &oid_rt_pro_set_fw_ra_state_hdl}	,					
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_rt_get_rx_total_packet_hdl},						
	{1, &oid_rt_get_tx_beacon_ok_hdl},						
	{1, &oid_rt_get_tx_beacon_err_hdl},						
	{1, &oid_rt_get_rx_icv_err_hdl},							
	{1, &oid_rt_set_encryption_algorithm_hdl},				
	{1, &oid_null_function},									
	{1, &oid_rt_get_preamble_mode_hdl},						
	{1, &oid_null_function},									
	{1, &oid_rt_get_ap_ip_hdl},								
	{1, &oid_rt_get_channelplan_hdl},						
	{1, &oid_rt_set_preamble_mode_hdl},	 					
	{1, &oid_rt_set_bcn_intvl_hdl},							
	{1, &oid_null_function},									
	{1, &oid_rt_dedicate_probe_hdl},							
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_rt_get_total_tx_bytes_hdl},						
	{1, &oid_rt_get_total_rx_bytes_hdl},						
	{1, &oid_rt_current_tx_power_level_hdl},					
	{1, &oid_rt_get_enc_key_mismatch_count_hdl},	 		
	{1, &oid_rt_get_enc_key_match_count_hdl},				
	{1, &oid_rt_get_channel_hdl},							
	{1, &oid_rt_set_channelplan_hdl},							
	{1, &oid_rt_get_hardware_radio_off_hdl},					
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_rt_get_key_mismatch_hdl},						
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},	 								
	{1, &oid_rt_supported_wireless_mode_hdl},				
	{1, &oid_rt_get_channel_list_hdl},						
	{1, &oid_rt_get_scan_in_progress_hdl},					
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_rt_forced_data_rate_hdl},						
	{1, &oid_rt_wireless_mode_for_scan_list_hdl},				
	{1, &oid_rt_get_bss_wireless_mode_hdl},					
	{1, &oid_rt_scan_with_magic_packet_hdl},				
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},	 								
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	
};

struct oid_obj_priv oid_rtl_seg_01_03[] =
{
	{1, &oid_rt_ap_get_associated_station_list_hdl},			
	{1, &oid_null_function},									
	{1, &oid_rt_ap_switch_into_ap_mode_hdl},				
	{1, &oid_null_function},									
	{1, &oid_rt_ap_supported_hdl},							
	{1, &oid_rt_ap_set_passphrase_hdl},						

};

struct oid_obj_priv oid_rtl_seg_01_11[] =
{
	{1, &oid_null_function},				
	{1, &oid_null_function},				
	{1, &oid_null_function},				
	{1, &oid_null_function},				
	{1, &oid_null_function},				
	{1, &oid_null_function},				
	{1, &oid_null_function},				
	{1, &oid_null_function},				
	{1, &oid_rt_pro_rf_write_registry_hdl},
	{1, &oid_rt_pro_rf_read_registry_hdl},
	{1, &oid_null_function}				
	
};

struct oid_obj_priv oid_rtl_seg_03_00[] =
{
	{1, &oid_null_function},									
	{1, &oid_rt_get_connect_state_hdl},						
	{1, &oid_null_function},									
	{1, &oid_null_function},									
	{1, &oid_rt_set_default_key_id_hdl},						

	
};



NDIS_STATUS oid_rt_pro_set_fw_dig_state_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS		status = NDIS_STATUS_SUCCESS;
#if 0
	PADAPTER		Adapter = (PADAPTER)(poid_par_priv->adapter_context);
	_irqL			oldirql;
	
	_func_enter_;
	
	if(poid_par_priv->type_of_oid != SET_OID) 
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}
			
	_irqlevel_changed_(&oldirql,LOWER);
	if(poid_par_priv->information_buf_len >= sizeof(struct setdig_parm))
	{
		if(!rtw_setfwdig_cmd(Adapter,*((unsigned char*)poid_par_priv->information_buf )))			
		{
			status = NDIS_STATUS_NOT_ACCEPTED;
		}
		                   
	}
	else{
		status = NDIS_STATUS_NOT_ACCEPTED;
	}  
	_irqlevel_changed_(&oldirql,RAISE);
	_func_exit_;
#endif
	return status;
}
NDIS_STATUS oid_rt_pro_set_fw_ra_state_hdl(struct oid_par_priv* poid_par_priv)
{

	NDIS_STATUS		status = NDIS_STATUS_SUCCESS;
#if 0
	PADAPTER		Adapter = (PADAPTER)(poid_par_priv->adapter_context);
	_irqL			oldirql;
	
	_func_enter_;	
	if(poid_par_priv->type_of_oid != SET_OID) 
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}

					
	_irqlevel_changed_(&oldirql,LOWER);
	
	if(poid_par_priv->information_buf_len >= sizeof(struct setra_parm))
	{
		if(!rtw_setfwra_cmd(Adapter,*((unsigned char*)poid_par_priv->information_buf )))			
		{
			status = NDIS_STATUS_NOT_ACCEPTED;
		}
		                   
	}
	else{
		status = NDIS_STATUS_NOT_ACCEPTED;
	}  
	_irqlevel_changed_(&oldirql,RAISE);
	_func_exit_;
#endif
	return status;
}
NDIS_STATUS oid_rt_get_signal_quality_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

#if 0
		if(pMgntInfo->mAssoc || pMgntInfo->mIbss)
		{
			ulInfo = pAdapter->RxStats.SignalQuality;
			*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;
		}
		else
		{
			ulInfo = 0xffffffff;
		}
		break;
#endif

	return status;
}


NDIS_STATUS oid_rt_get_small_packet_crc_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		
		
	if(poid_par_priv->information_buf_len >=  sizeof(ULONG) )
	{		
		*(ULONG *)poid_par_priv->information_buf = padapter->recvpriv.rx_smallpacket_crcerr;
		*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;		
	}
	else
	{
		status = NDIS_STATUS_INVALID_LENGTH;
	}

	return status;
}
NDIS_STATUS oid_rt_get_middle_packet_crc_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	if(poid_par_priv->information_buf_len >=  sizeof(ULONG) )
	{		
		*(ULONG *)poid_par_priv->information_buf = padapter->recvpriv.rx_middlepacket_crcerr;
		*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;		
	}
	else
	{
		status = NDIS_STATUS_INVALID_LENGTH;
	}


	return status;
}
NDIS_STATUS oid_rt_get_large_packet_crc_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}	

	if(poid_par_priv->information_buf_len >=  sizeof(ULONG) )
	{		
		*(ULONG *)poid_par_priv->information_buf = padapter->recvpriv.rx_largepacket_crcerr;
		*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;		
	}
	else
	{
		status = NDIS_STATUS_INVALID_LENGTH;
	}


	return status;
}

NDIS_STATUS oid_rt_get_tx_retry_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}
NDIS_STATUS oid_rt_get_rx_retry_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		
	*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;
	return status;
}
NDIS_STATUS oid_rt_get_rx_total_packet_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS		status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}	
	if(poid_par_priv->information_buf_len >=  sizeof(ULONG) )
	{		
		*(u64 *)poid_par_priv->information_buf = padapter->recvpriv.rx_pkts + padapter->recvpriv.rx_drop;
		*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;		
	}
	else
	{
		status = NDIS_STATUS_INVALID_LENGTH;
	}


	return status;
}
NDIS_STATUS oid_rt_get_tx_beacon_ok_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}
NDIS_STATUS oid_rt_get_tx_beacon_err_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}
NDIS_STATUS oid_rt_get_rx_icv_err_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}
	if(poid_par_priv->information_buf_len>= sizeof(u32))
	{
		*(uint *)poid_par_priv->information_buf = padapter->recvpriv.rx_icv_err;
		*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;
	}
	else
	{
		status = NDIS_STATUS_INVALID_LENGTH ;
	}
	

	return status;
}
NDIS_STATUS oid_rt_set_encryption_algorithm_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != SET_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}	

	return status;
}
NDIS_STATUS oid_rt_get_preamble_mode_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);
	ULONG			preamblemode = 0 ;			
		
	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		
	if(poid_par_priv->information_buf_len>= sizeof(ULONG))
	{		
		if(padapter->registrypriv.preamble == PREAMBLE_LONG)
			preamblemode = 0;
		else if (padapter->registrypriv.preamble == PREAMBLE_AUTO)
			preamblemode = 1;
		else if (padapter->registrypriv.preamble == PREAMBLE_SHORT)
			preamblemode = 2;
		
			
		*(ULONG *)poid_par_priv->information_buf = preamblemode ;
		*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;
	}
	else
	{
		status = NDIS_STATUS_INVALID_LENGTH ;
	}
	return status;
}
NDIS_STATUS oid_rt_get_ap_ip_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}

NDIS_STATUS oid_rt_get_channelplan_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);
	struct eeprom_priv*	peeprompriv = &padapter->eeprompriv;	

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		
	*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;
	*(u16 *)poid_par_priv->information_buf = peeprompriv->channel_plan ;

	return status;
}
NDIS_STATUS oid_rt_set_channelplan_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);
	struct eeprom_priv*	peeprompriv = &padapter->eeprompriv;	
	
	if(poid_par_priv->type_of_oid != SET_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		
	
	peeprompriv->channel_plan = *(u16 *)poid_par_priv->information_buf ;

	return status;
}
NDIS_STATUS oid_rt_set_preamble_mode_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);
	ULONG			preamblemode = 0;
	if(poid_par_priv->type_of_oid != SET_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}
	 
	if(poid_par_priv->information_buf_len>= sizeof(ULONG))
	{		
		preamblemode = *(ULONG *)poid_par_priv->information_buf ;
		if( preamblemode == 0)
			padapter->registrypriv.preamble = PREAMBLE_LONG;
		else if (preamblemode==1 )
			padapter->registrypriv.preamble = PREAMBLE_AUTO;
		else if ( preamblemode==2 )
			padapter->registrypriv.preamble = PREAMBLE_SHORT;		
			
		*(ULONG *)poid_par_priv->information_buf = preamblemode ;
		*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;
	}
	else
	{
		status = NDIS_STATUS_INVALID_LENGTH ;
	}	

	return status;
}
NDIS_STATUS oid_rt_set_bcn_intvl_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != SET_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}

	return status;
}
NDIS_STATUS oid_rt_dedicate_probe_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	return status;
}
NDIS_STATUS oid_rt_get_total_tx_bytes_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}	
	if(poid_par_priv->information_buf_len>= sizeof(ULONG))
	{		
		*(u64 *)poid_par_priv->information_buf = padapter->xmitpriv.tx_bytes;
		*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;
	}
	else
	{
		status = NDIS_STATUS_INVALID_LENGTH ;
	}
	

	return status;
}
NDIS_STATUS oid_rt_get_total_rx_bytes_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS		status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}
	if(poid_par_priv->information_buf_len>= sizeof(ULONG))
	{
		*(u64 *)poid_par_priv->information_buf = padapter->recvpriv.rx_bytes;
		*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;
	}
	else
	{
		status = NDIS_STATUS_INVALID_LENGTH ;
	}
	return status;
}
NDIS_STATUS oid_rt_current_tx_power_level_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	return status;
}
NDIS_STATUS oid_rt_get_enc_key_mismatch_count_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}
NDIS_STATUS oid_rt_get_enc_key_match_count_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}
NDIS_STATUS oid_rt_get_channel_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);
	struct	mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	NDIS_802_11_CONFIGURATION		*pnic_Config;

	ULONG   channelnum;

	_func_enter_;
	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	if ( (check_fwstate(pmlmepriv, _FW_LINKED) == _TRUE) ||
		(check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE))
		pnic_Config = &pmlmepriv->cur_network.network.Configuration;
	else
		pnic_Config = &padapter->registrypriv.dev_network.Configuration;

	channelnum = pnic_Config->DSConfig;
	*(ULONG *)poid_par_priv->information_buf = channelnum;
	
	*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;

	_func_exit_;



	return status;
}
NDIS_STATUS oid_rt_get_hardware_radio_off_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}
NDIS_STATUS oid_rt_get_key_mismatch_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}
NDIS_STATUS oid_rt_supported_wireless_mode_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);
	ULONG 			ulInfo = 0 ;
	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		
	if(poid_par_priv->information_buf_len >= sizeof(ULONG)){
		ulInfo |= 0x0100;
		ulInfo |= 0x0200;
		ulInfo |= 0x0400;

		*(ULONG *) poid_par_priv->information_buf = ulInfo;		
		*poid_par_priv->bytes_rw = poid_par_priv->information_buf_len;
	}
	else{
		status = NDIS_STATUS_INVALID_LENGTH;
	}		

	return status;
}
NDIS_STATUS oid_rt_get_channel_list_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}
NDIS_STATUS oid_rt_get_scan_in_progress_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}


NDIS_STATUS oid_rt_forced_data_rate_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	return status;
}
NDIS_STATUS oid_rt_wireless_mode_for_scan_list_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	return status;
}
NDIS_STATUS oid_rt_get_bss_wireless_mode_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}

NDIS_STATUS oid_rt_scan_with_magic_packet_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	return status;
}

NDIS_STATUS oid_rt_ap_get_associated_station_list_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}
NDIS_STATUS oid_rt_ap_switch_into_ap_mode_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	return status;
}
NDIS_STATUS oid_rt_ap_supported_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	return status;
}
NDIS_STATUS oid_rt_ap_set_passphrase_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != SET_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	return status;
}


NDIS_STATUS oid_rt_pro_rf_write_registry_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS		status = NDIS_STATUS_SUCCESS;
	PADAPTER		Adapter = (PADAPTER)(poid_par_priv->adapter_context);
	_irqL			oldirql;
	_func_enter_;
	if(poid_par_priv->type_of_oid != SET_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}
	
	_irqlevel_changed_(&oldirql,LOWER);
	if(poid_par_priv->information_buf_len== (sizeof(unsigned long)*3))
	{      
		if(!rtw_setrfreg_cmd(Adapter, 
						*(unsigned char*)poid_par_priv->information_buf, 
						(unsigned long)(*((unsigned long*)poid_par_priv->information_buf+2))))
		{
			status = NDIS_STATUS_NOT_ACCEPTED;
		}
	                   
	}
	else{
		status = NDIS_STATUS_INVALID_LENGTH;
	}   
	_irqlevel_changed_(&oldirql,RAISE);
	_func_exit_;

	return status;
}

NDIS_STATUS oid_rt_pro_rf_read_registry_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS		status = NDIS_STATUS_SUCCESS;
#if 0
	PADAPTER		Adapter = (PADAPTER)(poid_par_priv->adapter_context);
	_irqL	oldirql;
	_func_enter_;

	if(poid_par_priv->type_of_oid != SET_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}	
	
	_irqlevel_changed_(&oldirql,LOWER);
	if(poid_par_priv->information_buf_len== (sizeof(unsigned long)*3))
	{
		if(Adapter->mppriv.act_in_progress == _TRUE)
		{
			status = NDIS_STATUS_NOT_ACCEPTED;
		}
		else
		{
			Adapter->mppriv.act_in_progress = _TRUE;
			Adapter->mppriv.workparam.bcompleted= _FALSE;
			Adapter->mppriv.workparam.act_type = MPT_READ_RF;
			Adapter->mppriv.workparam.io_offset = *(unsigned long*)poid_par_priv->information_buf;		
			Adapter->mppriv.workparam.io_value = 0xcccccccc;
				       
			if(!rtw_getrfreg_cmd(Adapter, 
							*(unsigned char*)poid_par_priv->information_buf, 
							(unsigned char*)&Adapter->mppriv.workparam.io_value))
			{
				status = NDIS_STATUS_NOT_ACCEPTED;
			}
		}
				      		   
		                   
	}
	else	{
		status = NDIS_STATUS_INVALID_LENGTH;
	}
	_irqlevel_changed_(&oldirql,RAISE);
	_func_exit_;
#endif
	return status;
}



enum _CONNECT_STATE_{
	CHECKINGSTATUS,
	ASSOCIATED,
	ADHOCMODE,
	NOTASSOCIATED
};

NDIS_STATUS oid_rt_get_connect_state_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS		status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	struct mlme_priv	*pmlmepriv = &(padapter->mlmepriv);

	ULONG ulInfo;
		
	if(poid_par_priv->type_of_oid != QUERY_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}		

	
	if(check_fwstate(pmlmepriv, _FW_UNDER_LINKING) == _TRUE)
		ulInfo = CHECKINGSTATUS;
	else if(check_fwstate(pmlmepriv, _FW_LINKED) == _TRUE)	
		ulInfo = ASSOCIATED;
	else if(check_fwstate(pmlmepriv, WIFI_ADHOC_STATE)== _TRUE)
		ulInfo = ADHOCMODE;
	else
		ulInfo = NOTASSOCIATED ;

	*(ULONG *)poid_par_priv->information_buf = ulInfo;
	*poid_par_priv->bytes_rw =  poid_par_priv->information_buf_len;

#if 0
	RT_TRACE(COMP_OID_QUERY, DBG_LOUD, ("===> Query OID_RT_GET_CONNECT_STATE.\n"));
	if(pMgntInfo->mAssoc)
		ulInfo = 1;
	else if(pMgntInfo->mIbss)
		ulInfo = 2;
	else if(pMgntInfo->bScanInProgress)
		ulInfo = 0;
	else
		ulInfo = 3;
	ulInfoLen = sizeof(ULONG);
	RT_TRACE(COMP_OID_QUERY, DBG_LOUD, ("<=== Query OID_RT_GET_CONNECT_STATE: %d\n", ulInfo));
#endif

	return status;
}

NDIS_STATUS oid_rt_set_default_key_id_hdl(struct oid_par_priv* poid_par_priv)
{
	NDIS_STATUS	 	status = NDIS_STATUS_SUCCESS;
	PADAPTER		padapter = (PADAPTER)(poid_par_priv->adapter_context);

	if(poid_par_priv->type_of_oid != SET_OID)
	{
		status = NDIS_STATUS_NOT_ACCEPTED;
		return status;
	}
	
	return status;
}

