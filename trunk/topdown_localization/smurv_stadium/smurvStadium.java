/**
 * Title:        UDP Listener<p>
 * Description:  <p>
 * Copyright:    Mark Buller<p>
 * Company:      Brown<p>
 * @author Mark Buller
 */

import java.net.*;
import java.io.*;
import java.util.*;
import java.nio.*;  //for ByteBuffer

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;


public class smurvStadium extends JFrame
{

	JFrame appWindow;
	final int appWidth=600;
	final int appHeight=400;
	PanelCanvas mainGUI;
	
	ByteBuffer smurvbb;  //Byte Buffer for Parsing and Constructing 
	DatagramSocket mcsocket;
	InetAddress group;
	int port=8856;
	//String groupIP="255.255.255.255";
	boolean stop=false;
    byte[] buffer;
	//Construct the application
	public smurvStadium() throws IOException
	{
		smurvbb=ByteBuffer.allocate(1000);
		//mcsocket=new DatagramSocket(port, InetAddress.getByName("10.100.0.0"));
		mcsocket=new DatagramSocket(port);
		mcsocket.setSoTimeout(100);
		//group=InetAddress.getByName(groupIP);
		buffer=new byte[1000];
		
		setUpGUI();
		runTheListener();
	}
	
	//-----------------------------------------------------------------------------------------------------
	//Main method
	public static void main(String[] args) throws IOException
	{
		new smurvStadium();
	}
	
	void setUpGUI()
	{
		appWindow=this;
		appWindow.setTitle("Smurv Stadium");
		appWindow.setSize(new Dimension(appWidth, appHeight));
		appWindow.setDefaultCloseOperation(EXIT_ON_CLOSE);
		//JPanel top=new JPanel(new FlowLayout(FlowLayout.CENTER));
		mainGUI=new PanelCanvas();
		mainGUI.winHeight=appHeight;
		mainGUI.winWidth=appWidth;
		
		//top.add(mainGUI);
		appWindow.getContentPane().add(mainGUI);
		appWindow.setVisible(true);
	}
	void runTheListener() throws IOException
	{
		//mcsocket.joinGroup(group);
		while(!stop)
		{
			//Receive Packet
			DatagramPacket packet=new DatagramPacket(buffer,1000);
			
			try
			{
				mcsocket.receive(packet);
				buffer=packet.getData();
				//System.out.println("Receiving Packet >>>"+buffer+"<<< "+packet.getLength());
				//Parse the data packet
				parseSmurvLocations(buffer,packet.getLength());
			}
			catch(SocketTimeoutException stoe)
			{
				//Do Nothing...
			}
		

			//System.out.println("AppWindow"+appWindow);
			
			try {Thread.sleep(50);}
			catch (InterruptedException e) {System.out.println("Sleep Exception! "+e);}

		}
		//mcsocket.leaveGroup(group);
		mcsocket.close();
	}
	void parseSmurvLocations(byte[] buff, int length)
	{
		//Note: there is no sanity checking on the UDP packet
		smurvbb.position(0);
		smurvbb.put(buff,0,length);
		smurvbb.position(0);
		//Parse Header
		byte numObjects=smurvbb.get();
		System.out.println("Smurv Position Packet with "+numObjects+" object(s)");
		//Parse each object
		dataPacket[] dp=new dataPacket[numObjects];
		for (int no=0;no<numObjects;no++)
		{
			dp[no]=new dataPacket();
			System.out.print("Object # "+(no+1)+" >>>");
			dp[no].type=(int)smurvbb.get();
			System.out.print("  Type "+dp[no].type);
			dp[no].id=(int)smurvbb.get();
			System.out.print("  ID "+dp[no].id);
			dp[no].time=smurvbb.getInt();
			System.out.print("  Time "+dp[no].time);
			smurvbb.order(ByteOrder.LITTLE_ENDIAN);
			dp[no].x=smurvbb.getFloat();
			System.out.print("  X: "+dp[no].x);
			dp[no].y=smurvbb.getFloat();
			System.out.print("  Y: "+dp[no].y);
			dp[no].yaw=smurvbb.getFloat();
			System.out.println("  yaw: "+dp[no].yaw);
		}
		mainGUI.dp=dp;
		mainGUI.dpsize=numObjects;
		repaint();
		
	}
	
}
