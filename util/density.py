import sys


def density(edges, nodes):
  return str((edges / (nodes * (nodes-1))) / 2);

def main():
  if len(sys.argv) != 2:
    print("USAGE: density.py <filename>");
  count = 0;
  current_edges = 0;
  with open(sys.argv[1], 'rb') as stream:
    nodes = int.from_bytes(stream.read(4), "little");
    length = int.from_bytes(stream.read(8), "little");
    updates_per = length // 1000;
    print("Updates per print: " + str(updates_per))
    for i in range(length):
      which = int.from_bytes(stream.read(1), "little");
      if (which == 0):
        current_edges += 1;
      elif (which == 1):
        current_edges -= 1;
      else:
        print("ERROR BAD WHICH = " + str(which))
      if (count % updates_per == 0):
        print(str(count) + " " + density(current_edges, nodes));
      count += 1
      stream.read(8); # discard the actual update

    print(str(count) + " " + density(current_edges, nodes));
  
main()
