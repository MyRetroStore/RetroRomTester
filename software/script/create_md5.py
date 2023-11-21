#!/usr/bin/python3

#Python script to generate md5 hashes for ROM binary files used by the Arduino based Retro ROM Tester

folder_path="ROMs/"

def search_and_insert(search_word,insert_word, marker):
	try:
        	with open("crc.h", 'r') as file:
                	lines = file.readlines()
	except FileNotFoundError:
		print(f"Error: File crc.h not found.")
		quit()
	except Exception as e:
		print(f"Error: Unable to open file crc.h. Reason: {e}")
		quit

	found = False
	for i, line in enumerate(lines):
		if search_word in line:
			print ("Found existing md5 hash in crc.h file. No need to update")
			found = True
			break

	if not found:
		for i, line in enumerate(lines):
			if marker in line:
				lines.insert(i, insert_word + '\n')
				break

		with open("crc.h", 'w') as file:
			print ("Hash added to crc.h file.")
			file.writelines(lines)

def calculate_md5(file_path, chunk_size):
    md5_first_chunk = hashlib.md5()
    md5_second_chunk = hashlib.md5()

    try:
        with open(file_path, "rb") as file:
            data_chunk = file.read(chunk_size)
            md5_first_chunk.update(data_chunk)

            if size > 4096:
                remaining_data = file.read()

                md5_second_chunk.update(remaining_data)

    except FileNotFoundError:
        print(f"Error: File not found - {file_path}")
        return None, None

    return md5_first_chunk.hexdigest(), md5_second_chunk.hexdigest()

if __name__ == "__main__":
	import sys
	import argparse
	import os
	import hashlib

	valid_file_sizes = [2048, 4096, 8192]	#File size of the ROM must be one of these

	parser = argparse.ArgumentParser(description="A Python script to generate ROM file md5's used with Reto ROM Tester")
	parser.add_argument('-i', '--input', help="Input file")
	args = parser.parse_args()

	input_file = args.input

	if not len(sys.argv) > 1:
		print ("No arguments passed.")
		print ("Try running create_md5.py -h")
		quit()

	print ("Calculating md5 hash for " + input_file)

	if not os.path.isfile(folder_path + input_file):
		print (f"Error: File '{input_file}' does not exist!")
		quit()

	size = os.path.getsize(folder_path + input_file)

	if size not in valid_file_sizes:
		print ("Error: File size needs to be 2048, 4096 or 8192 bytes")
		quit()

	if size > 2048:
		md5_first_chunk, md5_second_chunk = calculate_md5(folder_path + input_file, 4096)

	if size == 2048:
		md5_first_chunk, md5_second_chunk = calculate_md5(folder_path + input_file,2048)

	if input_file.endswith(".bin"):
		input_file = input_file[:-4]
		input_file = input_file.replace('-', '_').replace('.', '_')

	if size==8192:
		search_and_insert(md5_first_chunk, '  {"' + input_file + '", "' + md5_first_chunk + '", "' + md5_second_chunk + '"},','End of 8k marker')
	if size<8192:
		search_and_insert(md5_first_chunk,'  {"' + input_file + '", "' + md5_first_chunk + '"},','End of 4k marker')
