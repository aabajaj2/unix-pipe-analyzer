# Pipe Analyzer
The program pa.c will help you read through Unix Pipes. Let's take an example, there is `./pa seq 100000 \| wc -l -c` 
In this example, the first child process will calculate seq 100000 and the second child process will calculate wc -l -c. I added a middle process which gives us analysis of the data flow.

Add a \ before the pipe to run the program. 
Eg:  `./pa seq 100000 \| wc -l -c`
Also, pa.log will be created in same folder.
