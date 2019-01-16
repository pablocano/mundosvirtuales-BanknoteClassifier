package com.mv.serverethernetip.ethernetip;

import java.util.function.Consumer;

import com.digitalpetri.enip.cip.CipResponseException;
import com.digitalpetri.enip.cip.epath.EPath.PaddedEPath;
import com.digitalpetri.enip.cip.services.CipService;
import com.digitalpetri.enip.cip.structs.MessageRouterRequest;
import com.digitalpetri.enip.cip.structs.MessageRouterResponse;

import io.netty.buffer.ByteBuf;
import io.netty.util.ReferenceCountUtil;

public class SetAttributeAllServiceEIP implements CipService<Void> {

	private final Consumer<ByteBuf> attributeEncoder;

	private final PaddedEPath requestPath;
	public final int serviceCode;

	public SetAttributeAllServiceEIP(PaddedEPath requestPath, Consumer<ByteBuf> attributeEncoder) {
		this.requestPath = requestPath;
		this.attributeEncoder = attributeEncoder;
		this.serviceCode = 0x02;
	}

	public SetAttributeAllServiceEIP(PaddedEPath requestPath, Consumer<ByteBuf> attributeEncoder, int service) {
		this.requestPath = requestPath;
		this.attributeEncoder = attributeEncoder;
		this.serviceCode = service;
	}

	@Override
	public Void decodeResponse(ByteBuf buffer) throws CipResponseException, PartialResponseException {
		MessageRouterResponse response = MessageRouterResponse.decode(buffer);

		try {
			if (response.getGeneralStatus() == 0x00) {
				return null;
			} else {
				throw new CipResponseException(response.getGeneralStatus(), response.getAdditionalStatus());
			}
		} finally {
			ReferenceCountUtil.release(response.getData());
		}
	}

	@Override
	public void encodeRequest(ByteBuf buffer) {
		MessageRouterRequest request = new MessageRouterRequest(serviceCode, requestPath, attributeEncoder);

		MessageRouterRequest.encode(request, buffer);
	}

}
