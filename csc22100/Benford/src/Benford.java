/**
 * Name: Benford.java
 * Author: Murtaza Yaqoob
 */

import java.io.BufferedReader; //Needed for BufferReader
import java.io.IOException; //Needed to handle file errors
import java.nio.file.Files; //Needed to direct file into buffer
import java.nio.file.Path; //Needed to create file variable
import java.nio.file.Paths; //Needed to convert file path
import java.nio.charset.Charset; //Needed to specify character set

/*SOURCES
 * http://www.adam-bien.com/roller/abien/entry/reading_a_file_into_a
 * Used for reading a file.
 */

public class Benford {

	public static void main(String[] args) throws IOException{
		//Converts the string into path object which defines the file object
		Path file = Paths.get("data.txt");
		//Creates a new BufferReader to read in from the file specified by the line above.
		//Buffer is created with the default character set for the purposes of this assignment.
		BufferedReader reader = Files.newBufferedReader(file, Charset.defaultCharset());
		//Initializes a new string called line to null.
		String line = null;
		//Initializes a new array of type int and size 10.
		int[] array = new int[10];
		//Creates a variable of type int called total and initializes it to 0.
		int total = 0;
		/*
		 * The following while loop is used to parse through the file and read in each line 
		 * one by one, implementing all the variables defined previously.
		 * The loop is to keep going until it reaches the end of the file.
		 */
		while((line = reader.readLine())!= null){
			total++;
			//Creates the pointer for the array of type char.
			char[] A;
			//Initializes the array to the new copy of the line string that 
			//has been converted to an array of type char.
			A = line.toCharArray();
			//Used for debugging purpose and code benchmark to mark reading of file correctly.
			//System.out.println(line);
			//Initializes an counter i, of type int to 0.
			int i = 0;
			/*
			 * The following while loop goes through the array A of type char that
			 * and searches for the first significant digit and stops, or until the 
			 * array is at its end.
			 */
			while(i != A.length){
				//If statement checks if the char at the value given by the 
				//current index i, is 1-9, if so the char type is converted to 
				//an int and the appropriate index is incremented in the array array.
				if(A[i] == '1' || A[i] == '2' || A[i] == '3' ||
				   A[i] == '4' || A[i] == '5' || A[i] == '6' ||
				   A[i] == '7' || A[i] == '8' || A[i] == '9'){
					//Converts and increments the array array accordingly.
					array[Character.getNumericValue(A[i])]++;
					//Break statement is used to exit out the current
					//loop and thus the next line is read.
					break;
				}
				else{
					//The value at the current index was not a significant digit
					//therefore check the next one.
					i++;
				}
			}
		}
		//Make a call to the method printResults that prints the results of the 
		//data collected above.
		printResults(array, total);
		
	}
	/**
	 * public static void printResults(int[] array)
	 * @param array
	 * The method takes in an array of data type int,
	 * and an int total that has the total number of lines read,
	 * and prints out the averages of the number of
	 * occurrences for each index of the array after 
	 * the index of 0.
	 */
	public static void printResults(int[] array, int total){
		/*
		 * The following for loop computes the average for each index number of the 
		 * array and prints it out along with the index number, percentage and stars.
		 */
		for(int i = 1; i < array.length; i++)
		{
			//Average computation line, the average is displayed as a double.
			double a = ((double)array[i]/(double)total);
			//Print out line for the index and percentage.
			//The average is being multiplied by 100 and the 
			//average is truncated to only the 3rd decimal place
			//as well as a minimum number of characters for better reading.
			System.out.printf("%d (%6.3f%%)\t: ",i,a*100);
			/*
			 * The following for loop prints out the number of stars
			 * in the same line as the one by the printf statement above.
			 */
			//The Math.round function is used to round the average*100 accordingly.
			for(int j = 0; j < Math.round(a*100); j++){
				//Prints one * for each loop iteration.
				System.out.print("*");
			}
			//Used to print out a newline character.
			System.out.println();
		}
	}
	
}
