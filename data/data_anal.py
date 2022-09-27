import pandas as pd
from math import log10
import matplotlib.pyplot as plt

df = pd.read_csv("data.tsv", sep='\t')
print(df.head())
# print(df.describe())

# Checking the number of unique keys (numVotes)
print(f"\nTotal Keys: {len(df['numVotes'])}")
print(f"Unique Keys: {len(set(df['numVotes']))}\n")

# Aggregating on numVotes (to check it's distribution)
data = df.groupby('numVotes').size()
print(data.head())

# Log Scaling the values
data = data.apply(lambda x: log10(x))
print(data.head())

# Visualize and save plot
ax = data.plot.hist()
ax.set_xlabel("numVotes (log scaled)")
ax.set_ylabel("frequency (log scaled)")
plt.savefig("frequency.png")
plt.show()
