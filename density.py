import sys


def density(edges, nodes):
  return str((edges / (nodes * (nodes-1))) / 2);

def main():
  if len(sys.argv) != 2:
    print("USAGE: density.py <filename>");
  count = 0;
  current_edges = 0;
  with open(sys.argv[1], 'r') as stream:
    [nodes, length] = [int(x) for x in stream.readline().split(' ')]
    updates_per = length // 1000;
    print(updates_per)
    for line in stream.readlines():
      if (int(line.split(' ')[0]) == 0):
        current_edges += 1;
      else:
        current_edges -= 1;
      if (count % updates_per == 0):
        print(str(count) + " " + density(current_edges, nodes));
      count += 1

    print(str(count) + " " + density(current_edges, nodes));
  
main()
