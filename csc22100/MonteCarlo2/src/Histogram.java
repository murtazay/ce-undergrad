import java.awt.Color;
import java.awt.Graphics;
import java.text.DecimalFormat;
import javax.swing.JPanel;


public class Histogram extends JPanel {
	
	final int TOP_MARGIN = 20;
	final int BOTTOM_MARGIN = 20;
	final int LEFT_MARGIN = 20;
	final int RIGHT_MARGIN = 20;
	//Color variable for dark green
	final Color DARK_GREEN = new Color(0x00, 0xC0, 0x00);
	//Private variables used for calculations
	private Simulation simulation;
	private int binWidth;
	private double yFactor;
	
	/**
	 * Constructor takes in s of type Simulation and initializes private variables
	 * @param s of type Simulation
	 */
	public Histogram(Simulation s)
	{
		setBackground(Color.BLACK);
		setSimulation(s);
	}
	public void paintComponent(Graphics g)
	{
		super.paintComponent(g);
		drawXAxis(g);
		drawYAxis(g);
		drawBins(g);
		drawXLabels(g);
		drawYLabels(g);
	}
	
	/**
	 * Draws the X-Axis line
	 * @param g of type Graphics
	 */
	public void drawXAxis(Graphics g)
	{
		g.setColor(Color.WHITE);
		int x1 = LEFT_MARGIN;
		int y1 = getHeight() - BOTTOM_MARGIN;
		int x2 = getWidth() - RIGHT_MARGIN;
		int y2 = y1;
		g.drawLine(x1, y1, x2, y2);
	}
	
	/**
	 * Draws the Y-Axis line
	 * @param g of type Graphics
	 */
	public void drawYAxis(Graphics g)
	{
		g.setColor(Color.WHITE);
		int x1 = LEFT_MARGIN;
		int y1 = getHeight() - BOTTOM_MARGIN;
		int x2 = x1;
		int y2 = TOP_MARGIN;
		g.drawLine(x1, y1, x2, y2);
	}
	
	/**
	 * Draws the rectangles("Bars") of the graph
	 * @param g of type Graphics
	 */
	public void drawBins(Graphics g)
	{
		//(x,y) is the lower left corner of the rectangle
		int x = LEFT_MARGIN;
		int y;
		//Call is made to setFactor to calculate factor
		setYFactor();
		//Call is made to setBinWidth to calculate bin width
		setBinWidth();
		/*
		 * Enhanced for loop draws each bar
		 */
		for(int i : getSimulation().bins)
		{
			//y is calculated
			y = getHeight() - scaleY(i, getYFactor()) - 20;
			//Color is set to dark green
			g.setColor(DARK_GREEN);
			//A solid rectangle is made
			g.fillRect(x, y, getBinWidth(), scaleY(i, getYFactor()));
			//Color is set to yellow
			g.setColor(Color.YELLOW);
			//Outline of rectangle is made
			g.drawRect(x, y, getBinWidth(), scaleY(i, getYFactor()));
			//x is moved
			x += getBinWidth();
		}
	}
	
	/**
	 * Draws the bin ranges on the X-Axis
	 * @param g of type Graphics
	 */
	//Code used in provided in template
	public void drawXLabels(Graphics g)
	{
		g.setColor(Color.WHITE);
		
		DecimalFormat formatter = new DecimalFormat();
		formatter.setMinimumFractionDigits(2);
		formatter.setMaximumFractionDigits(2);
		
		Double labelVal = getSimulation().min;
		String label = formatter.format(labelVal);
		int x = LEFT_MARGIN;
		int y = getHeight() - BOTTOM_MARGIN + 12;
		for(int i : getSimulation().bins)
		{
			g.drawString(label, x-12, y);
			x += getBinWidth();
			labelVal += getSimulation().binSize;
			label = formatter.format(labelVal);
		}
		g.drawString(label, x-12, y);
	}
	
	/**
	 * Draws the "count" above each bar in the graph
	 * @param g of type Graphics
	 */
	//Based off of the draw XLabels
	public void drawYLabels(Graphics g)
	{
		//Set the color to white
		g.setColor(Color.WHITE);
		//Make the formatter
		DecimalFormat formatter = new DecimalFormat("##,###");
		//Instantiate but not initialize 3 variables
		int labelVal;
		String label;
		int height;
		//Set the starting x position to be offset by bin width after margin
		int x = LEFT_MARGIN + (getBinWidth()/4);
		/*
		 * Enhanced for loop prints out the value above
		 * each bar(rectangle)
		 */
		for(int i : getSimulation().bins)
		{
			//Save the int
			labelVal = i;
			//Covert the int into a string after formatting it
			label = formatter.format(labelVal);
			//Caluclate the height of the rectangle and go above it(-21)
			//so it is printed above the rectangle
			height = getHeight() - scaleY(i, getYFactor()) - 21;
			//Draw the string
			g.drawString(label, x, height);
			//Move the x over
			x += getBinWidth();
		}
	}
	/**
	 * The function returns the result of the scaling operation
	 * on the parameters actual
	 * @param actual of type int is the number to be scaled
	 * @param factor of type double is the scaling factor
	 * @return of type int is the result of the scaling operation
	 */
	public int scaleY(int actual, double factor)
	{
		//Type cast need for double to int conversion
		return (int) (actual*factor);
	}
	
	/*
	 * Auto generated getter and setters
	 */
	public Simulation getSimulation() {
		return simulation;
	}
	public void setSimulation(Simulation simulation) {
		this.simulation = simulation;
	}

	public int getBinWidth() {
		return binWidth;
	}
	
	//Overridden auto generated setter
	//Computes the optimal bin width
	public void setBinWidth() {
		int length = getWidth() - LEFT_MARGIN - RIGHT_MARGIN;
		this.binWidth = length/getSimulation().numberOfBins;
	}

	public double getYFactor() {
		return yFactor;
	}

	//Overridden auto generated setter
	//Computes the optimal scale factor
	public void setYFactor() {
		int max = 0;
		for(int i : getSimulation().bins)
		{
			if(i > max)
				max = i;
		}
		double factor = ((double)(getHeight()-TOP_MARGIN-BOTTOM_MARGIN)/(double)(max));
		this.yFactor = factor;
	}
}
