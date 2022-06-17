import os

#distributions=["last_letter_different", "uniform", "normal", "powerlaw", "increasing_degree"]
distributions=["normal"]
querytypes=["similar", "random", "common_prefix", "last_letter"]
dir="15_2"

workload_dir = "workload-gen/workloads/%s/" % dir
results_dir = "vis"
if not os.path.exists(results_dir):
    os.makedirs(results_dir)

def generate_node(words, pos):
    current = None
    new_words = []
    res = ""
    for word in words:
        if pos >= len(word):
            continue
        if current == None or word[pos] == current:
            current = word[pos]
            new_words.append(word)
            continue
        res += "[" + current + " " + generate_node(new_words, pos+1) + " ]"
        current = word[pos]
        new_words = []
        new_words.append(word)
    if current != None:
        res += "[" + current + " " + generate_node(new_words, pos+1) + " ]"
    return res

def generate_node_color(words, pos, input, queries):
    current = None
    new_words = []
    res = ""
    for word in words:
        if pos >= len(word):
            continue
        if current == None or word[pos] == current:
            current = word[pos]
            new_words.append(word)
            continue
        res += "[" + current + " " + generate_node(new_words, pos+1) + " ]"
        current = word[pos]
        new_words = []
        new_words.append(word)
    if current != None:
        res += "[" + current + " " + generate_node(new_words, pos+1) + " ]"
    return res


# for dist in distributions:
#     with open("workloads/%s/%s_input.txt" % (dir, dist)) as f:
#         words = [word.strip() for word in f.readlines()]

#         for qt in querytypes:
#             w = words.copy()
#             with open("workloads/%s/%s_queries_%s.txt" % (dir, dist, qt)) as f2:
#                 queries = [word.strip() for word in f2.readlines()]

#                 w.extend(queries)
#                 w = sorted(w)
#                 res = generate_node(w, 0)
#                 print(res)
#                 print()

# with open("workloads/15/normal_input.txt") as f1:
#     input = [word.strip() for word in f1.readlines()]
#     for qt in querytypes:
#         with open("workloads/15/normal_queries_%s.txt" % qt) as f2:
#             queries = [word.strip() for word in f2.readlines()]

with open("workloads/example.txt") as f2:
    words = [word.strip() for word in f2.readlines()]
    res = generate_node(words, 0)
    print(res)