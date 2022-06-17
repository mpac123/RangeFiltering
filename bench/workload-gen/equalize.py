import glob

files = glob.glob("range_queries_workloads/*/powerlaw_input.txt")
for file in files:
    words = set()
    with open(file, "r") as f:
        for line in f.readlines():
            line = line.strip()
            if len(line) <= 15:
                words.add(line)
            else:
                words.add(line[:15])

    words = sorted(list(words))
    with open(file, "w") as f:
        f.write('\n'.join([str(w) for w in words]))