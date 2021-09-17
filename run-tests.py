#!/usr/bin/python3

import os
import threading
import time
import subprocess
import sys

MODE_GRADING = True
TIMEOUT = 20 # terminate test after 30 seconds
KILL_STATUS = -9

def CheckForTimeout(process):

	# Check if process is still alive every 10 ms
	small_wait = 0.01
	t = 0
	while process.poll() is None and t < TIMEOUT:
		time.sleep(small_wait)
		t += small_wait

	# Terminate if runtime exceeded max allowance
	if (t >= TIMEOUT):
		process.kill()

def LaunchProcess(timeout, cmd, stdout_arg=None, stderr_arg=None):
	
	# Launch process with appropriate STDOUT and STDERR redirection
	process = subprocess.Popen(cmd.split(), stdout=stdout_arg, stderr=stderr_arg)
	
	# Terminate process if it takes too long, something is wrong
	CheckForTimeout(process)

	return process

def RunExternalCommand(timeout, cmd, stdout_name=None, stderr_name=None):
	if (stdout_name == None and stderr_name == None):
		process = LaunchProcess(timeout, cmd, subprocess.PIPE, subprocess.PIPE)
		output, error = process.communicate()
	elif (stderr_name == None):
		fd_out = open(stdout_name, "w")
		process = LaunchProcess(timeout, cmd, fd_out)
		output, error = process.communicate()
		fd_out.close()
	else:
		fd_out = open(stdout_name, "w")
		fd_err = open(stderr_name, "w")
		process = LaunchProcess(timeout, cmd, fd_out, fd_err)
		output, error = process.communicate()
		fd_out.close()
		fd_err.close()

	ret = process.returncode

	# Program execution takes too long, something is wrong

	return output, error, ret

def IsFile(f):
	return os.path.isfile(f)

def CheckDiff(expected, recovered, diff):

	# Outcome: 0 same, -1 different
	outcome = 0

	# set file type for pretty print
	if ".fout" in expected:
		ftype = "redirected"
	elif ".out" in expected:
		ftype = "STDOUT"
	else:
		ftype = "STDERR"

	# compute file deltas
	output, error, ret = RunExternalCommand(TIMEOUT, "diff "+expected+" "+recovered, diff, None)
	output, error, ret = RunExternalCommand(TIMEOUT, "stat -c %s "+diff)
	
	# check if the two files are identical
	if (int(output.decode()[0]) != 0):
		print("TEST "+str(test_no)+": bad "+ftype+" output")
		print("** Expected:")
		output, error, ret = RunExternalCommand(TIMEOUT, "head -10 "+expected)
		if (output != None):
			print(output.decode())
		print("** Recovered:")
		output, error, ret = RunExternalCommand(TIMEOUT, "head -10 "+recovered)
		if (output != None):
			print(output.decode())
		print("** Diff file (which should be empty) contains:")
		output, error, ret = RunExternalCommand(TIMEOUT, "head -10 "+diff)
		if (output != None):
			print(output.decode())
		outcome = -1

	# cleanup
	if (MODE_GRADING == True):
		output, error, ret = RunExternalCommand(TIMEOUT, "rm -f " + diff)

	return outcome

def RunTest(test_no, test_path, results_path, max_redirects):

	# outcome of the test (PASSE/FAIL)
	test_outcome = "PASSED"

	# run test
	test_script = os.path.join(test_path, "test" + str(test_no) + ".csh")
	output, error, ret = RunExternalCommand(TIMEOUT, test_script, \
									   				 os.path.join(results_path, "tmp.out"), \
									   				 os.path.join(results_path, "tmp.err"))
	rtvd_status = ret

	if (rtvd_status == KILL_STATUS):
		print("TEST "+str(test_no)+": Test ran more than " +str(TIMEOUT)+" seconds and timed out")
		return "FAILED"

	# check redirection (if any)
	for marker in range(1,max_redirects+1):
		rtvd_redirect = os.path.join(results_path, str(test_no) + ".fout." + str(marker))
		expt_redirect = os.path.join(results_path, "fout." + str(marker))
		if (IsFile(expt_redirect) == True):
			if (IsFile(rtvd_redirect) == False):
				print("TEST "+str(test_no)+": Bad redirection output (or not found)")
				print("** Expected:")
				if (output != None):
					print(output.decode())
				output, error, ret = RunExternalCommand(TIMEOUT, "head -10 "+expt_redirect)
				print("** Recovered:")
				print(rtvd_redirect + " is not created")
				test_outcome = "FAILED"
			else:
				diff_redirect  = os.path.join(results_path, "tmp.diff")
				if (CheckDiff(expt_redirect, rtvd_redirect, diff_redirect) != 0):
					test_outcome = "FAILED"
		# cleanup
		if (MODE_GRADING == True):
			output, error, ret = RunExternalCommand(TIMEOUT, "rm -f " + rtvd_redirect + " " + expt_redirect)


	# check STDOUT and STDERR
	for ftype in ([".out", ".err"]):
		expt_standard = os.path.join(results_path, str(test_no) + ftype)
		rtvd_standard = os.path.join(results_path, "tmp" + ftype)
		if (IsFile(rtvd_standard) == False):
			if (ftype == ".out"):
				print("TEST "+str(test_no)+": Bad STDOUT (or not found)")
			else:
				print("TEST "+str(test_no)+": Bad STDERR (or not found)")
			print("** Expected:")
			output, error, ret = RunExternalCommand(TIMEOUT, "head -10 "+expt_standard)
			if (output != None):
				print(output.decode())
			print("** Recovered:")
			print(rtvd_standard + " is not created")
			test_outcome = "FAILED"
		else:
			diff_standard  = os.path.join(results_path, "tmp.diff")
			if (CheckDiff(expt_standard, rtvd_standard, diff_standard) != 0):
				test_outcome = "FAILED"
		# cleanup
		if (MODE_GRADING == True):
			output, error, ret = RunExternalCommand(TIMEOUT, "rm -f " + rtvd_standard + " " + expt_standard)
	   
	# Check return code
	expt_status = os.path.join(results_path, str(test_no) + ".status")
	output, error, ret = RunExternalCommand(TIMEOUT, "head -10 "+expt_status)
	if (rtvd_status != int(output.decode()[0])):
		print("TEST "+str(test_no)+": Bad exist status")
		print("** Expected:  " + output.decode())
		print("** Retrieved: " + str(rtvd_status))
		test_outcome = "FAILED"

	return test_outcome

if __name__ == '__main__':

	mode = sys.argv[1]
	max_tests = sys.argv[2]
	test_path = sys.argv[3]
	results_path = "./results"
	output, error, ret = RunExternalCommand(TIMEOUT, "rm -r "+results_path)
	output, error, ret = RunExternalCommand(TIMEOUT, "mkdir "+results_path)
	if (ret != 0):
		print("Failed to initialized results dir")
		sys.exit(-1)

	if (mode == "all"):
		correct = 0
		for test_no in range(int(max_tests)):
			try:
				test_outcome = RunTest(test_no, test_path, results_path, 2)
			except Exception as e:
				test_outcome = "FAILED"
			if (test_outcome == "PASSED"):
				correct = correct + 1
				print ("## TEST " + str(test_no) +": PASSED")
			else:
				print ("## TEST " + str(test_no) +": FAILED")
			print("##----\n")

		print("\n###### SUMMARY ######")
		print("Passed: " + str(correct) + "/" + str(max_tests))
		print("Score:  " + str(correct*5) + "/100")

	elif (mode == "single"):
		
		MODE_GRADING = False
		test_no = int(sys.argv[2])
		test_outcome = RunTest(test_no, test_path, results_path, 2)
		if (test_outcome == "PASSED"):
			print ("## TEST " + str(test_no) +": PASSED")
		else:
			print ("## TEST " + str(test_no) +": FAILED")
		print("##----\n")

	# cleanup
	output, error, ret = RunExternalCommand(TIMEOUT, "rm -f in.txt nested.txt")
