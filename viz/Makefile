
all: combo.pdf

combo.pdf: viz.py snapshots.dat
	./viz.py
	convert *.png combo.pdf

clean:
	rm -f combo.pdf [0-9][0-9].png

