all:
	g++ -g -Wall -Werror -o predictor Predictor.cpp

run: all
	./predictor short_trace1.txt output_file.txt
clean:
	rm -rf *.o $(D)
