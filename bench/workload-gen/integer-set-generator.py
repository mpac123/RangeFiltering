import click
import numpy as np
import random

@click.command()
@click.option('--n', type=int, help="Number of elements to be generated", required=True)
@click.option('--u', type=int, help="Size of the universe", required=True)
@click.option('--output-dir', help="Name of the directory to save the output files")
def generator(n, u, output_dir):
    
    dir = "integer_workloads"
    if (output_dir is not None):
        dir += "/" + output_dir

    click.echo("Generating words with uniform distribution")
    elements = sorted([int(e) for e in np.random.uniform(0, u, n)])
    with open('%s/uniform_%d.txt' % (dir, n), "w") as f:
        f.write("\n".join([str(e) for e in elements]))

    click.echo("Generating words with multiple-uniform distribution")
    cnt = 0
    elements = []
    while cnt < n:
        new_elements_size = random.randint(0, n - cnt)
        cnt += new_elements_size
        new_elements = sorted([int(e) for e in np.random.uniform(0, u, new_elements_size)])
        elements = merge_sorted_arrays(elements, new_elements, u)
        print(len(new_elements), len(elements))
    with open('%s/multiple_uniform_%d.txt' % (dir, n), "w") as f:
        f.write("\n".join([str(e) for e in elements]))

    click.echo("Generating words with normal distribution")
    gen = sorted(np.random.normal(0, 0.15, n))
    elements = [int((e - gen[0]) * (u / (gen[-1] - gen[0]))) for e in gen]
    with open('%s/normal_%d.txt' % (dir, n), "w") as f:
        f.write("\n".join([str(e) for e in elements]))

    click.echo("Generating words with multiple normal distribution")
    elements = []
    while len(elements) < n:
        new_elements_size = random.randint(0, n - len(elements))
        cnt += new_elements_size
        shift = random.uniform(0.4, 0.6)
        new_elements = sorted([int(e*u) for e in np.random.normal(shift, 0.1, new_elements_size)])
        elements = merge_sorted_arrays(elements, new_elements, u)
        cnt = len(elements)
    with open('%s/multiple_normal_%d.txt' % (dir, n), "w") as f:
        f.write("\n".join([str(e) for e in elements]))

def merge_sorted_arrays(arr1, arr2, u):
    arr = []
    i = 0
    j = 0
    while i < len(arr1) and j < len(arr2):
        if arr1[i] < arr2[j]:
            if arr1[i] >= 0 and arr1[i] < u:
                arr.append(arr1[i])
            i += 1
        else:
            if arr2[j] >= 0 and arr2[j] < u:
                arr.append(arr2[j])
            j += 1
    while i < len(arr1):
        if arr1[i] >= 0 and arr1[i] < u:
            arr.append(arr1[i])
        i += 1
    while j < len(arr2):
        if arr2[j] >= 0 and arr2[j] < u:
            arr.append(arr2[j])
        j += 1
    return arr

if __name__ == '__main__':
    generator()