package com.mv.serverethernetip.ethernetip;

import com.digitalpetri.enip.cip.CipResponseException;
import com.digitalpetri.enip.cip.epath.EPath.PaddedEPath;
import com.digitalpetri.enip.cip.services.CipService;
import com.digitalpetri.enip.cip.structs.MessageRouterRequest;
import com.digitalpetri.enip.cip.structs.MessageRouterResponse;

import io.netty.buffer.ByteBuf;
import io.netty.util.ReferenceCountUtil;

public class GetAttributeServiceEIP implements CipService<ByteBuf> {

	public static final int SERVICE_CODE_DEFAULT = 0x0E;
	private final PaddedEPath requestPath;

	public final int serviceCode;

	public GetAttributeServiceEIP(PaddedEPath requestPath) {
		this.requestPath = requestPath;
		this.serviceCode = SERVICE_CODE_DEFAULT;
	}

	public GetAttributeServiceEIP(PaddedEPath requestPath, int service) {
		this.requestPath = requestPath;
		this.serviceCode = service;
	}

	private ByteBuf decode(ByteBuf buffer) {

		return buffer.readSlice(buffer.readableBytes()).retain();

	}

	@Override
	public ByteBuf decodeResponse(ByteBuf buffer) throws CipResponseException, PartialResponseException {

		MessageRouterResponse response = MessageRouterResponse.decode(buffer.retain());

		try {

			if (response.getGeneralStatus() == 0x00) {

				return decode(response.getData());

			} else {

				throw new CipResponseException(response.getGeneralStatus(), response.getAdditionalStatus());

			}

		} finally {

			ReferenceCountUtil.release(response.getData());

		}
	}

	private void encode(ByteBuf buffer) {

	}

	@Override
	public void encodeRequest(ByteBuf buffer) {

		MessageRouterRequest request = new MessageRouterRequest(serviceCode, requestPath, this::encode);

		MessageRouterRequest.encode(request, buffer);

	}

}
