import java.util.ArrayList;


public class Metrics {

	/**
	 * This function returns the percentage of numbers within a certain
	 * standard deviation from the mean.
	 * @param list of type ArrayList(Double) is the source
	 * @param mean of type Double is the mean of the list
	 * @param standardD of type Double is the standard deviation of the list
	 * @param range of type Double is the number of standard deviations to be included
	 * @return The percentage of occurrence of the numbers in list given the values: mean
	 * , standardD and range.
	 */
	public static double verifyDistribution(ArrayList<Double> list, Double mean, Double standardD, Double range)
	{
		//Calculates the lower testing bound
		Double lowEnd = mean - (standardD * range);
		//Calculates the upper testing bound
		Double highEnd = mean + (standardD * range);
		//Count keeps track of the number of elements that fit the restraints
		double count = 0;
		//Total keeps track of the number of elements in the list
		double total = 0;
		//Enhanced for loop to traverse the list
		for(Double i : list)
		{
			//Total is incremented
			total++;
			//Statement verifys that the element is in the range
			if(i >= lowEnd && i <= highEnd)
			{
				//If so increment count
				count++;
			}
		}
		return (count/total)*100;
	}
}
