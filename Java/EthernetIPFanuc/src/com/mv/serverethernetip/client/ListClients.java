package com.mv.serverethernetip.client;

import java.util.concurrent.ConcurrentHashMap;

import com.mv.serverethernetip.utils.ExceptionGeneric;
import com.mv.serverethernetip.utils.IProcessCommand;

public class ListClients extends ConcurrentHashMap<Integer, ClientAbstract> implements IProcessCommand {

	private static final long serialVersionUID = -7303193514201199737L;

	@Override
	public String processCommand(String cmds) throws ExceptionGeneric {
		
		return null;
	}
	
	public void closeAllClients() {
		
		for(int id : this.keySet()) {
			
			try {
				
				ClientAbstract thClient = this.get(id);
				thClient.interrupt();
				thClient.join();
				
			} catch (InterruptedException e) {
				
				e.printStackTrace();
				
			}
		}
		
	}

}

