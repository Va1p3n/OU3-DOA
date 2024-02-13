import re
import subprocess
from datetime import date

def update_hand_in_date(file_path):
	file_path = f"{file_path}.c"

    # Get today's date
	today = date.today().strftime("%Y-%m-%d")
    
    # Open the file
	with open(file_path, 'r') as file:
		content = file.read()

    # Use regular expression to find and replace the hand in date
	updated_content = re.sub(r'(Hand in date:\s*)(\d{4}-\d{2}-\d{2})', f'Hand in date: {today}', content)

    # Write back the updated content to the file
	with open(file_path, 'w') as file:
		file.write(updated_content)

	print("Hand in date updated successfully.")
    
def compile_file(file_name):
	compile_command = f"gcc -std=c99 -Wall -Werror -g -I lib -o {file_name}test tabletest-1.9.c {file_name}.c"

	if file_name == "mtftable" or file_name == "table":
		compile_command += " lib/dlist.c"
    
	try:
		subprocess.run(compile_command, shell=True, check=True)
		print(f"Compiled {file_name} sucessfully..")
	except subprocess.CalledProcessError as e:
		print(f"Error compiling {file_name}: {e}")
         

def mem_leak_test():
	pass


# Specify the file path here
files = ["mtftable", "arraytable", "table"]
update_hand_in_date(files[0])
compile_file(files[0])
compile_file(files[2])
