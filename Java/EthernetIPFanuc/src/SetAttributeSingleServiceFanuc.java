import com.digitalpetri.enip.cip.CipResponseException;
import com.digitalpetri.enip.cip.epath.EPath.PaddedEPath;
import com.digitalpetri.enip.cip.services.CipService;
import com.digitalpetri.enip.cip.structs.MessageRouterRequest;
import com.digitalpetri.enip.cip.structs.MessageRouterResponse;

import io.netty.buffer.ByteBuf;
import io.netty.util.ReferenceCountUtil;

public class SetAttributeSingleServiceFanuc implements CipService<ByteBuf> {
	
	public static final int SERVICE_CODE = 0x10;

    private final PaddedEPath requestPath;
    private final ByteBuf bufferReg;

    public SetAttributeSingleServiceFanuc(PaddedEPath requestPath, ByteBuf buffer) {
        this.requestPath = requestPath;
        this.bufferReg = buffer;
    }

    @Override
    public void encodeRequest(ByteBuf buffer) {
        MessageRouterRequest request = new MessageRouterRequest(
            SERVICE_CODE,
            requestPath,
            this::encode
        );

        MessageRouterRequest.encode(request, buffer);
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
            ReferenceCountUtil.release(buffer);
        }
    }

    private void encode(ByteBuf buffer) {
    	buffer.writeBytes(this.bufferReg.array());
    }

    private ByteBuf decode(ByteBuf buffer) {
    	return buffer.readSlice(buffer.readableBytes()).retain();
    }
}
