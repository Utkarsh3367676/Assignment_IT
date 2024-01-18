Assumptions :

1)The input file is a CSV file.
2)The data fields in each line are assumed to be separated by spaces,
and the order is assumed to be positionId, name, startTime, endTime.
3)The date and time formats in the file are assumed to follow the pattern "%m/%d/%Y %I:%M %p".
4)The program currently prints error messages to cerr when there are issues parsing time.
This assumes that error messages are sufficient for debugging and don't need to be handled programmatically.
5)The file path for the input CSV file is hard-coded in the main function. 
This assumes that the file is available at the specified location
