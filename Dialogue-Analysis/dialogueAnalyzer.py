import pandas as pd
import matplotlib.pyplot as plt
import math
import numpy as np

# Finds the RMSE between a current set of dialogue and the ideal target for dialogue within a narrative piece
def find_RMS(keys, ideal, actual, printer=False):
    sum = 0
    ideal_total = 0
    dialogue_total = 0

    # Find the total 
    for key in keys:
        ideal_total += ideal.get(key)
        dialogue_total += actual.get(key)

    ideal_ratio = []
    actual_ratio = []

    # Append lists with proportional totals
    if (printer):
        print("Proportions are:\nCategory\t\tIdeal\tActual") 
    
    for key in keys:
        idc = (ideal.get(key)/ideal_total)*100
        adc = (actual.get(key)/dialogue_total)*100
        ideal_ratio.append(idc)
        actual_ratio.append(adc)

        # Print value differences between targets and actual if requested
        if (printer):
            print(f"{key}: \t\t{idc:.2f} \t{adc:.2f}" if key != "Direct Opinion" else f"{key}: \t{idc:.2f} \t{adc:.2f}")
            

    # Sum the totals and return the RMSE
    for i in range(len(ideal_ratio)):
        fig = actual_ratio[i] - ideal_ratio[i]
        sum += math.pow(fig, 2)
    
    return np.sqrt(sum)/6.0

# Ideal Counts based on average analysis of games (and some arbitrary rounding):
ideal_counts = {
    "Expository": 10,
    "Reactionary": 30,
    "Introspective": 30,
    "Direct Opinion": 10,
    "Objective": 5,
    "Player Choice": 10,
    "Filler": 5
}

# Filepath of dialogue data
csv_file = r"C:\Users\Justen Webster\OneDrive\Documents\Python Scripts\Dialogue Data\DE Titus to Klaasje.csv"

# CSVs: "Natlan AQ3/4.csv", "5.1 Event 2.csv", "Springs TQ1.csv", "Fontaine AQ3.csv"
# "DE Titus to Klaasje.csv", "Va-11 Hall-A Stella 2.csv"

# Read CSV into panda dataframe ignoring non utf-8 chars
data = pd.read_csv(csv_file, encoding='utf-8', encoding_errors='ignore')

#  Find the proportion of types of dialogue in terms of percentages
dialogue_type_counts = data['Dialogue Type'].value_counts(normalize=True) * 100

# Add all categories to dialogue_type_counts and set them to 0 if not instantiated
for category in ideal_counts.keys():
    if category not in dialogue_type_counts:
        dialogue_type_counts[category] = 0

# Calculate the proportion of each dialogue type per character
character_type_proportions = (
    data.groupby('Character')['Dialogue Type']
        .value_counts(normalize=True)
        .unstack(fill_value=0) * 100
).sort_index()

# Add any missing dialogue types with a proportion of 0
for dialogue_type in ideal_counts.keys():
    if dialogue_type not in character_type_proportions.columns:
        character_type_proportions[dialogue_type] = 0


# Compute RMS values and add as a new column to the DataFrame
character_type_proportions['RMS'] = character_type_proportions.apply(
    lambda row: find_RMS(ideal_counts.keys(), ideal_counts, row),
    axis=1
)

line_counts = data['Character'].value_counts()
character_type_proportions['Count'] = character_type_proportions.index.map(line_counts)

# Sort by the RMS column
character_type_proportions_sorted = character_type_proportions.sort_values(by='RMS')

print("\nCharacter dialogue proportions ranked by best to worst RMS:")
print(character_type_proportions_sorted)

# Define a colour palette for each dialogue type
colour_map = {
    "Direct Opinion": "#ff8080",   # Pale red
    "Expository": "#ffd480",       # Pale Orange
    "Reactionary": "#fffd80",      # Pale Yellow
    "Introspective": "#80ff9e",    # Pale green
    "Objective": "#80e1ff",        # Sky blue
    "Player Choice": "#8088ff",    # Darker blue
    "Filler": "#9c9c9c"            # Grey
}

# Create the Series for ideal counts
ideal_counts_series = pd.Series(ideal_counts)

# Reorder actual counts based on the colour_map keys
ordered_keys = list(colour_map.keys())
actual_counts_ordered = dialogue_type_counts.reindex(ordered_keys, fill_value=0)
ideal_counts_series = ideal_counts_series.reindex(ordered_keys, fill_value=0)

# Remove entries with values less than or equal to 0
actual_counts_filtered = actual_counts_ordered[actual_counts_ordered > 0]
ideal_counts_filtered = ideal_counts_series[ideal_counts_series > 0]

# Assign colours
actual_colours = [colour_map[key] for key in ordered_keys if key in actual_counts_filtered.index]
ideal_colours = [colour_map[key] for key in ideal_counts_filtered.index]

# Set up pie chart formatting
fig, axs = plt.subplots(1, 2, figsize=(12, 6))

# Actual distribution setup
axs[0].pie(actual_counts_filtered, labels=actual_counts_filtered.index, autopct='%1.1f%%', startangle=90, colors=actual_colours)
axs[0].set_title('DE Titus to Klaasje')

# Ideal distribution setup
axs[1].pie(ideal_counts_filtered, labels=ideal_counts_filtered.index, autopct='%1.1f%%', startangle=90, colors=ideal_colours)
axs[1].set_title('Ideal Dialogue Distribution')
axs[0].text(-1, -1.3, 'Lines of dialogue = ' + str(int(data.size/4)), fontsize=12, bbox=dict(facecolor='white', alpha=0.5))

# Show the plots
plt.tight_layout()  # Adjust layout to prevent clipping
plt.show()

# Print out RMS value of overall quest
print("\nRMS = " + str(find_RMS(ordered_keys, ideal_counts, dialogue_type_counts, printer=True)))