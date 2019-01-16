package com.mv.serverethernetip.client;

import com.mv.serverethernetip.utils.ExceptionGeneric;

public class ExceptionClient extends ExceptionGeneric {

	private static final long serialVersionUID = 275621916587609359L;

	private final ETypeActionException typeAction;

	public ExceptionClient(String msg, String description) {

		super(msg, description);

		typeAction = ETypeActionException.NONE;

	}

	public ExceptionClient(String msg, String description, ETypeActionException a) {

		super(msg, description);

		this.typeAction = a;
	}

	public ETypeActionException getAction() {

		return typeAction;

	}

}
