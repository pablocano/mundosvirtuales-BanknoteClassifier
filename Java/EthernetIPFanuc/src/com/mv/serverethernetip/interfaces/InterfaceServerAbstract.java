package com.mv.serverethernetip.interfaces;

import com.mv.serverethernetip.server.ServerAbstract;
import com.mv.serverethernetip.utils.IProcessCommand;
import com.mv.serverethernetip.utils.ObjectGeneric;

public abstract class InterfaceServerAbstract extends ObjectGeneric implements IProcessCommand {

	protected final ServerAbstract server;
	
	protected boolean logDebug = true;
	protected boolean logError = true;
	protected boolean logInfo = true;

	protected InterfaceServerAbstract(String name, ServerAbstract server) {

		super(name);

		this.server = server;

	}
	
	public boolean isLogDebug() {
		
		return logDebug;
		
	}

	public void setLogDebug(boolean logDebug) {
		
		this.logDebug = logDebug;
		
	}

	public boolean isLogError() {
		
		return logError;
		
	}

	public void setLogError(boolean logError) {
		
		this.logError = logError;
		
	}

	public boolean isLogInfo() {
		
		return logInfo;
		
	}

	public void setLogInfo(boolean logInfo) {
		
		this.logInfo = logInfo;
		
	}

	public abstract void execute();

}
