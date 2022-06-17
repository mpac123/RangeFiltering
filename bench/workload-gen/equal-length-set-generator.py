dir = "range_queries_workloads/100k_26_15_3__2_100"
output = "equal"
dataset = "normal"

words = []
max_length = 0
with open('%s/%s_input.txt' % (dir, dataset), "r") as f:
    for line in f.readlines():
        words.append(line.strip())
        max_length = max(max_length, len(line.strip()))

with open('%s/%s_input.txt' % (dir, output), "w") as f:
    padded_words = [word.ljust(max_length, 'a') for word in words]
    f.write("\n".join(padded_words))