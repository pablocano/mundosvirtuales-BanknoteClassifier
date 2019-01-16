package com.mv.serverethernetip.interfaces;

import java.io.PrintStream;
import java.util.function.BiConsumer;

import com.mv.serverethernetip.server.ServerAbstract;
import com.mv.serverethernetip.utils.logger.LOGData;

public class InterfaceServerConsole extends InterfaceServerFile {

	public InterfaceServerConsole(String name, ServerAbstract server, BiConsumer<LOGData, PrintStream> fnConsumer) {

		super(name, server, System.out, fnConsumer);

	}

}
