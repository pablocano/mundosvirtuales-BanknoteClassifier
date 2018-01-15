
public interface CommandEthernetIPFanuc {

	public final short NO_ERROR 			= 0x01;
	
	public final short WRITE_REG 			= 0x10;
	public final short WRITE_REG_OK		 	= 0x11;
	public final short WRITE_REG_ERROR 		= 0x12;
	
	public final short WRITE_POS 			= 0x20;
	public final short WRITE_POS_OK			= 0x21;
	public final short WRITE_POS_ERROR		= 0x22;
	
	public final short WRITE_JPOS 			= 0x30;
	public final short WRITE_JPOS_OK		= 0x31;
	public final short WRITE_JPOS_ERROR		= 0x32;
	
	public final short READ_REG 			= 0x40;
	public final short READ_REG_OK			= 0x41;
	public final short READ_REG_ERROR		= 0x42;
	
	public final short READ_POS 			= 0x50;
	public final short READ_POS_OK			= 0x51;
	public final short READ_POS_ERROR		= 0x52;
	
	public final short READ_JPOS 			= 0x60;
	public final short READ_JPOS_OK			= 0x61;
	public final short READ_JPOS_ERROR		= 0x62;
	
	public final short READ_CURR_POS		= 0x70;
	public final short READ_CURR_POS_OK		= 0x71;
	public final short READ_CURR_POS_ERROR	= 0x72;
	
	public final short READ_CURR_JPOS		= 0x80;
	public final short READ_CURR_JPOS_OK	= 0x81;
	public final short READ_CURR_JPOS_ERROR	= 0x82;
}
