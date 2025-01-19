import os
import re
import struct

def get_heap_memory(pid):
    """Fetch the heap memory range for the target process."""
    heap = None
    with open(f"/proc/{pid}/maps", "r") as maps_file:
        for line in maps_file:
            if "[heap]" in line:
                heap = line.split(" ")[0]
                break
    if heap is None:
        raise RuntimeError("Heap memory not found for the process.")
    start, end = heap.split("-")
    return int(start, 16), int(end, 16)

def search_and_modify_heap(pid, search_string, replace_string):
    """Search for a string in the heap and replace it."""
    start, end = get_heap_memory(pid)
    search_bytes = search_string.encode()
    replace_bytes = replace_string.encode()

    if len(replace_bytes) > len(search_bytes):
        raise ValueError("Replacement string cannot be longer than the search string.")

    with open(f"/proc/{pid}/mem", "rb+", buffering=0) as mem_file:
        # Seek to the start of the heap
        mem_file.seek(start)

        # Read heap memory
        heap_data = mem_file.read(end - start)

        # Search for the string
        match = re.search(re.escape(search_bytes), heap_data)
        if not match:
            print("String not found in the heap.")
            return

        # Calculate the position to modify
        position = match.start()

        # Write the replacement string
        mem_file.seek(start + position)
        mem_file.write(replace_bytes)

        # Fill the remaining bytes with nulls if replacement is shorter
        if len(replace_bytes) < len(search_bytes):
            mem_file.write(b"\x00" * (len(search_bytes) - len(replace_bytes)))

        print(f"String '{search_string}' replaced with '{replace_string}' at position {start + position}.")

# Example Usage
if __name__ == "__main__":
    target_pid = int(input("Enter the PID of the target process: "))
    search_str = input("Enter the string to search: ")
    replace_str = input("Enter the replacement string: ")

    try:
        search_and_modify_heap(target_pid, search_str, replace_str)
    except Exception as e:
        print(f"Error: {e}")
