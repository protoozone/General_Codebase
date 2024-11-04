import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import csv
import os
import pytesseract
from PIL import ImageGrab
import cv2
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# Feel free to place your own filepaths
filePath = r"Dialogue Data"
fileDir = r"Dialogue Data\dialogue.csv"

# Initial drop-down menu data
characters = ["Narrator"]
dialogue_types = ["Expository", "Reactionary", "Introspective", "Direct Opinion", "Objective", "Player Choice", "Filler"]  # Dialogue Categories

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

ordered_keys = list(colour_map.keys())
actual_colours = [colour_map[key] for key in ordered_keys]

# Initialise dialogue type counts
dialogue_type_count = {dialogue_type: 0 for dialogue_type in dialogue_types}

# pie chart global var
pie_canvas = None

# Dictionary for remapping dialogue types to easier to understand category names
dialogue_mapping = {
    "Expository": "Exposition",
    "Reactionary": "Reactions",
    "Introspective": "Character Building",
    "Direct Opinion": "Unexplained Opinion",
    "Objective": "Developer Speak",
    "Player Choice": "Dialogue Option",
    "Filler": "Removeable"
}

# Writes entries to CSV file
def write_to_csv(entry_id, character, dialogue_type, text):
    with open(filePath, mode='a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([entry_id, character, dialogue_type, text])

# Updates the live tracker pie chart based on new distribution
def update_pie_chart():
    global dialogue_type_count

    # Clear the previous plot
    ax.clear()

    # Remove entries with a value of 0 from dialogue_type_count
    filtered_count = {key: value for key, value in dialogue_type_count.items() if value > 0}

    # Create labels for the pie chart based on dictionary
    labels = [dialogue_mapping[key] for key in filtered_count.keys()]
    sizes = filtered_count.values()

    # Adjust the label distance for graphical niceness
    ax.pie(sizes, labels=labels, autopct='%1.1f%%', startangle=90, colors=actual_colours, labeldistance=0.8)
    ax.axis('equal') 

    # Redraw the canvas with updated chart
    pie_canvas.draw()  

# Submits the dialogue form
def submit_dialogue():
    global entry_counter
    character = character_var.get()
    dialogue_type = dialogue_type_var.get()

    # Get text input
    text = dialogue_text.get("1.0", tk.END).strip()  

    if character and dialogue_type and text:
        write_to_csv(entry_counter, character, dialogue_type, text)
        entry_counter += 1

        # Update the dialogue type count
        dialogue_type_count[dialogue_type] += 1

        # Update the pie chart with new data
        update_pie_chart()

        # Clear text field after submission
        dialogue_text.delete("1.0", tk.END)  

# Adds a new character to the dropdown list
def add_character():
    new_character = new_character_var.get().strip()
    if new_character and new_character not in characters:
        characters.append(new_character)  # Add the new character to the list
        character_dropdown['values'] = characters  # Update the dropdown list
        new_character_var.set('')  # Clear the input field after adding

# Loads existing data from the CSV file
def load_existing_data():
    global entry_counter
    if os.path.exists(filePath):
        with open(filePath, mode='r') as file:
            reader = csv.reader(file)
            next(reader)  # Skip the header row
            existing_data = list(reader)
            if existing_data:
                entry_counter = len(existing_data) + 1  # Set the ID to one more than the last entry
                # Update character dropdown with existing characters
                existing_characters = {row[1] for row in existing_data}  # Use a set for unique characters
                for char in existing_characters:
                    if char not in characters:
                        characters.append(char)
                character_dropdown['values'] = characters  # Update the dropdown list with unique characters

                # Load existing dialogue type counts for the pie chart
                for row in existing_data:
                    dialogue_type_count[row[2]] += 1
                update_pie_chart()  # Show the pie chart with existing data

# Captures a screenshot and extract text
def capture_and_extract():
    #Genshin Impact: 521, 830, 1885, 924
    #Disco Elysium: 1420, 568, 1776, 703
    x1, y1, x2, y2 = 1420, 200, 1776, 703  # (adjust as needed)
    img = ImageGrab.grab(bbox=(x1, y1, x2, y2))

    # Convert to numpy array for processing
    img_np = np.array(img)  

    # Convert the image to grayscale
    gray = cv2.cvtColor(img_np, cv2.COLOR_BGR2GRAY)

    # Create a binary mask where white text will be isolated
    lower_bound = np.array([180, 180, 180])  # Minimum white hex
    upper_bound = np.array([255, 255, 255])  # Maximum white hex

    mask = cv2.inRange(img_np, lower_bound, upper_bound)
    mask_inv = cv2.bitwise_not(mask)

    isolated_text = cv2.bitwise_and(img_np, img_np, mask=mask)
    isolated_gray = cv2.cvtColor(isolated_text, cv2.COLOR_BGR2GRAY)

    # Extract the text with tesseract
    text = pytesseract.image_to_string(isolated_gray, lang='eng', config='--psm 6')
    
    # Clear the previous text
    dialogue_text.delete("1.0", tk.END)  

    # Insert the extracted text
    dialogue_text.insert(tk.END, text.strip())  

    # Reset the Tinker box
    root.deiconify()  

# Initialise main window
root = tk.Tk()
root.title("Dialogue Categoriser")
root.geometry("400x650")

# Counter for primary entry key (starting from 1)
entry_counter = 1

# Label and text entry for dialogue text
tk.Label(root, text="Dialogue Text:").pack(pady=5)
dialogue_text = tk.Text(root, height=4, width=40)
dialogue_text.pack(pady=5)

# Drop-down for character selection
tk.Label(root, text="Select Character:").pack(pady=5)
character_var = tk.StringVar()
character_dropdown = ttk.Combobox(root, textvariable=character_var, values=characters, state="readonly")
character_dropdown.pack(pady=5)

# Drop-down for dialogue type selection
tk.Label(root, text="Select Dialogue Type:").pack(pady=5)
dialogue_type_var = tk.StringVar()
dialogue_type_dropdown = ttk.Combobox(root, textvariable=dialogue_type_var, values=dialogue_types, state="readonly")
dialogue_type_dropdown.pack(pady=5)

# New character input field
tk.Label(root, text="Add New Character:").pack(pady=5)
new_character_var = tk.StringVar()
new_character_entry = tk.Entry(root, textvariable=new_character_var)
new_character_entry.pack(pady=5)

# Button to add the new character to the dropdown
add_character_button = tk.Button(root, text="Add Character", command=add_character)
add_character_button.pack(pady=5)

# Button to capture screenshot and extract text
capture_button = tk.Button(root, text="Capture and Extract", command=capture_and_extract)
capture_button.pack(pady=10)

# Submit button
submit_button = tk.Button(root, text="Submit", command=submit_dialogue)
submit_button.pack(pady=10)

# Pie chart section
tk.Label(root, text="Dialogue Type Distribution:").pack(pady=5)

# Create a figure and axis for the pie chart
fig, ax = plt.subplots()

# Embed the pie chart in the Tkinter window
pie_canvas = FigureCanvasTkAgg(fig, master=root)
pie_canvas.get_tk_widget().pack()

# Ensure the CSV file has headers if starting fresh
os.makedirs(fileDir, exist_ok=True)  # Ensure the directory exists
if not os.path.exists(filePath):  # Only write headers if the file doesn't exist
    with open(filePath, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["ID", "Character", "Dialogue Type", "Text"])

# Load existing data from the CSV file
load_existing_data()

# Run the application
root.mainloop()
