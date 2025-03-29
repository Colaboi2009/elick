all: compile play

PROJECT := "elric"

compile:
	@mkdir -p build \
		&& cd build \
		&& cmake -S ../ -B . \
		&& make \
		&& cd .. \
		&& cp build/$(PROJECT) . \

play:
	@./$(PROJECT)

killall:
	ps -h | grep $(PROJECT) | grep -v grep | awk '{print $1}' | xargs -r kill -9
