all:
	g++ -g -Wall -Werror -o Driver Driver.cpp Predictor.cpp

run: all
	./$(D) short_trace1.txt output_file.txt
clean:
	rm -rf *.o $(D)
