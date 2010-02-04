package localization;

import java.io.IOException;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.util.Map;
import java.util.HashMap;
import java.util.concurrent.locks.ReentrantLock;
import java.net.DatagramPacket;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;

public class LocalizationServer implements Runnable {
	public static final int DATAGRAM_MAX_SIZE = 65507;
	
	public static final int LISTEN_PORT = 8865;
	public static final int BROADCAST_PORT = 8866;
	public static final String BROADCAST_ADDRESS = "10.100.0.255";
//	public static final String BROADCAST_ADDRESS = "255.255.255.255";
	
	private Receiver receiver;
	private Broadcaster broadcaster;
	private Map<String, Map<String, Pose>> map;		// Map<objtype_objid, Map<ipaddress:port, Pose>>
	private ReentrantLock mapLock;
	
	public LocalizationServer(int listenPort, InetAddress broadcastAddress, int broadcastPort) throws SocketException, UnknownHostException {
		receiver = new Receiver(listenPort);
		broadcaster = new Broadcaster(broadcastAddress, broadcastPort);
		map = new HashMap<String, Map<String, Pose>>();
		mapLock = new ReentrantLock();
	}
	
	private Map<String, Map<String, Pose>> swapMap() {
		mapLock.lock();
		Map<String, Map<String, Pose>> toReturn = map;
		map = new HashMap<String, Map<String, Pose>>();
		mapLock.unlock();
		return toReturn;
	}
	
	private void insertPose(String sender, Pose p) {
		mapLock.lock();
		String objId = p.getObjIdentifier();
		if (! map.containsKey(objId)) {
			map.put(objId, new HashMap<String, Pose>());
		}
		map.get(objId).put(sender, p);
		mapLock.unlock();
	}
	
//	private static short parseUint8(byte byteIn) {
//		short value = (short) byteIn;
//		if (value > 127) {
//			value = (short) (value - 256);
//		}
//		return value;
//	}
//	
//	private static byte shortToUint8(short shortIn) {
//		assert(shortIn < 256);
//		return (byte) (shortIn & 0xff);
//	}
	
	public void run() {
		System.out.println("Server is running.");
		new Thread(broadcaster).start();
		new Thread(receiver).start();
	}
	
	private class Receiver implements Runnable {
		private DatagramSocket receiverSocket;
		
		public Receiver(int listenPort) throws SocketException {
			receiverSocket = new DatagramSocket(listenPort);
		}
		
		private Pose[] readPacket(byte[] bs) {
			/*ByteBuffer bb = ByteBuffer.wrap(bs);
			bb.order(java.nio.ByteOrder.BIG_ENDIAN);
			
			byte nrobjects = bb.get();
			
			Pose[] payload = new Pose[nrobjects];
			
			for (byte i = 0; i < nrobjects; i++) {
				bb.order(java.nio.ByteOrder.BIG_ENDIAN);
				byte type = bb.get();
				byte id = bb.get();
				
				// Ignore the timestamp
				bb.get();
				bb.get();
				bb.get();
				bb.get();
				
				bb.order(java.nio.ByteOrder.LITTLE_ENDIAN);
				float x = bb.getFloat();
				float y = bb.getFloat();
				float yaw = bb.getFloat();
				
				payload[i] = new Pose(type, id, x, y, yaw, new Date().getTime());
				
				System.out.println("Received " + payload[i]);
			}*/
      Pose[] payload = new Pose[2];
      payload[0] = new Pose((byte)1,(byte)1,3.0f,4.0f,0.0f,new Date().getTime());
      payload[1] = new Pose((byte)2,(byte)2,5.0f,6.0f,0.0f,new Date().getTime());
			
			return payload;
		}
		
		public void run() {
			// Read from the socket and store messages in the buffer
			byte[] buffer = new byte[DATAGRAM_MAX_SIZE];
			DatagramPacket p = new DatagramPacket(buffer, buffer.length);
			
			while (true) {
				/*try {
					receiverSocket.receive(p);
				} catch (IOException e) {
					e.printStackTrace();
					break;
				}*/
				Pose[] poses = readPacket(buffer);
				String sender = "127.0.0.1:12345";//p.getAddress().toString() + ":" + p.getPort();
				for (int i = 0; i < poses.length; i++) {
					insertPose(sender, poses[i]);
				}
			}
		}
	}
	
	private class Broadcaster implements Runnable {
		private DatagramSocket broadcastSocket;
		private InetAddress broadcastAddress;
		private int broadcastPort;
		
		public Broadcaster(InetAddress broadcastAddress, int broadcastPort) throws SocketException {
			broadcastSocket = new DatagramSocket();
      broadcastSocket.setBroadcast(true);
			this.broadcastAddress = broadcastAddress;
			this.broadcastPort = broadcastPort;
		}
		
		private int writePacket(List<Pose> pl, byte[] buffer) {
			ByteBuffer bb = ByteBuffer.wrap(buffer);
			bb.order(java.nio.ByteOrder.BIG_ENDIAN);
			bb.put((byte) pl.size());
			
			Iterator<Pose> it = pl.iterator();
			while (it.hasNext()) {
				Pose p = it.next();
				
				bb.order(java.nio.ByteOrder.BIG_ENDIAN);
				bb.put(p.getType());
				bb.put(p.getId());
				
				bb.putInt((int) p.getTimestamp());
				
				bb.order(java.nio.ByteOrder.LITTLE_ENDIAN);
				bb.putFloat(p.getX());
				bb.putFloat(p.getY());
				bb.putFloat(p.getYaw());
				
				System.out.println("Sent " + p);
			}
			
			return bb.position();
		}
		
		private void doBroadcast(byte[] buffer) throws IOException {
			List<Pose> pl = new ArrayList<Pose>();
			Map<String, Map<String, Pose>> basemap = swapMap();
			
			// Average duplicates and add to basemap
			Iterator<String> baseKeys = basemap.keySet().iterator();
			while (baseKeys.hasNext()) {
				Map<String, Pose> objmap = basemap.get(baseKeys.next());
				Iterator<String> cameraKeys = objmap.keySet().iterator();
				Pose p = null;
				float x = 0f;
				float y = 0f;
				float yaw = 0f;
				int cameras = 0;
				long timestamp = 0L;
				while (cameraKeys.hasNext()) {
					p = objmap.get(cameraKeys.next());
					
					x += p.getX();
					y += p.getY();
					yaw += p.getYaw();
					timestamp += p.getTimestamp();
					
					cameras++;
				}
				pl.add(new Pose(p.getType(), p.getId(), x/cameras, y/cameras, yaw/cameras, timestamp/cameras));
			}
			
			int len = writePacket(pl, buffer);
			DatagramPacket p = new DatagramPacket(buffer, len, broadcastAddress, broadcastPort);
			broadcastSocket.send(p);
		}
		
		public void run() {
			// Write messages to the socket
			byte[] buffer = new byte[DATAGRAM_MAX_SIZE];
			
			while (true) {
				// Sleep for some time
				try {
					Thread.sleep(1000L);
				} catch (InterruptedException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				
				// Broadcast a packet
				try {
					doBroadcast(buffer);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					break;
				}
			}
		}
	}
	
	public static void main(String[] args) throws IOException, InterruptedException {
		new Thread(new LocalizationServer(LISTEN_PORT, InetAddress.getByName(BROADCAST_ADDRESS), BROADCAST_PORT)).start();
	}
}
