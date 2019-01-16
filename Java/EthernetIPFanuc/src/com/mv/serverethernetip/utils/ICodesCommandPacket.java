package com.mv.serverethernetip.utils;

public interface ICodesCommandPacket {

	/************************************************
	 * 
	 * GENERIC
	 * 
	 ************************************************/
	
	public final short NO_ERROR 			= 0x00;
	public final short DEVICE_AVAILABLE 	= 0x01;
	public final short NO_EXIST_DEVICE 		= 0x02;
	public final short ERROR_CONN_DEVICE	= 0x03;
	public final short DEVICE_NOT_AVAILABLE = 0x04;
	public final short ACKNOWLEDGE 			= 0x05;
	public final short ERROR_CLIENT 		= 0x06;
	
	/************************************************
	 * 
	 * ROBOT FANUC
	 * 
	 ************************************************/

	public final short WRITE_REG 				= 0x10;
	public final short WRITE_REG_OK 			= 0x11;
	public final short WRITE_REG_ERROR 			= 0x12;
	
	public final short WRITE_REG_BLOCK 			= 0x13;
	public final short WRITE_REG_BLOCK_OK 		= 0x14;
	public final short WRITE_REG_BLOCK_ERROR 	= 0x15;
	
	public final short WRITE_POS 				= 0x16;
	public final short WRITE_POS_ERROR 			= 0x17;
	public final short WRITE_POS_OK 			= 0x18;
	
	public final short WRITE_JPOS 				= 0x19;
	public final short WRITE_JPOS_ERROR 		= 0x1A;
	public final short WRITE_JPOS_OK 			= 0x1B;
	
	public final short READ_POS 				= 0x1C;
	public final short READ_POS_OK 				= 0x1D;
	public final short READ_POS_ERROR 			= 0x1E;
	
	public final short READ_JPOS 				= 0x1F;
	public final short READ_JPOS_OK 			= 0x20;
	public final short READ_JPOS_ERROR 			= 0x21;
	
	public final short READ_CURR_POS 			= 0x22;
	public final short READ_CURR_POS_OK 		= 0x23;
	public final short READ_CURR_POS_ERROR 		= 0x24;
	
	public final short READ_CURR_JPOS 			= 0x25;
	public final short READ_CURR_JPOS_ERROR 	= 0x26;
	public final short READ_CURR_JPOS_OK 		= 0x27;
	
	public final short READ_REG 				= 0x40;
	public final short READ_REG_OK 				= 0x41;
	public final short READ_REG_ERROR 			= 0x42;
	
	public final short READ_REG_BLOCK			= 0x43;
	public final short READ_REG_BLOCK_OK 		= 0x44;
	public final short READ_REG_BLOCK_ERROR 	= 0x45;

	
	/************************************************
	 * 
	 * SERVO SMC
	 * 
	 ************************************************/
	
	public final short READ_MEM_IN 				= 0x96;
	public final short READ_MEM_IN_OK 			= 0x97;
	public final short READ_MEM_IN_ERROR 		= 0x98;

	public final short READ_MOV 				= 0x99;
	public final short READ_MOV_OK 				= 0x9A;
	public final short READ_MOV_ERROR 			= 0x9B;	

	public final short WRITE_MEM_OUT 			= 0x90;
	public final short WRITE_MEM_OUT_ERROR 		= 0x92;
	public final short WRITE_MEM_OUT_OK 		= 0x91;
	
	public final short WRITE_MOV 				= 0x93;
	public final short WRITE_MOV_OK 			= 0x94;
	public final short WRITE_MOV_ERROR 			= 0x95;
	
	public final short SERVO_SMC_RESET 			= 0x9C;
	public final short SERVO_SMC_RESET_OK 		= 0x9D;
	public final short SERVO_SMC_RESET_ERROR 	= 0x9E;
	
	public final short SERVO_SMC_MOVE 			= 0x9F;
	public final short SERVO_SMC_MOVE_OK 		= 0xA0;
	public final short SERVO_SMC_MOVE_ERROR 	= 0xA1;
	
	public final short SERVO_SMC_STATE 			= 0xA2;
	public final short SERVO_SMC_STATE_OK 		= 0xA3;
	public final short SERVO_SMC_STATE_ERROR 	= 0xA4;
	
	public final short SERVO_SMC_INIT 			= 0xA5;
	public final short SERVO_SMC_INIT_OK 		= 0xA6;	
	public final short SERVO_SMC_INIT_ERROR 	= 0xA7;
	
	public final short SERVO_SMC_TIMEOUT_MOVE 	= 0xA8;

	public static String getCommand(short cmd) {

		String strCom = "";

		switch (cmd) {

		case 0x00:
			strCom = "NO_ERROR";
			break;
		case 0x01:
			strCom = "DEVICE_AVAILABLE";
			break;
		case 0x02:
			strCom = "NO_EXIST_DEVICE";
			break;
		case 0x03:
			strCom = "ERROR_CONN_DEVICE";
			break;
		case 0x04:
			strCom = "DEVICE_NOT_AVAILABLE";
			break;

		// REQUEST
		case 0x05:
			strCom = "ACKNOWLEDGE";
			break;

		case 0x06:
			strCom = "ERROR_CLIENT";
			break;

		/************************************************
		 * 
		 * ROBOT FANUC
		 * 
		 ************************************************/

		case 0x10:
			strCom = "WRITE_REG";
			break;
		case 0x11:
			strCom = "WRITE_REG_OK";
			break;
		case 0x12:
			strCom = "WRITE_REG_ERROR";
			break;

		case 0x13:
			strCom = "WRITE_REG_BLOCK";
			break;
		case 0x14:
			strCom = "WRITE_REG_BLOCK_OK";
			break;
		case 0x15:
			strCom = "WRITE_REG_BLOCK_ERROR";
			break;

		case 0x16:
			strCom = "WRITE_POS";
			break;
		case 0x17:
			strCom = "WRITE_POS_OK";
			break;
		case 0x18:
			strCom = "WRITE_POS_ERROR";
			break;

		case 0x19:
			strCom = "WRITE_JPOS";
			break;
		case 0x1A:
			strCom = "WRITE_JPOS_OK";
			break;
		case 0x1B:
			strCom = "WRITE_JPOS_ERROR";
			break;

		case 0x40:
			strCom = "READ_REG";
			break;
		case 0x41:
			strCom = "READ_REG_OK";
			break;
		case 0x42:
			strCom = "READ_REG_ERROR";
			break;

		case 0x43:
			strCom = "READ_REG_BLOCK";
			break;
		case 0x44:
			strCom = "READ_REG_BLOCK_OK";
			break;
		case 0x45:
			strCom = "READ_REG_BLOCK_ERROR";
			break;

		case 0x1C:
			strCom = "READ_POS";
			break;
		case 0x1D:
			strCom = "READ_POS_OK";
			break;
		case 0x1E:
			strCom = "READ_POS_ERROR";
			break;

		case 0x1F:
			strCom = "READ_JPOS";
			break;
		case 0x20:
			strCom = "READ_JPOS_OK";
			break;
		case 0x21:
			strCom = "READ_JPOS_ERROR";
			break;

		case 0x22:
			strCom = "READ_CURR_POS";
			break;
		case 0x23:
			strCom = "READ_CURR_POS_OK";
			break;
		case 0x24:
			strCom = "READ_CURR_POS_ERROR";
			break;

		case 0x25:
			strCom = "READ_CURR_JPOS";
			break;
		case 0x26:
			strCom = "READ_CURR_JPOS_OK";
			break;
		case 0x27:
			strCom = "READ_CURR_JPOS_ERROR";
			break;

		/************************************************
		 * 
		 * SERVO SMC
		 * 
		 ************************************************/

		case 0x90:
			strCom = "WRITE_MEM_OUT";
			break;
		case 0x91:
			strCom = "WRITE_MEM_OUT_OK";
			break;
		case 0x92:
			strCom = "WRITE_MEM_OUT_ERROR";
			break;

		case 0x93:
			strCom = "WRITE_MOV";
			break;
		case 0x94:
			strCom = "WRITE_MOV_OK";
			break;
		case 0x95:
			strCom = "WRITE_MOV_ERROR";
			break;

		case 0x96:
			strCom = "READ_MEM_IN";
			break;
		case 0x97:
			strCom = "READ_MEM_IN_OK";
			break;
		case 0x98:
			strCom = "READ_MEM_IN_ERROR";
			break;

		case 0x99:
			strCom = "READ_MOV";
			break;
		case 0x9A:
			strCom = "READ_MOV_OK";
			break;
		case 0x9B:
			strCom = "READ_MOV_ERROR";
			break;

		//
		// COMPLEX MESSAGES
		//

		case 0x9C:
			strCom = "SERVO_SMC_RESET";
			break;
		case 0x9D:
			strCom = "SERVO_SMC_RESET_OK";
			break;
		case 0x9E:
			strCom = "SERVO_SMC_RESET_ERROR";
			break;

		case 0x9F:
			strCom = "SERVO_SMC_MOVE";
			break;
		case 0xA0:
			strCom = "SERVO_SMC_MOVE_OK";
			break;
		case 0xA1:
			strCom = "SERVO_SMC_MOVE_ERROR";
			break;

		case 0xA2:
			strCom = "SERVO_SMC_STATE";
			break;
		case 0xA3:
			strCom = "SERVO_SMC_STATE_OK";
			break;
		case 0xA4:
			strCom = "SERVO_SMC_STATE_ERROR";
			break;

		case 0xA5:
			strCom = "SERVO_SMC_INIT";
			break;
		case 0xA6:
			strCom = "SERVO_SMC_INIT_OK";
			break;
		case 0xA7:
			strCom = "SERVO_SMC_INIT_ERROR";
			break;

		case 0xA8:
			strCom = "SERVO_SMC_TIMEOUT_MOVE";
			break;

		default:

			strCom = "UNKNOW COMMAND";

		}

		return strCom;

	}

}
