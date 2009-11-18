#! /usr/bin/env python

# Check ASBeautifier constructor and copy constructor to class variables 
# in the header file to verify all variables are initialized and copied.

import sys
import os

# global variables ------------------------------------------------------------

header_lines = [0,0]			# line numbers for header
class_lines = [0,0]			# line numbers for class constructor
class_lines_init = [0,0]		# line numbers for class init() function
copy_lines = [0,0]			# line numbers for copy constructor

header_total = 0				# total variables for header
class_total = 0				# total variables for class constructor
class_total_init = 0			# total variables for class init() function
copy_total = 0				# total variables for copy constructor
class_duplicates = 0			# total number of class duplicates removed

# -----------------------------------------------------------------------------

def process_files():
	"""Main processing function."""

	global header_lines, class_lines, class_lines_init , copy_lines
	global header_total, class_total, class_total_init , copy_total, class_duplicates
	header_variables = []			# variables in astyle.h
	class_variables = []				# variables in the class constructor
	copy_variables = []				# variables in the copy constructor
	header_path = get_source_directory() + "/astyle.h"
	beautifier_path = get_source_directory() + "/ASBeautifier.cpp"

	header_variables = get_header_variables(header_path)
	class_variables = get_constructor_variables(beautifier_path)
	class_variables += get_initializer_variables(beautifier_path)
	copy_variables = get_copy_variables(beautifier_path)
	header_variables.sort()
	class_variables.sort()
	copy_variables.sort()
	class_variables = remove_class_duplicates(class_variables)
	
	print "Checking ASBeautifier header to class constructor and copy constructor."
	total_variables = len(header_variables)
	print "There are {0} variables in the header list.".format(total_variables)

	find_class_diffs(header_variables, class_variables)
	find_copy_diffs(header_variables, copy_variables)

	# print "{0} {1} header".format(header_lines, header_total)
	# print "{0} {1} copy constructor".format(copy_lines, copy_total)
	# print "{0} {1} class constructor".format(class_lines, class_total)
	# print "{0} {1} class initializer".format(class_lines_init, class_total_init)
	# print "   -{0} class duplicates".format(class_duplicates)

	# print header_variables
	# print class_variables
	# print copy_variables

# -----------------------------------------------------------------------------

def convert_class_functions(line):
	"""Convert class initializer functions to the corresponding variable."""
	first_paren = line.find('(')
	if first_paren == -1:
		return line
	if line.find("initContainer") != -1:
		line = line[first_paren + 1:]
		first_comma = line.find(',')
		if first_comma != -1:
			line = line[:first_comma]
		line = line.strip()
	elif line.find("->") != -1:
		line = ''
	elif line.find("ASBase::init") != -1:
		line = ''
	elif line.find("initStatic") != -1:
		line = ''
	elif line.find("setSpaceIndentation") != -1:
		line = "indentLength"
	elif line.find("setMaxInStatementIndentLength") != -1:
		line = "maxInStatementIndent"
	elif line.find("setClassIndent") != -1:
		line = "classIndent"
	elif line.find("setSwitchIndent") != -1:
		line = "switchIndent"
	elif line.find("setCaseIndent") != -1:
		line = "caseIndent"
	elif line.find("setBlockIndent") != -1:
		line = "blockIndent"
	elif line.find("setBracketIndent") != -1:
		line = "bracketIndent"
	elif line.find("setNamespaceIndent") != -1:
		line = "namespaceIndent"
	elif line.find("setLabelIndent") != -1:
		line = "labelIndent"
	elif line.find("setEmptyLineFill") != -1:
		line = "emptyLineFill"
	elif line.find("setCStyle") != -1:
		line = "fileType"
	elif line.find("setPreprocessorIndent") != -1:
		line = "preprocessorIndent"
	else:
		line = "unidentified function: " + line
	return line

# -----------------------------------------------------------------------------

def find_class_diffs(header_variables, class_variables):
	"""Find differences in header and class variables lists."""
	diffs = 0
	i = 0
	j = 0

	# print "Checking ASBeautifier header variables to class constructor."
	while i < len(header_variables):
		if ( j >= len(class_variables)
		or header_variables[i] < class_variables[j]):
			print "missing class: " + header_variables[i]
			diffs += 1
			i += 1
			continue
		if header_variables[i] > class_variables[j]:
			print "missing header: " + class_variables[j]
			diffs += 1
			j +=1
			continue
		i += 1
		j += 1

	# get extra class_variables
	while j < len(class_variables):
		print "missing header: " + class_variables[j]
		diffs += 1
		j += 1
		continue
	if diffs == 0:
		print "There are NO diffs in the class constructor variables!!!"
	else:
		print "There are {0} diffs in the class constructor variables.".format(diffs)

# -----------------------------------------------------------------------------

def find_copy_diffs(header_variables, copy_variables):
	"""Find differences in header and copy variables lists."""
	diffs = 0
	i = 0
	j = 0

	# print "Checking ASBeautifier header variables to copy constructor."
	while i < len(header_variables):
		if ( j >= len(copy_variables)
		or header_variables[i] < copy_variables[j]):
			print "missing copy: " + header_variables[i]
			diffs += 1
			i += 1
			continue
		if header_variables[i] > copy_variables[j]:
			print "missing header: " + copy_variables[j]
			diffs += 1
			j +=1
			continue
		i += 1
		j += 1

	# get extra copy_variables
	while j < len(copy_variables):
		print "missing header: " + copy_variables[j]
		diffs += 1
		j += 1
		continue
	if diffs == 0:
		print "There are NO diffs in the copy constructor variables!!!"
	else:
		print "There are {0} diffs in the copy constructor variables.".format(diffs)

# -----------------------------------------------------------------------------

def get_constructor_variables(beautifier_path):
	"""Read the ASBeautifier file and save the class constuctor variables."""

	global class_lines, class_total
	variables = []
	lines = 0
	file_in = open(beautifier_path, 'r')

	# get class constructor lines
	for line_in in file_in:
		lines += 1
		line = line_in.strip()
		if len(line) == 0:
			continue
		if line[:2] == "//":
			continue
		# start between the following lines
		if line.find("ASBeautifier::ASBeautifier()") != -1:
			class_lines[0] = lines + 1
			continue
		if (class_lines[0]  == 0
		or class_lines[0]  == lines):
			continue
		# find ending bracket
		if line.find('}') != -1:
			class_lines[1] = lines
			break
		# get the variable name
		variable_name = line
		if line.find('(') != -1:
			variable_name = convert_class_functions(line)
		else:
			first_space = line.find(' ')
			if first_space != -1:
				variable_name = line[0:first_space].strip()
		if len(variable_name) == 0:
			continue
		variables.append(variable_name)
		if variable_name == "indentLength":
			variables.append("indentString")
			variables.append("minConditionalIndent")

	file_in.close()
	class_total = len(variables)
	return variables

# -----------------------------------------------------------------------------

def get_copy_variables(beautifier_path):
	"""Read the ASBeautifier file and save the copy constuctor variables."""

	global copy_lines, copy_total
	variables = []
	file_in = open(beautifier_path, 'r')
	lines = 0

	for line_in in file_in:
		lines += 1
		line = line_in.strip()
		if len(line) == 0:
			continue
		if line[:2] == "//":
			continue

		# start between the following lines
		if line.find("ASBeautifier(const ASBeautifier &other)") != -1:
			copy_lines[0] = lines + 1
			continue
		if (copy_lines[0]  == 0
		or copy_lines[0]  == lines):
			continue
		# find ending bracket
		if line.find('}') != -1:
			copy_lines[1] = lines
			break
		# get the variable name
		first_space = line.find(' ')
		if first_space == -1:
			continue
		variable_name = line[:first_space].strip()
		# value-of variables are duplicates
		if variable_name[0] == '*':
			continue
		variables.append(variable_name)

	file_in.close()
	copy_total = len(variables)
	return variables

# -----------------------------------------------------------------------------

def get_header_variables(header_path):
	"""Read the header file and save the ASBeautifier variables."""

	global header_lines, header_total
	variables = []
	file_in = open(header_path, 'r')
	lines = 0

	for line_in in file_in:
		lines += 1
		line = line_in.strip()
		if len(line) == 0:
			continue
		if line[:2] == "//":
			continue

		# start between the following lines
		if line.find("class ASBeautifier") != -1:
			header_lines[0] = lines + 1
			continue
		if (header_lines[0]  == 0
		or header_lines[0]  == lines):
			continue
		# find ending bracket
		if line.find('}') != -1:
			header_lines[1] = lines
			break
		if (line.find("public:") != -1
		or line.find("private:") != -1
		or line.find("protected:") != -1):
			continue
		# bypass functions
		if (line.find('(') != -1
		or line.find(')') != -1):
			continue
		# bypass static variables
		if line[:6] == "static":
			continue
		# get the variable name
		last_space = line.rfind(' ')
		if last_space == -1:
			continue
		variable_name = line[last_space:-1].strip()
		if variable_name[0] == '*':
			variable_name = variable_name[1:]
		variables.append(variable_name)

	file_in.close()
	header_total = len(variables)
	return variables

# -----------------------------------------------------------------------------

def get_initializer_variables(beautifier_path):
	"""Read the ASBeautifier file and save the class initializer variables."""

	global class_lines_init, class_total_init
	variables = []
	lines_init = 0
	file_in_init = open(beautifier_path, 'r')

	# get class initializer lines
	for line_in in file_in_init:
		lines_init += 1
		line = line_in.strip()
		if len(line) == 0:
			continue
		if line[:2] == "//":
			continue
		# start between the following lines
		if line.find("void ASBeautifier::init()") != -1:
			class_lines_init[0] = lines_init + 1
			continue
		if (class_lines_init[0]  == 0
		or class_lines_init[0]  == lines_init):
			continue
		# find ending bracket
		if line.find('}') != -1:
			class_lines_init[1] = lines_init
			break
		# get the variable name
		variable_name = line
		if line.find('(') != -1:
			variable_name = convert_class_functions(line)
		else:
			first_space = line.find(' ')
			if first_space != -1:
				variable_name = line[:first_space].strip()
		if len(variable_name) == 0:
			continue
		variables.append(variable_name)

	file_in_init.close()
	class_total_init = len(variables)
	return variables

# -----------------------------------------------------------------------------

def get_source_directory():
	"""Get the AStyle/src directory for the os environment"""
	
	if os.name == "nt":
		return os.getenv("USERPROFILE") + "/Projects/AStyle/src"
	else:
		return os.getenv("HOME") + "/Projects/AStyle/src"
	
# -----------------------------------------------------------------------------

def remove_class_duplicates(class_variables):
	"""Remove duplicates in class variables list."""
	global class_duplicates
	i = 1
	while i < len(class_variables):
		if class_variables[i - 1] == class_variables[i]:
			class_variables.remove(class_variables[i])
			class_duplicates += 1
			continue
		i +=1
	return class_variables

# -----------------------------------------------------------------------------

# make the module executable
if __name__ == "__main__":
	process_files()
	# pause if script is not run from SciTE (argv[1] = 'scite')
	if len(sys.argv) == 1:
		raw_input("\nPress Enter to continue . . .")

# -----------------------------------------------------------------------------