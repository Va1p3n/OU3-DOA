import re
import sys
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
    elif file_name == "arraytable":
         compile_command += " lib/array_1d.c"

    try:
        subprocess.run(compile_command, shell=True, check=True)
        print(f"Compiled {file_name} sucessfully..")
    except subprocess.CalledProcessError as e:
        print(f"Error compiling {file_name}: {e}")
        return 1
 
    return 0


def mem_leak_test(file_name): 
    mem_test_command = f"valgrind --leak-check=full --show-reachable=yes ./{file_name}test 10"

    with open('/dev/null', 'w') as devnull:
        try:
            subprocess.run(mem_test_command, shell=True, stdout=devnull, stderr=subprocess.STDOUT, check=True)
            print(f"No memory-leaks in {file_name}.")
        except subprocess.CalledProcessError:
            print(f"Memory-leaks in {file_name}.")
            return 1

    return 0


# Specify the file path here
files = ["mtftable", "arraytable", "table"]
comp_err = 0
update_hand_in_date(files[0])
update_hand_in_date(files[1])

for file in files:
    comp_err += compile_file(file)

if comp_err > 0:
    quit()
    
if "--no-mem" not in sys.argv:
	for file in files:
		mem_leak_test(file)
