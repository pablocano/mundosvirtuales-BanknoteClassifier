package com.mv.serverethernetip.utils;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.mv.serverethernetip.utils.logger.Logger;

public class ObjectGeneric {

	@JsonProperty
	public String name;
	
	@JsonIgnore
	public static final Logger logger = Logger.getInstance();

	protected ObjectGeneric(String name) {

		this.name = name;

	}

	public static void sleep(int msec) {

		try {

			Thread.sleep(msec);

		} catch (InterruptedException e) {

			e.printStackTrace();

		}

	}

	@Override
	public String toString() {

		return name;

	}

}
