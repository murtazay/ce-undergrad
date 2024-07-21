import java.util.ArrayList;
//import java.io.*;//Imports all the classes for file writing
import javax.swing.JFrame;


public class MonteCarlo {

	public static void main(String[] args) {
		//Empty list is made
		ArrayList<Double> list = new ArrayList<Double>();
		//Obj of type Simulation is made
		Simulation gen = new Simulation();
		//100000 numbers are generated
		gen.generateNormalRandomNumbers(list, 100000);
		//bins array is made of size 11
		int[] bins = gen.makeBins(list, 11);
		/*
		//IOException handling with try
		try{
			//BufferedWriter for writing to file "Gauss.txt"
			BufferedWriter writer = new BufferedWriter( new OutputStreamWriter(new FileOutputStream("Gauss.txt")));
			//Enhanced for loop traverses the bin array
			for(int i : bins)
			{
				//Prints to screen
				System.out.println(i);
				//Writes to file
				writer.write(i + "\n");
			}
			//Closes the file
			writer.close();
		}
		//Exception handling
		catch (IOException e)
		{
			//Prints the exception message
			System.out.println(e.toString());
		}
		//For loop to run verifyDistribution
		for(Double i = 1.0; i < 4; i++)
		{
			//Call made to verifyDistribution from with print statement
			System.out.printf("The Percentage of numbers from mean 0, with standard deviation 1 and number of standard deviations %.2f is : %.3f%%%n",i ,Metrics.verifyDistribution(list, 0.0, 1.0, i));
		}
		*/
		Histogram h = new Histogram(gen);
		JFrame visuals = new JFrame();
		
		visuals.setTitle("CSc 221 Histogram");
		visuals.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		visuals.add(h);
		visuals.setSize(1200, 800);
		visuals.setVisible(true);		
	}

}
