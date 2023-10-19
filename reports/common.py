import pathlib
import numpy as np
import matplotlib.pyplot as plt


def read_instance(path: str) -> np.ndarray[int]:
    with open(path, "r") as f:
        text = f.read()

    instance = [
        [int(cell) for cell in line.split(";") if cell]
        for line in text.split("\n")
        if line
    ]
    return np.array(instance)


def read_solution(path: str) -> tuple[np.ndarray[int], int]:
    with open(path, "r") as f:
        text = f.read()

    solution = [int(num.strip()) for num in text.split("\n") if num]
    return np.array(solution[:-1]), solution[-1]


def plot_solution_for_instance(
    instance: np.ndarray[int], solution: np.ndarray[int], ax: plt.Axes
) -> None:
    solution = np.append(solution, solution[0])
    ax.plot(instance[solution, 0], instance[solution, 1], c="r")
    ax.scatter(instance[:, 0], instance[:, 1], s=instance[:, 2] / 20, c=instance[:, 2])


def parse_all_results(path: pathlib.Path) -> tuple[dict, dict, dict]:
    solution_files = [path for path in pathlib.Path(path).iterdir() if path.is_file()]

    all_results = {}
    all_costs = {}
    all_stats = {}

    for file in solution_files:
        file_name = file.name.removesuffix(".txt")
        instance, solver, start_idx = file_name.split("-")
        start_idx = int(start_idx)
        solution, cost = read_solution(file)
        all_results[instance] = all_results.get(instance, {})
        all_costs[instance] = all_costs.get(instance, {})
        all_stats[instance] = all_stats.get(instance, {})

        all_results[instance][solver] = all_results[instance].get(solver, []) + [
            solution
        ]
        all_costs[instance][solver] = all_costs[instance].get(solver, []) + [cost]

    for instance in all_results:
        for solver in all_results[instance]:
            all_results[instance][solver] = np.array(all_results[instance][solver])

    for instance in all_results:
        for solver in all_results[instance]:
            all_stats[instance][solver] = {
                "mean": np.mean(all_costs[instance][solver]),
                "std": np.std(all_costs[instance][solver]),
                "min": np.min(all_costs[instance][solver]),
                "max": np.max(all_costs[instance][solver]),
            }

    return all_results, all_costs, all_stats
