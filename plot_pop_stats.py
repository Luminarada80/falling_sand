import pandas as pd
import matplotlib
matplotlib.use('Agg')  # Use a non-interactive backend
import matplotlib.pyplot as plt

# Load the CSV file
data = pd.read_csv("./animal_logs.csv")

# Function to sample 10 rows from each time_step group
def sample_per_time_step(group, n=10):
    if len(group) <= n:
        return group  # If group size is smaller than n, take all
    return group.sample(n=n, random_state=42)

# Apply sampling
sampled_data = data.groupby("generation").apply(sample_per_time_step, n=10).reset_index(drop=True)

# Create subplots
fig, axes = plt.subplots(3, 2, figsize=(12, 10), dpi=100)  # Set DPI to 100 for performance
axes = axes.flatten()  # Flatten the 2D array for easy iteration

# Metrics to plot
metrics = [
    ("Population", "population"),
    ("Movement Chance", "movement_chance"),
    ("Food per Unit Eaten", "food_per_unit_eaten"),
    ("Spawn Delay", "spawn_delay"),
    ("Spawning Threshold", "spawning_threshold"),
]

# Plot each metric
for ax, (label, column) in zip(axes, metrics):
    ax.scatter(sampled_data["generation"], sampled_data[column], label=label, alpha=1, s=10)  # Smaller, less dense points
    ax.set_title(label, fontsize=10)
    ax.set_xlabel("Generation", fontsize=8)
    ax.set_ylabel("Value", fontsize=8)
    ax.tick_params(axis='both', which='major', labelsize=8)
    ax.grid(False)  # Disable grid for faster rendering

# Adjust layout
plt.tight_layout()
plt.savefig('./Population_Stats_Over_Time_Subplots.png', dpi=200)
