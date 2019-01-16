package com.mv.serverethernetip.ethernetip;

import java.time.Duration;
import java.util.Random;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.atomic.AtomicInteger;

import com.digitalpetri.enip.cip.CipClient;
import com.digitalpetri.enip.cip.CipConnectionPool.CipConnection;
import com.digitalpetri.enip.cip.CipConnectionPool.CipConnectionFactory;
import com.digitalpetri.enip.cip.CipResponseException;
import com.digitalpetri.enip.cip.epath.EPath.PaddedEPath;
import com.digitalpetri.enip.cip.epath.LogicalSegment.ClassId;
import com.digitalpetri.enip.cip.epath.LogicalSegment.InstanceId;
import com.digitalpetri.enip.cip.services.CipService.PartialResponseException;
import com.digitalpetri.enip.cip.services.ForwardCloseService;
import com.digitalpetri.enip.cip.services.ForwardOpenService;
import com.digitalpetri.enip.cip.services.LargeForwardOpenService;
import com.digitalpetri.enip.cip.structs.ForwardCloseRequest;
import com.digitalpetri.enip.cip.structs.ForwardCloseResponse;
import com.digitalpetri.enip.cip.structs.ForwardOpenRequest;
import com.digitalpetri.enip.cip.structs.ForwardOpenResponse;
import com.digitalpetri.enip.cip.structs.LargeForwardOpenRequest;
import com.digitalpetri.enip.cip.structs.LargeForwardOpenResponse;
import com.digitalpetri.enip.cip.structs.NetworkConnectionParameters;

import io.netty.util.ReferenceCountUtil;

public class DefaultCIPConnectionFactory implements CipConnectionFactory {

	private static final Duration DEFAULT_RPI = Duration.ofSeconds(2);
	private static final Duration DEFAULT_TIMEOUT = Duration.ofSeconds(15);

	private static final PaddedEPath MESSAGE_ROUTER_CP_PATH = new PaddedEPath(new ClassId(0x02), new InstanceId(0x01));

	private static final AtomicInteger T2O_CONNECTION_ID = new AtomicInteger(0);

	private final CipClient client;
	private final PaddedEPath connectionPath;
	private final int connectionSize;

	public DefaultCIPConnectionFactory(CipClient client, PaddedEPath connectionPath, int connectionSize) {
		this.client = client;
		this.connectionPath = connectionPath;
		this.connectionSize = connectionSize;
	}

	@Override
	public CompletableFuture<ForwardCloseResponse> close(CipConnection connection) {
		CompletableFuture<ForwardCloseResponse> future = new CompletableFuture<>();

		ForwardCloseRequest request = new ForwardCloseRequest(Duration.ofNanos(connection.getTimeoutNanos()),
				connection.getSerialNumber(), connection.getOriginatorVendorId(),
				connection.getOriginatorSerialNumber(), connectionPath.append(MESSAGE_ROUTER_CP_PATH));

		ForwardCloseService service = new ForwardCloseService(request);

		client.sendUnconnectedData(service::encodeRequest).whenComplete((b, ex) -> {
			if (b != null) {
				try {
					ForwardCloseResponse response = service.decodeResponse(b);

					future.complete(response);
				} catch (CipResponseException | PartialResponseException e) {
					future.completeExceptionally(e);
				} finally {
					ReferenceCountUtil.release(b);
				}
			} else {
				future.completeExceptionally(ex);
			}
		});

		return future;
	}

	private CompletableFuture<CipConnection> forwardOpen() {
		CompletableFuture<CipConnection> future = new CompletableFuture<>();

		NetworkConnectionParameters parameters = getNetworkConnectionParameters();

		ForwardOpenRequest request = new ForwardOpenRequest(DEFAULT_TIMEOUT, 0, T2O_CONNECTION_ID.incrementAndGet(),
				new Random().nextInt(), client.getConfig().getVendorId(), client.getConfig().getSerialNumber(), 1, // 0
																													// =
																													// x4,
																													// 1
																													// =
																													// x8,
																													// 2
																													// =
																													// x16,
																													// 3
																													// =
																													// x32,
																													// 4
																													// =
																													// x128,
																													// 5
																													// =
																													// x256,
																													// 6
																													// =
																													// x512
				connectionPath.append(MESSAGE_ROUTER_CP_PATH), DEFAULT_RPI, parameters, DEFAULT_RPI, parameters, 0xA3);

		ForwardOpenService service = new ForwardOpenService(request);

		client.sendUnconnectedData(service::encodeRequest).whenComplete((b, ex) -> {
			if (b != null) {
				try {
					ForwardOpenResponse response = service.decodeResponse(b);

					CipConnection connection = new CipConnection(DEFAULT_TIMEOUT.toNanos(),
							response.getO2tConnectionId(), response.getT2oConnectionId(),
							response.getConnectionSerialNumber(), response.getOriginatorVendorId(),
							response.getOriginatorSerialNumber());

					ReferenceCountUtil.release(response.getApplicationReply());

					future.complete(connection);
				} catch (CipResponseException | PartialResponseException e) {
					future.completeExceptionally(e);
				} finally {
					ReferenceCountUtil.release(b);
				}
			} else {
				future.completeExceptionally(ex);
			}
		});

		return future;
	}

	protected NetworkConnectionParameters getNetworkConnectionParameters() {
		return new NetworkConnectionParameters(connectionSize, NetworkConnectionParameters.SizeType.Variable,
				NetworkConnectionParameters.Priority.Low, NetworkConnectionParameters.ConnectionType.PointToPoint,
				false);
	}

	private CompletableFuture<CipConnection> largeForwardOpen() {
		CompletableFuture<CipConnection> future = new CompletableFuture<>();

		NetworkConnectionParameters parameters = getNetworkConnectionParameters();

		LargeForwardOpenRequest request = new LargeForwardOpenRequest(DEFAULT_TIMEOUT, // timeout
				0, // o2tConnectionId
				T2O_CONNECTION_ID.incrementAndGet(), // t2oConnectionId
				new Random().nextInt(), // connectionSerialNumber
				client.getConfig().getVendorId(), // vendorId
				client.getConfig().getSerialNumber(), // vendorSerialNumber
				1, // connectionTimeoutMultiplier
				connectionPath.append(MESSAGE_ROUTER_CP_PATH), // connectionPath
				DEFAULT_RPI, // o2tRpi
				parameters, // o2tParameters
				DEFAULT_RPI, // t2oRpi
				parameters, // t2oParameters
				0xA3); // transportClassAndTrigger

		LargeForwardOpenService service = new LargeForwardOpenService(request);

		client.sendUnconnectedData(service::encodeRequest).whenComplete((b, ex) -> {
			if (b != null) {
				try {
					LargeForwardOpenResponse response = service.decodeResponse(b);

					CipConnection connection = new CipConnection(DEFAULT_TIMEOUT.toNanos(),
							response.getO2tConnectionId(), response.getT2oConnectionId(),
							response.getConnectionSerialNumber(), response.getOriginatorVendorId(),
							response.getOriginatorSerialNumber());

					ReferenceCountUtil.release(response.getApplicationReply());

					future.complete(connection);
				} catch (CipResponseException | PartialResponseException e) {
					future.completeExceptionally(e);
				} finally {
					ReferenceCountUtil.release(b);
				}
			} else {
				future.completeExceptionally(ex);
			}
		});

		return future;
	}

	@Override
	public CompletableFuture<CipConnection> open() {
		return connectionSize <= 500 ? forwardOpen() : largeForwardOpen();
	}

}
