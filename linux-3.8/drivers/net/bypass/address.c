#include "addressing.h"
#include "bypass.h"

static int bypass_num=0;
static int slot_num=0;

int set_eeprom(struct bp_addressing_adapter *adapter,int slot_num, int modelname,int bypass_num){
	int ret=0;
	uint16_t eeprom_data;
	switch(slot_num){
		case 1:
			switch(bypass_num){
				case 0:
					bp_addressing_read_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W20_434C)){
						eeprom_data=A484_EEP_W20_434C;
					}else if((modelname==ABN482)&&(eeprom_data!=A482_EEP_W20_C180)){
						eeprom_data=A482_EEP_W20_C180;
					}
					bp_addressing_write_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
				break;
				case 1:
					bp_addressing_read_eeprom(&adapter->hw, W10, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W10_430C)){
						eeprom_data=A484_EEP_W10_430C;
					}else if((modelname==ABN482)&&(eeprom_data!=A482_EEP_W10_C100)){
						eeprom_data=A482_EEP_W10_C100;
					}
					bp_addressing_write_eeprom(&adapter->hw, W10, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
				break;
				case 2:
				case 3:
					bp_addressing_read_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W20_030C)){
						eeprom_data=A484_EEP_W20_030C;
					}
					bp_addressing_write_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
				break;
			}
		break;
		case 2:
			switch(bypass_num){
				case 0:
					bp_addressing_read_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W20_434D)){
						eeprom_data=A484_EEP_W20_434D;
						bp_addressing_write_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					}else if((modelname==ABN482)&&(eeprom_data!=A482_EEP_W20_C181)){
						eeprom_data=A482_EEP_W20_C181;
					}
					bp_addressing_write_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
                                break;
                                case 1:
					bp_addressing_read_eeprom(&adapter->hw, W10, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W10_430D)){
						eeprom_data=A484_EEP_W10_430D;
					}else if((modelname==ABN482)&&(eeprom_data!=A482_EEP_W10_C101)){
						eeprom_data=A482_EEP_W10_C101;
					}
					bp_addressing_write_eeprom(&adapter->hw, W10, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
                                break;
                                case 2:
                                  case 3:                                                                  
					bp_addressing_read_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W20_030D)){
						eeprom_data=A484_EEP_W20_030D;
					}
					bp_addressing_write_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
                                break;
                        }
		break;
		case 0:
			switch(bypass_num){
				case 0:
					bp_addressing_read_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W20_434E)){
						eeprom_data=A484_EEP_W20_434E;
					}else if((modelname==ABN482)&&(eeprom_data!=A482_EEP_W20_C1C0)){
						eeprom_data=A482_EEP_W20_C1C0;
					}
					bp_addressing_write_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
                                break;
                                case 1:
					bp_addressing_read_eeprom(&adapter->hw, W10, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W10_430E)){
						eeprom_data=A484_EEP_W10_430E;
					}else if((modelname==ABN482)&&(eeprom_data!=A482_EEP_W10_C140)){
						eeprom_data=A482_EEP_W10_C140;
					}
					bp_addressing_write_eeprom(&adapter->hw, W10, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
                                break;
                                case 2:
                                case 3:
					bp_addressing_read_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W20_030E)){
						eeprom_data=A484_EEP_W20_030E;
					}
					bp_addressing_write_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
                                break;
                        }
		break;
		case 3:
			switch(bypass_num){
                                case 0:
					bp_addressing_read_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W20_434F)){
						eeprom_data=A484_EEP_W20_434F;
					}else if((modelname==ABN482)&&(eeprom_data!=A482_EEP_W20_C1C1)){
						eeprom_data=A482_EEP_W20_C1C1;
					}
					bp_addressing_write_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
                                break;
                                case 1:
					bp_addressing_read_eeprom(&adapter->hw, W10, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W10_430F)){
						eeprom_data=A484_EEP_W10_430F;
					}else if((modelname==ABN482)&&(eeprom_data!=A482_EEP_W10_C141)){
						eeprom_data=A482_EEP_W10_C141;
					}
					bp_addressing_write_eeprom(&adapter->hw, W10, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
                                break;
                                case 2:
					bp_addressing_read_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					if((modelname==ABN484)&&(eeprom_data!=A484_EEP_W20_030F)){
						eeprom_data=A484_EEP_W20_030F;
					}
					bp_addressing_write_eeprom(&adapter->hw, W20, 1, &eeprom_data);
					bp_addressing_update_eeprom_checksum(&adapter->hw);
                                break;
                        }
		break;
	}
	return ret;
}

int adressing(struct bp_addressing_adapter *adapter,int model){
	uint16_t eeprom_data;
	uint32_t ctrl_ext1;
	uint32_t ctrl1;

	bp_addressing_read_eeprom(&adapter->hw, 0x08, 1, &eeprom_data);
	if(eeprom_data!=0xffff)
	{
		if((((model>>((2-slot_num)*4))&0xf)==0)&&(bypass_num==0))
		{
			slot_num++;
		}
//	printk("card_conf=%x slot_num=%d eeprom_data=%x bypass_num=%d\n",((model>>((2-slot_num)*4))&0xf),slot_num,eeprom_data,bypass_num);
	}

	if(eeprom_data==ABN484)
	{
	        //  SMBUS address             PCA9555           PCA9559
	        //  -----------------+------------------------+----------------
	        //                      SMBUS addr__(A1,A0)
	        //  First  ABN-484	0x24__(00)  0x20__(00)	0x4C__(00)
	        //  Second ABN-484	0x25__(01)  0x21__(01)	0x4D__(01)
	        //  Third  ABN-484	0x26__(10)  0x22__(10)	0x4E__(10)
	        //  Four   ABN-484	0x27__(11)  0x23__(11)	0x4F__(11)
		ctrl1 = BP_ADDRESSING_READ_REG(&adapter->hw, CTRL);
		switch(slot_num){
			case 1:
				if(bypass_num<=2)
				{
					ctrl1=(ctrl1 & 0xFFF3DFFF )              ;      //00
					BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL, ctrl1);
				}
					set_eeprom(adapter,slot_num,eeprom_data,bypass_num);
			break;
			case 2:
				if(bypass_num<=2)
				{
					ctrl1=(ctrl1 & 0xFFF3DFFF ) | 0x00040000 ;      //01
					BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL, ctrl1);
				}
					set_eeprom(adapter,slot_num,eeprom_data,bypass_num);
			break;
			case 0:
				if(bypass_num<=2)
				{
					ctrl1=(ctrl1 & 0xFFF3DFFF ) | 0x00080000 ;      //10
					BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL, ctrl1);
				}
					set_eeprom(adapter,slot_num,eeprom_data,bypass_num);
			break;
			case 3:
				if(bypass_num<=2)
				{
					ctrl1=(ctrl1 & 0xFFF3DFFF ) | 0x000C0000 ;      //11
					BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL, ctrl1);
				}
					set_eeprom(adapter,slot_num,eeprom_data,bypass_num);
			break;
		}
		if((bypass_num%4)==3)
		{
			bypass_num=0;
			slot_num++;
		}
		else
			bypass_num++;
	}
	else if (eeprom_data==ABN482)
	{
		ctrl1 = BP_ADDRESSING_READ_REG(&adapter->hw, CTRL);
		ctrl_ext1 = BP_ADDRESSING_READ_REG(&adapter->hw, CTRL_EXT);
		switch(slot_num){
				case 1:
						ctrl1=(ctrl1 & 0xFFFBFFFF )             ;               	//Set SDP0_0='0'
						BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL, ctrl1);
						if(bypass_num==0)
						{
								ctrl_ext1= (ctrl_ext1 & 0xFFFFFF3F) | 0x00000080;	//Set SDP0_3='1',SDP0_2='0'
								BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL_EXT, ctrl_ext1);
						}else if(bypass_num==1)
						{
								ctrl_ext1= (ctrl_ext1 & 0xFFFFFF3F)	;		//Set SDP0_3='0',SDP0_2='0'
								BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL_EXT, ctrl_ext1);
						}
						set_eeprom(adapter,slot_num,eeprom_data,bypass_num);
				break;
				case 2:
						ctrl1=(ctrl1 & 0xFFFBFFFF ) | 0x00040000;               	//Set SDP0_0='1'
						BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL, ctrl1);
						if(bypass_num==0)
						{
								ctrl_ext1= (ctrl_ext1 & 0xFFFFFF3F) | 0x00000080;       //Set SDP0_3='1',SDP0_2='0'
								BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL_EXT, ctrl_ext1);
						}else if(bypass_num==1)
						{
								ctrl_ext1= (ctrl_ext1 & 0xFFFFFF3F)     ;               //Set SDP0_3='0',SDP0_2='0'
								BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL_EXT, ctrl_ext1);
						}
						set_eeprom(adapter,slot_num,eeprom_data,bypass_num);
				break;
				case 0:
						ctrl1=(ctrl1 & 0xFFFBFFFF )           ;               		//Set SDP0_0='0'
						BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL, ctrl1);
						if(bypass_num==0)
						{
								ctrl_ext1= (ctrl_ext1 & 0xFFFFFF3F) | 0x000000C0;       //Set SDP0_3='1',SDP0_2='1'
								BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL_EXT, ctrl_ext1);
						}else if(bypass_num==1)
						{
								ctrl_ext1= (ctrl_ext1 & 0xFFFFFF3F) | 0x00000040;	//Set SDP0_3='0',SDP0_2='1'
								BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL_EXT, ctrl_ext1);
						}
						set_eeprom(adapter,slot_num,eeprom_data,bypass_num);
				break;
				case 3:
						ctrl1=(ctrl1 & 0xFFFBFFFF ) | 0x00040000;              		 //Set SDP0_0='1'
						BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL, ctrl1);
						if(bypass_num==0)
						{
								ctrl_ext1= (ctrl_ext1 & 0xFFFFFF3F) | 0x000000C0;       //Set SDP0_3='1',SDP0_2='1'
								BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL_EXT, ctrl_ext1);
						}else if(bypass_num==1)
						{
								ctrl_ext1= (ctrl_ext1 & 0xFFFFFF3F)| 0x00000040;	//Set SDP0_3='0',SDP0_2='1'
								BP_ADDRESSING_WRITE_REG(&adapter->hw, CTRL_EXT, ctrl_ext1);
						}
						set_eeprom(adapter,slot_num,eeprom_data,bypass_num);
				break;
		}
	if((bypass_num%2)==1)
	{
		bypass_num=0;
		slot_num++;
	}
	else
		bypass_num++;
	} 

return 0;

}

