import matplotlib.pyplot as plt

x = [10000, 20000, 30000, 40000, 50000, 60000]
x2 = [100000, 150000, 200000, 250000]
x3 = [10000, 20000, 30000, 40000, 50000]

forwardLevenshtein =                [0.345842, 1.519301, 3.626702, 6.293141, 9.199267, 13.429229]
backwardLevenshtein =               [0.338444, 1.529102, 3.404311, 6.001849, 9.053014, 13.321899]
fbLevenshtein =                     [0.258670, 1.187424, 2.500014, 4.317889, 6.505424, 9.554413]
diagonalLevenshteinMemory =         [0.239417, 0.943647, 2.102897, 3.743161, 5.900386, 9.689873]
diagonalLevenshteinMemoryParallel = [0.831118, 1.856795, 2.899954, 3.889169, 5.475029, 7.340391] #8 threads

forwardLevenshteinSpace =          [26.395551, 58.962033, 105.193894, 167.749272]
backwardLevenshteinSpace =         [24.271178, 54.160883, 95.668674,  149.694305]
fbLevenshteinSpace =               [13.435573, 30.358760, 53.150599,  83.564126]
diagonalLevenshteinSpace =         [12.954458, 26.667030, 48.488514,  75.765589]
diagonalLevenshteinSpaceParallel = [12.361829, 22.076464, 38.742937,  50.713466] #8 threads


forwardLCS =  [0.319667, 1.429334, 3.211196, 6.018281, 10.109169, 12.394485]
backwardLCS = [0.318628, 1.438086, 3.223706, 6.271317, 9.850569, 12.581315]
fbLCS =       [0.247322, 1.136386, 2.406986, 4.189387, 7.354680, 9.246377]

forwardLCSspace =  [11.047188, 23.489699, 41.342735, 65.269299]
backwardLCSspace = [10.564286, 23.267623, 42.785099, 68.833067]
fbLCSspace =       [6.365848, 14.281909, 26.146862, 40.857753]

#the old function
diagonalLevenshtein =         [0.766702, 3.249734, 7.991920, 16.164053, 35.703142]
diagonalLevenshteinParallel = [1.018414, 2.634201, 5.033297, 8.543985, 17.881329]

diagonalLCS =         []
diagonalLCSParallel = []

nThreads2 =  [1.307152, 4.930329, 10.830216, 36.209834, 55.973594]
nThreads3 =  [1.049671, 3.664200, 8.004958,  14.237673, 34.572387]
nThreads4 =  [0.872162, 2.981476, 6.441798,  11.365335, 26.947444]
nThreads5 =  [0.846055, 2.625845, 5.494065,  9.388782,  19.622980]
nThreads6 =  [0.846184, 2.393235, 4.984522,  8.507192,  17.990511]
nThreads7 =  [0.928700, 2.387165, 4.719933,  8.053903,  15.864173]
nThreads8 =  [1.031289, 2.458716, 4.608730,  7.937492,  16.417606]
nThreads9 =  [1.253855, 3.000064, 5.682924,  9.514016,  17.121239]
nThreads10 = [1.451054, 3.191380, 5.892956,  9.580704,  18.361809]
nThreads11 = [1.669980, 3.346404, 5.933924,  9.661376,  17.336488]
nThreads12 = [1.747410, 3.475634, 6.001397,  9.517540,  17.887280]

plt.figure(1)
plt.plot(x, list(zip(forwardLevenshtein, backwardLevenshtein, fbLevenshtein, diagonalLevenshteinMemory, diagonalLevenshteinMemoryParallel)), label=['naprej', 'nazaj', 'naprej-nazaj', 'diagonalno', 'diagonalno paralelno (8 niti)'])
plt.title('Levenshtein primerjava')
plt.xlabel('dolžina niza')
plt.ylabel('čas izvajanja (sekunde)')
plt.legend()


plt.figure(2)
plt.plot(x, list(zip(forwardLCS, backwardLCS, fbLCS)), label=['forward', 'backward', 'forward-backward'])
plt.title('LCS comparison')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()


plt.figure(3)
plt.plot(x2, list(zip(forwardLevenshteinSpace, backwardLevenshteinSpace, fbLevenshteinSpace)), label=['forward', 'backward', 'forward-backward'])
plt.title('Levenshtein comparison with space optimization')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()


plt.figure(4)
plt.plot(x2, list(zip(forwardLevenshteinSpace, backwardLevenshteinSpace, fbLevenshteinSpace, diagonalLevenshteinSpace, diagonalLevenshteinSpaceParallel)), label=['naprej', 'nazaj', 'naprej nazaj', 'diagonalno', 'diagonalno paralelno (8 niti)'])
plt.title('Levenshtein primerjava')
plt.xlabel('dolžina niza')
plt.ylabel('čas izvajanja (sekunde)')
plt.legend()


plt.figure(5)
plt.plot(x3, list(zip(diagonalLevenshtein, diagonalLevenshteinParallel)), label=['diagonal', 'diagonal parallel'])
plt.title('Diagonal Levenshtein comparison')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()


plt.figure(6)
plt.plot(x3, list(zip(diagonalLevenshtein, nThreads2, nThreads3, nThreads4, nThreads5, nThreads6, nThreads7, nThreads8, nThreads9, nThreads10, nThreads11, nThreads12)), label=['baseline (1 thread)', '2 threads', '3 threads', '4 threads', '5 threads', '6 threads', '7 threads', '8 threads', '9 threads', '10 threads', '11 threads', '12 threads'])
plt.title('Diagonal Levenshtein comparison with various numbers of threads')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()

plt.figure(7)
plt.plot(x3, list(zip(diagonalLevenshtein, nThreads3, nThreads4, nThreads5, nThreads6, nThreads7, nThreads8, nThreads9, nThreads10, nThreads11, nThreads12)), label=['baseline (1 thread)', '3 threads', '4 threads', '5 threads', '6 threads', '7 threads', '8 threads', '9 threads', '10 threads', '11 threads', '12 threads'])
plt.title('Diagonal Levenshtein comparison with various numbers of threads')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()

plt.show()
