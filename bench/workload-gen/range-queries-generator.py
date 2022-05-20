import click
from random import normalvariate, shuffle
from numpy import random
import scipy.stats

@click.command()
@click.option('--n', type=int, help="Number of words to be generated", required=True)
@click.option('--alph-size', default=10, help='Size of the alphabet')
@click.option('--alph-start', help='First letter of the alphabet')
@click.option('--length-avg', type=int, help='The average length of the generated words', required=True)
@click.option('--length-sigma', type=int, help='Standard deviation in lengths of the generated words', required=True)
@click.option('--letter-distribution', 
                type=click.Choice(["uniform", "normal", "powerlaw", "last_letter_different", "increasing_degree"]), 
                help='Distribution with which node`s child letters will be generated', 
                required=True)
@click.option('--normal_letter_distr_stddev',default=1, help='If letter-distribution=normal, set stddev to this value')
@click.option('--output', help="Name of the output file")
@click.option('--output-dir', help="Name of the directory to save the output files")
@click.option('--range-min-size', type=int, default=2, help="Minimum size of the range")
@click.option('--range-max-size', type=int, default=20, help="Maximum size of the range")
def generator(n, alph_size, alph_start, length_avg, length_sigma, letter_distribution, normal_letter_distr_stddev, output, output_dir, range_min_size, range_max_size):
    """Generate workload for filter and range filtering benchmarking"""
    # First, check if it is possible to generate n words with given alphabet size and averge word length
    if (pow(alph_size, length_avg) < n):
        click.echo("Provided n is too big to generate words from given alphabet and with given average word length")
        return

    words = []
    queries_random = []
    queries_similar = []
    queries_common_prefix = []
    queries_last_letter_changed = []
    norm = scipy.stats.norm(length_avg, length_sigma)
    
    alphabet = [chr((ord(alph_start) + i) % 256) for i in range(alph_size)]
    choice_func = uniform_choice if letter_distribution == "uniform" else powerlaw_choice if letter_distribution=="powerlaw" else normal_choice

    click.echo("Generating words")
    if letter_distribution=="last_letter_different":
        generate_words_with_last_letter_different_dist(words, n, alphabet, choice_func, norm, normal_letter_distr_stddev)
    elif letter_distribution=="increasing_degree":
        generate_node_with_increasing_degree(words, 2*n, alphabet, "", choice_func, norm, normal_letter_distr_stddev, 1, length_avg)
    else:
        generate_node(words, 2*n, alphabet, "", choice_func, norm, normal_letter_distr_stddev)

    click.echo("Splitting words into input and queries")
    random.shuffle(words)

    click.echo("Sorting back words")
    queries_similar = words[int(len(words) / 2):len(words) - 1]
    queries_similar.sort()

    click.echo("Sorting back similar queries")
    words = words[:int(len(words) / 2)]
    words.sort()
    
    click.echo("Generating queries with common prefix")
    generate_common_prefix_queries(words, n, queries_common_prefix, alphabet)

    click.echo("Generating queries with last letter different")
    generate_last_letter_queries(words, n, queries_last_letter_changed, alphabet)

    click.echo("Generate random queries")
    query_norm = scipy.stats.norm(int(length_avg / 2), int(length_sigma/2))
    generate_node(queries_random, n, alphabet, "", uniform_choice, query_norm, normal_letter_distr_stddev)

    click.echo("Generate intervals ends")
    queries_random = [(word, shift_word(word, range_min_size, range_max_size, alphabet)) for word in queries_random]
    queries_similar = [(word, shift_word(word, range_min_size, range_max_size, alphabet)) for word in queries_similar]
    queries_last_letter_changed = [(word, shift_word(word, range_min_size, range_max_size, alphabet)) for word in queries_last_letter_changed]
    queries_common_prefix = [(word, shift_word(word, range_min_size, range_max_size, alphabet)) for word in queries_common_prefix]

    dir = "range_queries_workloads"
    if (output_dir is not None):
        dir += "/" + output_dir

    click.echo("Saving results to file")
    with open('%s/%s_input.txt' % (dir, output), "w") as f:
        f.write("\n".join(words))
    with open('%s/%s_queries_random.txt' % (dir, output), "w") as f:
        f.write("\n".join([";".join(pair) for pair in queries_random]))
    with open('%s/%s_queries_similar.txt' % (dir, output), "w") as f:
        f.write("\n".join([";".join(pair) for pair in queries_similar]))
    with open('%s/%s_queries_last_letter.txt' % (dir, output), "w") as f:
        f.write("\n".join([";".join(pair) for pair in queries_last_letter_changed]))
    with open('%s/%s_queries_common_prefix.txt' % (dir, output), "w") as f:
        f.write("\n".join([";".join(pair) for pair in queries_common_prefix]))

def generate_words_with_last_letter_different_dist(words, n, alphabet, choice_func, norm, normal_letter_distr_stddev):
    base = []
    generate_node(base, int(n/2), alphabet, "", choice_func, norm, normal_letter_distr_stddev)
    for word in base:
        for i in range(4):
            words.append(word + alphabet[i])

def generate_node_with_increasing_degree(words, n, alphabet, prefix, choice_func, norm, normal_letter_distr_stddev, level, avg_length):
    mean = (len(alphabet) - 1) / 2
    shuffled_alphabet = alphabet.copy()
    shuffle(shuffled_alphabet)

    # Check if this is the end of the word
    prob_end = norm.cdf(len(prefix))
    rand = random.uniform(0, 1)
    if rand < prob_end:
        words.append(prefix)
        n -= 1

    next_letters = [choice_func(shuffled_alphabet, mean, normal_letter_distr_stddev * (level/avg_length) * (level / avg_length)) for _ in range(n)]
    # Go with one node on top of the tree
    rand = random.uniform(0,1)
    if rand > level / avg_length:
        next_letters = [choice_func(shuffled_alphabet, mean, normal_letter_distr_stddev * (level/avg_length) * (level / avg_length))] * n
    
    for letter in alphabet:
        cnt = len([1 for lt in next_letters if lt == letter])
        if cnt == 0:
            continue
        new_prefix = prefix + letter
        generate_node_with_increasing_degree(words, cnt, alphabet, new_prefix, choice_func, norm, normal_letter_distr_stddev, level + 1, avg_length)


def generate_node(words, n, alphabet, prefix, choice_func, norm, normal_letter_distr_stddev):
    mean = (len(alphabet) - 1) / 2
    shuffled_alphabet = alphabet.copy()
    shuffle(shuffled_alphabet)

    # Check if this is the end of the word
    prob_end = norm.cdf(len(prefix))
    rand = random.uniform(0, 1)
    if rand < prob_end:
        words.append(prefix)
        n -= 1

    next_letters = [choice_func(shuffled_alphabet, mean, normal_letter_distr_stddev) for l in range(n)]

    for letter in alphabet:
        cnt = len([1 for lt in next_letters if lt == letter])
        if cnt == 0:
            continue
        new_prefix = prefix + letter
        generate_node(words, cnt, alphabet, new_prefix, choice_func, norm, normal_letter_distr_stddev)
        

def normal_choice(lst, mean, stddev):
    while True:
        index = int(normalvariate(mean, stddev) + 0.5)
        if 0 <= index < len(lst):
            return lst[index]

def uniform_choice(lst, param1=None, param2=None):
    return lst[random.randint(0, len(lst))]

def powerlaw_choice(lst, mean, stddev):
    return lst[int(random.power(0.1) * len(lst) + 0.5) - 1]

def generate_common_prefix_queries(words, n, similar_queries, alphabet):
    queries = set()
    for word in words:
        # Choose index to cull the prefix at (higher probability for bigger indices)
        rand1 = random.randint(1, len(word))
        rand2 = random.randint(1, len(word))
        ind = max(rand1, rand2)
        new_word = word[:ind] + random.choice(alphabet)
        cnt = 0
        while new_word in queries and cnt < 10:
            rand1 = random.randint(1, len(word))
            rand2 = random.randint(1, len(word))
            ind = max(rand1, rand2)
            new_word = word[:ind] + random.choice(alphabet)
            cnt += 1
        queries.add(word[:ind] + random.choice(alphabet))
    similar_queries.extend(list(queries))

def generate_last_letter_queries(words, n, similar_queries, alphabet):
    queries = set()
    for word in words:
        ind = len(word) - 1
        new_word = word[:ind] + random.choice(alphabet)
        cnt = 0
        while new_word in queries and cnt < 10:
            new_word = word[:ind] + random.choice(alphabet)
            cnt += 1
        queries.add(word[:ind] + random.choice(alphabet))
    similar_queries.extend(list(queries))

def generate_similar_queries(words, n, similar_queries, alphabet):
    queries = set()
    for word in words:
        # Choose index to cull the prefix at (higher probability for bigger indices)
        rand1 = random.randint(1, len(word))
        rand2 = random.randint(1, len(word))
        ind = max(rand1, rand2)
        new_word = word[:ind] + random.choice(alphabet)
        cnt = 0
        while new_word in queries and cnt < 10:
            rand1 = random.randint(1, len(word))
            rand2 = random.randint(1, len(word))
            ind = max(rand1, rand2)
            new_word = word[:ind] + random.choice(alphabet)
            cnt += 1
        queries.add(word[:ind] + random.choice(alphabet))
    similar_queries.extend(list(queries))

def shift_word(word, n_min, n_max, alphabet):
    n = random.randint(n_min, n_max)
    new_word = [w for w in word]
    i = 1
    while n > 0 and i < len(word) + 1:
        curr_idx = alphabet.index(word[-i])
        new_idx = (curr_idx + n) % len(alphabet)
        n = (curr_idx + n) // len(alphabet)
        new_word[-i] = alphabet[new_idx]
        i += 1
    return "".join(new_word)

if __name__ == '__main__':
    generator()
