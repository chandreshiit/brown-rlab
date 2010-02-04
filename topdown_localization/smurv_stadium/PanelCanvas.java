/*
 * Author: Mark.Buller
 * Created: Saturday, October 06, 2007 9:52:25 PM
 * Modified: Saturday, October 06, 2007 9:52:25 PM
 */
import javax.swing.*;
import java.awt.*;



class PanelCanvas extends JPanel
{
	
	ImageIcon smurvLogo;
	Color chartBackgroundColor;
	Color cBlack;
	Color cRed;
	Color cGray;
	Color cGreen;
	Color cBlue;
	Color cFieldGreen;
	Color cWhite;
	Color cYellow;

	final float pitchLength=4.00f; //units=meters Length will serve as the X-axis
	final float pitchWidth=2.55f; //units=meters Width will serve as the Y-axis
	final float goalWidth=0.71f; //units=meters 
	final float scale=100.0f;
	final float smurvSize=0.33f;
	final float ballSize=0.22f;
	final float tagSize=0.27f;
	
	//Scaling 1 pixel = 1cm by 1cm square
	
	boolean paintActive=false;
	int winHeight=400;
	int winWidth=600;
	
	dataPacket[] dp;
	int dpsize=0;
	
	PanelCanvas()
	{
		chartBackgroundColor=getBackground();//
		cGray=new Color(75,75,75);
		
		cBlack=new Color(0,0,0);
		cRed=new Color(255,100,100);
		cGreen=new Color(0,255,100);
		cBlue=new Color(0,0,255);
		cWhite=new Color(255,255,255);
		cYellow=new Color(255,255,0);
		cFieldGreen=new Color(0,200,0);
		dataPacket[] dp=new dataPacket[1];
		dp[0]=new dataPacket();
		init();
	}
	
	public void paint (Graphics g)
	{
		paintActive=true;						//Used so runnable does not call paint while paint is running
		Graphics screengc = null;
		screengc=g; 					//double buffering screengc is the screen graphics
		Graphics buffer=g.create();			//g is now the buffered image. drawing to g only draws to buffer
			
		//Clear Buffer and draw
		Color cBackground = getBackground();
		buffer.setColor(cGray);
		buffer.fillRect(0,0,800,600);		//clear to color
		drawStadium(buffer);
		drawLocations(buffer);
		screengc=buffer;
		paintActive=false;										//tells thread that repaint is finished
	}
	void init()
	{
		smurvLogo=new ImageIcon("smurfSoccerBackground.gif");
	}
	void drawStadium(Graphics g)
	{
		
		//player area with white lines
		//System.out.println("Drawing Stadium");
		float xos=(((float)winWidth-(pitchLength*scale))/2.0f);
		float yos=(((float)winHeight-(pitchWidth*scale))/2.0f);
		
		float smurvPosX=(((pitchLength*scale))/2.0f)-98.0f;
		float smurvPosY=(((pitchWidth*scale))/2.0f)-89.0f;

		g.setColor(cWhite);
		g.fillRect((int)xos,(int)yos,(int)(pitchLength*scale),(int)(pitchWidth*scale));
		g.setColor(cFieldGreen);
		g.fillRect((int)xos+3,(int)yos+3,(int)(pitchLength*scale)-6,(int)(pitchWidth*scale)-6);
		//Logo
		//g.drawImage(smurvLogo.getImage(), (int)(xos+smurvPosX),(int)(yos+smurvPosY), this);
		//Halfway Line
		int xlinepos=(int)((pitchLength/2.0f)*scale);
		int ylinepos=(int)((pitchWidth/2.0f)*scale);
		g.setColor(cWhite);
		g.fillRect((int)xos+xlinepos-21,(int)yos+ylinepos-21,42,42);
		g.setColor(cFieldGreen);
		g.fillRect((int)xos+xlinepos-19,(int)yos+ylinepos-19,38,38);
		g.setColor(cWhite);
		g.fillRect((int)xos+xlinepos-1,(int)yos,3,(int)(pitchWidth*scale));
		//Goals
		g.setColor(cGreen);
		g.fillRect((int)(xos+(pitchLength*scale)-5.0f),(int)(yos+((((pitchWidth*scale))/2.0f)-(goalWidth*scale)/2.0f)),25,(int)(goalWidth*scale));
		g.setColor(cRed);
		g.fillRect((int)(xos-20.0f),(int)(yos+((((pitchWidth*scale))/2.0f)-(goalWidth*scale)/2.0f)),25,(int)(goalWidth*scale));
	}
	void drawLocations(Graphics g)
	{
		float xos=(((float)winWidth-(pitchLength*scale))/2.0f);
		float yos=(((float)winHeight-(pitchWidth*scale))/2.0f);
		for (int x=0;x<dpsize;x++)
		{
			//System.out.println("Object "+x+" Value of X = "+dp[x].x);
			
			if (dp[x].type==1)
			{
				//draw smurv
				g.setColor(cWhite);
				int xloc=(int)(xos+(dp[x].x*scale));
				int yloc=(int)(yos+(dp[x].y*scale));
				g.fillOval(xloc-((int)(smurvSize*scale)/2),yloc-((int)(smurvSize*scale)/2),(int)(smurvSize*scale),(int)(smurvSize*scale));
				g.setColor(cBlack);
				g.fillRect(xloc,yloc,3,3);
				String yaws=""+dp[x].yaw+"     ";
				g.drawString("("+(int)(dp[x].x*scale)+","+(int)(dp[x].y*scale)+","+yaws.substring(0,4)+")",xloc-35,yloc+35);
				
				//roatation
				int xr=(int)(Math.sin(dp[x].yaw)*(double)((int)(smurvSize*scale)/2));
				int yr=(int)(Math.cos(dp[x].yaw)*(double)((int)(smurvSize*scale)/2));
				
				g.drawLine(xloc,yloc,xloc+xr,yloc-yr);
				
			}
			if (dp[x].type==2)
			{
				//draw Ball
				//ballSize
				//yellow 25cm
				g.setColor(cYellow);
				int xloc=(int)(xos+(dp[x].x*scale));
				int yloc=(int)(yos+(dp[x].y*scale));
				g.fillOval(xloc-((int)(ballSize*scale)/2),yloc-((int)(ballSize*scale)/2),(int)(ballSize*scale),(int)(ballSize*scale));
			}
			if (dp[x].type==3)
			{
				//draw Box
				//draw Ball 30cm
				//Red
				g.setColor(cRed);
				int xloc=(int)(xos+(dp[x].x*scale));
				int yloc=(int)(yos+(dp[x].y*scale));
				g.fillOval(xloc-((int)(tagSize*scale)/2),yloc-((int)(tagSize*scale)/2),(int)(tagSize*scale),(int)(tagSize*scale));
			}
		}
	}

}
