import pandas as pd
from math import log10
import matplotlib.pyplot as plt

df = pd.read_csv("data.tsv", sep='\t')
print(df.head())
# print(df.describe())

print(f"\nTotal Keys: {len(df['numVotes'])}")
print(f"Unique Keys: {len(set(df['numVotes']))}\n")
# Checking the number of unique keys (numVotes)

data = df.groupby('numVotes').size()
print(data.head())

data = data.apply(lambda x: log10(x))
print(data.head())

# visualize and save
ax = data.plot.hist()
ax.set_xlabel("numVotes (log scaled)")
ax.set_ylabel("frequency (log scaled)")
plt.savefig("frequency.png")
plt.show()
