"""
Test script to verify the list.sort() mutation guard fix.

This script tests that modifying a list during sorting now raises
a ValueError instead of causing memory corruption or crashes.
"""

def test_list_sort_mutation():
    """Test that list.sort() prevents mutation during sorting."""

    print("Testing list.sort() mutation guard...")

    # Test case 1: Mutation via append during comparison
    class MutatingComparer:
        def __lt__(self, other):
            lst.append(0)  # This should raise ValueError
            return False

    lst = [MutatingComparer() for _ in range(5)]

    try:
        lst.sort()
        print("ERROR: Expected ValueError but sort completed successfully")
        return False
    except ValueError as e:
        if "list modified during sort" in str(e):
            print("SUCCESS: ValueError raised as expected:", e)
        else:
            print("ERROR: Unexpected ValueError:", e)
            return False
    except Exception as e:
        print("ERROR: Unexpected exception:", e)
        return False

    # Test case 2: Mutation via extend during comparison
    class ExtendingComparer:
        def __lt__(self, other):
            lst.extend([1, 2, 3])  # This should raise ValueError
            return False

    lst = [ExtendingComparer() for _ in range(3)]

    try:
        lst.sort()
        print("ERROR: Expected ValueError but sort completed successfully")
        return False
    except ValueError as e:
        if "list modified during sort" in str(e):
            print("SUCCESS: ValueError raised as expected:", e)
        else:
            print("ERROR: Unexpected ValueError:", e)
            return False
    except Exception as e:
        print("ERROR: Unexpected exception:", e)
        return False

    # Test case 3: Mutation via pop during comparison
    class PoppingComparer:
        def __lt__(self, other):
            lst.pop()  # This should raise ValueError
            return False

    lst = [PoppingComparer() for _ in range(3)]

    try:
        lst.sort()
        print("ERROR: Expected ValueError but sort completed successfully")
        return False
    except ValueError as e:
        if "list modified during sort" in str(e):
            print("SUCCESS: ValueError raised as expected:", e)
        else:
            print("ERROR: Unexpected ValueError:", e)
            return False
    except Exception as e:
        print("ERROR: Unexpected exception:", e)
        return False

    # Test case 4: Normal sorting should still work
    try:
        normal_list = [3, 1, 4, 1, 5, 9, 2, 6]
        normal_list.sort()
        if normal_list == [1, 1, 2, 3, 4, 5, 6, 9]:
            print("SUCCESS: Normal sorting works correctly")
        else:
            print("ERROR: Normal sorting failed")
            return False
    except Exception as e:
        print("ERROR: Normal sorting failed with exception:", e)
        return False

    print("All tests passed!")
    return True

if __name__ == "__main__":
    test_list_sort_mutation()
