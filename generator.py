import string

import random
import sys

test_dir = sys.argv[1]
count_test = int(sys.argv[2])
file = test_dir + f"/test_{count_test}"

file_test = file + ".t"
with open(file_test, "w") as t:
    if count_test <= 1000:
        s = ''.join(random.choice(string.ascii_lowercase) for _ in range(count_test))
    else:
        s = ''.join(random.choice(string.ascii_lowercase) for _ in range(1000))
        s *= count_test // 1000
        s.join(random.choice(string.ascii_lowercase) for _ in range(count_test - (count_test // 1000 * 1000)))
    t.write("compress\n" + s)