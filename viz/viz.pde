import java.util.*;

ArrayList<ArrayList<Integer>> lines;

String fn = "snapshots.dat";
void parseData() {
  String tmp[];
  String line[];

  String data[] = loadStrings(fn);
  lines = new ArrayList<ArrayList<Integer>>();

  for (int i = 0; i < data.length; i++) {
    line = splitTokens(data[i], ",");
    lines.add(new ArrayList<Integer>());
    for (int j = 0; j < line.length; j++) {
      lines.get(i).add(Integer.parseInt(line[j]));
    }
  }
}

void setup() {
  size(800,600);
  background(255);
  parseData();
}

int curr = 1;
int loop = 0;

void plotInXY(int curr, int min, int max, int num_points) {
  //float y = width / 2.0;
  //float x = ((curr - min) / (max - min)) * width;
  
  int boxes = max - min;
  int n = ((int)sqrt(boxes)) + 1;
  
  int x_box = (curr - min) % n;
  int y_box = (curr - x_box) / n;

  float xFactor = width* 1.0 / n; // pixels / box (x)
  float yFactor = height*1.0 / n; // pixels / box (y)

  float x = x_box * xFactor;
  float y = y_box * yFactor;

  fill(0.0);
  println(x,y,xFactor,yFactor,width,height);
  rect(x, y, x + xFactor, y + yFactor);
}

void draw() {
  background(255);
  int min,max;

  //println("draw.");
  try {
    min = Collections.min(lines.get(curr));
    max = Collections.max(lines.get(curr));
  } catch (NoSuchElementException e) {
    return;
  }

  for (int j = 0; j < lines.get(curr).size(); j++) {
    plotInXY(lines.get(curr).get(j), min, max, lines.get(curr).size());
  }

  loop++;
  if (loop == 100) {
    println("Next frame ",curr);
    loop = 0;
    curr = (curr + 1) % lines.size();
  }
}

