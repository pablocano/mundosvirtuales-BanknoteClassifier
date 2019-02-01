package com.mv.serverethernetip.ethernetip;

import com.digitalpetri.enip.cip.CipResponseException;
import com.digitalpetri.enip.cip.epath.EPath.PaddedEPath;
import com.digitalpetri.enip.cip.services.CipService;
import com.digitalpetri.enip.cip.structs.MessageRouterRequest;
import com.digitalpetri.enip.cip.structs.MessageRouterResponse;

import io.netty.buffer.ByteBuf;

public class GetAttributeAllServiceEIP implements CipService<ByteBuf> {

	private final PaddedEPath requestPath;

	public final int serviceCode;

	public GetAttributeAllServiceEIP(PaddedEPath requestPath) {
		this.requestPath = requestPath;
		this.serviceCode = 0x01;
	}

	public GetAttributeAllServiceEIP(PaddedEPath requestPath, int service) {
		this.requestPath = requestPath;
		this.serviceCode = service;
	}

	@Override
	public ByteBuf decodeResponse(ByteBuf buffer) throws CipResponseException, PartialResponseException {
		MessageRouterResponse response = MessageRouterResponse.decode(buffer);

		if (response.getGeneralStatus() == 0x00) {
			return response.getData();
		} else {
			throw new CipResponseException(response.getGeneralStatus(), response.getAdditionalStatus());
		}
	}

	@Override
	public void encodeRequest(ByteBuf buffer) {
		MessageRouterRequest request = new MessageRouterRequest(serviceCode, requestPath, byteBuf -> {
		});

		MessageRouterRequest.encode(request, buffer);
	}

}
