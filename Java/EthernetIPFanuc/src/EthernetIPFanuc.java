import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.Map;
import java.util.Scanner;
import java.util.UUID;

public class EthernetIPFanuc implements Runnable {
	
	public 	final int PORT_SERVER = 3333;
	public 	final int TIMEOUT_SOC = 5000; // 5 seconds
	private final ServerSocket serverSocket;
	private final Map<UUID, ClientRobotFanuc> mapClient = new ConcurrentHashMap<UUID, ClientRobotFanuc>();
	private final AtomicBoolean isClosed = new AtomicBoolean(false);
	private final Thread thServer;
	
	public EthernetIPFanuc() throws IOException {
		
		serverSocket = new ServerSocket(PORT_SERVER);
		serverSocket.setSoTimeout(TIMEOUT_SOC);
		thServer = new Thread(this);
		
	}
	
	public void start() {
		
		thServer.start();
		
	}
	
	public void close() {
		
		isClosed.set(true);
		thServer.interrupt();
		
		for(UUID key : mapClient.keySet()) {
			
			Thread thClient = mapClient.get(key);
			thClient.interrupt();
			
			try {
				
				thClient.join();
				
			} catch (InterruptedException e) {
				
				e.printStackTrace();
			}
		}
		
		try {
			
			serverSocket.close();
			
		} catch (IOException e) {

			e.printStackTrace();
		}
		
		try {
			
			thServer.join();
			
		} catch (InterruptedException e) {

			e.printStackTrace();
		}
	}
	
	@Override
	public void run() {
		
		do {

			try {
				
				Socket socket = serverSocket.accept();
				
				UUID uuid = UUID.randomUUID();
				ClientRobotFanuc thClient = new ClientRobotFanuc("10.0.42.100", socket, uuid);
				
				thClient.setCloseListener(new ClientRobotFanucCloseListener() {
					
					@Override
					public void onClose(UUID key) {
						mapClient.remove(key);
					}
				});
				
				mapClient.put(uuid, thClient);
				
				thClient.connect();
				
				thClient.start();
				
			} catch (SocketTimeoutException e) {
				
				
		        
		    } catch (IOException e) {

				e.printStackTrace();
			} 
			
		} while(!isClosed.get());
		
	}
	
	public static void main(String[] args) {
		
		EthernetIPFanuc ethernetIPFanucServer;
		
		try {
			
			ethernetIPFanucServer =  new EthernetIPFanuc();
			
			Scanner scanner = new Scanner(System.in);
			String command;
			String prompt = "> ";
			
			ethernetIPFanucServer.start();
			
			do {
				
	            System.out.print(prompt);
	            command = scanner.next();
	            
	            String result = ethernetIPFanucServer.processCommand(command);
	            
	            if(!result.isEmpty()) {
	            	System.out.print(prompt + result);
	            }
				
			} while (!(command.equals("quit") | command.equals("q")));
			
			scanner.close();
			
			ethernetIPFanucServer.close();
			
		} catch (IOException e) {
			
			e.printStackTrace();
		}
		
	}

	public String processCommand(String command) {
		
		String result = "";
		
		String[] cmds = command.split("[, ]");
		
		switch(cmds[0]) {
		
		case "list":
			
			for (UUID key : mapClient.keySet()) {
				ClientRobotFanuc thClient = mapClient.get(key);
				result += thClient.toString() + "\n"; 
			}
			break;
			
		case "close":
			
			UUID key = UUID.fromString(cmds[1]);
			ClientRobotFanuc thClient = mapClient.get(key);
			thClient.close();
			break;
			
		}
		
		return result;
	}

}
