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
                type=click.Choice(["uniform", "normal", "zipfian"]), 
                help='Distribution with which node`s child letters will be generated', 
                required=True)
@click.option('--normal_letter_distr_stddev',default=1, help='If letter-distribution=normal, set stddev to this value')
@click.option('--output', help="Name of the output file")
def generator(n, alph_size, alph_start, length_avg, length_sigma, letter_distribution, normal_letter_distr_stddev, output):
    """Generate workload for filter and range filtering benchmarking"""
    # First, check if it is possible to generate n words with given alphabet size and averge word length
    if (pow(alph_size, length_avg) < n):
        click.echo("Provided n is too big to generate words from given alphabet and with given average word length")
        return

    words = []
    queries_random = []
    queries_similar = []
    queries_last_letter_changed = []
    norm = scipy.stats.norm(length_avg, length_sigma)
    
    alphabet = [chr((ord(alph_start) + i) % 256) for i in range(alph_size)]
    choice_func = uniform_choice if letter_distribution == "uniform" else normal_choice

    click.echo("Generating words")
    generate_node(words, 2*n, alphabet, "", choice_func, norm, normal_letter_distr_stddev)

    click.echo("Splitting words into input and queries")
    random.shuffle(words)

    click.echo("Sorting back words")
    queries_similar = words[int(len(words) / 2):len(words) - 1]
    queries_similar.sort()

    click.echo("Sorting back similar queries")
    words = words[:int(len(words) / 2)]
    words.sort()
    
    click.echo("Generating queries with last letter different")
    generate_similar_queries(words, n, queries_last_letter_changed, alphabet)

    click.echo("Generate random queries")
    query_norm = scipy.stats.norm(int(length_avg / 2), int(length_sigma/2))
    generate_node(queries_random, n, alphabet, "", uniform_choice, query_norm, normal_letter_distr_stddev)

    click.echo("Saving results to file")
    with open('workloads/%s_input.txt' % output, "w") as f:
        f.write("\n".join(words))
    with open('workloads/%s_queries_random.txt' % output, "w") as f:
        f.write("\n".join(queries_random))
    with open('workloads/%s_queries_similar.txt' % output, "w") as f:
        f.write("\n".join(queries_similar))
    with open('workloads/%s_queries_last_letter.txt' % output, "w") as f:
        f.write("\n".join(queries_last_letter_changed))

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

def generate_similar_queries(words, n, similar_queries, alphabet):
    queries = set()
    while len(queries) < n:
        word = random.choice(words)
        # Choose index to cull the prefix at (higher probability for bigger indices)
        rand1 = random.randint(1, len(word))
        rand2 = random.randint(1, len(word))
        ind = max(rand1, rand2)
        queries.add(word[:ind] + random.choice(alphabet))
    similar_queries.extend(list(queries))


if __name__ == '__main__':
    generator()
