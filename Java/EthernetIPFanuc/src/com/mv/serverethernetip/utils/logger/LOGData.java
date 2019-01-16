package com.mv.serverethernetip.utils.logger;

import java.util.Date;

public class LOGData {

	public final Date date;
	public final String errMessage;
	public final String message;
	public final String tag;
	public final ETypeLog typeLog;

	public LOGData(ETypeLog type, String tag, String msg, String err) {

		this.typeLog = type;
		this.message = msg;
		this.tag = tag;
		this.errMessage = err;
		this.date = new Date();

	}

	public LOGData(ETypeLog type, String tag, String msg, String err, Date date) {

		this.typeLog = type;
		this.tag = tag;
		this.message = msg;
		this.errMessage = err;
		this.date = date;

	}

}
