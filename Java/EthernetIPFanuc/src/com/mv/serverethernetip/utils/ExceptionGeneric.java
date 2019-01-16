package com.mv.serverethernetip.utils;

public class ExceptionGeneric extends Exception {

	private static final long serialVersionUID = 5553107053086016493L;

	protected final String description;
	protected final String message;

	private PacketCommServer packet = null;

	protected final ETypeError typeError;

	public ExceptionGeneric(ETypeError type, String msg, String description) {

		this.typeError = type;
		this.message = msg;
		this.description = description;

	}

	public ExceptionGeneric(String msg, String description) {

		this.typeError = ETypeError.OTHER;
		this.message = msg;
		this.description = description;

	}

	public ExceptionGeneric(String message, String description, PacketCommServer packet) {

		this.typeError = ETypeError.OTHER;
		this.message = message.trim();
		this.description = description.trim();
		this.packet = packet;

	}

	public boolean existPacketResponseError() {

		return packet != null;

	}

	public String getDescription() {

		return description;

	}

	@Override
	public String getMessage() {

		if (description == null || description.isEmpty()) {

			return message;

		} else {

			return message + ", " + description;

		}

	}

	public PacketCommServer getPacket() {

		return packet;

	}

	public ETypeError getTypeError() {

		return typeError;

	}

	public void setPacket(PacketCommServer packet) {

		this.packet = packet;

	}

}
