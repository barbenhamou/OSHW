import subprocess
import json
import os
import shutil
import signal

class TimeoutException(Exception):
    pass

def timeout_handler(signum, frame):
    raise TimeoutException

def compile_c_files():
    print("Compiling C files...")
    subprocess.run(["sudo", "gcc", "part4.c", "copytree.c", "-o", "copytree"], check=True)
    print("Compilation complete.")

def setup_test_environment(test_case):
    print(f"Setting up test environment for test case: {test_case['description']}")
    os.makedirs(test_case['source_directory'], exist_ok=True)
    for item in test_case['items']:
        item_path = os.path.join(test_case['source_directory'], item['name'])
        if item['type'] == 'file':
            with open(item_path, 'w') as f:
                f.write(item['content'])
            os.chmod(item_path, item['permissions'])
        elif item['type'] == 'directory':
            os.makedirs(item_path, exist_ok=True)
            os.chmod(item_path, item['permissions'])
        elif item['type'] == 'symlink':
            os.symlink(item['target'], item_path)
    print(f"Test environment setup complete for test case: {test_case['description']}")

def run_test_case(test_case):
    print(f"Running test case: {test_case['description']}")
    setup_test_environment(test_case)
    command = ["sudo", "./copytree"]
    if test_case['copy_symlinks']:
        command.append("-l")
    if test_case['copy_permissions']:
        command.append("-p")
    command.extend([test_case['source_directory'], test_case['destination_directory']])
    print(f"Running command: {' '.join(command)}")
    subprocess.run(command, check=True)
    print(f"Test case execution complete: {test_case['description']}")

def verify_test_result(test_case):
    print(f"Verifying test result for test case: {test_case['description']}")
    for item in test_case['items']:
        dest_item_path = os.path.join(test_case['destination_directory'], item['name'])
        if item['type'] == 'file':
            if not os.path.isfile(dest_item_path):
                print(f"File not found: {dest_item_path}")
                return False
            if (os.stat(dest_item_path).st_mode & 0o777) != (item['permissions'] & 0o777):
                print(f"File permissions mismatch for {dest_item_path}")
                return False
        elif item['type'] == 'directory':
            if not os.path.isdir(dest_item_path):
                print(f"Directory not found: {dest_item_path}")
                return False
            if (os.stat(dest_item_path).st_mode & 0o777) != (item['permissions'] & 0o777):
                print(f"Directory permissions mismatch for {dest_item_path}")
                return False
        elif item['type'] == 'symlink':
            if not os.path.islink(dest_item_path):
                print(f"Symlink not found: {dest_item_path}")
                return False
            if os.readlink(dest_item_path) != item['target']:
                print(f"Symlink target mismatch for {dest_item_path}")
                return False
    print(f"Test result verification complete for test case: {test_case['description']}")
    return True

def run_tests():
    signal.signal(signal.SIGALRM, timeout_handler)
    signal.alarm(30)  # Set timeout to 30 seconds

    try:
        with open('test_cases.json', 'r') as f:
            test_cases = json.load(f)

        compile_c_files()

        results = []
        for i, test_case in enumerate(test_cases, 1):
            if os.path.exists(test_case['source_directory']):
                shutil.rmtree(test_case['source_directory'])
            if os.path.exists(test_case['destination_directory']):
                shutil.rmtree(test_case['destination_directory'])

            run_test_case(test_case)
            result = {
                "test_name": test_case['description'],
                "test_number": i,
                "status": "PASSED" if verify_test_result(test_case) else "FAILED"
            }
            results.append(result)
            print(f"Test case {i} result: {result['status']}")

        signal.alarm(0)  # Disable the alarm

        # Write results to part4_output.txt
        with open('part4_output.txt', 'w') as f:
            for result in results:
                f.write(f"TEST_{result['test_number']}, {result['status']}\n")

        return json.dumps(results, indent=4)

    except TimeoutException:
        # Return failure for all tests if the timeout is reached
        results = []
        for i, test_case in enumerate(test_cases, 1):
            results.append({
                "test_name": test_case['description'],
                "test_number": i,
                "status": "FAILED"
            })

        # Write results to part4_output.txt
        with open('part4_output.txt', 'w') as f:
            for result in results:
                f.write(f"TEST_{result['test_number']}, {result['status']}\n")

        return json.dumps(results, indent=4)

def main():
    run_tests()

if __name__ == "__main__":
    main()
